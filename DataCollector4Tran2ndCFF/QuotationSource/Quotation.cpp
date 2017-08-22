#include <math.h>
#include <iostream>
#include <algorithm>
#include "MVPlat.h"
#include "Quotation.h"
#include "../Infrastructure/IniFile.h"
#include "../DataCollector4Tran2ndCFF.h"


WorkStatus::WorkStatus()
: m_eWorkStatus( ET_SS_UNACTIVE )
{
}

WorkStatus::WorkStatus( const WorkStatus& refStatus )
{
	CriticalLock	section( m_oLock );

	m_eWorkStatus = refStatus.m_eWorkStatus;
}

WorkStatus::operator enum E_SS_Status()
{
	CriticalLock	section( m_oLock );

	return m_eWorkStatus;
}

std::string& WorkStatus::CastStatusStr( enum E_SS_Status eStatus )
{
	static std::string	sUnactive = "未激活";
	static std::string	sDisconnected = "断开状态";
	static std::string	sConnected = "连通状态";
	static std::string	sLogin = "登录成功";
	static std::string	sInitialized = "初始化中";
	static std::string	sWorking = "推送行情中";
	static std::string	sUnknow = "不可识别状态";

	switch( eStatus )
	{
	case ET_SS_UNACTIVE:
		return sUnactive;
	case ET_SS_DISCONNECTED:
		return sDisconnected;
	case ET_SS_CONNECTED:
		return sConnected;
	case ET_SS_LOGIN:
		return sLogin;
	case ET_SS_INITIALIZING:
		return sInitialized;
	case ET_SS_WORKING:
		return sWorking;
	default:
		return sUnknow;
	}
}

WorkStatus&	WorkStatus::operator= ( enum E_SS_Status eWorkStatus )
{
	CriticalLock	section( m_oLock );

	if( m_eWorkStatus != eWorkStatus )
	{
		QuoCollector::GetCollector()->OnLog( TLV_INFO, "WorkStatus::operator=() : Exchange Session Status [%s]->[%s]"
											, CastStatusStr(m_eWorkStatus).c_str(), CastStatusStr(eWorkStatus).c_str() );
				
		m_eWorkStatus = eWorkStatus;
	}

	return *this;
}


///< ----------------------------------------------------------------


Quotation::Quotation()
 : m_pDataBuff( NULL )
{
	m_pDataBuff = new char[MAX_IMAGE_BUFF];
}

Quotation::~Quotation()
{
	Release();

	if( NULL != m_pDataBuff )
	{
		delete [] m_pDataBuff;
		m_pDataBuff = NULL;
	}
}

WorkStatus& Quotation::GetWorkStatus()
{
	return m_oWorkStatus;
}

int Quotation::Initialize()
{
	if( NULL == m_pDataBuff )
	{
		QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::Initialize() : invalid data buffer ptr(NULL)." );
		return -1;
	}

	if( GetWorkStatus() == ET_SS_UNACTIVE )
	{
		unsigned int				nSec = 0;
		int							nErrCode = 0;
		tagCcComm_MarketInfoHead	tagHead = { 0 };
		tagCcComm_MarketInfoDetail	tagDetail[32] = { 0 };
		QuoCollector::GetCollector()->OnLog( TLV_INFO, "Quotation::Initialize() : ............ Quotation Is Activating............" );

		Release();
		if( m_oSHOPTDll.Instance( "tran2ndcff.dll" ) < 0 )
		{
			QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::Initialize() : failed 2 initialize tran2ndsz.dll" );
			Release();
			return -2;
		}

		for( nSec = 0; nSec < 60*2; nSec++ )
		{
			SimpleTask::Sleep(1000);

			if( true == m_oSHOPTDll.IsWorking() )
			{
				if( (nErrCode = m_oSHOPTDll.GetMarketInfo( &tagHead, tagDetail, sizeof tagDetail/sizeof tagDetail[0] )) < 0 )
				{
					continue;
				}

				if( tagHead.MarketStatus <= 0 )
				{	
					continue;
				}

				break;
			}
		}

		if( nSec >= 60*2 )
		{
			QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::Initialize() : overtime > (2mins)" );
			Release();
			return -3;
		}

		if( (nErrCode = BuildImageData()) < 0 )
		{
			QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::Initialize() : failed 2 build image data 2 database" );
			return -4;
		}

		m_oWorkStatus = ET_SS_WORKING;				///< 更新Quotation会话的状态

		QuoCollector::GetCollector()->OnLog( TLV_INFO, "Quotation::Initialize() : ............ Quotation Activated!.............." );
	}

	return 0;
}

