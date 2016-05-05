#pragma once

#include <vector>

//class CMyServer;
class CServerDlg;
class CClientContext
{
public:
	CClientContext(SOCKET s, CServerDlg* pMainDlg);
	virtual ~CClientContext();
	enum state
	{
		UNKNOWN = 0,	// 未知 AGV调度系统未能读到该小车的信息，小车未插入到系统中
		FREE,			// 空闲 车辆未被分配任务
		ASSIGNMENTED,	// 已分配 车辆已被分配任务
		LINKED,			// 已连接 一般是小车已经到达装货站台或已载货状态
		CANCELED,		// 取消 小车处于急停或手动状态
		//DISCONN			// 小车掉线
		STATEBUF
	};

public:
	int					m_carno = 0;			//小车车号
	int					m_m1tag = 0;			//m1消息标签
	int					m_m2tag = 0;			//m2消息标签
	int					m_m6tag = 0;			//m6消息标签
	int					m_curDist = 0;			//当前距离
	int					m_curSideno = 0;		//当前段号
	int					m_curPoint = 0;			//当前点号
	state				m_eState = UNKNOWN;		//小车状态
	int					m_errcode = 0;			//错误码
	int					m_curSpeed = 0;			//当前速度
	int					m_optCompleted = 0;		//移动&操作完成
	int					m_curTaskno = 0;		//当前任务号

	CTime				m_time;			//心跳时间
	BOOL				m_bChange;		//小车位置或状态发生变化
	int					m_prevPoint = 0;		//之前点号
	state				m_prevState = STATEBUF;	//之前状态
	int					m_prevTaskno = -1;		//之前任务号

	int				m_Sendm6tag = 255;		//最近发送的m6消息标签
	int				m_Sendm2tag = 255;		//最近发送的m2消息标签
	int				m_Sendm1tag = 255;		//最近发送的m1消息标签

	BOOL				m_bM6Ok = FALSE;	//M6发送成功
	BOOL				m_bM2Ok = FALSE;	//M2发送成功
	//BOOL				m_bM1Ok = FALSE;	//M1发送成功

	int					m_target = 0;		//目标点
	int					m_optcode = 0;		//操作码

	CPoint				m_curXY;			//当前点坐标


	IO_OPERATION_DATA	m_iIO;			//读扩展重叠结构数据结构
	IO_OPERATION_DATA	m_oIO;			//写扩展重叠结构数据结构

	CServerDlg* m_pMainDlg;	//主窗口指针

protected:
	SOCKET				m_s;			//套接字
	

public:
	BOOL		AsyncRecvE1();					//接收E1
	void		OnRecvE1Completed(DWORD dwIOSize);	//接收E1完毕

	BOOL		AsyncSendM6(char* buf, char* bufm2, char* bufm1);//发送M6
	BOOL		AsyncSendM2(char* buf);				//发送M2
	BOOL		AsyncSendM1(char* buf);				//发送M1
	//void		OnSendM6Completed();				//发送M6完毕
	//void		OnSendM2Completed();				//发送M2完毕
	//void		OnSendM1Completed();				//发送M1完毕
	void		GetCurrentXY(CClientContext* pClient, CPoint& pt);	//获取小车当前坐标

private:
	void		GetCarInfo(char* buf);				//获取小车信息
	BOOL		JudgeChangeAndLockPoint();			//判断小车位置或状态是否变化并解锁当前点
	BOOL		GetRoute(int ptSrc, int ptDest);	//获取小车行走路径
	void		SetM1Buf();							//将计算所得的段号赋值给buf
	void		ShowAGV();							//显示小车
	BOOL		GetOverlappedHandle(HANDLE** arr, int& len, std::vector<UINT16>& tasksnap);//获取与已有任务的重叠点
	BOOL		GetOverlappedPoint(HANDLE** arr, int& len);					//获取与小车当前点的重叠点

	char		m_m2buf[M2_BUFFSIZE];				//缓存的M2消息
	char		m_m1buf[M1_BUFFSIZE];				//缓存的M1消息
	std::vector<int> m_vecRoute;					//小车行走路线

private:
	CRITICAL_SECTION m_csCDC;						//DC锁
};