/***
 * Name:	Node.h
 * Purpose:	m3c.basics.Node Implementation
 * Notice:  Copyright Stuart Golodetz, 2008. All rights reserved.
* This work is licensed under the Creative Commons Attribution-NonCommercial 3.0
* Unported License. To view a copy of this license, visit
* http://creativecommons.org/licenses/by-nc/3.0/
* or send a letter to Creative Commons,
* 171 Second Street, Suite 300,
* San Francisco, California, 94105, USA.
 ***/

#ifndef H_m3c_BASICS_NODE
#define H_m3c_BASICS_NODE

#include <cassert>
#include <set>

#include "MXA/Common/MXASetGetMacros.h"
#include <m3c/basics/math/vectors/Vector3.h>

namespace m3c { namespace basics {

template <typename Label>
class Node
{
	//#################### TYPEDEFS ####################
private:
	typedef m3c_basics::math_vectors::VectorType VectorType;

	//#################### PUBLIC VARIABLES ####################
public:
	M3C_SHARED_POINTERS(Node<Label>)
	M3C_CONTAINER_TYPE(Node<Label>, std::vector);

	bool valid;
	VectorType position;
	std::set<Label> labels;
	std::set<int> adjacentNodes;	// the global node indices of any nodes adjacent to this one
	std::set<int> triangles; // The indices of the triangles that this Node belongs to

	//#################### CONSTRUCTORS ####################
public:
	Node() : valid(false)
	{
		assert(false && "This constructor should never be invoked: it exists only to make storing Nodes in a std::map easier.");
	}

	explicit Node(const VectorType& position_) : valid(true), position(position_) {}
};

}}

#endif
