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
 *
 * Base.h -- Contains basic function definitions 
 *
 */

#ifndef _HSMOOTH_BASE
#define _HSMOOTH_BASE

#include "igl/slice.h"

#include "Types.h"


namespace HSmoothBase{ 

/* ismember:
 * Mimics the basic functionality of Matlab's 'ismember' function, currently only for 
 * integer arrays with 3 columns because it is used on triangulations.
 */
	trimesh ismember( trimesh& Array1, std::vector<int>& Array2 );

/* getindex:
 * Returns a matindex object (defined in Types.h) of integers given an 
 * STL std::vector< int > object. Variant returns position of each integer 
 * with respect to a user-supplied matindex.
 */
	matindex getindex( std::vector< int >& );
	matindex getindex( std::vector< int >&, matindex& );

/* getcomplement:
 * Given a matindex M and an integer N > max( M ), returns a matindex containing 
 * { integer i | i < N and i not in M }. Used to find indices of mobile nodes 
 * given the foxed ones and vice versa.
 */
	matindex getcomplement( matindex&, int );

/* matunion:
 * Returns the set union of the indexes in the two input matindexes, 
 * sorted in ascending order.
 */
	matindex matunion( matindex&, matindex& );


/* merge:
 * Copies row data from source array into target array at the locations specified 
 * by the input matindex. NOTE: Source array should have same number of rows 
 * as elements in the matindex. Two versions are provided, for sparse and dense.
 */
	void merge( meshnode&, meshnode&, matindex& );
	void merge( SpMat&, SpMat&, matindex& );
}



#endif


