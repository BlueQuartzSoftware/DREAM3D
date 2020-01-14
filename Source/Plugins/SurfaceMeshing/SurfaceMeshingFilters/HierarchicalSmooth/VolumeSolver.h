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
 * VolumeSolver.h -- contains the definition of the object that takes the 
 * surface mesh data of the entire volume and returns an array containing 
 * hierarchical-smoothed mesh nodes. This object does all the necessary 
 * bookkeeping with node types, connectivity and smoothed status.
 */

#ifndef _HSMOOTH_VOLSOLVER
#define _HSMOOTH_VOLSOLVER

#include <iostream>
#include <fstream>

#include "Eigen/Eigen"

#include "Types.h"
#include "Base.h"
#include "Triangulation.h"
#include "HierarchicalSmooth.h"

#include "igl/slice.h"	// for slicing operations on Eigen matrix types

namespace base 		= HSmoothBase;
namespace smooth	= HSmoothMain;
namespace tri		= HSmoothTri;

namespace VolumeSolver {

	class VolumeSolver {
	
		public:
		// constructor
		VolumeSolver( trimesh&, meshnode&, facelabel&, nodetype&, int=53 );
			// the last integer default is the number of bisections in each call
			// to the core smoothing routine. Obtained from a typical machine 
			// zero value of ~10^16. 
			// TODO: Generalize this to work for machine zero of the specific 
			// machine in use.

		// smoother
		meshnode HierarchicalSmooth( bool=false, std::string="Smooth.Default.log" );

		// writers
		meshnode GetSmoothed( void ) { return vsNodeSmooth; }
		is_smoothed GetNodeSmoothStatus( void ) {return Status; }

		private:
		// member objects; all these are instantiated in the constructor
		is_smoothed Status;
		trimesh vsMesh;
		meshnode vsNode, vsNodeSmooth;
		facelabel vsLabel;
		nodetype vsType;
		int MaxIterations;
		double fError, fErrorThreshold;
		std::ofstream fout;	// log file handle
		DictBase< std::vector< int > >::EdgeDict vsBoundaryDict;


		// member functions
		trimesh SliceMesh( std::vector< int >& );
		void MarkSectionAsComplete( matindex& );

		//scratch
		matindex one, three;

	};

}

#endif
