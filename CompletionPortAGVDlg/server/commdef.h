#pragma once


#define		BUFFSIZE			(128)			//最大缓冲区大小
#define		E1_BUFFSIZE			(24)			
#define		M1_BUFFSIZE			(60)
#define		M6_BUFFSIZE			(11)
#define		M2_BUFFSIZE			(11)
#define		OVERLAPPEDPLUSLEN	(sizeof(IO_OPERATION_DATA))	//扩展重叠结构长度

//I/O操作类型
#define		IOReadE1			12							//接收E1
#define		IOWriteM1			13							//发送M1
#define		IOWriteM2			14							//发送M2
#define		IOWriteM6			15							//发送M6
#define		IOEXIT				16							//客户端退出

#define RET_ERRCODE_IFTRUE(ret, errcode) \
	if(ret) { \
		return (errcode); \
	}

#define RET_ERRCODE_IFSOCKETERROR(ret, errcode) \
	if(SOCKET_ERROR == ret) { \
		closesocket(m_sListen); \
		WSACleanup(); \
		return errcode; \
	}

#define RET_IFERR(ret) \
	if(Err_Buf != ret) {\
		return ret; \
	}

#define CONTINUE_IFTRUE(ret) \
	if(ret) { \
		continue; \
	}


#define DELETE_PTR(ptr) \
	if(ptr) { \
		delete ptr; \
		ptr = nullptr; \
	}


#define CLOSEHANDLE(h) \
	CloseHandle(h); \
	h = nullptr; 




enum ERRORCODE {
	Err_WSAStartup,
	Err_WSASocket,
	Err_BindSocket,
	Err_ListenSocket,
	Err_WSACreateEvent,
	Err_WSAEventSelect,
	Err_CreateIOCP,
	Err_CreateAcceptThread,
	Err_CreateServiceThread,
	Err_Buf
};




// I/O 操作数据结构
/*
windows用OVERLAPPED绑定每一个IO操作，
你可以扩展这个结构，当IO完成时，
			GetQueuedCompletionStatus(hComPort, &dwIoSize,
				(LPDWORD)&pClient, 当有io完成时，
				&lpOverlapped,  INFINITE);
会返回这个标识，而有一个神奇的宏：
	PIO_OPERATION_DATA pIO = CONTAINING_RECORD(pOverlapped,
		IO_OPERATION_DATA,
		overlapped);
你又可以获得这个扩展的结构。
那你又是什么时候给它赋值的呢？
在你执行异步函数的时候：
	ZeroMemory(&m_iIO, OVERLAPPEDPLUSLEN);
	m_iIO.type = IOReadE1;

	WSABUF wsaBuf;
	wsaBuf.buf = m_iIO.buf;
	wsaBuf.len = E1_BUFFSIZE;
	if (WSARecv(m_s, &wsaBuf, 1, &recvBytes,
	&flags, &m_iIO.overlapped, nullptr) == SOCKET_ERROR)
*/
typedef	struct _io_operation_data
{
	OVERLAPPED	overlapped;					//重叠结构
	char		buf[BUFFSIZE];				//数据缓冲区
	byte		type;						//操作类型
}IO_OPERATION_DATA, *PIO_OPERATION_DATA;