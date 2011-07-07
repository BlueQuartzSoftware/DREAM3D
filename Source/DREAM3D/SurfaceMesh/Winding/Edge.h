/***
 * Name:	Edge.h
 * Purpose:	m3c.construction.Edge Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#ifndef H_m3c_CONSTRUCTION_EDGE
#define H_m3c_CONSTRUCTION_EDGE

#include <set>

#include "MXA/Common/MXASetGetMacros.h"



namespace m3c {

class Edge
{
  public:

    MXA_SHARED_POINTERS(Edge);
    static Pointer New(int u_, int v_)
    {
      Pointer sharedPtr (new Edge(u_, v_));
      return sharedPtr;
    }

	//#################### PUBLIC VARIABLES ####################
	int u, v;
	std::set<int> triangles;

	uint64_t getId() {
	  uint64_t edgeId;
	  int32_t* e0 = (int32_t*)(&edgeId);
	  int32_t* e1 = e0+1;
	  if (u < v)
	  {
	    *e0 = u;
	    *e1 = v;
	  }
	  else
	  {
	    *e0 = v;
	    *e1 = u;
	  }
	  return edgeId;
	}


	//#################### CONSTRUCTORS ####################
	Edge(int u_, int v_) : u(u_), v(v_) {}

  private:
//	  Edge(const Edge&); // Copy Constructor Not Implemented
    void operator=(const Edge&); // Operator '=' Not Implemented
};

//#################### GLOBAL OPERATORS ####################
bool operator<(const Edge& lhs, const Edge& rhs);

}

#endif
