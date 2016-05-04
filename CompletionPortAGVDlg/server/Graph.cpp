#include "stdafx.h"
#include "Graph.h"
#include <functional>  //bind
#include <algorithm>   //find

// 全局地图
Graph g_Graph;
BYTE TAG = 0;    // 消息标签 1-255循环使用
UINT16 TASK = 0; // 任务号
//extern UINT16 TASK;

//static int no = 0;

//找到m_vertex中指定点
bool FindVertex(const Vertex& v, const Vertex& w) {
	return (v.vertexNo == w.vertexNo);
}

//找到m_vertex中指定点
bool FindVertexByVertexNo(const Vertex& v, const int vertexNo) {
	return (v.vertexNo == vertexNo);
}

Graph::Graph()
    : m_startVertex(0)
{
    VET_BEGIN;  

#include "AGVmap_1.h"      
	//SetVertexBusy(2, tBusy);
	m_pHandle = new HANDLE[m_vertex.size() + 1];
	InitHandle();
}


Graph::~Graph()
{
	delete[] m_pHandle; m_pHandle = nullptr;
}

void Graph::InitHandle()
{
	size_t vertexNum = m_vertex.size();
	for (size_t i = 0; i < vertexNum + 1; ++i) {
		m_pHandle[i] = CreateEvent(nullptr, TRUE, TRUE, nullptr);
		m_mapHandle[(int)i] = m_pHandle[i];
	}
}

/*
* 计算路径
*/
BOOL Graph::Dijkstra(int ptSrc, int ptDest)
{
	m_vecRoute.clear();

	Vertex te;
	list<Vertex> newList;

	te.vertexNo = ptSrc;
	UnweightedGraph(&te);

	memset(&te, 0, sizeof(Vertex));
	te.vertexNo = ptDest;
	printPath(&te);
	m_vecRoute.push_back(ptDest);

	ResetVertex(); // 重置顶点状态
	m_casheTask[TASK] = m_vecRoute; //加入当前地图的任务

	return TRUE;
}

int Graph::UnweightedGraph(Vertex * start)
{
    // We find out the shortest path inside m_baseGraph from start point 'start'
    UnweightedGraph(start, start);
    return 0;
}


int Graph::UnweightedGraph(Vertex * start, Vertex * end)
{
    // We find out the shortest path inside m_baseGraph from start point 'start' to 'end' point
    int CurrDist = 0;
    list<Vertex>::iterator V, W;
    
    TRACE("Enter UnweightedGraph\n");
    setStartVertex(start);
    TRACE("Start vertex is(%d)\n", start->vertexNo);

    for (CurrDist = 0; CurrDist < NumVertext; CurrDist++)
    {
        TRACE("CurrDist=(%4d)\n", CurrDist);
        for (list<Vertex>::iterator r = m_vertex.begin(); r != m_vertex.end(); ++r)
        // for (list<list<Vertex> >::iterator r = m_baseGraph.begin(); r != m_baseGraph.end(); r++)
        {
            {
                // V = r->begin();
                V = r;
                TRACE("Link Table Head(%d)Known(%s)Dist(%d=>%d)Path(%d)\n", (V->vertexNo), (V->Known==Known)?"Known":"Unknown", V->Dist, CurrDist, V->Path);
                if (V->Known == Unknown && V->Dist == CurrDist/* && V->busy == tEasy*/)
                {
                    TRACE("We got V(%d)Dist(%d)\n", V->vertexNo, V->Dist);
                    V->Known = Known;
                    list<list<Vertex> >::iterator adjunctList = findAdjunctList(V);
                    for (list<Vertex>::iterator adj = adjunctList->begin(); adj != adjunctList->end(); ++adj)
                    {
                        list<Vertex>::iterator v = findV2(adj);
                        W = v;
                        if (W->Dist == VERT_DISTANCE)
                        {
                             W->Dist = CurrDist + 1;
                             W->Path = V->vertexNo;

                            TRACE("We set adjunction V(%d):W(%d) with Dist(%d)Path(%d)\n", V->vertexNo, W->vertexNo, W->Dist, V->vertexNo);
                        }
                    }
                }
            }
        }
    }


    return 0;
}


int Graph::setStartVertex(Vertex * startVertex)
{

	using std::placeholders::_1;
	auto it = std::find_if(m_vertex.begin(), m_vertex.end(),
		std::bind(FindVertex, _1, *startVertex));

	it->Dist = 0;
	m_startVertex = it->vertexNo;

    return 0;
}


int Graph::findAdjunctList(Vertex * V)
{
    return 0;
}


list <list<Vertex> >::iterator Graph::findAdjunctList(list<Vertex>::iterator V)
{
    for (list<list<Vertex> >::iterator r = m_baseGraph.begin(); r != m_baseGraph.end(); ++r)
    {
        list<Vertex>::iterator startVertex = r->begin();
        if (startVertex->vertexNo == V->vertexNo)
        {
            return r;
        }
    }

    // list<Vertex> l;
    // list <list<Vertex> >::iterator ll = find(m_baseGraph.begin(), m_baseGraph.end(), l);

    return list <list<Vertex> >::iterator();
}


