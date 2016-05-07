
// clientDlg.h : ͷ�ļ�
//

#pragma once
#include "afxcmn.h"
#include <vector>

class CClientSocket;
// CclientDlg �Ի���
class CclientDlg : public CDialogEx
{
// ����
public:
	CclientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CclientDlg();

	enum state
	{
		UNKNOWN = 0,
		FREE,
		ASSIGNMENTED,
		LINKED,
		CANCELED
	};

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	static	unsigned WINAPI ReceiveServer(void *pParam);//���ܷ�������Ϣ

public:
	BOOL Init();
	BOOL ConnectServer();

public:
	CClientSocket	*m_pClientSocket = nullptr;	//ͨѶ��ָ��
	state			m_state = UNKNOWN;			//�û�״̬
	BOOL			m_bPulsePacket = FALSE;		//����������
	BYTE			m_carno = 0;			//С����
	BOOL			m_bConn = FALSE;
	
	BYTE				m_m6tag = 0;		//���һ��m6��Ϣ��ǩ
	BYTE				m_m2tag = 0;		//���һ��m2��Ϣ��ǩ
	BYTE				m_m1tag = 0;		//���һ��m1��Ϣ��ǩ

	UINT16				m_curPt = 0;		//С����ǰ��
	UINT16				m_curTaskno = 0;	//���������

public:
	CListCtrl m_ctlsend;
	CListCtrl m_ctlrecv;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void ConstructVecE1(char* buf);


private:
	void ConstructTestE1(char* buf);
	void ConstructTestE1(char* buf, int curDist, int curPt);
	void setVecE1();
	void ConstructVecE1(std::vector<int>& vecRoute, std::vector<char*>& vec);
};
