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
	SOCKADDR_IN	servAddr;//��������ַ
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

	//Ϊ�����׽���ע��FD_ACCEPT�����¼�
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

	// ���������߳�
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
	SOCKET		sListen = pServ->m_sListen;		//�����׽���
	SOCKET		sAccept = INVALID_SOCKET;		//�����׽���
	HANDLE		hEvent = pServ->m_hEvent;
	BOOL		bRunning = pServ->m_bRunning;
	CClientManager& clientMgr = pServ->m_clientMgr;

	while (bRunning)
	{
		DWORD dwRet;
		/*
		ֻҪָ���¼������е�һ����ȫ���������ź�״̬�����߳�ʱ��������򷵻�
		int nRet = WSAEventSelect(m_sListen,  m_hEvent, ...
		*/
		dwRet = WSAWaitForMultipleEvents(1,			//�ȴ������¼�
			&hEvent,
			FALSE,  // wait all
			100,    // 100ms
			FALSE); // fAlertable 
		if (!bRunning)					//������ֹͣ����
			break;

		if (dwRet == WSA_WAIT_TIMEOUT)				//�������ó�ʱ
			continue;

		WSANETWORKEVENTS events;					//�鿴�����������¼�
		int nRet = WSAEnumNetworkEvents(sListen,
			hEvent,//�¼����������
			&events);
		if (nRet == SOCKET_ERROR)
		{
			AfxMessageBox(_T("WSAEnumNetworkEvents��������"));
			break;
		}

		if (events.lNetworkEvents & FD_ACCEPT)		//����FD_ACCEPT�����¼�
		{
			if (events.iErrorCode[FD_ACCEPT_BIT] == 0 && bRunning)
			{
				//���ܿͻ�������
				SOCKADDR_IN servAddr;
				int	serAddrLen = sizeof(servAddr);
				if ((sAccept = WSAAccept(sListen,
					(SOCKADDR*)&servAddr,
					&serAddrLen,
					NULL,
					0)) == SOCKET_ERROR)
				{
					AfxMessageBox(_T("WSAAccept��������"));
					break;
				}

				//�����ͻ��˽ڵ㣨client�ǲ�ͬ�ģ���Ҫ�Լ�д���ⲿ�ֲ�֪����γ���
				CClientContext *pClient = new CClientContext(sAccept, pServ->m_pMainDlg);

				/*GetQueuedCompletionStatus(hComPort, &dwIoSize,
				(LPDWORD)&pClient, ���ǹ�����socket��iocp�����Ե���io���ʱ���ú����������ȷ����ɼ�
				&lpOverlapped,  INFINITE);
				*/
				if (!iOCP.AssociateSocket(sAccept, (ULONG_PTR)pClient)) {
					AfxMessageBox(_T("�׽�������ɶ˿ڹ�������"));
					return -1;
				}
				//if (CreateIoCompletionPort((HANDLE)sAccept,	//�׽�������ɶ˿ڹ�������
				//	hComPort,
				//	(DWORD)pClient,//��ɼ�
				//	0) == NULL)
				//{
				//	return -1;
				//}

				//�������ͻ�������
				clientMgr.AddClient(pClient);
				
				//CClientManager *pClientMgr = CClientManager::GetClientManager();
				//pClientMgr->AddClient(pClient);

				if (!pClient->AsyncRecvE1())				//�첽����E1���ݣ�ȥ�����̴߳���
				{
					clientMgr.DeleteClient(pClient);		//�����ж�

				}
			}
		}
	}

	//�ͷ���Դ
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

	DWORD			dwIoSize;		//�����ֽ���
	CClientContext	*pClient;		//�ͻ���ָ��
	LPOVERLAPPED	lpOverlapped;	//�ص��ṹָ��
	BOOL			bExit = FALSE;	//�����߳��˳�
	BOOL			bRunning = pServ->m_bRunning;

	while (!bExit)
	{
		dwIoSize = -1;
		lpOverlapped = NULL;
		pClient = NULL;
		//�ȴ�I/O�������
		BOOL bIORet = iOCP.GetStatus((PULONG_PTR)&pClient, &dwIoSize, &lpOverlapped);
		//BOOL bIORet = GetQueuedCompletionStatus(,
		//	&dwIoSize,
		//	(LPDWORD)&pClient,
		//	&lpOverlapped,
		//	INFINITE);
		/*
		msdn:  bIORet ��Ϊ0��bytes��compkey��overlap�����������Ϣ
		bIORet ��Ϊ0������ȫ0���������˳�
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
		//	//�ͻ��˶Ͽ�
		//	//CClientManager *pClientMgr = CClientManager::GetClientManager();
		//	//pClientMgr->DeleteClient(pClient);
		//	//pClientMgr->UpdateOnDisconn();
		//}

		//if (bIORet && dwIoSize == 0)
		//{
		//	//ǿ�ƹر��˿ͻ��˻������
		//	//CClientManager *pClientMgr = CClientManager::GetClientManager();
		//	//pClientMgr->DeleteClient(pClient);
		//	//pClientMgr->UpdateOnDisconn();
		//	continue;
		//}

		////�ɹ��Ĳ������
		//if (bIORet && lpOverlapped && pClient)
		//{
		//	//CClientManager *pClientMgr = CClientManager::GetClientManager();
		//	//pClientMgr->ProcessIO(pClient, lpOverlapped, dwIoSize);
		//}
		////�������˳�
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