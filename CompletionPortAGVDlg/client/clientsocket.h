#pragma once

class CclientDlg;
class CClientSocket : public CObject
{
public:
	CClientSocket(CclientDlg *pClientDlg);
	virtual ~CClientSocket(void);

public:
	BOOL	ConnectServer(/*DWORD &dwServIP, short &sServPort*/);		//���ӷ�����
	BOOL	SendPulsePacket(char* buf);									//����������
	BOOL	SendTestE1(char* buf);

	SOCKET	m_s;				//�׽���

private:
	//char	m_buf[E1_BUFFSIZE];	//������
	CclientDlg *m_pClientDlg;	//������ָ��
private:
	CClientSocket(void);
};