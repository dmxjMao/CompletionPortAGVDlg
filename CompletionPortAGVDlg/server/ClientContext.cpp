#include "stdafx.h"
#include "ClientContext.h"

//#include "MyServer.h"
//
#include <map>
#include <algorithm>
#include <functional>
#include "Graph.h"

#include "resource.h"
#include "serverDlg.h"
//#include "ClientManager.h"
//
//#include "IoComplationPortAGV_serverDoc.h"
//#include "IoComplationPortAGV_serverView.h"

//#include "MsgStruct.h"

#define USER_PRECISION 10	// �û�ָ������ľ������
extern Graph g_Graph;
extern UINT16 TASK;

//CClientContext::CClientContext(SOCKET s)
//	:m_s(s)
//{
//	InitializeCriticalSection(&m_csCDC);
//}
CClientContext::CClientContext(SOCKET s, CServerDlg* pMainDlg)
	: m_pMainDlg(pMainDlg), m_s(s)
{
	InitializeCriticalSection(&m_csCDC);
}


CClientContext::~CClientContext()
{
	DeleteCriticalSection(&m_csCDC);
}


/*
* ����E1
*/
BOOL CClientContext::AsyncRecvE1()
{
	DWORD flags = 0;
	DWORD recvBytes = 0;

	ZeroMemory(&m_iIO, OVERLAPPEDPLUSLEN);
	m_iIO.type = IOReadE1;

	WSABUF wsaBuf;
	wsaBuf.buf = m_iIO.buf;
	wsaBuf.len = E1_BUFFSIZE;

	// ��ȡ����
	// m_s����accept�̴߳����ģ���m_pListenSocket ��һ�������� WSA_FLAG_OVERLAPPED
	// WSARecv ����0��ʾ�����ж�
	if (WSARecv(m_s, &wsaBuf, 1, &recvBytes,
		&flags, &m_iIO.overlapped, nullptr) == SOCKET_ERROR) 
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			return FALSE;
		}
	}

	return TRUE;  // I/O������Ͷ��
}


/*
* ����E1��� 
*/
void CClientContext::OnRecvE1Completed(DWORD dwIOSize)
{
	// ��ȡС����Ϣ
	GetCarInfo(m_iIO.buf);

	// �ж�M6��Ϣ�Ƿ񵽴�
	if (m_m6tag == m_Sendm6tag) {
		if (!m_bM6Ok) {
			AsyncSendM2(m_m2buf);
			m_bM6Ok = TRUE;
		}
	}

	// M2��Ϣ�Ƿ񵽴�
	if (m_m2tag == m_Sendm2tag) {
		if (!m_bM2Ok) {
			AsyncSendM1(m_m1buf);
			m_bM2Ok = TRUE;
		}
	}

	// M1��Ϣ�Ƿ񵽴�
	if (m_m1tag == m_Sendm1tag) {
		if (!m_bM1Ok) {
			m_bM1Ok = TRUE;
		}
	}

	// ��ʾС��λ��
	ShowAGV();

	// ��������E1��Ϣ
	AsyncRecvE1();
}

/*
* ��ȡС��״̬
*/
void CClientContext::GetCarInfo(char* buf)
{
	//Msg_E1* p = (Msg_E1*)&buf[0];

	m_carno = buf[5];
	m_m1tag = (BYTE)buf[6];
	m_m2tag = (BYTE)buf[7];
	m_m6tag = (BYTE)buf[8];

	//char buf13 = buf[13]; //0
	//char buf14 = buf[14]; //14

	m_curDist = (buf[10] << 8) | buf[9];
	m_curSideno = (buf[12] << 8) | buf[11];
	//m_curPoint = (buf[13] << 8) | buf[14];  //python�汾
	m_curPoint = (buf[14] << 8) | buf[13];
	//m_curPoint = ntohs((unsigned char)buf[13]);
	//m_eState = (buf[16] << 8) | buf[15];
	m_errcode = buf[17];
	//m_e1.reserve = buf[18];
	m_curSpeed = (buf[20] << 8) | buf[19];
	m_optCompleted = buf[21];
	m_curTaskno = (buf[23] << 8) | buf[22];
	//���С�����������񣬽�֮ǰ������յ���Ϊ����&ɾ����ɵ�����
	if (m_curTaskno != m_prevTaskno) {
		m_prevTaskno = m_curTaskno;
		SetEvent(g_Graph.m_mapHandle[m_curPoint]);
		if (auto it = g_Graph.m_casheTask.find(m_prevTaskno)
			!= g_Graph.m_casheTask.end()) {
			g_Graph.m_casheTask.erase(m_prevTaskno);
		}
	}

	//UINT16 state(0);
	//memcpy_s(&state, 2, buf + 15, 2);
	UINT16 state(0);
	state = (buf[15] << 8 | buf[16]);

	unsigned bit = 1;
	if (state == 0 || ((state >> 10 & bit) == 0) ||
		((state >> 10 & bit) == 1 && (state >> 11 & bit) == 0)) {	
		m_eState = UNKNOWN;  // δ�ܶ���С������Ϣ��С��δ���뵽ϵͳ�У�����ϵͳδ����
	}

	if ((state >> 10 & bit) == 1 && (state >> 11 & bit) == 1) {	
		m_eState = FREE; // ����û�в��������ǿ��ص�
	}

	if ((state >> 12 & bit) == 1) {
		m_eState = CANCELED; // С�����ڼ�ͣ���ֶ�״̬
	}

	//m_time = CTime::GetCurrentTime();

	m_bChange = JudgeChangeAndLockPoint();
}

