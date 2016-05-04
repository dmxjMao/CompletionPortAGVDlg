#include "stdafx.h"
//#include "commdef.h"
#include "MyServer.h"

#include "ClientContext.h"


CMyServer::CMyServer()
{

}

CMyServer::~CMyServer()
{

}

/*
* attach the main wnd
*/
void CMyServer::AttachWnd(CServerDlg* pMainDlg)
{
	m_pMainDlg = pMainDlg;
}

/*
* start server
*/
int CMyServer::StartServer()
{
	//init listen socket
	int nRet = Err_Buf;
	nRet = InitListenSocket();
	RET_IFERR(nRet);

	m_bRunning = TRUE;

	nRet = Err_Buf;
	nRet = InitListenEvent();
	RET_IFERR(nRet);

	//create io compport
	if (!m_IOCP.Create()) {
		WSACloseEvent(m_hEvent);
		closesocket(m_sListen);
		WSACleanup();
		return Err_CreateIOCP;
	}

	//create accept thread
	nRet = Err_Buf;
	nRet = CreateAcceptThread();
	RET_IFERR(nRet);

	nRet = Err_Buf;
	nRet = CreateServiceThread();
	RET_IFERR(nRet);

	return Err_Buf;
}


int CMyServer::InitListenSocket()
{
	//init socket library
	int	reVal = Err_Buf;
	WSADATA wsaData;
	reVal = WSAStartup(0x0202, &wsaData);
	RET_ERRCODE_IFTRUE(reVal, Err_WSAStartup);

	//create listen socket
	if ((m_sListen = WSASocket(AF_INET, SOCK_STREAM,
		0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
	{
		WSACleanup();
		return Err_WSASocket;
	}

	//bind socket
	SOCKADDR_IN	servAddr;//服务器地址
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(SERV_PORT);
	reVal = bind(m_sListen, (SOCKADDR*)&servAddr, sizeof(servAddr));
	RET_ERRCODE_IFSOCKETERROR(reVal, Err_BindSocket);

	//listen
	reVal = listen(m_sListen, SOMAXCONN);
	RET_ERRCODE_IFSOCKETERROR(reVal, Err_ListenSocket);

	return Err_Buf;
}


int CMyServer::InitListenEvent()
{
	m_hEvent = WSACreateEvent();
	if (m_hEvent == WSA_INVALID_EVENT)
	{
		closesocket(m_sListen);
		WSACleanup();
		return Err_WSACreateEvent;
	}

	//为监听套接字注册FD_ACCEPT网络事件
	int nRet = WSAEventSelect(m_sListen,
		m_hEvent,
		FD_ACCEPT);
	RET_ERRCODE_IFSOCKETERROR(nRet, Err_WSAEventSelect);

	return Err_Buf;
}


int CMyServer::CreateAcceptThread()
{
	unsigned threadID;
	//DWORD threadID;
	m_hThread[0] = (HANDLE)_beginthreadex(
		NULL, 0, AcceptThread, this, 0, &threadID);
	//m_hThread[0] = CreateThread(NULL, 0, AcceptThread, this, 0, &threadID);
	if (NULL == m_hThread[0])
	{
		WSACloseEvent(m_hEvent);
		closesocket(m_sListen);
		WSACleanup();
		return Err_CreateAcceptThread;
	}
	m_nThreadNum = 1;

	return Err_Buf;
}


int CMyServer::CreateServiceThread()
{
	SYSTEM_INFO SystemInfo;
	GetSystemInfo(&SystemInfo);

	// 创建服务线程
	unsigned threadID;
	for (DWORD i = 0; i < SystemInfo.dwNumberOfProcessors * 2; ++i)
	{
		if ((m_hThread[m_nThreadNum++] = (HANDLE)_beginthreadex(NULL,
			0, ServiceThread, this, 0, &threadID)) == NULL)
			//if((m_hThread[m_nThreadNum++] = CreateThread(NULL,
			//	0,ServiceThread, this,0,&threadID)) == NULL)
		{
			WSACloseEvent(m_hEvent);
			closesocket(m_sListen);
			WSACleanup();
			return Err_CreateServiceThread;
		}
	}

	return Err_Buf;
}



unsigned WINAPI CMyServer::AcceptThread(PVOID pParam)
{
	CMyServer*	pServ = (CMyServer*)pParam;
	CIOCP&	iOCP = pServ->m_IOCP;
	SOCKET		sListen = pServ->m_sListen;		//监听套接字
	SOCKET		sAccept = INVALID_SOCKET;		//接受套接字
	HANDLE		hEvent = pServ->m_hEvent;
	BOOL		bRunning = pServ->m_bRunning;
	CClientManager& clientMgr = pServ->m_clientMgr;

	while (bRunning)
	{
		DWORD dwRet;
		/*
		只要指定事件对象中的一个或全部处于有信号状态，或者超时间隔到，则返回
		int nRet = WSAEventSelect(m_sListen,  m_hEvent, ...
		*/
		dwRet = WSAWaitForMultipleEvents(1,			//等待网络事件
			&hEvent,
			FALSE,  // wait all
			100,    // 100ms
			FALSE); // fAlertable 
		if (!bRunning)					//服务器停止服务
			break;

		if (dwRet == WSA_WAIT_TIMEOUT)				//函数调用超时
			continue;

		WSANETWORKEVENTS events;					//查看发生的网络事件
		int nRet = WSAEnumNetworkEvents(sListen,
			hEvent,//事件对象会重置
			&events);
		if (nRet == SOCKET_ERROR)
		{
			AfxMessageBox(_T("WSAEnumNetworkEvents函数错误"));
			break;
		}

		if (events.lNetworkEvents & FD_ACCEPT)		//发生FD_ACCEPT网络事件
		{
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0 && bRunning)
			{
				//接受客户端请求
				SOCKADDR_IN servAddr;
				int	serAddrLen = sizeof(servAddr);
				if ((sAccept = WSAAccept(sListen,
					(SOCKADDR*)&servAddr,
					&serAddrLen,
					NULL,
					0)) == SOCKET_ERROR)
				{
					AfxMessageBox(_T("WSAAccept函数错误"));
					break;
				}

				//创建客户端节点（client是不同的，需要自己写，这部分不知道如何抽象）
				CClientContext *pClient = new CClientContext(sAccept, pServ->m_pMainDlg);

				/*GetQueuedCompletionStatus(hComPort, &dwIoSize,
				(LPDWORD)&pClient, 我们关联了socket和iocp，所以当有io完成时，该函数会填充正确的完成键
				&lpOverlapped,  INFINITE);
				*/
				if (!iOCP.AssociateSocket(sAccept, (ULONG_PTR)pClient)) {
					AfxMessageBox(_T("套接字与完成端口关联错误"));
					return -1;
				}
				//if (CreateIoCompletionPort((HANDLE)sAccept,	//套接字与完成端口关联起来
				//	hComPort,
				//	(DWORD)pClient,//完成键
				//	0) == NULL)
				//{
				//	return -1;
				//}

				//加入管理客户端链表
				clientMgr.AddClient(pClient);
				
				//CClientManager *pClientMgr = CClientManager::GetClientManager();
				//pClientMgr->AddClient(pClient);

				if (!pClient->AsyncRecvE1())				//异步接收E1数据，去服务线程处理
				{
					clientMgr.DeleteClient(pClient);		//连接中断

				}
			}
		}
	}

	//释放资源
	clientMgr.DeleteAllClient();
	//CClientManager *pClientMgr = CClientManager::GetClientManager();
	//pClientMgr->DeleteAllClient();
	//pClientMgr->ReleaseManager();
	//_endthreadex(0);

	return 0;
}


unsigned WINAPI CMyServer::ServiceThread(PVOID pParam)
{
	CMyServer* pServ = (CMyServer*)pParam;
	CIOCP&	iOCP = pServ->m_IOCP;
	CClientManager& clientMgr = pServ->m_clientMgr;

	DWORD			dwIoSize;		//传输字节数
	CClientContext	*pClient;		//客户端指针
	LPOVERLAPPED	lpOverlapped;	//重叠结构指针
	BOOL			bExit = FALSE;	//服务线程退出
	BOOL			bRunning = pServ->m_bRunning;

	while (!bExit)
	{
		dwIoSize = -1;
		lpOverlapped = NULL;
		pClient = NULL;
		//等待I/O操作结果
		BOOL bIORet = iOCP.GetStatus((PULONG_PTR)&pClient, &dwIoSize, &lpOverlapped);
		//BOOL bIORet = GetQueuedCompletionStatus(,
		//	&dwIoSize,
		//	(LPDWORD)&pClient,
		//	&lpOverlapped,
		//	INFINITE);
		/*
		msdn:  bIORet 不为0，bytes，compkey，overlap都存有相关信息
		bIORet 不为0，三个全0，服务器退出
		*/
		if (bIORet) {
			if (!bRunning)
				bExit = TRUE;
			else
				clientMgr.ProcessIO(pClient, lpOverlapped, dwIoSize);
		}
		if (FALSE == bIORet) 
		{
			if (nullptr == lpOverlapped)
				continue;
			DWORD dwErr = GetLastError();

		}
		//if (FALSE == bIORet && NULL != pClient)
		//{
		//	//客户端断开
		//	//CClientManager *pClientMgr = CClientManager::GetClientManager();
		//	//pClientMgr->DeleteClient(pClient);
		//	//pClientMgr->UpdateOnDisconn();
		//}

		//if (bIORet && dwIoSize == 0)
		//{
		//	//强制关闭了客户端或服务器
		//	//CClientManager *pClientMgr = CClientManager::GetClientManager();
		//	//pClientMgr->DeleteClient(pClient);
		//	//pClientMgr->UpdateOnDisconn();
		//	continue;
		//}

		////成功的操作完成
		//if (bIORet && lpOverlapped && pClient)
		//{
		//	//CClientManager *pClientMgr = CClientManager::GetClientManager();
		//	//pClientMgr->ProcessIO(pClient, lpOverlapped, dwIoSize);
		//}
		////服务器退出
		//if (pClient == NULL&& lpOverlapped == NULL && !pServerView->m_bRunning)
		//{
		//	bExit = TRUE;
		//	//_endthreadex(0);
		//}
	}

	//CClientManager *pClientMgr = CClientManager::GetClientManager();
	//pClientMgr->ReleaseManager();

	return 0;
}