int Quotation::Release()
{
	if( m_oWorkStatus != ET_SS_UNACTIVE )
	{
		QuoCollector::GetCollector()->OnLog( TLV_INFO, "Quotation::Release() : ............ Destroying .............." );

		m_oWorkStatus = ET_SS_UNACTIVE;	///< 更新Quotation会话的状态

		QuoCollector::GetCollector()->OnLog( TLV_INFO, "Quotation::Release() : ............ Destroyed! .............." );
	}

	return 0;
}

int Quotation::BuildImageData()
{
	int								nErrCode = 0;
	tagCcComm_MarketInfoHead		tagHead = { 0 };
	tagCcComm_MarketInfoDetail		tagDetail[32] = { 0 };
	tagCFFFutureMarketInfo_LF172	tagMkInfo = { 0 };
	tagCFFFutureMarketStatus_HF174	tagMkStatus = { 0 };

	if( (nErrCode = m_oSHOPTDll.GetMarketInfo( &tagHead, tagDetail, sizeof tagDetail/sizeof tagDetail[0] )) < 0 )
	{
		QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::BuildImageData() : cannot fetch marketinfo, errorcode=%d", nErrCode );
		return -1;
	}

	if( tagHead.MarketStatus <= 0 )
	{
		QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::BuildImageData() : marketstatus =%d ", tagHead.MarketStatus );
		return -2;
	}

	::strcpy( tagMkStatus.Key, "status" );
	tagMkStatus.StatusFlag[0] = tagHead.MarketStatus==0?'0':'1';
	tagMkStatus.MarketTime = tagHead.Time;
	QuoCollector::GetCollector()->OnImage( 174, (char*)&tagMkStatus, sizeof(tagCFFFutureMarketStatus_HF174), false );

	::strcpy( tagMkInfo.Key, "mkinfo" );
	tagMkInfo.KindCount = tagHead.KindCount;
	tagMkInfo.MarketDate = tagHead.Date;
	tagMkInfo.MarketID = Configuration::GetConfig().GetMarketID();
	::memcpy( &(tagMkInfo.MarketPeriods), &(tagHead.Periods), sizeof(tagHead.Periods) );
	tagMkInfo.PeriodsCount = tagHead.PeriodsCount;
	tagMkInfo.WareCount = tagHead.WareCount;
	QuoCollector::GetCollector()->OnImage( 172, (char*)&tagMkInfo, sizeof(tagCFFFutureMarketInfo_LF172), false );

	for( int i = 0 ; i < tagMkInfo.KindCount; i++ )
	{
		char							pszKey[12] = { 0 };
		tagCFFFutureKindDetail_LF173	tagCategory = { 0 };

		::sprintf( pszKey, "%d", i );
		::strcpy( tagCategory.Key, pszKey );
		::strncpy( tagCategory.KindName, tagDetail[i].KindName, sizeof(tagDetail[i].KindName) );
		tagCategory.LotSize = tagDetail[i].LotSize;
		tagCategory.PriceRate = tagDetail[i].PriceRate;
		tagCategory.WareCount = tagDetail[i].WareCount;

		QuoCollector::GetCollector()->OnImage( 173, (char*)&tagCategory, sizeof(tagCFFFutureKindDetail_LF173), false );
	}

	tagCcComm_CffexNameTable*	pTable = (tagCcComm_CffexNameTable*)m_pDataBuff;
	if( (nErrCode = m_oSHOPTDll.GetNameTable( 0, pTable, tagHead.WareCount )) < 0 )
	{
		QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::BuildImageData() : cannot fetch nametable from dll, errorcode = %d ", nErrCode );
		return -3;
	}

	for( int i = 0; i < tagHead.WareCount; ++i )
	{
		tagCFFFutureReferenceData_LF175	tagRef = { 0 };

		::strncpy( tagRef.Code, pTable[i].Code, sizeof(pTable[i].Code) );
		//tagRef.ContractUnit
		tagRef.ContractMult = pTable[i].ContractMult;
		//tagRef.DeliveryDate = pTable[i].DeliveryDate;
		//tagRef.EndDate = pTable[i].EndDate;
		//tagRef.ExpireDate = pTable[i].ExpireDate;
		tagRef.Kind = pTable[i].Type;
		//tagRef.LotSize = pTable[i].LotSize;
		::strncpy( tagRef.Name, pTable[i].Name, sizeof(pTable[i].Name) );
		//tagRef.StartDate = pTable[i].StartDate;
		//::strncpy( tagRef.StatusFlag, pTable[i].StatusFlag, sizeof(pTable[i].StatusFlag) );
		::strncpy( tagRef.UnderlyingCode, pTable[i].ObjectCode, sizeof(pTable[i].ObjectCode) );
		//tagRef.XqDate = pTable[i].XqDate;
		//tagRef.XqPrice = pTable[i].XqPrice;

		QuoCollector::GetCollector()->OnImage( 175, (char*)&tagRef, sizeof(tagCFFFutureReferenceData_LF175), false );
	}

	//5,获取个股快照
	tagCcComm_CffexStockDataEx*	pStock = (tagCcComm_CffexStockDataEx*)m_pDataBuff;
	if( (nErrCode = m_oSHOPTDll.GetStock(0, pStock, tagHead.WareCount)) < 0 )
	{
		QuoCollector::GetCollector()->OnLog( TLV_WARN, "Quotation::BuildImageData() : cannot fetch stocktable from dll, errorcode = %d ", nErrCode );
		return -6;
	}

	for (int i = 0; i < nErrCode; ++i)
	{
		tagCFFFutureSnapData_LF176		tagStockLF = { 0 };
		tagCFFFutureSnapData_HF177		tagStockHF = { 0 };
		tagCFFFutureSnapBuySell_HF178	tagStockBS = { 0 };

		::strncpy( tagStockLF.Code, pStock[i].Code, sizeof(pStock[i].Code) );
		::strncpy( tagStockHF.Code, pStock[i].Code, sizeof(pStock[i].Code) );
		::strncpy( tagStockBS.Code, pStock[i].Code, sizeof(pStock[i].Code) );

		tagStockLF.Open = pStock[i].Open;
		//tagStockLF.PreClose
		//tagStockLF.PreOpenInterest = pStock[
		tagStockLF.PreSettlePrice = pStock[i].PreSettlePrice;
		tagStockLF.SettlePrice = pStock[i].SettlePrice;
		tagStockLF.TradingPhaseCode[0] = 'T';
		///<::memcpy( tagStockLF.TradingPhaseCode, pStock[i].TradingPhaseSecurityID, sizeof(pStock[i].TradingPhaseSecurityID) );

		tagStockHF.Amount = pStock[i].Amount;
		tagStockHF.High = pStock[i].High;
		tagStockHF.Low = pStock[i].Low;
		tagStockHF.Now = pStock[i].Now;
		tagStockHF.Position = pStock[i].OpenInterest;
		tagStockHF.Volume = pStock[i].Volume;

		memcpy( tagStockBS.Buy, pStock[i].Buy, sizeof(tagStockBS.Buy) );
		memcpy( tagStockBS.Sell, pStock[i].Sell, sizeof(tagStockBS.Sell) );

		QuoCollector::GetCollector()->OnImage( 176, (char*)&tagStockLF, sizeof(tagCFFFutureSnapData_LF176), false );
		QuoCollector::GetCollector()->OnImage( 177, (char*)&tagStockHF, sizeof(tagCFFFutureSnapData_HF177), false );
		QuoCollector::GetCollector()->OnImage( 178, (char*)&tagStockBS, sizeof(tagCFFFutureSnapBuySell_HF178), true );
	}

	return 0;
}

