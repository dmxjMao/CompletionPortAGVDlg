
// serverDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"

#include <map>
#include <vector>
#include "MyServer.h"
#include "TaskSend.h"


// CServerDlg �Ի���
class CServerDlg : public CDialogEx
{
// ����
public:
	CServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SERVER_DIALOG };
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
public:
//	CStatic m_stsWatcher;
	CRect	m_rcWatcher;
	CRect	m_rcDlg;
	CRect	m_rcTarget;

	CMyServer		m_server;

	std::map<int, CPoint> m_mapPoint;				//map point no & xy
	unsigned		m_uCurNum = 0;					//cur point no

public:
	//int StartServer();

private:
	void ShowTheMap(int nID);
	void GetMapXYFromFile();

	//���ܲ���
	struct BIGDATA {
		BYTE agvno = 0;
		UINT16 target = 0;
	};
	std::vector<BIGDATA> m_vecBigdata;				//��������

	void SendOneTask(int carno, int target);
	void SendHugeTask();
	//int InitListenSocket();
	//int InitListenEvent();

public:
	//afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnClickedButtonExportxy();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnClickedButtonDoone();
	afx_msg void OnClickedButtonDooneDohuge();
};
