#include "demo.h"

CKdmRtspHandler *g_pKdmRtspHandler = NULL;

int Init()
{
	int nRet = OspInit(true, 60000);
	if(nRet != true)
	{
		printf("osp init error\n");
		return -1;
	}
	KdmPosaStartup();

	g_pKdmRtspHandler = new CKdmRtspHandler;
	if(NULL == g_pKdmRtspHandler)
	{
		printf("init error, g_pKdmRtspHandler = NULL\n");
		return -1;
	}
	return 0;
}

u32 RtspMsgCallBack (void* dwContext, u32 dwRetCode, u32 dwOperateType, string& strResponse)
{
	printf("rtsp msg cb here ...\n");
	return 0;
}


u32 RtpDataCallBack (void*  dwContext, u32 dwChnID,  u8*  pRtpData, u32 dwLenRtp)
{
	printf("rtsp data cb here ... \n");
	return 0;
}

void Uninit()
{
	KdmPosaCleanup();
	delete g_pKdmRtspHandler;
}

int main()
{
	int nRet = Init();
	if(nRet != 0)
	{
		printf("Init error\n");
		return -1;
	}

	TKdmRtspUrl tKdmRtspUrl;
	memset(&tKdmRtspUrl, 0, sizeof(tKdmRtspUrl));
	tKdmRtspUrl.strRtspUrl = "rtsp://admin:admin123@172.16.248.141:554";
	tKdmRtspUrl.bStreamUsingTCP = false;
	tKdmRtspUrl.dwRtpPort = 9758;
	tKdmRtspUrl.strUserName = "admin";
	tKdmRtspUrl.strPassword = "admin123";
	tKdmRtspUrl.dwContext = (void*)malloc(sizeof(char) * 1024);
	if(tKdmRtspUrl.dwContext == NULL)
	{
		printf("malloc error\n");
		Uninit();
		return -1;
	}
	tKdmRtspUrl.bForceMulticast = false;
	tKdmRtspUrl.funRtspMessage = RtspMsgCallBack;
	tKdmRtspUrl.funRtpData = RtpDataCallBack;
	tKdmRtspUrl.m_tKdmRtspConnectModeType = KDMRTSP_CONNECT_MODE_TYPE_SECOND;
	//设置rtsp会话需要的参数
	nRet = g_pKdmRtspHandler->SetRtspUrl(&tKdmRtspUrl);
	if(nRet != TRUE)
	{
		printf("set url fail, line:%d, nRet = %d\n", __LINE__, nRet);
		free(tKdmRtspUrl.dwContext);
		tKdmRtspUrl.dwContext = NULL;
		Uninit();
		return -1;
	}

	//设置其他rtsp基本会话参数
	TKdmRtspOtherParam tRtspOtherParam;
	memset(&tRtspOtherParam, 0,sizeof(tRtspOtherParam));
	tRtspOtherParam.m_bUseStopPlay = false;
	nRet = g_pKdmRtspHandler->SetRtspOtherParam(&tRtspOtherParam);
	if(nRet != true)
	{
		printf("rtst set other param error, line=%d, nRet = %d\n", __LINE__, nRet);
		free(tKdmRtspUrl.dwContext);
		tKdmRtspUrl.dwContext = NULL;
		Uninit();
		return -1;
	}

	nRet = g_pKdmRtspHandler->OpenConnect();
	if(nRet != true)
	{
		printf("open connect error, line=%d, nRet = %d\n", __LINE__, nRet);
		Uninit();
		return -1;
	}

	//获取能力集
	nRet = g_pKdmRtspHandler->SendOptionsRequest();
	if(nRet != true)
	{
		printf("send option request error, line=%d, nRet = %d\n", __LINE__, nRet);
		Uninit();
		return -1;
	}
	//发送描述信息
	nRet = g_pKdmRtspHandler->SendDescribeRequest();
	if(nRet != true)
	{
		printf("send describe request error, line=%d, nRet = %d\n", __LINE__, nRet);
		Uninit();
		return -1;
	}
	nRet = g_pKdmRtspHandler->SendDescribeRequest();
	if(nRet != true)
	{
		printf("send describe request error, line=%d, nRet = %d\n", __LINE__, nRet);
		Uninit();
		return -1;
	}
	//TKdmMediaSubSession tMediaSubSessInfo;
	//memset(&tMediaSubSessInfo, 0, sizeof(tMediaSubSessInfo));
	//nRet = g_pKdmRtspHandler->GetMediaSubSessionInfo(0, tMediaSubSessInfo);
	//if(nRet != true)
	//{
	//	printf("get submediasess info error, line=%d, nRet = %d\n", __LINE__, nRet);
	//	Uninit();
	//	return -1;
	//}
	bool streamUsingTcp = true;
	bool streamOutgoing = false;
	bool forceMulticastOnUnspecified = false;
	int nIdx = 0xFF;
	nRet = g_pKdmRtspHandler->SendSetupRequest(streamUsingTcp, streamOutgoing, forceMulticastOnUnspecified, nIdx);
	if(nRet != true)
	{
		printf("send setup request error, line=%d, nRet = %d\n", __LINE__, nRet);
		Uninit();
		return -1;
	}

	nRet = g_pKdmRtspHandler->SendPlayRequest();
	if(nRet != 0)
	{
		printf("start play fail, line:%d\n", __LINE__);
		Uninit();
		return -1;
	}

	delete g_pKdmRtspHandler;
	return 0;
}