BOOL CClientContext::JudgeChangeAndLockPoint()
{
	//
	if (m_prevPoint != m_curPoint && 
		m_curPoint != m_target) {
		SetEvent(g_Graph.m_mapHandle[m_curPoint]);
	}

	if (m_prevPoint != m_curPoint ||
		m_prevState != m_eState) {

		m_prevPoint = m_curPoint;
		m_prevState = m_eState;
		return TRUE;
	}

	return FALSE;
}

/*
* ����M1
*/
BOOL CClientContext::AsyncSendM1(char* buf)
{
	m_Sendm1tag = buf[5];
	// ��ȡС��������·��
	//std::vector<int> vecRoute;
	GetRoute(m_curPoint, m_target);
	
	// ��ֵbuf
	SetM1Buf();

	DWORD flags(0);
	DWORD sendBytes(0);

	ZeroMemory(&m_oIO, OVERLAPPEDPLUSLEN);
	m_oIO.type = IOWriteM1;
	memcpy_s(m_oIO.buf, BUFFSIZE, buf, M1_BUFFSIZE);

	// ����M1����
	WSABUF wsaBuf;
	wsaBuf.buf = m_oIO.buf;
	wsaBuf.len = M1_BUFFSIZE;

	// ����
	if (WSASend(m_s, &wsaBuf, 1, &sendBytes,
		flags, &m_oIO.overlapped, nullptr) == SOCKET_ERROR)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*
* ����M6
*/
BOOL CClientContext::AsyncSendM6(char* buf, char* bufm2, char* bufm1)
{
	m_Sendm6tag = buf[5]; //�����M6��ǩ
	m_target = buf[9];

	DWORD flags(0);
	DWORD sendBytes(0);

	ZeroMemory(&m_oIO, OVERLAPPEDPLUSLEN);
	m_oIO.type = IOWriteM6;
	memcpy_s(m_oIO.buf, BUFFSIZE, buf, M6_BUFFSIZE);

	WSABUF wsaBuf;
	wsaBuf.buf = m_oIO.buf;
	wsaBuf.len = M6_BUFFSIZE;

	// ����
	if (WSASend(m_s, &wsaBuf, 1, &sendBytes,
		flags, &m_oIO.overlapped, nullptr) == SOCKET_ERROR)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			return FALSE;
		}
	}

	//����M2��M1��Ϣ
	memcpy_s(m_m2buf, M2_BUFFSIZE, bufm2, M2_BUFFSIZE);
	memcpy_s(m_m1buf, M1_BUFFSIZE, bufm1, M1_BUFFSIZE);
	
	return TRUE;
}


/*
* ����M6���
*/
//void CClientContext::OnSendM6Completed()
//{
//	// ��������E1��Ϣ
//	AsyncRecvE1();
//}


/*
* ����M2
*/
BOOL CClientContext::AsyncSendM2(char* buf)
{
	m_Sendm2tag = buf[5];
	m_optcode = buf[9];

	DWORD flags(0);
	DWORD sendBytes(0);

	ZeroMemory(&m_oIO, OVERLAPPEDPLUSLEN);
	m_oIO.type = IOWriteM2;
	memcpy_s(m_oIO.buf, BUFFSIZE, buf, M2_BUFFSIZE);

	WSABUF wsaBuf;
	wsaBuf.buf = m_oIO.buf;
	wsaBuf.len = M2_BUFFSIZE;

	// ����
	if (WSASend(m_s, &wsaBuf, 1, &sendBytes,
		flags, &m_oIO.overlapped, nullptr) == SOCKET_ERROR)
	{
		if (ERROR_IO_PENDING != WSAGetLastError())
		{
			return FALSE;
		}
	}

	return TRUE;
}