void Quotation::OnPushMarketInfo(const char *buf, size_t len)
{
	tagCFFFutureMarketStatus_HF174		tagMkStatus = { 0 };
	tagCcComm_ShOptMarketStatus*		marketinfo = (tagCcComm_ShOptMarketStatus*)buf;

	tagMkStatus.StatusFlag[0] = '1';
	::strcpy( tagMkStatus.Key, "status" );
	tagMkStatus.MarketTime = marketinfo->MarketTime;
	tagMkStatus.StatusFlag[1] = '1';
	QuoCollector::GetCollector()->OnData( 174, (char*)&tagMkStatus, sizeof(tagCFFFutureMarketStatus_HF174), true );
}

void Quotation::OnPushStock(const char * buf, size_t InSize)
{
	tagCcComm_CffexStockData*		stock = (tagCcComm_CffexStockData*)buf;
	tagCFFFutureSnapData_LF176		tagStockLF = { 0 };
	tagCFFFutureSnapData_HF177		tagStockHF = { 0 };
	tagCFFFutureSnapBuySell_HF178	tagStockBS = { 0 };

	::strncpy( tagStockLF.Code, stock->Code, sizeof(stock->Code) );
	::strncpy( tagStockHF.Code, stock->Code, sizeof(stock->Code) );
	::strncpy( tagStockBS.Code, stock->Code, sizeof(stock->Code) );

	tagStockLF.Open = stock->Open;
	//tagStockLF.PreClose
	//tagStockLF.PreOpenInterest = pStock[
	tagStockLF.PreSettlePrice = stock->PreSettlePrice;
	tagStockLF.SettlePrice = stock->SettlePrice;
	tagStockLF.TradingPhaseCode[0] = 'T';
	///<::memcpy( tagStockLF.TradingPhaseCode, pStock[i].TradingPhaseSecurityID, sizeof(pStock[i].TradingPhaseSecurityID) );

	tagStockHF.Amount = stock->Amount;
	tagStockHF.High = stock->High;
	tagStockHF.Low = stock->Low;
	tagStockHF.Now = stock->Now;
	tagStockHF.Position = stock->OpenInterest;
	tagStockHF.Volume = stock->Volume;

	memcpy( tagStockBS.Buy, stock->Buy, sizeof(tagStockBS.Buy) );
	memcpy( tagStockBS.Sell, stock->Sell, sizeof(tagStockBS.Sell) );

	QuoCollector::GetCollector()->OnData( 176, (char*)&tagStockLF, sizeof(tagCFFFutureSnapData_LF176), true );
	QuoCollector::GetCollector()->OnData( 177, (char*)&tagStockHF, sizeof(tagCFFFutureSnapData_HF177), true );
	QuoCollector::GetCollector()->OnData( 178, (char*)&tagStockBS, sizeof(tagCFFFutureSnapBuySell_HF178), true );
}

