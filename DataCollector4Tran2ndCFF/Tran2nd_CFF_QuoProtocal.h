#ifndef __CTP_QUOTATION_PROTOCAL_TRAN2ND_CFF_H__
#define	__CTP_QUOTATION_PROTOCAL_TRAN2ND_CFF_H__
#pragma pack(1)


typedef struct
{
	char						Key[20];					///< 索引键值
	unsigned int				MarketID;					///< 市场编号
	unsigned int				MarketDate;					///< 市场日期
	unsigned int				KindCount;					///< 类别数量
	unsigned int				WareCount;					///< 商品数量
} tagCFFFutureMarketInfo_LF172;


typedef struct
{
	char						Key[20];					///< 索引键值
	char						KindName[64];				///< 类别的名称
	unsigned int				PriceRate;					///< 价格放大倍数[10的多少次方]
	unsigned int				LotSize;					///< 一手等于几张合约
	unsigned int				LotFactor;					///< “手”比率
	unsigned int				PriceTick;					///< 最小变动价位
	unsigned int				ContractMult;				///< 合约乘数
	unsigned int				ContractUnit;				///< 合约单位(经过除权除息调整后的合约单位，一定为整数)
	char						UnderlyingCode[20];			///< 标的证券代码
	unsigned int				UnderlyingMarketID;			///< 标的市场编号
	char						OptionType;					///< 期权类型（'E' = 欧式期权 'A' = 美式期权）【期权】
	unsigned int				PeriodsCount;				///< 交易时段信息列表长度
	unsigned int				MarketPeriods[10][2];		///< 交易时段描述信息列表
} tagCFFFutureKindDetail_LF173;


typedef struct
{
	char						Key[20];					///< 索引键值
	unsigned int				MarketTime;					///< 市场时间
	char						StatusFlag[8];				///< 市场状态(8个字符,详细定义见文档)
} tagCFFFutureMarketStatus_HF174;


typedef struct
{
	char						Code[20];					///< 合约代码
	char						Name[64];					///< 合约名称
	unsigned short				Kind;						///< 证券类型
	char						CallOrPut;					///< 认沽认购（'C' = 认购 'P' = 认沽）【期权】
	char						UpdateVersion;				///< 期权合约的版本号(新挂合约是'1')
	unsigned int				XqPrice;					///< 行权价格[*放大倍数]
	unsigned int				StartDate;					///< 首个交易日(YYYYMMDD)
	unsigned int				EndDate;					///< 最后交易日(YYYYMMDD)
	unsigned int				XqDate;						///< 行权日(YYYYMM)
	unsigned int				DeliveryDate;				///< 交割日(YYYYMMDD)
	unsigned int				ExpireDate;					///< 到期日(YYYYMMDD)
	char						StatusFlag[8];				///< 期权合约状态(8个字符,详细定义见文档)
} tagCFFFutureReferenceData_LF175;


typedef struct
{
	char						Code[20];					///< 合约代码
	unsigned int				Open;						///< 开盘价[*放大倍数]
	unsigned int				Close;						///< 今收价[*放大倍数]
	unsigned int				PreClose;					///< 昨收价[*放大倍数]
	unsigned int				UpperPrice;					///< 当日涨停价格[*放大倍数], 0表示无限制
	unsigned int				LowerPrice;					///< 当日跌停价格[*放大倍数], 0表示无限制
	unsigned int				SettlePrice;				///< 今结价[*放大倍数]
	unsigned int				PreSettlePrice;				///< 合约昨结[*放大倍数]
	unsigned __int64			PreOpenInterest;			///< 昨日持仓量(张)
	char						TradingPhaseCode[4];		///< 交易状态
} tagCFFFutureSnapData_LF176;


typedef struct
{
	char						Code[20];					///< 合约代码
	unsigned int				Now;						///< 最新价[*放大倍数]
	unsigned int				High;						///< 最高价[*放大倍数]
	unsigned int				Low;						///< 最低价[*放大倍数]
	double						Amount;						///< 总成交金额[元]
	unsigned __int64			Volume;						///< 总成交量[股/张]
	unsigned __int64			Position;					///< 持仓量
} tagCFFFutureSnapData_HF177;


typedef struct
{
	unsigned int				Price;						///< 委托价格[* 放大倍数]
	unsigned __int64			Volume;						///< 委托量[股]
} tagCFFBuySellItem;


typedef struct
{
	char						Code[20];					///< 合约代码
	tagCFFBuySellItem			Buy[5];						///< 买五档
	tagCFFBuySellItem			Sell[5];					///< 卖五档
} tagCFFFutureSnapBuySell_HF178;


#pragma pack()
#endif