/*
* ����M2���
*/
//void CClientContext::OnSendM2Completed()
//{
//	// ��������E1��Ϣ
//	AsyncRecvE1();
//}


/*
* ����M1���
*/
//void CClientContext::OnSendM1Completed()
//{
//	// ��������E1��Ϣ
//	AsyncRecvE1();
//}


/*
* ��ȡС������·��
*/
BOOL CClientContext::GetRoute(int ptSrc, int ptDest)
{
	m_vecRoute.clear();

	//�ȴӻ���·������
	auto& cashe = g_Graph.m_casheRoute;
	auto itFind = cashe.find(std::make_pair(ptSrc, ptDest));
	if (cashe.end() != itFind) {
		//�ҵ�����·��
		m_vecRoute = itFind->second;
		return TRUE;
	}

	//û�ҵ������·��
	g_Graph.Dijkstra(ptSrc, ptDest);
	m_vecRoute = g_Graph.m_vecRoute;

	HANDLE* p(nullptr);
	int len(0);
	if (GetOverlappedHandle(&p, len)) { //��������������ͻ
		WaitForMultipleObjects(len, p, TRUE, INFINITE); //�ȴ���Ŀ���
		//�ȴ��ɹ������õ�δ����
		for (auto pt : m_vecRoute) {
			ResetEvent(g_Graph.m_mapHandle[pt]);
		}
		delete[] p; p = nullptr;
	}

	//cashe.insert(std::make_pair(std::make_pair(ptSrc, ptDest), m_vecRoute));
	return TRUE;
}

/*
* ��ȡС����ǰ����
*/
void CClientContext::GetCurrentXY(CClientContext* pClient, CPoint& pt)
{
	//extern Graph g_Graph;
	auto& mapPoint = m_pMainDlg->m_mapPoint;

	CPoint curPt;
	auto itFind = mapPoint.find(m_curPoint);
	if (mapPoint.end() == itFind) {
		pt = pClient->m_curXY;
		//pt = CPoint(0, 0);
		return;
	}

	// ������ǵ�ǰС��
	if (m_carno != pClient->m_carno) {
		pt = pClient->m_curXY;
		return;
	}

	curPt = itFind->second;
	// �����С����ʼλ��
	if (m_vecRoute.empty()) {
		pt = curPt;
		m_curXY = pt;
		return;
	}

	int nxPtNo; // ��һ�����
	auto curPtIter = std::find(m_vecRoute.begin(), m_vecRoute.end(), m_curPoint);
	if (curPtIter == m_vecRoute.end()) {
		pt = curPt;
		m_curXY = pt;
		return;
	}
	std::advance(curPtIter, 1);
	if (curPtIter == m_vecRoute.end()) {
		pt = curPt;
		m_curXY = pt;
		return;
	}

	nxPtNo = *curPtIter;
	CPoint nxPt = mapPoint[nxPtNo];// ��һ����ŵ�����

	// �ж���x����y��
	bool bXY = false;	// y��
	bool bPositive = false; // ������
	if (fabs(nxPt.y - curPt.y) < USER_PRECISION) {
		bXY = true;		// x��
		// �ж����������Ƿ�����
		if (curPt.x < nxPt.x)
			bPositive = true;
	}
	else {
		if (curPt.y < nxPt.y)
			bPositive = true;
	}


	// ����������� pix�������mm��
	// ��E1��Ϣ�ĵ�ǰ���뻻�����Ļ���� 5�� 1mm  1-2 57pix 
	int curDist = m_curDist * 3;

	// ���㵱ǰ��
	if (bXY) {
		bPositive ? pt.x = curPt.x + curDist : pt.x = curPt.x - curDist;
		pt.y = curPt.y;
	}
	else {
		bPositive ? pt.y = curPt.y + curDist : pt.y = curPt.y - curDist;
		pt.x = curPt.x;
	}

	pClient->m_curXY = pt;
	//m_curXY = pt;
}


/*
* ���������õĶκŸ�ֵ��buf
*/
void CClientContext::SetM1Buf()
{
	//��ʱ���κ�������
	//int sideno[] = { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25 };
	
	m_m1buf[9] = (char)(m_vecRoute.size() - 1); //����
	size_t sidenum = m_m1buf[9];
	for (size_t i = 0; i < sidenum + 1; ++i) {
		//m_m1buf[10 + i * 2] = ;
		memcpy_s(m_m1buf + 10 + i * 2, M1_BUFFSIZE, (char*)&m_vecRoute[i], 2);
	}
}


