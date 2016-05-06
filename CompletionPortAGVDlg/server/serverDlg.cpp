
// serverDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "server.h"
#include "serverDlg.h"
#include "afxdialogex.h"

#include "ClientContext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_PULSE 1
#define TIMER_BIGDATA 2

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


// CServerDlg 对话框



CServerDlg::CServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Control(pDX, IDC_STATIC_WATCHER, m_stsWatcher);
}

BEGIN_MESSAGE_MAP(CServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_BN_CLICKED(IDC_BUTTON_EXPORTXY, &CServerDlg::OnClickedButtonExportxy)
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_BUTTON_DOONE, &CServerDlg::OnClickedButtonDoone)
	ON_BN_CLICKED(IDC_BUTTON_DOONE_DOHUGE, &CServerDlg::OnClickedButtonDooneDohuge)
END_MESSAGE_MAP()


// CServerDlg 消息处理程序

BOOL CServerDlg::OnInitDialog()
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
	//get init dialog size
	GetClientRect(&m_rcDlg);
	int cxx = GetSystemMetrics(SM_CXFULLSCREEN);
	int cyy = GetSystemMetrics(SM_CYFULLSCREEN);
	CRect rc(0, 0, cxx, cyy);
	rc.DeflateRect(cxx / 20, cyy / 20);
	MoveWindow(rc);

	//get IDC_STATIC_WATCHER size m_rcWatcher & m_rcDlg & m_rcTarget
	((CStatic*)GetDlgItem(IDC_STATIC_WATCHER))->GetClientRect(&m_rcWatcher);
	GetClientRect(&m_rcDlg);

	m_rcTarget = CRect(m_rcDlg.left, m_rcDlg.top,
		m_rcDlg.right - m_rcWatcher.right - 30, m_rcDlg.bottom);

	//start the server
	m_server.StartServer();

	//attach the main wnd
	m_server.AttachWnd(this);

	//get xy from file
	GetMapXYFromFile();

	//start pulse
	SetTimer(TIMER_PULSE, 10 * 1000, nullptr);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CServerDlg::OnPaint()
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

	//draw the map
	ShowTheMap(IDB_BITMAP_REPO);
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



