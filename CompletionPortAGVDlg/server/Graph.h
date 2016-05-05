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
	BOOL Dijkstra(int ptSrc, int ptDest);	//计算路径
	void GetTaskSnap(vector<UINT16>& taskSnap); //获取任务快照

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

	void ResetVertex(); // 重置关系，解决只能2-3，不能3-2的问题
	void SetVertexBusy(int vertexNo, Busy b);	// 将顶点设为闲置或占用
    list<Vertex>::iterator findV2(list<Vertex>::iterator W);
    int setDist2(Vertex * vertex);
    //int copyNewList(list<Vertex> & newList);

public:
	std::map<std::pair<int, int>, std::vector<int>>  m_casheRoute; //缓存路径
	std::map<UINT16, std::vector<int>> m_casheTask;	//缓存任务
	vector<int>		m_vecRoute;						//行走路线
	std::map<int, HANDLE>	m_mapHandle;
};