/*
* ��ȡ������������ص���
*/
auto IsPointInOtherTask(std::pair<UINT16, vector<int>> p, int pt) -> bool
{
	auto it = find(p.second.begin(), p.second.end(), pt);
	if (p.second.end() != it)
		return true;
	return false;
}

BOOL CClientContext::GetOverlappedHandle(HANDLE** arr, int& len)
{
	auto casheTask = g_Graph.m_casheTask; //����һ������
	if (casheTask.empty()) {
		return(FALSE); //û���ص�
	}

	casheTask.erase(TASK); //ȥ����ǰ����
	vector<int> vecOverlapped;
	using std::placeholders::_1;
	for (auto pt : m_vecRoute) {
		auto it = std::find_if(casheTask.begin(), casheTask.end(),
			bind(IsPointInOtherTask, _1, pt));
		if (casheTask.end() != it) { //�����ǰ���Ѿ���������������
			vecOverlapped.push_back(pt);
		}
	}

	if (vecOverlapped.empty()) {
		for (auto pt : m_vecRoute) { 
			//û���ص���������Ϊδ����
			ResetEvent(g_Graph.m_mapHandle[pt]);
		}
		return(FALSE);
	}

	len = (int)vecOverlapped.size();
	*arr = new HANDLE[len];
	for (int i = 0; i < len; ++i) {
		(*arr)[i] = g_Graph.m_mapHandle[vecOverlapped[i]];
	}

	return(TRUE);
}


/*
* ��ʾС��
*/
void CClientContext::ShowAGV()
{
	EnterCriticalSection(&m_csCDC);

	CDC* pDC = m_pMainDlg->GetDC();

	CDC memdc1, memdc2;
	memdc1.CreateCompatibleDC(pDC);
	memdc2.CreateCompatibleDC(pDC);

	// ��ȡ�����С
	RECT rc;
	m_pMainDlg->GetClientRect(&rc);

	/*
	�Ȱ�λͼ��view�ͻ�����Сѡ���ڴ��豸��
	Ȼ�������л�ͼ
	*/
	CBitmap bitmapSrc, bitmap2;
	bitmapSrc.LoadBitmapW(IDB_BITMAP_REPO);
	bitmap2.CreateCompatibleBitmap(pDC, rc.right, rc.bottom);

	CBitmap* pOld1 = memdc1.SelectObject(&bitmap2);
	CBitmap* pOld2 = memdc2.SelectObject(&bitmapSrc);

	BITMAP bm;
	bitmapSrc.GetBitmap(&bm);

	// ����memdc1���view�Ĵ�С
	memdc1.StretchBlt(0, 0, rc.right, rc.bottom,
		&memdc2, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

	// ��ȡС����ǰ���ڵ������
	CPoint curXY;
	CString strAgvNo;

	//CClientManager *pClientMgr = CClientManager::GetClientManager();
	//CObList& clientList = pClientMgr->m_clientList;
	//POSITION pos = clientList.GetHeadPosition();
	auto& clientList = m_pMainDlg->m_server.m_clientMgr.m_clientList;
	for (auto it = clientList.begin(); it != clientList.end(); ++it) {
		CClientContext* pClient = *it;
		// ��ȡС����ǰ���ڵ������
		GetCurrentXY(pClient, curXY);
		// ��һ��Բ����ʾ����
		memdc1.Ellipse(curXY.x - 4, curXY.y - 4, curXY.x + 4, curXY.y + 4);
		strAgvNo.Format(_T("%d"), pClient->m_carno);
		memdc1.TextOutW(curXY.x, curXY.y, strAgvNo);
	}
	//while (pos) { // �����еĵ㻭����
	//	CClientContext* pClient = (CClientContext*)clientList.GetNext(pos);
	//	// ��ȡС����ǰ���ڵ������
	//	GetCurrentXY(pClient, curXY);
	//	// ��һ��Բ����ʾ����
	//	memdc1.Ellipse(curXY.x - 4, curXY.y - 4, curXY.x + 4, curXY.y + 4);
	//	strAgvNo.Format(_T("%d"), pClient->m_carno);
	//	memdc1.TextOutW(curXY.x, curXY.y, strAgvNo);
	//}

	pDC->StretchBlt(0, 0, rc.right, rc.bottom,
		&memdc1, 0, 0, rc.right, rc.bottom, SRCCOPY);

	bitmapSrc.DeleteObject();
	bitmap2.DeleteObject();

	memdc1.SelectObject(pOld1);
	memdc1.DeleteDC();
	memdc2.SelectObject(pOld2);
	memdc2.DeleteDC();
	m_pMainDlg->ReleaseDC(pDC);

	LeaveCriticalSection(&m_csCDC);
}