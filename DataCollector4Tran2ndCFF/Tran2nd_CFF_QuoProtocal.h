#ifndef __CTP_QUOTATION_PROTOCAL_TRAN2ND_CFF_H__
#define	__CTP_QUOTATION_PROTOCAL_TRAN2ND_CFF_H__
#pragma pack(1)


typedef struct
{
	char						Key[20];					///< ������ֵ
	unsigned int				MarketID;					///< �г����
	unsigned int				MarketDate;					///< �г�����
	unsigned int				KindCount;					///< �������
	unsigned int				WareCount;					///< ��Ʒ����
} tagCFFFutureMarketInfo_LF172;


typedef struct
{
	char						Key[20];					///< ������ֵ
	char						KindName[64];				///< ��������
	unsigned int				PriceRate;					///< �۸�Ŵ���[10�Ķ��ٴη�]
	unsigned int				LotSize;					///< һ�ֵ��ڼ��ź�Լ
	unsigned int				LotFactor;					///< ���֡�����
	unsigned int				PriceTick;					///< ��С�䶯��λ
	unsigned int				ContractMult;				///< ��Լ����
	unsigned int				ContractUnit;				///< ��Լ��λ(������Ȩ��Ϣ������ĺ�Լ��λ��һ��Ϊ����)
	char						UnderlyingCode[20];			///< ���֤ȯ����
	unsigned int				UnderlyingMarketID;			///< ����г����
	char						OptionType;					///< ��Ȩ���ͣ�'E' = ŷʽ��Ȩ 'A' = ��ʽ��Ȩ������Ȩ��
	unsigned int				PeriodsCount;				///< ����ʱ����Ϣ�б���
	unsigned int				MarketPeriods[10][2];		///< ����ʱ��������Ϣ�б�
} tagCFFFutureKindDetail_LF173;


typedef struct
{
	char						Key[20];					///< ������ֵ
	unsigned int				MarketTime;					///< �г�ʱ��
	char						StatusFlag[8];				///< �г�״̬(8���ַ�,��ϸ������ĵ�)
} tagCFFFutureMarketStatus_HF174;


typedef struct
{
	char						Code[20];					///< ��Լ����
	char						Name[64];					///< ��Լ����
	unsigned short				Kind;						///< ֤ȯ����
	char						CallOrPut;					///< �Ϲ��Ϲ���'C' = �Ϲ� 'P' = �Ϲ�������Ȩ��
	char						UpdateVersion;				///< ��Ȩ��Լ�İ汾��(�¹Һ�Լ��'1')
	unsigned int				XqPrice;					///< ��Ȩ�۸�[*�Ŵ���]
	unsigned int				StartDate;					///< �׸�������(YYYYMMDD)
	unsigned int				EndDate;					///< �������(YYYYMMDD)
	unsigned int				XqDate;						///< ��Ȩ��(YYYYMM)
	unsigned int				DeliveryDate;				///< ������(YYYYMMDD)
	unsigned int				ExpireDate;					///< ������(YYYYMMDD)
	char						StatusFlag[8];				///< ��Ȩ��Լ״̬(8���ַ�,��ϸ������ĵ�)
} tagCFFFutureReferenceData_LF175;


typedef struct
{
	char						Code[20];					///< ��Լ����
	unsigned int				Open;						///< ���̼�[*�Ŵ���]
	unsigned int				Close;						///< ���ռ�[*�Ŵ���]
	unsigned int				PreClose;					///< ���ռ�[*�Ŵ���]
	unsigned int				UpperPrice;					///< ������ͣ�۸�[*�Ŵ���], 0��ʾ������
	unsigned int				LowerPrice;					///< ���յ�ͣ�۸�[*�Ŵ���], 0��ʾ������
	unsigned int				SettlePrice;				///< ����[*�Ŵ���]
	unsigned int				PreSettlePrice;				///< ��Լ���[*�Ŵ���]
	unsigned __int64			PreOpenInterest;			///< ���ճֲ���(��)
	char						TradingPhaseCode[4];		///< ����״̬
} tagCFFFutureSnapData_LF176;


typedef struct
{
	char						Code[20];					///< ��Լ����
	unsigned int				Now;						///< ���¼�[*�Ŵ���]
	unsigned int				High;						///< ��߼�[*�Ŵ���]
	unsigned int				Low;						///< ��ͼ�[*�Ŵ���]
	double						Amount;						///< �ܳɽ����[Ԫ]
	unsigned __int64			Volume;						///< �ܳɽ���[��/��]
	unsigned __int64			Position;					///< �ֲ���
} tagCFFFutureSnapData_HF177;


typedef struct
{
	unsigned int				Price;						///< ί�м۸�[* �Ŵ���]
	unsigned __int64			Volume;						///< ί����[��]
} tagCFFBuySellItem;


typedef struct
{
	char						Code[20];					///< ��Լ����
	tagCFFBuySellItem			Buy[5];						///< ���嵵
	tagCFFBuySellItem			Sell[5];					///< ���嵵
} tagCFFFutureSnapBuySell_HF178;


#pragma pack()
#endif









