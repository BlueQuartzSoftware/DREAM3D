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
 * VolumcSolver.cpp:
 * Defines classes and routines declared in VolumeSolver.h
 */

#include "VolumeSolver.h"


//=======================================================================================

VolumeSolver::VolumeSolver::VolumeSolver( 
	trimesh& VolumeMesh, meshnode& SurfaceNodes, 
	facelabel& FLabels, nodetype& NodeType, 
	int nIterations ) {

	// loading primary data into solver
	vsMesh = VolumeMesh;
	vsNode = SurfaceNodes;
	vsLabel = FLabels;
	vsType = NodeType;
	MaxIterations = nIterations;

	Status = is_smoothed( vsType.size() );
	for( int i = 0; i < vsType.size(); i++ )
		Status( i ) = ( vsType(i) % 10 == 4 );	// quad jn points considered already smoothed.

	std::vector< int > vtemp{ 0, 1, 2 };
	std::vector< int > vtemp2{ 0 };

	one = base::getindex( vtemp2 );
	three = base::getindex( vtemp );

	vsNodeSmooth = vsNode;

	fError = ( vsNode.col( vsMesh( 0, 0 ) ).array() - vsNode.col( vsMesh( 0, 1 ) ).array() ).matrix().norm();
	fError = std::min( fError, 
			( vsNode.col( vsMesh( 0, 1 ) ).array() - vsNode.col( vsMesh( 0, 2 ) ).array() ).matrix().norm()
		);
	fError = sqrt( 3.0 * fError * fError );
	fErrorThreshold = 2.0;
	
	// Filling in boundary dictionary. This also takes care of 
	// flipping the direction of a mesh element as computed by 
	// Dream.3d so that every element has the same hendedness.
	for( int i = 0; i <  vsLabel.rows(); i++ ) {
		int this_min = std::min( vsLabel( i, 0 ), vsLabel( i, 1 ) );
		int this_max = std::max( vsLabel( i, 0 ), vsLabel( i, 1 ) );
		if( vsLabel( i, 0 ) == this_min )  {	// this line ensures consistent handedness for entire surface
			int temp = vsMesh( i, 0 );
			vsMesh( i, 0 ) = vsMesh( i, 1 );
			vsMesh( i, 1 ) = temp;
		}
		EdgePair this_pair = std::make_pair( this_min, this_max );
		DictBase< std::vector< int > >::EdgeDict::iterator got = vsBoundaryDict.find( this_pair );
		if( got == vsBoundaryDict.end() ) {		// new boundary, new dictionary entry
			std::vector< int > v;
			v.push_back( i );
			vsBoundaryDict.insert( { this_pair, v } );
		}
		else {									// this patch belongs to an already found boundary
			std::vector< int >& myref = got->second;
			myref.push_back( i );
		}
	}
}

//=======================================================================================

