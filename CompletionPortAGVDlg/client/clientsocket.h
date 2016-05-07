#pragma once

class CclientDlg;
class CClientSocket : public CObject
{
public:
	CClientSocket(CclientDlg *pClientDlg);
	virtual ~CClientSocket(void);

public:
	BOOL	ConnectServer(/*DWORD &dwServIP, short &sServPort*/);		//连接服务器
	BOOL	SendPulsePacket(char* buf);									//发送心跳包
	BOOL	SendTestE1(char* buf);

	SOCKET	m_s;				//套接字

private:
	//char	m_buf[E1_BUFFSIZE];	//缓冲区
	CclientDlg *m_pClientDlg;	//主窗口指针
private:
	CClientSocket(void);
};