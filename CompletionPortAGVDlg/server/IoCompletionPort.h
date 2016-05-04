#pragma once


class CIOCP {
public:
	CIOCP();
	~CIOCP();

	BOOL Create();

	BOOL Close();

	BOOL AssociateDevice(HANDLE hDevice, ULONG_PTR CompKey);

	BOOL AssociateSocket(SOCKET hSocket, ULONG_PTR CompKey);

	BOOL PostStatus(ULONG_PTR CompKey, DWORD dwNumBytes = 0,
		OVERLAPPED* po = NULL);

	BOOL GetStatus(ULONG_PTR* pCompKey, PDWORD pdwNumBytes,
		OVERLAPPED** ppo, DWORD dwMilliseconds = INFINITE);

private:
	HANDLE		m_hIOCP;
};