meshnode VolumeSolver::VolumeSolver::HierarchicalSmooth( bool logging, std::string logfile ) {
	fout.open( logfile.c_str() );
	std::ostream& outfile = ( logging ? fout : std::cout );

	int ncount = 1;

	for( 
		DictBase< std::vector< int > >::EdgeDict::iterator it = vsBoundaryDict.begin(); 
		it != vsBoundaryDict.end(); 
		++it 
	) {
//		outfile << "Boundary " << ncount<< " of " << vsBoundaryDict.size() << '\n';
		trimesh triSub = SliceMesh( it->second );
		tri::Triangulation T( triSub );
		
		std::tuple< SpMat, matindex > Topology = T.GraphLaplacian();
	 	SpMat GL = std::get<0>( Topology );
		matindex nUniq = std::get<1>( Topology );
		
		std::tuple< EdgeList, EdgeList > FreeBndData = T.freeBoundary();
		EdgeList FB = std::get<0>( FreeBndData );
		EdgeList fbsec = std::get<1>( FreeBndData );

		for( int i = 0; i < fbsec.size(); i++ ) {	// smooth each free boundary first
			int start, stop, count;
			start = std::get<0>( fbsec[i] );
			stop = std::get<1>( fbsec[i] );
			for( count = start; count <= stop; count++ ) 
				if( vsType( std::get<0>( FB[ count ] ) ) % 10 == 4 )
					break;
			std::vector< int > vtemp;
			if( count > stop ) {	// no quad jns in this free boundary. smooth without constraints and get out.
				for( count = start; count <= stop; count++ ) vtemp.push_back( std::get<0>( FB[ count ] ) );
				matindex thisFreeBoundaryIdx = base::getindex( vtemp );
				meshnode thisFreeBoundary;
				igl::slice( vsNodeSmooth, three, thisFreeBoundaryIdx, thisFreeBoundary );
				meshnode thisFreeBoundarySmooth = smooth::Smooth( thisFreeBoundary, std::string( "cyclic" ) );
				base::merge( thisFreeBoundarySmooth, vsNodeSmooth, thisFreeBoundaryIdx );
				MarkSectionAsComplete( thisFreeBoundaryIdx );
			}
			else {					// triple line sections found; smooth separately.
				vtemp.push_back( std::get<0>( FB[ count ] ) );
				int thissize = 1 + ( stop - start );
				for( int j = count+1; j < 1 + count + thissize; j++ ) {
					int effective_j = j % thissize;
					vtemp.push_back( std::get<0>( FB[ effective_j ] ) );
					if( vsType( std::get<0>( FB[ effective_j ] ) ) % 10 == 4 ) {	// reached terminal quad point
						matindex thisTripleLineIndex = base::getindex( vtemp );
						is_smoothed thisStatus;
						igl::slice( Status, thisTripleLineIndex, one, thisStatus );
						if( !thisStatus.all() ) {
							meshnode thisTripleLine, thisTripleLineSmoothed; 
							igl::slice( vsNodeSmooth, three, thisTripleLineIndex, thisTripleLine );
							thisTripleLineSmoothed = smooth::Smooth( thisTripleLine );
							base::merge( thisTripleLineSmoothed, vsNodeSmooth, thisTripleLineIndex ); /* HAVEN'T CHECKED FOR BUGS */
							MarkSectionAsComplete( thisTripleLineIndex );
						}
						vtemp.clear();
						vtemp.push_back( std::get<0>( FB[ effective_j] ) );
					}
				}
			}
		}
		// NOW, smooth entire boundary subject to fixed triple points.
		meshnode BoundaryNode;
		igl::slice( vsNodeSmooth, three, nUniq, BoundaryNode );
		std::vector< int > fixed; 
		for( int i = 0; i < FB.size(); i++ ) fixed.push_back( std::get<0>( FB[i] ) );
		matindex nFixed = base::getindex( fixed, nUniq );
		meshnode BoundaryNodeSmooth = smooth::Smooth( BoundaryNode, nFixed, GL );
		base::merge( BoundaryNodeSmooth, vsNodeSmooth, nUniq );
		MarkSectionAsComplete( nUniq );
		ncount++;
	// Done. Get out.
	}
	
	Eigen::ArrayXXd fTemp = vsNodeSmooth.array() - vsNode.array();
	Eigen::ArrayXXd fNorm = ( fTemp * fTemp ).colwise().sum().sqrt() / fError;
	if( ( fNorm > fErrorThreshold ).any() ) {
		for( int i = 0; i < Status.rows(); i++ )
			if( fNorm( 0, i ) > fErrorThreshold ) {
				Status( i, 0 ) = false;
				vsNodeSmooth.col( i ) << vsNode.col( i );	// reset to old values.
			}
	}

	if( !Status.all() ) 
		outfile 	<< "WARNING: " 
					<< ( fNorm > fErrorThreshold ).count() << " of " 
					<< vsNodeSmooth.cols() << " nodes not smoothed. " 
					<< "Query VolumeSolver::Status for more information. "
					<< '\n';
	else
		outfile << "All nodes smoothed. " << '\n';
	fout.close();
	return vsNodeSmooth;
}

//=======================================================================================

trimesh VolumeSolver::VolumeSolver::SliceMesh( std::vector< int >& FromThesePatches ) {
	trimesh triSub;
	igl::slice( vsMesh, base::getindex( FromThesePatches ), three, triSub );
	return triSub;
}

//=======================================================================================

void VolumeSolver::VolumeSolver::MarkSectionAsComplete( matindex& idx ) {
	for( int i = 0; i < idx.size(); i++ )
		Status( idx( i ) ) = true;
	return;
}

//=======================================================================================















