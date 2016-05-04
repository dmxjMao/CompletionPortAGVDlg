#pragma once

#include "IoCompletionPort.h"
#include "ClientManager.h"

class CServerDlg;
class CMyServer
{

public:
	CMyServer();
	~CMyServer();
	int StartServer();
	void AttachWnd(CServerDlg* pMainDlg);

public:
	SOCKET			m_sListen = INVALID_SOCKET;		//监听套接字
	HANDLE			m_hEvent = INVALID_HANDLE_VALUE;//监听事件句柄
	CIOCP			m_IOCP;							//IoCompPort
	CClientManager	m_clientMgr;					//ClientManager
	HANDLE			m_hThread[9];					//子线程=监听线程+服务线程
	DWORD			m_nThreadNum = 0;				//实际线程数量
	BOOL			m_bRunning = FALSE;				//服务器运行状态
	CServerDlg*		m_pMainDlg;						//服务器依附的窗口

private:
	int InitListenSocket();
	int InitListenEvent();
	int CreateAcceptThread();
	int CreateServiceThread();

	static	unsigned WINAPI AcceptThread(void *pParam);//接受客户端请求
	static	unsigned WINAPI ServiceThread(void *pParam);//服务线程
};