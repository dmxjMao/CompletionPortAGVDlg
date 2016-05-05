#pragma once

#include <list>
//#include <forward_list>
#include <vector>
#include <map>
#include "TableEntry.h"

using std::vector;
using std::list;
//using std::map;
//using std::make_pair;


#define VERT_DISTANCE   (0x0fffffff)

#define VET_BEGIN   \
    list <Vertex> lst;  \
    Vertex te;  \
    memset(&te, 0, sizeof(Vertex)); \

#define VET_HEAD(nom)  {   \
    memset(&te, 0, sizeof(Vertex)); \
    lst.clear();    \
    te.Dist = VERT_DISTANCE; \
    te.Known = Unknown; \
    te.Path = Unpath;   \
    te.vertexNo = nom; \
    te.isAdjunct = false;    \
	te.busy = tEasy;    \
    lst.push_front(te);  \
    m_vertex.push_front(te);    \
}


#define VET_ADJ(nom)  {   \
    memset(&te, 0, sizeof(Vertex)); \
    te.Dist = VERT_DISTANCE; \
    te.Known = Unknown; \
    te.Path = Unpath;   \
    te.vertexNo = nom; \
    te.isAdjunct = true;    \
	te.busy = tEasy;    \
    lst.push_back(te);  \
}

#define VET_END {   \
    m_baseGraph.push_front(lst);    \
}


class Graph
{
public:
    Graph();
    ~Graph();
private:
    list <list<Vertex> > m_baseGraph;
	int m_startVertex;
	list<Vertex> m_vertex;
	HANDLE*		 m_pHandle;
	//map<SIDE, unsigned> m_sideNo;

public:
	void InitHandle();
	BOOL Dijkstra(int ptSrc, int ptDest);	//����·��
	void GetTaskSnap(vector<UINT16>& taskSnap); //��ȡ�������

private:
	int UnweightedGraph(Vertex * start);
	int UnweightedGraph(Vertex * start, Vertex * end);
	int setStartVertex(Vertex * startVertex);
	int findAdjunctList(Vertex * V);
	list <list<Vertex> >::iterator findAdjunctList(list<Vertex>::iterator V);
	int setDist(Vertex * vertex);
	list<list<Vertex> >::iterator findV(list<Vertex>::iterator W);
	int printPath(list<Vertex>::iterator vertex);
	int printPath(Vertex * vertex);

	//int generateSideNo();
	//inline vector<int>& GetRoute() { return m_vecRoute; }
	//inline map<SIDE, unsigned>& GetSideNoMap() { return m_sideNo; }

	void ResetVertex(); // ���ù�ϵ�����ֻ��2-3������3-2������
	void SetVertexBusy(int vertexNo, Busy b);	// ��������Ϊ���û�ռ��
    list<Vertex>::iterator findV2(list<Vertex>::iterator W);
    int setDist2(Vertex * vertex);
    //int copyNewList(list<Vertex> & newList);

public:
	std::map<std::pair<int, int>, std::vector<int>>  m_casheRoute; //����·��
	std::map<UINT16, std::vector<int>> m_casheTask;	//��������
	vector<int>		m_vecRoute;						//����·��
	std::map<int, HANDLE>	m_mapHandle;
};

