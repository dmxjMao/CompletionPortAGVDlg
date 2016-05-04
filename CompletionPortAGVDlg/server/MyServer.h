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
	SOCKET			m_sListen = INVALID_SOCKET;		//�����׽���
	HANDLE			m_hEvent = INVALID_HANDLE_VALUE;//�����¼����
	CIOCP			m_IOCP;							//IoCompPort
	CClientManager	m_clientMgr;					//ClientManager
	HANDLE			m_hThread[9];					//���߳�=�����߳�+�����߳�
	DWORD			m_nThreadNum = 0;				//ʵ���߳�����
	BOOL			m_bRunning = FALSE;				//����������״̬
	CServerDlg*		m_pMainDlg;						//�����������Ĵ���

private:
	int InitListenSocket();
	int InitListenEvent();
	int CreateAcceptThread();
	int CreateServiceThread();

	static	unsigned WINAPI AcceptThread(void *pParam);//���ܿͻ�������
	static	unsigned WINAPI ServiceThread(void *pParam);//�����߳�
};