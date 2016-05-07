
// clientDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "client.h"
#include "clientDlg.h"
#include "afxdialogex.h"

#include "clientsocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define TIMER_INIT		1
#define TIMER_M6		2
#define	TIMER_M2		3
#define TIMER_M1		4

//#include <vector>
//std::vector<char*>	g_vecE1No1;
//std::vector<char*>  g_vecE1No2;
//std::vector<char*>	g_vecE1No3;
//std::vector<char*>	g_vecE1No4;
//std::vector<char*>	g_vecE1No5;
std::vector<char*>		g_vecE1;
int g_InitPt[11] = { 0, 1, 6, 35, 40, 52, 63, 75, 81, 96, 139 }; //小车初始位置

void CclientDlg::ConstructVecE1(std::vector<int>& vecRoute, std::vector<char*>& vec)
{
	char buf[E1_BUFFSIZE];
	size_t pt = 0;
	for (; pt < vecRoute.size(); ++pt) {
		for (int i = 0; i < 8; ++i) {
			ConstructTestE1(buf, i, vecRoute[pt]);
			char* p = new char[E1_BUFFSIZE];
			memcpy_s(p, E1_BUFFSIZE, buf, E1_BUFFSIZE);
			vec.push_back(p);
		}
	}

	ConstructTestE1(buf, 0, vecRoute[pt-1]);
	char* p = new char[E1_BUFFSIZE];
	memcpy_s(p, E1_BUFFSIZE, buf, E1_BUFFSIZE);
	vec.push_back(p);
}

void CclientDlg::setVecE1()
{
	extern BYTE g_AGVNo;

	//"1-2-3-4-26-24"  agv1;
	//"4-26-31-33-34"  agv2;
	//int i = 0;
	//char buf[E1_BUFFSIZE];
	std::vector<int> vecRoute;

	if (g_AGVNo == 1) {
		vecRoute.clear();
		int agv[] = { 1,2,3,4,26,24 };
		vecRoute.assign(agv, agv + sizeof(agv)/sizeof(agv[0]));
		//ConstructVecE1(vecRoute, g_vecE1No1);
		//for (; i < 8; ++i) {
		//	ConstructTestE1(buf, i, 1);
		//	char* p = new char[E1_BUFFSIZE];
		//	memcpy_s(p, E1_BUFFSIZE, buf, E1_BUFFSIZE);
		//	g_vecE1No1.push_back(p);
		//}
	}

	if (g_AGVNo == 2) {
		vecRoute.clear();
		int agv[] = { 31,33,34,35,36,37,39 };
		vecRoute.assign(agv, agv + sizeof(agv) / sizeof(agv[0]));
		//ConstructVecE1(vecRoute, g_vecE1No2);
	}

	if (g_AGVNo == 3) {
		vecRoute.clear();
		int agv[] = { 40,45,47,52,54,55,56 };
		vecRoute.assign(agv, agv + sizeof(agv) / sizeof(agv[0]));
		//ConstructVecE1(vecRoute, g_vecE1No3);
	}

	if (g_AGVNo == 4) {
		vecRoute.clear();
		int agv[] = { 50,49,48,47,52 };
		vecRoute.assign(agv, agv + sizeof(agv) / sizeof(agv[0]));
		//ConstructVecE1(vecRoute, g_vecE1No4);
	}

	if (g_AGVNo == 5) {
		vecRoute.clear();
		int agv[] = { 63,62,61,59,54,55 };
		vecRoute.assign(agv, agv + sizeof(agv) / sizeof(agv[0]));
		//ConstructVecE1(vecRoute, g_vecE1No5);
	}
}


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CclientDlg 对话框



CclientDlg::CclientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CLIENT_DIALOG, pParent)
{
	extern BYTE g_AGVNo;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_curPt = g_InitPt[g_AGVNo];
}

CclientDlg::~CclientDlg()
{
	delete m_pClientSocket;
	m_pClientSocket = nullptr;
}

void CclientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SEND, m_ctlsend);
	DDX_Control(pDX, IDC_LIST_RECV, m_ctlrecv);
}

