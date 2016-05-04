#include "stdafx.h"
#include "ClientManager.h"

#include <functional> //bind
#include <algorithm> //find_if

#include "ClientContext.h"

//lambda: find CClientContext* pClient
bool lambdaFindClient(const CClientContext* pClient,
	const CClientContext* c)
{
	return(pClient == c);
}

//lambda: delete CClientContext* pClient
void lambdaDeleteClient(const CClientContext* pClient)
{
	DELETE_PTR(pClient);
}

CClientManager::CClientManager()
{
	InitializeCriticalSection(&m_cs);
}


CClientManager::~CClientManager()
{
	DeleteCriticalSection(&m_cs);
}


void CClientManager::AddClient(CClientContext* pClient)
{
	EnterCriticalSection(&m_cs);
	m_clientList.push_back(pClient);
	LeaveCriticalSection(&m_cs);
}


void CClientManager::DeleteClient(CClientContext* pClient)
{
	EnterCriticalSection(&m_cs);
	
	using std::placeholders::_1;
	auto it = std::find_if(m_clientList.begin(), m_clientList.end(),
		std::bind(lambdaFindClient, _1, pClient));
	if (m_clientList.end() != it) {
		DELETE_PTR((*it));
		m_clientList.erase(it);
	}

	LeaveCriticalSection(&m_cs);
}


void CClientManager::DeleteAllClient()
{
	EnterCriticalSection(&m_cs);
	
	std::for_each(m_clientList.begin(), m_clientList.end(),
		lambdaDeleteClient);
	m_clientList.clear();

	LeaveCriticalSection(&m_cs);
}


void CClientManager::ProcessIO(CClientContext* pClient,
	LPOVERLAPPED pOverlapped, DWORD dwIOSize)
{

	//获取扩展重叠结构指针
	PIO_OPERATION_DATA pIO = CONTAINING_RECORD(pOverlapped,
		IO_OPERATION_DATA,
		overlapped);
	if (pIO)
	{
		switch (pIO->type)//重叠操作类型
		{
		case IOReadE1://读E1
		{
			pClient->OnRecvE1Completed(dwIOSize);
			break;
		}
		//case IOWriteM6:
		//{
		//	pClient->OnSendM6Completed();
		//	break;
		//}
		//case IOWriteM2:
		//{
		//	pClient->OnSendM2Completed();
		//	break;
		//}
		//case IOWriteM1:
		//{
		//	pClient->OnSendM1Completed();
		//	break;
		//}
		//case IOEXIT://客户端退出
		//{
		//	DeleteClient(pClient);
		//	break;
		//}
		default:break;
		}

		// 车辆管理
		//m_pCarMgr->UpdateOnConnect();
		//m_pCarMgr->UpdateOnChange();
	}
}