int Graph::setDist(Vertex * vertex)
{
    for (list<list<Vertex> >::iterator r = m_baseGraph.begin(); r != m_baseGraph.end(); r++)
    {
        // list<Vertex>::iterator s = r;
        list<Vertex>::iterator it = r->begin();
        // for (list<Vertex>::iterator vit = s.begin(); vit != s.end(); vit++)
        {
            if (vertex->vertexNo == it->vertexNo)
            {
                it->Dist = vertex->Dist;
                it->Path = vertex->Path;
                // it->Known = Unknown;
                return 0;
            }
        }
    }
    return 0;
}


list<list<Vertex> >::iterator Graph::findV(list<Vertex>::iterator W)
{
    for (list<list<Vertex> >::iterator r = m_baseGraph.begin(); r != m_baseGraph.end(); r++)
    {
        list<Vertex>::iterator startVertex = r->begin();
        if (startVertex->vertexNo == W->vertexNo)
        {
            return r;
        }
    }
    return list<list<Vertex> >::iterator();
}


int Graph::printPath(list<Vertex>::iterator vertex)
{
    if (vertex->Path != Unpath)
    {
        list<list<Vertex> >::iterator v = findV(vertex);
        printPath(v->begin());
    }
    return 0;
}


int Graph::printPath(Vertex * vertex)
{
    list<Vertex>::iterator r;
    list<Vertex>::iterator it;
    Vertex vv;

    for (r = m_vertex.begin(); r != m_vertex.end(); r++)
    {
        it = r;
        if (vertex->vertexNo == it->vertexNo)
        {
            // TRACE("(%d)\n", it->vertexNo);
            break;
        }
    }

    if (it->vertexNo == m_startVertex)
    {
        TRACE("Source(%d)\n", m_startVertex);
		//it->busy = tBusy;
		//m_vecRoute.push_back(m_startVertex);
        return 0;
    }

	//it->busy = tBusy;
	//m_vecRoute.push_back(it->vertexNo);

    memset(&vv, 0, sizeof(Vertex));
    vv.vertexNo = it->Path;
    vv.Path = it->Path;
    if (vv.Path != Unknown)
    {
        // TRACE("to(%d)\n", it->Path);
		//it->busy = tBusy;
        printPath(&vv);
        // TRACE(" to");
    }
    //TRACE("(%d)\n", vv.vertexNo);
	m_vecRoute.push_back(vv.vertexNo);

    return 0;
}


list<Vertex>::iterator Graph::findV2(list<Vertex>::iterator W)
{
	using std::placeholders::_1;
	auto it = std::find_if(m_vertex.begin(), m_vertex.end(),
		std::bind(FindVertex, _1, *W));

	if (m_vertex.end() != it) {
		return it;
	}

    return list<Vertex>::iterator();
}


int Graph::setDist2(Vertex * vertex)
{
    for (list<Vertex>::iterator r = m_vertex.begin(); r != m_vertex.end(); r++)
    {
        // list<Vertex>::iterator s = r;
        list<Vertex>::iterator it = r;
        // for (list<Vertex>::iterator vit = s.begin(); vit != s.end(); vit++)
        {
            if (vertex->vertexNo == it->vertexNo)
            {
                it->Dist = vertex->Dist;
                it->Path = vertex->Path;
                // it->Known = Unknown;
                return 0;
            }
        }
    }
    return 0;
}


//int Graph::copyNewList(list<Vertex> & newList)
//{
//    std::copy(m_vertex.begin(), m_vertex.end(), std::back_inserter(newList));
//
//    return 0;
//}



//int Graph::generateSideNo()
//{
//	static unsigned SIDE_NO = 1;
//	// 遍历所有顶点
//	for (auto verIter = m_vertex.begin(); verIter != m_vertex.end(); ++verIter)
//	{
//		int vertexNo = (*verIter).vertexNo;
//		// 找到邻居表
//		auto& adjList = *findAdjunctList(verIter);
//
//		// 遍历每个邻居，给段编号并保存
//		auto adjIter = adjList.begin();
//		std::advance(adjIter, 1); // 跳过顶点
//		for (; adjIter != adjList.end(); ++adjIter)
//		{
//			int sideNo = (*adjIter).vertexNo;
//			// 先查找有没有这个段：（1，2）和（2，1）相同
//			auto findIter = m_sideNo.find(make_pair(sideNo, vertexNo));
//			if (m_sideNo.end() == findIter)
//			{ // 没有找到
//				auto sidePair = make_pair(vertexNo, sideNo);
//				m_sideNo.insert(make_pair(sidePair, SIDE_NO++));
//			}
//			else
//			{
//				auto sidePair = make_pair(vertexNo, sideNo);
//				m_sideNo.insert(make_pair(sidePair, findIter->second));
//			}
//		}
//	}
//	return(0);
//}

void Graph::ResetVertex()
{
	// list<Vertex>
	for(auto& vertex : m_vertex)
	{ 
		vertex.Dist = VERT_DISTANCE;
		vertex.Known = Unknown;
		vertex.Path = Unpath;
		//vertex.busy = tBusy;
	}
}


/*
* 将顶点设为闲置或占用
*/
void Graph::SetVertexBusy(int vertexNo, Busy b)
{
	using std::placeholders::_1;
	auto it = std::find_if(m_vertex.begin(), m_vertex.end(),
		std::bind(FindVertexByVertexNo, _1, vertexNo));

	it->busy = b;
}