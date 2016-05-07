#include "stdafx.h"
#include "clientsocket.h"
#include "clientDlg.h"

CClientSocket::CClientSocket(CclientDlg *pClientDlg)
{
	int		nRet;//返回值
	WSADATA	wsaData;
	//初始化套接字动态库	
	if ((nRet = WSAStartup(0x0202, &wsaData)) != 0)
	{
		return;
	}
	//创建套接字
	if ((m_s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		AfxMessageBox(_T("创建套接字错误。"));
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
* 连接服务器
*/
BOOL CClientSocket::ConnectServer(/*DWORD &dwServIP, short &sServPort*/)
{
	int nRet;//返回值

	//服务器地址
	SOCKADDR_IN	servAddr;
	servAddr.sin_family = AF_INET;
	//servAddr.sin_addr.S_un.S_addr = htonl(dwServIP);
	servAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servAddr.sin_port = htons(SERVERPORT);
	int nServLen = sizeof(servAddr);

	//连接服务器
	nRet = connect(m_s, (SOCKADDR*)&servAddr, nServLen);
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}
	//使Nagle算法无效
	const char chOpt = 1;
	nRet = setsockopt(m_s, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
	if (SOCKET_ERROR == nRet)
	{
		return FALSE;
	}
	return TRUE;
}


/*
* 发送心跳包
*/
BOOL CClientSocket::SendPulsePacket(char* buf)
{
	return SendTestE1(buf);
}


/*
* 发送一条E1
*/
BOOL CClientSocket::SendTestE1(char* buf)
{
	int nRet = send(m_s, buf, E1_BUFFSIZE, 0);
	if (SOCKET_ERROR == nRet)
	{
		//AfxMessageBox(_T("SendPacket数据失败。"));
		return FALSE;
	}
	return TRUE;
}