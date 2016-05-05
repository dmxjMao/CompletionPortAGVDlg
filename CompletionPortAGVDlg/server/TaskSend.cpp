#include "stdafx.h"
#include "TaskSend.h"

extern UINT16 TASK;
extern BYTE TAG;

/*
* 构造M6
*/
void CTaskSend::ConstructM6(char* buf, int carno, int target)
{
	ZeroMemory(buf, M6_BUFFSIZE);

	int i = 0;
	buf[i++] = 0x21;
	buf[i++] = 0x8;
	buf[i++] = 0x41;
	buf[i++] = 0x4D;
	buf[i++] = 0x36;

	if (++TAG > 255)
		TAG = 0;

	buf[i++] = TAG;    //M6消息标签
	buf[i++] = carno;
	memcpy_s(buf + i, M6_BUFFSIZE, (char*)&(++TASK), 2);
	i += 2;
	memcpy_s(buf + i, M6_BUFFSIZE, (char*)&(target), 2);
}


/*
* 构造M2
*/
void CTaskSend::ConstructM2(char* buf, int carno, int optcode)
{
	ZeroMemory(buf, M2_BUFFSIZE);

	int i = 0;
	buf[i++] = 0x21;
	buf[i++] = 0x8;
	buf[i++] = 0x41;
	buf[i++] = 0x4D;
	buf[i++] = 0x32;

	//if (++TAG > 255)
	//	TAG = 0;

	buf[i++] = TAG;    //M2消息标签
	buf[i++] = carno;
	memcpy_s(buf + i, M2_BUFFSIZE, (char*)&(TASK), 2);
	i += 2;
	memcpy_s(buf + i, M2_BUFFSIZE, (char*)&(optcode), 2);
}

/*
* 构造M1
*/
void CTaskSend::ConstructM1(char* buf, int carno)
{
	ZeroMemory(buf, M1_BUFFSIZE);
	int i = 0;
	buf[i++] = 0x21;
	buf[i++] = 0x39;
	buf[i++] = 0x41;
	buf[i++] = 0x4D;
	buf[i++] = 0x31;

	buf[i++] = TAG;
	buf[i++] = carno;
	memcpy_s(buf + i, M1_BUFFSIZE, (char*)&(TASK), 2);
	i += 2;

	//buf[i++] = 3; //3段
	//int sideno[] = { 11,12,13 };
	//for (int j = 0; j < 3; ++j) {
	//	memcpy_s(buf + i, M1_BUFFSIZE, (char*)&sideno[j], 2);
	//	i += 2;
	//}
}