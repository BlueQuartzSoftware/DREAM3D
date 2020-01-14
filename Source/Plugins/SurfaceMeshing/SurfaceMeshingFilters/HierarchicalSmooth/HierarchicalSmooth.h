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
 * HierarchicalSmooth.h -- Contains main and auxiliary routine prototypes 
 * for smoothing according to the hierarchical filter method.
 *
 */

#ifndef _HSMOOTH_HSMOOTH
#define _HSMOOTH_HSMOOTH

#include <cmath>

#include "Types.h"
#include "Base.h"

namespace HSmoothMain{ 

	SpMat Laplacian2D( int N, std::string type="serial" );	// the options are 'serial' and 'cyclic'.
	std::tuple< SpMat, std::vector< int> > GraphLaplacian( trimesh& tri );	// multiple returns

	meshnode Smooth( meshnode&, std::string="serial", double=0.001, int=53 );
	meshnode Smooth( meshnode&, matindex&, SpMat&, double=0.001, int=53 );

	std::tuple< SpMat, SpMat > GetDirichletBVP( SpMat&, SpMat&, matindex&, matindex& );
	std::tuple< SpMat, SpMat > AnalyzeLaplacian( SpMat& );

	double GetObjFn( Smoother&, double, SpMat&, SpMat&, SpMat&, SpMat&, matindex&, SpMat&, SpMat&, SpMat&, SpMat& );

}

#endif



