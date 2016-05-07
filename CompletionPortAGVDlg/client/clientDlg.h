
// clientDlg.h : 头文件
//

#pragma once
#include "afxcmn.h"
#include <vector>

class CClientSocket;
// CclientDlg 对话框
class CclientDlg : public CDialogEx
{
// 构造
public:
	CclientDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CclientDlg();

	enum state
	{
		UNKNOWN = 0,
		FREE,
		ASSIGNMENTED,
		LINKED,
		CANCELED
	};

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	static	unsigned WINAPI ReceiveServer(void *pParam);//接受服务器消息

public:
	BOOL Init();
	BOOL ConnectServer();

public:
	CClientSocket	*m_pClientSocket = nullptr;	//通讯类指针
	state			m_state = UNKNOWN;			//用户状态
	BOOL			m_bPulsePacket = FALSE;		//发送心跳包
	BYTE			m_carno = 0;			//小车号
	BOOL			m_bConn = FALSE;
	
	BYTE				m_m6tag = 0;		//最近一条m6消息标签
	BYTE				m_m2tag = 0;		//最近一条m2消息标签
	BYTE				m_m1tag = 0;		//最近一条m1消息标签

	UINT16				m_curPt = 0;		//小车当前点
	UINT16				m_curTaskno = 0;	//缓存任务号

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