/*
* show the map
*/
void CServerDlg::ShowTheMap(int nID)
{
	CDC* pDC = GetDC();
	CBitmap bitmap;
	bitmap.LoadBitmapW(nID);
	CDC memdc;
	memdc.CreateCompatibleDC(pDC);
	CBitmap* pOldbitmap = memdc.SelectObject(&bitmap);

	BITMAP bm;
	bitmap.GetBitmap(&bm);

	pDC->StretchBlt(0, 0, m_rcTarget.right, m_rcTarget.bottom,
		&memdc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	bitmap.DeleteObject();
	memdc.SelectObject(pOldbitmap);
	memdc.DeleteDC();
	ReleaseDC(pDC);
}

//
//void CServerDlg::OnLButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if (1)
//	{
//		++m_uCurNum;
//		// 保存点的坐标
//		m_mapPoint[m_uCurNum] = point;
//	}
//
//	CDialogEx::OnLButtonDown(nFlags, point);
//}
//
//
//void CServerDlg::OnRButtonDown(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if (1)
//	{
//		if (m_mapPoint.empty()) {
//			m_uCurNum = 0;
//			return;
//		}
//
//		m_mapPoint.erase(unsigned(m_mapPoint.size()));
//		--m_uCurNum;
//	}
//
//	CDialogEx::OnRButtonDown(nFlags, point);
//}
//
//
//void CServerDlg::OnMouseMove(UINT nFlags, CPoint point)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	if (1)
//	{
//		/*
//		先把位图按view客户区大小选入内存设备，
//		然后在其中画图
//		*/
//		CDC* pDC = GetDC();
//		CDC memdc1, memdc2;
//		memdc1.CreateCompatibleDC(pDC);
//		memdc2.CreateCompatibleDC(pDC);
//
//		// 获取区域大小
//		//RECT rc;
//		//GetClientRect(&rc);
//
//		CBitmap bitmapSrc, bitmap2;
//		bitmapSrc.LoadBitmapW(IDB_BITMAP_REPO);
//		bitmap2.CreateCompatibleBitmap(pDC, m_rcTarget.right, m_rcTarget.bottom);
//
//		CBitmap* pOld1 = memdc1.SelectObject(&bitmap2);
//		CBitmap* pOld2 = memdc2.SelectObject(&bitmapSrc);
//
//		BITMAP bm;
//		bitmapSrc.GetBitmap(&bm);
//
//		// 在memedc1上画图，相当于在客户区上画
//		memdc1.StretchBlt(0, 0, m_rcTarget.right, m_rcTarget.bottom,
//			&memdc2, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
//
//		// 显示数字
//		unsigned key;
//		CString strMsg;
//		for (auto it = m_mapPoint.begin(); it != m_mapPoint.end(); ++it) {
//			key = it->first;
//			CPoint& pt = it->second;
//			strMsg.Format(_T("%d"), key);
//			memdc1.TextOutW(pt.x, pt.y, strMsg);
//		}
//
//		pDC->StretchBlt(0, 0, m_rcTarget.right, m_rcTarget.bottom,
//			&memdc1, 0, 0, m_rcTarget.right, m_rcTarget.bottom, SRCCOPY);
//
//		bitmapSrc.DeleteObject();
//		bitmap2.DeleteObject();
//
//		memdc1.SelectObject(pOld1);
//		memdc1.DeleteDC();
//		memdc2.SelectObject(pOld2);
//		memdc2.DeleteDC();
//		ReleaseDC(pDC);
//	}
//
//	CDialogEx::OnMouseMove(nFlags, point);
//}


void CServerDlg::OnClickedButtonExportxy()
{
	// TODO: 在此添加控件通知处理程序代码
	TCHAR szExePath[MAX_PATH];
	GetModuleFileName(nullptr, szExePath, MAX_PATH);
	CString str(szExePath);
	int strPos = str.ReverseFind(_T('\\'));
	str.Truncate(strPos);
	str += _T("\\point.txt");

	CStdioFile file(str,
		CFile::modeWrite | CFile::modeCreate /*| CFile::typeText*/);

	unsigned key;
	for (auto it = m_mapPoint.begin(); it != m_mapPoint.end(); ++it)
	{
		key = it->first;
		CPoint& pt = it->second;
		str.Format(_T("%d\t%d,%d\n"), key, pt.x, pt.y);
		file.WriteString(str);
	}
	file.Close();
}




void CServerDlg::GetMapXYFromFile()
{
	TCHAR szExePath[MAX_PATH];
	GetModuleFileName(nullptr, szExePath, MAX_PATH);
	CString strPath(szExePath);
	int strPos = strPath.ReverseFind(_T('\\'));
	strPath.Truncate(strPos);
	strPath += _T("\\point.txt");

	if (FALSE == PathFileExists(strPath)) {
		TRACE("GetMapXYFromFile : Can not read point.txt!");
		return;
	}

	CStdioFile file(strPath, CFile::modeRead);
	m_mapPoint.clear();

	// 点号 （坐标）->  2\t233,37
	CString strLine, strNo, strPtX, strPtY;
	while (file.ReadString(strLine))
	{
		int pos1 = strLine.Find(_T("\t"));
		strNo = strLine.Left(pos1);
		int pos2 = strLine.Find(_T(","));
		strPtX = strLine.Mid(pos1 + 1, pos2 - pos1);
		strPtY = strLine.Mid(pos2 + 1, strLine.GetLength() - pos2);

		m_mapPoint[_ttoi(strNo)] = CPoint(_ttoi(strPtX), _ttoi(strPtY));
	}
}


/*
* start server
*/
//int CServerDlg::StartServer()
//{
//	//init listen socket
//	int nRet = Err_Buf;
//	nRet = InitListenSocket();
//	RET_IFERR(nRet);
//
//	m_bRunning = TRUE;
//
//
//
//}


/*
* init listen socket
*/
//int CServerDlg::InitListenSocket()
//{
//	//init socket library
//	int	reVal = Err_Buf;
//	WSADATA wsaData;
//	reVal = WSAStartup(0x0202, &wsaData);
//	RET_ERRCODE_IFTRUE(reVal, Err_WSAStartup);
//
//	//create listen socket
//	if ((m_sListen = WSASocket(AF_INET, SOCK_STREAM,
//		0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET)
//	{
//		WSACleanup();
//		return Err_WSASocket;
//	}
//
//	//bind socket
//	SOCKADDR_IN	servAddr;//服务器地址
//	servAddr.sin_family = AF_INET;
//	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	servAddr.sin_port = htons(SERV_PORT);
//	reVal = bind(m_sListen, (SOCKADDR*)&servAddr, sizeof(servAddr));
//	RET_ERRCODE_IFSOCKETERROR(reVal, Err_BindSocket);
//
//	//listen
//	reVal = listen(m_sListen, SOMAXCONN);
//	RET_ERRCODE_IFSOCKETERROR(reVal, Err_ListenSocket);
//
//}



/*
* init listen event
*/
//int CServerDlg::InitListenEvent()
//{
//
//}

void CServerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (TIMER_PULSE == nIDEvent) {
		//auto& clientList = m_server.m_clientMgr.m_clientList;
		//for (auto it = clientList.begin(); it != clientList.end(); ++it)
		//{
		//	CClientContext *pClient = *it;
		//	//比较时间
		//	CTime curTime = CTime::GetCurrentTime();
		//	int	nDay = curTime.GetDay() - pClient->m_time.GetDay();			//日
		//	int	nHour = curTime.GetHour() - pClient->m_time.GetHour();		//小时
		//	int	nMinute = curTime.GetMinute() - pClient->m_time.GetMinute();//分钟
		//	int	nSecond = curTime.GetSecond() - pClient->m_time.GetSecond();//秒

		//	CTimeSpan spanTime(nDay, nHour, nMinute, nSecond);
		//	if (spanTime.GetSeconds() > 20)//大于20s
		//	{
		//		//删除该客户端
		//		m_server.m_clientMgr.DeleteClient(pClient);
		//	}
		//}
	}

	if (TIMER_BIGDATA == nIDEvent) {

		static int i = 0;
		if (i == m_vecBigdata.size()) {
			KillTimer(TIMER_BIGDATA);
			return;
		}
		BIGDATA bigdata = m_vecBigdata[i++];
		//static CTaskSend taskSend;

		SendOneTask(bigdata.agvno, bigdata.target);
	}

	CDialogEx::OnTimer(nIDEvent);
}


void CServerDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	//adjust to your screen
	if (SIZE_RESTORED == nType) {
		float ratio[2];
		POINT newDlgSize;
		CRect newRect;

		GetClientRect(&newRect);
		newDlgSize.x = newRect.right - newRect.left;
		newDlgSize.y = newRect.bottom - newRect.top;

		ratio[0] = (float)newDlgSize.x / (m_rcDlg.right - m_rcDlg.left);
		ratio[1] = (float)newDlgSize.y / (m_rcDlg.bottom - m_rcDlg.top);

		CRect rect;
		//int woc;

		//列出所有控件
		CWnd* pWnd = GetWindow(GW_CHILD);
		while (pWnd) {
			pWnd->GetWindowRect(&rect);
			ScreenToClient(&rect);

			rect.left = (LONG)(rect.left * ratio[0]);
			rect.right = (LONG)(rect.right * ratio[0]);
			rect.top = (LONG)(rect.top * ratio[1]);
			rect.bottom = (LONG)(rect.bottom * ratio[1]);

			pWnd->MoveWindow(rect);
			pWnd = pWnd->GetWindow(GW_HWNDNEXT);
		}
	}

	GetClientRect(&m_rcDlg);
}


void CServerDlg::OnClickedButtonDoone()
{
	// TODO: 在此添加控件通知处理程序代码
	CString strAGVno, strTarget;
	GetDlgItem(IDC_EDIT_CARNO)->GetWindowTextW(strAGVno);
	GetDlgItem(IDC_EDIT_TARGET)->GetWindowTextW(strTarget);
	if (strAGVno.IsEmpty() || strTarget.IsEmpty()) {
		return;
	}

	int agvno = _ttoi(strAGVno);
	int target = _ttoi(strTarget);

	SendOneTask(agvno, target);
}


//发送一条任务
void CServerDlg::SendOneTask(int carno, int target)
{
	static CTaskSend taskSend;
	auto& clientList = m_server.m_clientMgr.m_clientList;
	for (auto it = clientList.begin(); it != clientList.end(); ++it) {
		CClientContext* pClient = *it;
		if (pClient->m_carno == carno) {
			char buf[M6_BUFFSIZE];
			taskSend.ConstructM6(buf, carno, target);

			char bufm2[M2_BUFFSIZE];
			taskSend.ConstructM2(bufm2, carno, 1);

			char bufm1[M1_BUFFSIZE];
			taskSend.ConstructM1(bufm1, carno);

			pClient->AsyncSendM6(buf, bufm2, bufm1);
		}
	}
}


//发送huge任务
void CServerDlg::SendHugeTask()
{
	TCHAR szExePath[MAX_PATH];
	GetModuleFileName(nullptr, szExePath, MAX_PATH);
	CString str(szExePath);
	int strPos = str.ReverseFind(_T('\\'));
	str.Truncate(strPos);
	str += _T("\\task.txt");

	CStdioFile f(str, CFile::modeRead);
	CString line;
	CString strTmp;
	BIGDATA bigdata;

	while (f.ReadString(line))
	{
		int i = 0;
		AfxExtractSubString(strTmp, line, i++, _T(' '));
		bigdata.agvno = _ttoi(strTmp);

		AfxExtractSubString(strTmp, line, i++, _T(' '));
		bigdata.target = _ttoi(strTmp);

		m_vecBigdata.push_back(bigdata);
	}

	SetTimer(TIMER_BIGDATA, 10 * 1000, nullptr); //每10s发送一个任务
}


void CServerDlg::OnClickedButtonDooneDohuge()
{
	// TODO: 在此添加控件通知处理程序代码
	SendHugeTask();
}
