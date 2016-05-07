#include "stdafx.h"
#include "clientsocket.h"
#include "clientDlg.h"

CClientSocket::CClientSocket(CclientDlg *pClientDlg)
{
	int		nRet;//����ֵ
	WSADATA	wsaData;
	//��ʼ���׽��ֶ�̬��	
	if ((nRet = WSAStartup(0x0202, &wsaData)) != 0)
	{
		return;
	}
	//�����׽���
	if ((m_s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		AfxMessageBox(_T("�����׽��ִ���"));
		WSACleanup();
	}
	m_pClientDlg = pClientDlg;
}


CClientSocket::~CClientSocket()
{
	closesocket(m_s);
	WSACleanup();
}


/*
* ���ӷ�����
*/
BOOL CClientSocket::ConnectServer(/*DWORD &dwServIP, short &sServPort*/)
{
	int nRet;//����ֵ

	//��������ַ
	SOCKADDR_IN	servAddr;
	servAddr.sin_family = AF_INET;
	//servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(SERVERPORT);
	int nServLen = sizeof(servAddr);

	//���ӷ�����
	nRet = connect(m_s, (SOCKADDR*)&servAddr, nServLen);
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}
	//ʹNagle�㷨��Ч
	const char chOpt = 1;
	nRet = setsockopt(m_s, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}
	return TRUE;
}


/*
* ����������
*/
BOOL CClientSocket::SendPulsePacket(char* buf)
{
	return SendTestE1(buf);
}


/*
* ����һ��E1
*/
BOOL CClientSocket::SendTestE1(char* buf)
{
	int nRet = send(m_s, buf, E1_BUFFSIZE, 0);
	if (SOCKET_ERROR == nRet)
	{
		//AfxMessageBox(_T("SendPacket����ʧ�ܡ�"));
		return FALSE;
	}
	return TRUE;
}