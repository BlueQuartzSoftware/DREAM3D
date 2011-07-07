/***
 * Name:	Edge.cpp
 * Purpose:	m3c.construction.Edge Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#include "Edge.h"

namespace m3c {

//#################### GLOBAL OPERATORS ####################
bool operator<(const Edge& lhs, const Edge& rhs)
{
	// This is written this way to ensure that Edge(u,v) and Edge(v,u) are equivalent in associative containers.
	const int& lSmaller = lhs.u < lhs.v ? lhs.u : lhs.v;
	const int& lLarger = lhs.u > lhs.v ? lhs.u : lhs.v;
	const int& rSmaller = rhs.u < rhs.v ? rhs.u : rhs.v;
	const int& rLarger = rhs.u > rhs.v ? rhs.u : rhs.v;
	return	(lSmaller < rSmaller) ||
			(lSmaller == rSmaller && lLarger < rLarger);
}

}
