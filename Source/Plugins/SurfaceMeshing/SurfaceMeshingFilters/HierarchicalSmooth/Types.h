// Copyright (c) 2016-2018, Siddharth Maddali 
// All rights reserved. 
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are met: 
// 
//  * Redistributions of source code must retain the above copyright notice, 
//    this list of conditions and the following disclaimer. 
//  * Redistributions in binary form must reproduce the above copyright 
//    notice, this list of conditions and the following disclaimer in the 
//    documentation and/or other materials provided with the distribution. 
//  * Neither the name of Carnegie Mellon University nor the names of its 
//    contributors may be used to endorse or promote products derived from 
//    this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
// POSSIBILITY OF SUCH DAMAGE. 
/*
 * Types.h -- contains all user-defined types.
 */

#ifndef _HSMOOTH_TYPES
#define _HSMOOTH_TYPES

#include <iostream>
#include <unordered_map>
#include <vector>
#include <tuple>
#include <functional>
#include <cstdlib>

#include "Eigen/Eigen"
#include "Eigen/Sparse"
#include "Eigen/IterativeLinearSolvers"

#define _CIRC_		sizeof( std::size_t ) * 4

// NOTE: 'int' is typedefed in iostream as unsigned long long, or something.

/*
 * trimesh:
 * Eigen array of integer triplets; the prototype of Delaunay 
 * triangulations in this library.
 */
typedef Eigen::Array< int, Eigen::Dynamic, 3 > trimesh;

/* 
 * meshnode:
 * Eigen array of float triplets, each column representing 
 * a 3D cartesian mesh node.
 */
typedef Eigen::Matrix< double, 3, Eigen::Dynamic > meshnode;

/* 
 * facelabel:
 * This data type is analogous to Dream.3D's FaceLabels property
 * and represents a grain boundary patch by specifying the 
 * grain IDs on either side of the patch. 
 */
typedef Eigen::Array< int, Eigen::Dynamic, 2 > facelabel;

/*
 * nodetype:
 * Dream.3D-specific dataset which indicates the type of node 
 * in a surface mesh: interior, triple junction or quad junction
 * ( denoted by 2, 3, 4 respectively if on the interior and 
 * 12, 13, 14 if on the volume surface.
 */
typedef Eigen::Array< int, Eigen::Dynamic, 1 > nodetype;

/*
 * is_smoothed:
 * Boolean array specifying whether each node has been smoothed 
 * or not. At the beginning, only the nodetypes 4 and 14 should 
 * be initialized to true, the others should be false. 
 */
typedef Eigen::Array< bool, Eigen::Dynamic, 1 > is_smoothed;

/*
 * matindex:
 * Special typedef of an Eigen vector of integers to indicate
 * an array of indices, to be used in slicing with libigl.
 * Also, need to set data type int and not int because of 
 * compatibility with libigl.
 */
typedef Eigen::Matrix< int, Eigen::Dynamic, 1 > matindex;



/*
 * EdgePair, EdgeList:
 * Bookkeeping devices for edges in a Delaunay mesh, each 
 * edge being represented by an ordered pair of integers.
 */
typedef std::pair< int, int > EdgePair;
typedef std::vector< EdgePair > EdgeList;

/*
 * EdgePairEqual:
 * Comparison function for two different EdgePairs, used in custom hash function.
 */
struct EdgePairEqual {
	bool operator()( const EdgePair& lhs, const EdgePair& rhs ) const {
		return ( lhs.first == rhs.first ) && ( lhs.second == rhs.second );
	}
};

/*
 * The actual hash function
 */
namespace std {
	template<> struct hash< EdgePair > {
		std::size_t operator()( const EdgePair& EP ) const { 
			return std::hash< std::size_t >{}( (std::size_t)EP.first << _CIRC_ | (std::size_t)EP.second );
		}
	};
}

/*
 * Customized dictionary that takes EdgePairs as keys.
 */
template< typename T >
struct DictBase {
	typedef std::unordered_map< EdgePair, T, std::hash< EdgePair >, EdgePairEqual > EdgeDict;
};
/*
 * The dictionary initialization happens like this:
 *
 * DictBase< your-type >::EdgeDict my_dict;
 *
 */


/* 
 * SpMat:
 * Shorthand for Eigen's sparse matrix type.
 *
 * T:
 * Triplet containing a position indices for 
 * a single sparse matrix element (i, j ) and 
 * the floating point value at that position.
 * Defined in Eigen/Sparse
 *
 * SpMatMask:
 * Boolean mask for type SpMat
 *
 * bT:
 * Equivalent of T for boolean variables.
 */
typedef Eigen::SparseMatrix<double>	SpMat;
typedef Eigen::Triplet<double>		T;
typedef Eigen::SparseMatrix< bool > SpMatMask;
typedef Eigen::Triplet< bool >		bT;

/*
 * Typedef for the conjugate gradient solver for sparse systems.
 */
typedef Eigen::ConjugateGradient< SpMat, Eigen::Upper|Eigen::Lower > Smoother;

#endif
