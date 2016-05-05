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
		UNKNOWN = 0,	// δ֪ AGV����ϵͳδ�ܶ�����С������Ϣ��С��δ���뵽ϵͳ��
		FREE,			// ���� ����δ����������
		ASSIGNMENTED,	// �ѷ��� �����ѱ���������
		LINKED,			// ������ һ����С���Ѿ�����װ��վ̨�����ػ�״̬
		CANCELED,		// ȡ�� С�����ڼ�ͣ���ֶ�״̬
		//DISCONN			// С������
		STATEBUF
	};

public:
	int					m_carno = 0;			//С������
	int					m_m1tag = 0;			//m1��Ϣ��ǩ
	int					m_m2tag = 0;			//m2��Ϣ��ǩ
	int					m_m6tag = 0;			//m6��Ϣ��ǩ
	int					m_curDist = 0;			//��ǰ����
	int					m_curSideno = 0;		//��ǰ�κ�
	int					m_curPoint = 0;			//��ǰ���
	state				m_eState = UNKNOWN;		//С��״̬
	int					m_errcode = 0;			//������
	int					m_curSpeed = 0;			//��ǰ�ٶ�
	int					m_optCompleted = 0;		//�ƶ�&�������
	int					m_curTaskno = 0;		//��ǰ�����

	CTime				m_time;			//����ʱ��
	BOOL				m_bChange;		//С��λ�û�״̬�����仯
	int					m_prevPoint = 0;		//֮ǰ���
	state				m_prevState = STATEBUF;	//֮ǰ״̬
	int					m_prevTaskno = -1;		//֮ǰ�����

	int				m_Sendm6tag = 255;		//������͵�m6��Ϣ��ǩ
	int				m_Sendm2tag = 255;		//������͵�m2��Ϣ��ǩ
	int				m_Sendm1tag = 255;		//������͵�m1��Ϣ��ǩ

	BOOL				m_bM6Ok = FALSE;	//M6���ͳɹ�
	BOOL				m_bM2Ok = FALSE;	//M2���ͳɹ�
	//BOOL				m_bM1Ok = FALSE;	//M1���ͳɹ�

	int					m_target = 0;		//Ŀ���
	int					m_optcode = 0;		//������

	CPoint				m_curXY;			//��ǰ������


	IO_OPERATION_DATA	m_iIO;			//����չ�ص��ṹ���ݽṹ
	IO_OPERATION_DATA	m_oIO;			//д��չ�ص��ṹ���ݽṹ

	CServerDlg* m_pMainDlg;	//������ָ��

protected:
	SOCKET				m_s;			//�׽���
	

public:
	BOOL		AsyncRecvE1();					//����E1
	void		OnRecvE1Completed(DWORD dwIOSize);	//����E1���

	BOOL		AsyncSendM6(char* buf, char* bufm2, char* bufm1);//����M6
	BOOL		AsyncSendM2(char* buf);				//����M2
	BOOL		AsyncSendM1(char* buf);				//����M1
	//void		OnSendM6Completed();				//����M6���
	//void		OnSendM2Completed();				//����M2���
	//void		OnSendM1Completed();				//����M1���
	void		GetCurrentXY(CClientContext* pClient, CPoint& pt);	//��ȡС����ǰ����

private:
	void		GetCarInfo(char* buf);				//��ȡС����Ϣ
	BOOL		JudgeChangeAndLockPoint();			//�ж�С��λ�û�״̬�Ƿ�仯��������ǰ��
	BOOL		GetRoute(int ptSrc, int ptDest);	//��ȡС������·��
	void		SetM1Buf();							//���������õĶκŸ�ֵ��buf
	void		ShowAGV();							//��ʾС��
	BOOL		GetOverlappedHandle(HANDLE** arr, int& len, std::vector<UINT16>& tasksnap);//��ȡ������������ص���
	BOOL		GetOverlappedPoint(HANDLE** arr, int& len);					//��ȡ��С����ǰ����ص���

	char		m_m2buf[M2_BUFFSIZE];				//�����M2��Ϣ
	char		m_m1buf[M1_BUFFSIZE];				//�����M1��Ϣ
	std::vector<int> m_vecRoute;					//С������·��

private:
	CRITICAL_SECTION m_csCDC;						//DC��
};