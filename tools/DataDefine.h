//
//  DataDefine.h
//  Prepare_Data
//
//  Created by 梁田 on 15/11/13.
//  Copyright © 2015年 梁田. All rights reserved.
//

#ifndef data_define_h
#define data_define_h

struct st_XDRID{
    unsigned long long high;
    unsigned long long low;
};

union IPV6{
    unsigned char addr[16];
    
};

union IPV4{
    unsigned int addr1;
    unsigned char addr2[4];
};

union MME_ADDR {
    IPV4        ipv4;
    IPV6        ipv6;
};

union ENB_ADDR {
    IPV4        ipv4;
    IPV6        ipv6;
};

union SGW_ADDR {
    IPV4        ipv4;
    IPV6        ipv6;
};

enum CoveringProperties
{
    USER_INDOOR = 0,     //室内用户
    USER_OUTDOOR = 1     //室外用户
};

//小区覆盖子属性
enum CoveringChildProperties
{
    INDOOR_HOME = 0,            //占用室分的室内稳定用户（有室分）
    INDOOR_OUT = 1,             //占用宏站的室内稳定用户（无室分）
    INDOOR_HOME_OUT = 2,        //室内稳定用户（占用宏站和室分）
    OUTDOOR_SLOW = 3,           //室外低速用户（速度在20公里/小时）
    OUTDOOR_MID = 4,            //室外中速用户（20~60公里/小时）
    OUTDOOR_QUICK = 5           //室外高速用户（20~60公里/小时）
};

//小区场景属性
enum ScenceProperties
{
    UNKNOWN_ECI = 0,            //未知类型
    SCHOOL_ECI = 1,             //校园小区
    LIVING_ECI = 2,             //居民楼小区
    BUSSINESS_ECI = 3,          //商业区
    SCENIC_ECI = 4,             //风景区
    SUBWAY_ECI = 5,             //地铁
    HIGHSPEED_ECI = 6           //高铁
};

//小区位置属性
enum CellSiteProperties {
    CELL_INDOOR  = 0,                //室内小区
    CELL_OUTDOOR = 1,                //室外小区
    
} ;

struct st_PUBLICPART {
    unsigned short          nLength;
    unsigned short          nCity;
    unsigned char           cInterface;
    st_XDRID                XdrId;
    unsigned char           cRat;
    unsigned long long      llImsi;
    unsigned long long      llImei;
    unsigned char           sMsisdn[20];
};

struct st_MME_PART{
    unsigned char           cProcedureType;
    unsigned long long      llMMEStartTime;
    unsigned long long      llMMEEndTime;
    unsigned char           cProcedureStatus;
    unsigned short          nRequestCause;
    unsigned short          nFailureCause;
    unsigned char           cKeyword_1;
    unsigned char           cKeyword_2;
    unsigned char           cKeyword_3;
    unsigned char           cKeyword_4;
    unsigned int            nMmeUES1apId;
    unsigned short          nOldMmeGroupId;
    unsigned char           cOldMmeCode;
    unsigned int            nOldMtmsi;
    unsigned short          nMmeGroupId;
    unsigned char           cMmeCode;
    unsigned int            nMtmsi;
    unsigned int            nTmsi;
    IPV4                    eIpv4;
    IPV6                    eIpv6;
    MME_ADDR                eMMEAddr;
    ENB_ADDR                eENBAddr;
    unsigned short          nMMEPort;
    unsigned short          nENBPort;
    unsigned short          nMMETAC;
    unsigned int            nMMECellID;
    unsigned short          nOtherTAC;
    unsigned int            nOtherCellid;
    unsigned char           sMMEAPN[32];
    
    unsigned char           cEPSBearerNum;
    unsigned char           cBearer1ID;
    unsigned char           cBearer1Type;
    unsigned char           cBearer1QCI;
    unsigned char           cBearer1Status;
    unsigned int            nBearer1ENBTeid;
    unsigned int            nBearer1SGWTeid;
    unsigned char           cBearer2ID;
    unsigned char           cBearer2Type;
    unsigned char           cBearer2QCI;
    unsigned char           cBearer2Status;
    unsigned int            nBearer2ENBTeid;
    unsigned int            nBearer2SGWTeid;
    unsigned char           cBearer3ID;
    unsigned char           cBearer3Type;
    unsigned char           cBearer3QCI;
    unsigned char           cBearer3Status;
    unsigned int            nBearer3ENBTeid;
    unsigned int            nBearer3SGWTeid;
    
};


