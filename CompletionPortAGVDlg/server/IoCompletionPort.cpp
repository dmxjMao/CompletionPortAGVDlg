#include "stdafx.h"
#include "IoCompletionPort.h"


CIOCP::CIOCP()
{
	m_hIOCP = nullptr;
	Create();
}


CIOCP::~CIOCP()
{
	if (m_hIOCP) {
		CLOSEHANDLE(m_hIOCP);
	}
}

BOOL CIOCP::Create()
{
	m_hIOCP = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,nullptr, 0, 0);
	return(m_hIOCP != nullptr);
}


BOOL CIOCP::Close()
{
	BOOL bResult = CloseHandle(m_hIOCP);
	m_hIOCP = nullptr;
	return(bResult);
}



BOOL CIOCP::AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey)
{
	BOOL bOk = (CreateIoCompletionPort(hDevice, m_hIOCP, CompKey, 0)
		== m_hIOCP);
	return(bOk);
}



BOOL CIOCP::AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey)
{
	return(AssociateDevice((HANDLE)hSocket, CompKey));
}


BOOL CIOCP::PostStatus(ULONG_PTR CompKey, DWORD dwNumBytes, OVERLAPPED* po)
{
	BOOL bOk = PostQueuedCompletionStatus(m_hIOCP, dwNumBytes, CompKey, po);
	return(bOk);
}



BOOL CIOCP::GetStatus(ULONG_PTR* pCompKey, PDWORD pdwNumBytes,
	OVERLAPPED** ppo, DWORD dwMilliseconds)
{
	return(GetQueuedCompletionStatus(m_hIOCP, pdwNumBytes, 
		pCompKey, ppo, dwMilliseconds));
}