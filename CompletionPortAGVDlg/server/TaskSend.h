#pragma once


class CTaskSend
{
public:
	void ConstructM6(char* buf, int carno, int target);
	void ConstructM2(char* buf, int carno, int optcode);
	void ConstructM1(char* buf, int carno);
};