struct st_HTTP_PART {
    unsigned char           cMachineIDAddType;
    SGW_ADDR                eSgwAddr;
    ENB_ADDR                eEnbAddr;
    unsigned short          nSgwPort;
    unsigned short          nEnbPort;
    unsigned int            nEnbTeid;
    unsigned int            nSgwTeid;
    unsigned short          nHTTPTAC;
    unsigned int            nHTTPCellID;
    unsigned char           sHTTPAPN[32];
    unsigned char           cAppTypeCode;
    unsigned long long      llHTTPStartTime;
    unsigned long long      llHTTPEndTime;
    unsigned short          nProtocalType;
    unsigned short          nAppType;
    unsigned short          nAppSubType;
    unsigned char           cAppContent;
    unsigned char           cAppStatus;
    IPV4                    eUserIPv4;
    IPV6                    eUserIPv6;
    unsigned short          nUserPort;
    unsigned char           cL4Protocal;
    IPV4                    eServerIPv4;
    IPV6                    eServerIPv6;
    unsigned short          nAppServerPort;
    unsigned int            nULFlowByte;
    unsigned int            nDLFlowByte;
    unsigned int            nULIPPackets;
    unsigned int            nDLIPPackets;
    unsigned int            nTcpULDisorderedPackets;
    unsigned int            nTcpDLDisorderedPackets;
    unsigned int            nTcpULRetranPackets;
    unsigned int            nTcpDLRetranPackets;
    unsigned int            nTCpHandResDelay;
    unsigned int            nTCpHandAckDelay;
    unsigned int            nULIPFragPackets;
    unsigned int            nDLIPFragPackets;
    unsigned int            nFirReqHandAckDelay;
    unsigned int            nFirResReqDelay;
    unsigned int            nWindowSize;
    unsigned int            nMSS;
    unsigned char           cTcpSetupTrys;
    unsigned char           cTcpLinkStatus;
    unsigned char           cSessionOver;
    unsigned char           CVersion;
    unsigned short          nActionType;
    unsigned short          nActionState;
    unsigned int            nFirstResDelay;
    unsigned int            nLastResDelay;
    unsigned int            nLastACKDelay;
    /*字段太长，暂时不使用*/
//    unsigned char           sHOST[64];
//    unsigned char           sURL[512];
//    unsigned char           sXOnlineHost[128];
//    unsigned char           sUserAgent[256];
//    unsigned char           sHTTPContentType[128];
//    unsigned char           sReferURI[128];
//    unsigned char           sCookie[256];
//    unsigned int            nContentLength;
//    unsigned char           cUsrAction;
//    unsigned char           cWtpFailType;
//    unsigned char           cWtpFailReason;
//    unsigned char           sTitle[256];
//    unsigned char           sKeyword[256];
    unsigned char           cServiceAction;
    unsigned char           cSuccess;
    unsigned int            nServiceDelay;
    unsigned char           cBrowseTool;
    unsigned char           cWebsite;
    
    //切分http之后新加入的字段
    unsigned long long      llStartTimeNew;
    unsigned long long      llEndTimeNew;
    float                   fRATE;
    unsigned int            nCellIDNew;
    unsigned int            nULFlowByteNew;
    unsigned int            nDLFlowByteNew;
    unsigned int            nFirstResDelayNew;
    unsigned int            nLastResDelayNew;
    unsigned int            nLastACKDelayNew;

};

struct st_ADDITIONAL_PART  {
    unsigned char           cVendorId;              //供应商标识 1-中兴 2-华为 3-爱立信
    unsigned long long      nStartTime;             //xdr中的startTime
    unsigned long long      nPositionStartTime;     //定位是的时候需要的一个起始时间，和nStartTime相同，但是当用户切换的时候会不同（定位的时候用）
    unsigned long long      nTimeGroup;             //分组时间,用于把每条的xdr分到二分钟的时间组内，表示的是分组的首时间
    double                  dOldLongitude;          //小区经度
    double                  dOldLatitude;           //小区维度
    double                  dOtherLongitude;        //上一个小区的经度
    double                  dOtherLatitude;         //上一个小区的维度
    unsigned char           cRevise;                //经纬度是否经过了转换（当有临区经纬度时需要转换）
    double                  dEciper;                //占比
    unsigned int            nTimeDelay;             //用于计算经纬度用的时延
    unsigned int            nTimeLength;            //本条xdr的时长（nextxdr-xdr)
    double                  dLongitude;             //计算后的经度
    double                  dLatitude;              //计算后的维度
    unsigned int            nGroupID;              //组号
    unsigned int            nTimespan;              //每条xdr相对于本组的首条xdr的时长
    float                   fLengthspan;            //每条xdr相对于本组首条xdr的经纬度距离
    float                   fSpliteLength;          //每条xdr相对于上一条xdr的经纬度距离
    unsigned char           sGridID[20];            //栅格ID
    
    unsigned char           cCoveringProperties;            //覆盖属性
    unsigned char           cCoveringChildProperties;       //覆盖子属性
    unsigned char           cScenceProperties;              //场景属性
    unsigned int              nSpeed;                         //速度
    unsigned char           cCellScenceProperties;          //服务小区场景属性
    unsigned char           cCellIndoorProperties;          //服务小区室分宏站属性
    
    
};

struct st_XDR_INFO {
    
    st_PUBLICPART           public_part;                    //XDR公共部分
    
    union PRIVATE_PART{
        st_MME_PART         mme_part;
        st_HTTP_PART        http_part;
    }private_part;
    
    st_ADDITIONAL_PART      additional_part;                //额外增加部分
    
};



#endif /* data_define_h */
