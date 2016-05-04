#pragma once

#include <list>
using std::list;

#define NumVertext  (25)

typedef int DisType;
typedef int PATH;
typedef struct Vertex * List;
//typedef std::pair<int, int> SIDE;

typedef enum _Knowing {
	Unknown,
    Known,  
}Knowing;

typedef enum _busy {
	tEasy,
	tBusy,
}Busy;

typedef enum _Pathing {
    Unpath=10000,
    Path,
}Pathing;

typedef enum _Infiniting {
    Infinity,
    Finity,
}Infiniting;

typedef struct _dotp {
    int x;
    int y;
}dotp;

typedef struct Vertex
{
    int vertexNo;
    List Header;    /* Adjacency list */

    int Known;
    DisType Dist;
    PATH Path;
    // list<dotp> *ptr;

    Busy busy;  // if this section of road has a vehicle running on it.
    bool isAdjunct; // if this node is in the adjunct list.
    // list<dotp> pRoute;

    /*
     * This overrided operator function '=' will be uesed when find() is called, 
     * and overrided operator function '<' will be used when sort() is called.
     */
    bool operator==(const Vertex& g)const
    {
        if (vertexNo == g.vertexNo) 
        {
            return true;
        }
        else 
        {
            return false;
        }
    }
}Vertex;


/*---------------------------------------------------------*\

void    // Nonweight
Unweighted Table ( T )  // Assume T is initialized
{
    int CurrDist;
    Vertex V, W;

    for ( CurrDist = 0; CurrDist < NumVertext; CurrDist ++)
    {
        for each Vertext V;
        if (!T[ V ].Known && T[ V ].Dist = CurrDist)
        {
            T[ V ].Known = True;
            for each W adjanct to V
                if T[ W ].Dist = Infinity
                {
                    T[ W ].Dist = CurrDist + 1;
                    T[ W ].Path = V;
                }
        }
    }
}

\*---------------------------------------------------------*/


/*---------------------------------------------------------*\
void
InitTable( Vertex Start, Graph G, Table T)
{
    int i;

    ReadGraph( G, T );  // Read graph somehow
    for ( i = 0; i < NumVertex; i ++ )
    {
        T[ i ].Known = False;
        T[ i ].Dist = Infinity;
        T[ i ].Path = NotAVertex;
    }
    T[ Start ].dist = 0;
}



// Print shortest path to V after Dijkstra has run
// Assume that the path exists
void
PrintPath( Vertex V, Table T)
{
    if( T[ V ].Path != NotAVertex )
    {
        PrintPath( T[ V ].Path, T );
        printf( " to " );
    }
    printf( "%v", V );  // %v is pseudocode
}

void
Dijkstra( Table T )
{
    Vertex V, W;

    for ( ; ; )
    {
        V = smallest unknown distance vertext;
        if ( V == NotAVertext )
            break;

        T[ V ].Known = True;
        for each W adjacent to V
            if( !T[ W ].Known )
                if( T[ V ].Dist + Cvw < T[ W ].Dist )
                {   // Update W
                    Decrease( T[ W ].Dist to
                    T[ V ].Dist + Cvw );
                    T[ W ].Path = V;
                }
    }
}
\*---------------------------------------------------------*/


