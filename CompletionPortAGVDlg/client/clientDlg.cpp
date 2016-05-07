
// clientDlg.cpp : ʵ���ļ�
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
int g_InitPt[11] = { 0, 1, 6, 35, 40, 52, 63, 75, 81, 96, 139 }; //С����ʼλ��

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


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CclientDlg �Ի���



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


// CclientDlg ��Ϣ�������

BOOL CclientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	m_pClientSocket = new CClientSocket(this);		//����ͨѶ�����

	//setVecE1();

	//BOOL bConn = FALSE;
	while ( !m_bConn ) {
		if (ConnectServer()) {
			m_bConn = TRUE;
			// test : ����һ��E1
			//char buf[E1_BUFFSIZE];
			//ConstructTestE1(buf);
			//m_pClientSocket->SendTestE1(buf);

			break;
		}
	}

	SetTimer(TIMER_INIT, 1000, NULL);	//����ʱ�� 1s
	m_bPulsePacket = TRUE;		//����������

	//char buf[E1_BUFFSIZE];
	//ConstructTestE1(buf);
	//for (int i = 0; i < 3; ++i) {	
	//	m_pClientSocket->SendPulsePacket(buf);
	//}

	//�������ܷ�������Ϣ�߳�
	unsigned threadID;
	HANDLE hthread = (HANDLE)_beginthreadex(
		NULL, 0, ReceiveServer, this, 0, &threadID);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CclientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CclientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


/*
* ���ӷ�����
*/
BOOL CclientDlg::ConnectServer()
{
	//���ӷ�����
	if (FALSE == m_pClientSocket->ConnectServer())
	{
		//AfxMessageBox(_T("���ӷ�����ʧ�ܣ�"));
		return FALSE;
	}

	////���Ϳ���ѧ��
	//HDR hdr;			//��ͷ
	//hdr.ustype = STATE;	//״̬
	//hdr.usLen = sizeof(u_short) + m_loginDlg.m_strSN.GetLength();//״̬+ѧ�ų���	
	//m_pClientSocket->SendPacket(&hdr,
	//	CClientDlg::LOGIN,
	//	(char*)(LPCSTR)m_loginDlg.m_strSN);
	////���õ�¼״̬
	//m_state = CClientDlg::LOGIN;

	////�����Ծ�
	//return (m_pClientSocket->RecvPaper(m_strName, &m_listItem));
	return TRUE;
}


void CclientDlg::ConstructTestE1(char* buf)
{
	extern BYTE g_AGVNo;
	static int nState[] = { 0, 3072, 4096 }; //δ֪�����У���ͣ
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
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
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
			//����g_vecE1�ڴ�
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
	//static BOOL bRecv = FALSE; //������չ���
	CclientDlg* pDlg = (CclientDlg*)pParam;
	int nRet;
	while(pDlg->m_bConn)
	{
		// ������Ϣ
		char buf[M6_BUFFSIZE];
		//ZeroMemory(buf, M6_BUFFSIZE);

		nRet = recv(pDlg->m_pClientSocket->m_s, buf, M6_BUFFSIZE, 0);
		if (SOCKET_ERROR != nRet/*nRet == M6_BUFFSIZE*/) {
			pDlg->KillTimer(TIMER_INIT);
			pDlg->m_m6tag = buf[5]; //����m6��ǩ
			pDlg->m_curTaskno = buf[8] << 8 | buf[7]; //���������
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
			//�����ϱ���M1
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