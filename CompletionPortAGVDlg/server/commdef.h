#pragma once


#define		BUFFSIZE			(128)			//��󻺳�����С
#define		E1_BUFFSIZE			(24)			
#define		M1_BUFFSIZE			(60)
#define		M6_BUFFSIZE			(11)
#define		M2_BUFFSIZE			(11)
#define		OVERLAPPEDPLUSLEN	(sizeof(IO_OPERATION_DATA))	//��չ�ص��ṹ����

//I/O��������
#define		IOReadE1			12							//����E1
#define		IOWriteM1			13							//����M1
#define		IOWriteM2			14							//����M2
#define		IOWriteM6			15							//����M6
#define		IOEXIT				16							//�ͻ����˳�

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




// I/O �������ݽṹ
/*
windows��OVERLAPPED��ÿһ��IO������
�������չ����ṹ����IO���ʱ��
			GetQueuedCompletionStatus(hComPort, &dwIoSize,
				(LPDWORD)&pClient, ����io���ʱ��
				&lpOverlapped,  INFINITE);
�᷵�������ʶ������һ������ĺ꣺
	PIO_OPERATION_DATA pIO = CONTAINING_RECORD(pOverlapped,
		IO_OPERATION_DATA,
		overlapped);
���ֿ��Ի�������չ�Ľṹ��
��������ʲôʱ�������ֵ���أ�
����ִ���첽������ʱ��
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
	OVERLAPPED	overlapped;					//�ص��ṹ
	char		buf[BUFFSIZE];				//���ݻ�����
	byte		type;						//��������
}IO_OPERATION_DATA, *PIO_OPERATION_DATA;