BEGIN_MESSAGE_MAP(CclientDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CclientDlg 消息处理程序

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_pClientSocket = new CClientSocket(this);		//创建通讯类对象

	//setVecE1();

	//BOOL bConn = FALSE;
	while ( !m_bConn ) {
		if (ConnectServer()) {
			m_bConn = TRUE;
			// test : 发送一条E1
			//char buf[E1_BUFFSIZE];
			//ConstructTestE1(buf);
			//m_pClientSocket->SendTestE1(buf);

			break;
		}
	}

	SetTimer(TIMER_INIT, 1000, NULL);	//心跳时间 1s
	m_bPulsePacket = TRUE;		//发送心跳包

	//char buf[E1_BUFFSIZE];
	//ConstructTestE1(buf);
	//for (int i = 0; i < 3; ++i) {	
	//	m_pClientSocket->SendPulsePacket(buf);
	//}

	//创建接受服务器消息线程
	unsigned threadID;
	HANDLE hthread = (HANDLE)_beginthreadex(
		NULL, 0, ReceiveServer, this, 0, &threadID);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CclientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*
* 连接服务器
*/
BOOL CclientDlg::ConnectServer()
{
	//连接服务器
	if (FALSE == m_pClientSocket->ConnectServer())
	{
		//AfxMessageBox(_T("连接服务器失败！"));
		return FALSE;
	}

	////发送考生学号
	//HDR hdr;			//包头
	//hdr.ustype = STATE;	//状态
	//hdr.usLen = sizeof(u_short) + m_loginDlg.m_strSN.GetLength();//状态+学号长度	
	//m_pClientSocket->SendPacket(&hdr,
	//	CClientDlg::LOGIN,
	//	(char*)(LPCSTR)m_loginDlg.m_strSN);
	////设置登录状态
	//m_state = CClientDlg::LOGIN;

	////接收试卷
	//return (m_pClientSocket->RecvPaper(m_strName, &m_listItem));
	return TRUE;
}


void CclientDlg::ConstructTestE1(char* buf)
{
	extern BYTE g_AGVNo;
	static int nState[] = { 0, 3072, 4096 }; //未知，空闲，急停
	static unsigned val = 1;
	int i = 0;
	ZeroMemory(buf, E1_BUFFSIZE);

	buf[i++] = 0x21;
	buf[i++] = 0x15;
	buf[i++] = 0x41;
	buf[i++] = 0x45;
	buf[i++] = 0x31;
	buf[i++] = g_AGVNo;
	buf[i++] = 0;
	buf[i++] = 0;
	buf[i++] = 0;

	int curDist = 0;
	int curSideno = 0;
	int curPt = m_curPt;
	
	int state = nState[1];
	int errcode = val;
	int reserve = 0;
	int curSpeed = 0;
	int optComple = 0;
	int curTaskno = m_curTaskno;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curDist, 2);
	i += 2;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curSideno, 2);
	i += 2;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curPt, 2);
	i += 2;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&state, 2);
	i += 2;
	buf[i++] = errcode;
	buf[i++] = reserve;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curSpeed, 2);
	i += 2;
	buf[i++] = optComple;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curTaskno, 2);
	//i += 2;

	//++val;
}

void CclientDlg::ConstructTestE1(char* buf, int curDist, int curPt)
{
	extern BYTE g_AGVNo;

	int i = 0;
	ZeroMemory(buf, E1_BUFFSIZE);

	buf[i++] = 0x21;
	buf[i++] = 0x15;
	buf[i++] = 0x41;
	buf[i++] = 0x45;
	buf[i++] = 0x31;
	buf[i++] = g_AGVNo;  //agvno
	buf[i++] = m_m1tag;
	buf[i++] = m_m2tag;
	buf[i++] = m_m6tag;

	//int curDist = 0;
	int curSideno = 0;
	//int curPt = 1;
	int state = 0;
	int errcode = 0;
	int reserve = 0;
	int curSpeed = 0;
	int optComple = 0;
	int curTaskno = m_curTaskno;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curDist, 2);
	i += 2;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curSideno, 2);
	i += 2;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curPt, 2);
	i += 2;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&state, 2);
	i += 2;
	buf[i++] = errcode;
	buf[i++] = reserve;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curSpeed, 2);
	i += 2;
	buf[i++] = optComple;
	memcpy_s(buf + i, E1_BUFFSIZE, (char*)&curTaskno, 2);

}

void CclientDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	// every 5s
	extern BYTE g_AGVNo;

	char buf[E1_BUFFSIZE];
	ConstructTestE1(buf);
	if (TIMER_INIT == nIDEvent) {
		m_pClientSocket->SendPulsePacket(buf);
	}

	if (TIMER_M6 == nIDEvent) {
		buf[8] = m_m6tag;
		memcpy_s(buf + 22, E1_BUFFSIZE, (char*)&m_curTaskno, 2);
		m_pClientSocket->SendPulsePacket(buf);
	}

	if (TIMER_M2 == nIDEvent) {
		buf[8] = m_m6tag;
		buf[7] = m_m2tag;
		memcpy_s(buf + 22, E1_BUFFSIZE, (char*)&m_curTaskno, 2);
		m_pClientSocket->SendPulsePacket(buf);
	}

	if (TIMER_M1 == nIDEvent) {
		static int i = 0;
		m_pClientSocket->SendPulsePacket(g_vecE1[i++]);
		if (i == g_vecE1.size()) {
			m_curPt = g_vecE1[i - 1][14] << 8 | g_vecE1[i - 1][13];
			i = 0;
			KillTimer(TIMER_M1);
			//清理g_vecE1内存
			for (auto p : g_vecE1) {
				delete[] p; p = nullptr;
			}

			SetTimer(TIMER_INIT, 1000, nullptr);
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}




unsigned WINAPI CclientDlg::ReceiveServer(void *pParam)
{
	//static BOOL bRecv = FALSE; //如果接收过了
	CclientDlg* pDlg = (CclientDlg*)pParam;
	int nRet;
	while(pDlg->m_bConn)
	{
		// 接收消息
		char buf[M6_BUFFSIZE];
		//ZeroMemory(buf, M6_BUFFSIZE);

		nRet = recv(pDlg->m_pClientSocket->m_s, buf, M6_BUFFSIZE, 0);
		if (SOCKET_ERROR != nRet/*nRet == M6_BUFFSIZE*/) {
			pDlg->KillTimer(TIMER_INIT);
			pDlg->m_m6tag = buf[5]; //缓存m6标签
			pDlg->m_curTaskno = buf[8] << 8 | buf[7]; //缓存任务号
			pDlg->SetTimer(TIMER_M6, 1000, nullptr);
		}

		//static BOOL bRecvM2 = FALSE;
		char bufm2[M2_BUFFSIZE];
		nRet = recv(pDlg->m_pClientSocket->m_s, bufm2, M2_BUFFSIZE, 0);
		if (SOCKET_ERROR != nRet/*nRet == M2_BUFFSIZE*/) {
			pDlg->KillTimer(TIMER_M6);
			pDlg->m_m2tag = bufm2[5];
			pDlg->SetTimer(TIMER_M2, 1000, nullptr);			
		}

		//static BOOL bRecvM1 = FALSE;
		char bufm1[M1_BUFFSIZE];
		nRet = recv(pDlg->m_pClientSocket->m_s, bufm1, M1_BUFFSIZE, 0);
		if (SOCKET_ERROR != nRet/*nRet == M1_BUFFSIZE*/) {
			pDlg->KillTimer(TIMER_M2);
			pDlg->m_m1tag = bufm1[5];
			//构造上报的M1
			pDlg->ConstructVecE1(bufm1);
			pDlg->SetTimer(TIMER_M1, 1000, nullptr);
		}
	}

	return(0);
}



void CclientDlg::ConstructVecE1(char* buf)
{
	g_vecE1.clear();
	int sidenum = buf[9];
	for (int i = 0, j = 0; i < sidenum + 1; ++i) {
		char* p = new char[E1_BUFFSIZE];
		int curpt = buf[10 + j + 1] << 8 | buf[10 + j];
		ConstructTestE1(p, 0, curpt);
		j += 2;
		g_vecE1.push_back(p);
	}

}