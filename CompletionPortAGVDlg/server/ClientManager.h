#pragma once
#include <list>

class CClientContext;
class CClientManager
{
public:
	CClientManager();
	~CClientManager();
	void AddClient(CClientContext* pClient);
	void DeleteClient(CClientContext* pClient);
	void DeleteAllClient();

	void ProcessIO(CClientContext* pClient,		//I/O¥¶¿Ì
		LPOVERLAPPED pOverlapped, DWORD dwIOSize);

public:
	std::list<CClientContext*>		m_clientList;
	CRITICAL_SECTION		m_cs;
	
};