void Quotation::OnInnerPush( unsigned char MainType, unsigned char ChildType, const char * InBuf, unsigned short InSize, unsigned char marketid )
{
	int								nErrCode = 0;
	tagCFFFutureMarketStatus_HF174	tagMkStatus = { 0 };
	int								offset = sizeof(tagCcComm_TimeInfo);
	tagCcComm_TimeInfo*				updataTime = (tagCcComm_TimeInfo*)InBuf;

	if( (InSize < sizeof(tagCcComm_TimeInfo)) )
	{
		return;
	}

	if( ChildType == 103 )
	{
        while( offset < InSize )
        {
            char	type = InBuf[offset];
            offset += 1;

			tagMkStatus.StatusFlag[0] = '1';
			::strcpy( tagMkStatus.Key, "status" );
			tagMkStatus.MarketTime = updataTime->CurTime;
			tagMkStatus.StatusFlag[1] = '1';
			QuoCollector::GetCollector()->OnData( 174, (char*)&tagMkStatus, sizeof(tagCFFFutureMarketStatus_HF174), true );

            switch( type )
            {
            case 0:
                OnPushMarketInfo(InBuf + offset, 0);
                offset += sizeof( tagCcComm_ShOptMarketStatus );
                break;
            case 1:
                OnPushStock(InBuf+offset, 0);
                offset += sizeof( tagCcComm_CffexStockData );
                break;
            default:
                return;
            }
        }
	    return;	
    }
}

void Quotation::OnPush( unsigned char MainType, unsigned char ChildType, const char *InBuf, unsigned short InSize, unsigned char Marketid, unsigned short UnitNo, bool SendDirectFlag )
{
    char inbuf[16 * 1024] = {0};
    assert(InSize < sizeof inbuf);
    memcpy(inbuf, InBuf, InSize);

    if( SendDirectFlag )
    {
        char output[16*1024]={0};
        char *pszCurrentPosPt =NULL;
        //解压一下
        int rv = MVPlatIO::RestoreDataFrame(inbuf, InSize, &pszCurrentPosPt, output, 16*1024);
        if (rv <=0)
        {
            return;
        }
		QuoCollector::GetCollector().GetQuoObj().OnInnerPush(MainType, ChildType, pszCurrentPosPt+sizeof(tagComm_FrameHead), rv- sizeof(tagComm_FrameHead), Marketid);
    }
    else
    {
		QuoCollector::GetCollector().GetQuoObj().OnInnerPush(MainType, ChildType, inbuf, InSize, Marketid);
    }
}








