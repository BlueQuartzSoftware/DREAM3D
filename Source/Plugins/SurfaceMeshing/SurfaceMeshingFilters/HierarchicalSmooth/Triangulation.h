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
 * Triangulation.h -- contains a simple implementation of the 
 * 'triangulation' object from Matlab. Modeled from the Python 
 * prototype written by Siddharth Maddali. Customized for use 
 * in hierarchical smooth and doesn't have the full range of 
 * applicability as its Matlab counterpart. In particular, 
 * there is no functionality to computer mesh node-related 
 * quantities like local face and node normals. This will be 
 * added later as needed.
 */

#ifndef _HSMOOTH_TRI
#define _HSMOOTH_TRI

#include "Types.h"
#include "Base.h"


struct EdgeCount {
	EdgePair orig_pair;
	int ncount;
	EdgeCount( int x, int y ) {
		orig_pair = std::make_pair( x, y );
		ncount = 1;		// i.e. one edge already found at time of instantiation.
	}
};

namespace HSmoothTri {

	class Triangulation {
		public:
			Triangulation() {}					// default constructor
			Triangulation( trimesh& );			// not taking points for now; this will come later.
				
			trimesh connectivityList( void );
			EdgeList allEdges( void );
			std::tuple< EdgeList, EdgeList > freeBoundary( void );		// in proper winding order!
			std::tuple< SpMat, matindex > GraphLaplacian( void );

		private:
			// member objects
			trimesh Mesh, nSubTri;				// the Delaunay triangulation from which everything is derived
			EdgeList edge_list, free_boundary, free_boundary_segments;
			std::vector< int > nUnique;
			DictBase< EdgeCount >::EdgeDict MyDict;
			std::vector< double > fDiagCount;
	
			// member functions
			std::tuple< EdgeList, EdgeList > GetEdges( trimesh& );
			EdgeList FastChainLinkSort( EdgeList& );
			void differentiateFaces( void );

	};
}

#endif
