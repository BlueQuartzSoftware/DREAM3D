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
/* Triangulation.cpp -- implementation of routines declared in Triangulation.h */

#include "Triangulation.h"

//===================================================================================

HSmoothTri::Triangulation::Triangulation( trimesh& inTri ) {

	Mesh = inTri;
	std::tuple< EdgeList, EdgeList > GetAllEdges = GetEdges( Mesh );
	edge_list = std::get<0>( GetAllEdges );
	free_boundary = std::get<1>( GetAllEdges );
	differentiateFaces();

}

//===================================================================================

trimesh HSmoothTri::Triangulation::connectivityList( void ) {
	return Mesh;
}

//===================================================================================

EdgeList HSmoothTri::Triangulation::allEdges( void ) {
	return edge_list;
} 

//===================================================================================

std::tuple< EdgeList, EdgeList > HSmoothTri::Triangulation::freeBoundary( void ) {
	return std::make_tuple( free_boundary, free_boundary_segments );
}

//===================================================================================
void HSmoothTri::Triangulation::differentiateFaces( void ) {
	int start = std::get<0>( free_boundary[0] );
	std::vector< int > thissec{ 0 };
	int n = 1;
	while( n < free_boundary.size() ) {
		if( std::get<1>( free_boundary[n] ) == start ) {
			thissec.push_back( n );
			free_boundary_segments.push_back( std::make_pair( thissec[0], thissec[1] ) );
			thissec.clear();
		}
		else if( thissec.size() == 0 ) { 
			start = std::get<0>( free_boundary[n] );
			thissec.push_back( n );
		}
		n++;
	}
	return;
}

//===================================================================================

/*
 * NOTE: 
 * The GetEdges method defined here contains code that is extraneous to its 
 * primary function, in order to lay the groundwork for a much faster method 
 * that computes the graph Laplacian, than the standalone function defined in 
 * HierarchicalSmooth.cpp. This method, internal to the Triangulation object 
 * is implemented in the hierarchical smooth algorithm while the standalone 
 * is provided for choice.
 */

std::tuple< EdgeList, EdgeList > HSmoothTri::Triangulation::GetEdges( trimesh& inTri ) {

	for( int i = 0; i < inTri.rows(); i++ ) 
		for( int j = 0; j < inTri.cols(); j++ ) 
		nUnique.push_back( inTri( i, j ) );
	std::sort( nUnique.begin(), nUnique.end() );
	nUnique.erase( std::unique( nUnique.begin(), nUnique.end() ), nUnique.end() );

	fDiagCount = std::vector< double >( nUnique.size(), 0.0 );
	nSubTri = HSmoothBase::ismember( inTri, nUnique );

	for( int i = 0; i < nSubTri.rows(); i++ )  {
		for( int j = 0; j < 3; j++ ) {
			int l = ( j+3 ) % 3;
			int m = ( j+4 ) % 3;
			int this_row = nSubTri( i, l );
			int this_col = nSubTri( i, m );
			EdgePair EP = std::make_pair( std::min( this_row, this_col ), std::max( this_row, this_col ) );
			DictBase< EdgeCount >::EdgeDict::iterator got = MyDict.find( EP );
			if( got == MyDict.end() ) {	// not found yet; this is a new edge.
				EdgeCount EC( nUnique[ this_row ], nUnique[ this_col ] );
				MyDict.insert( { EP, EC } );
				fDiagCount[ this_row ] += 1.0;
				fDiagCount[ this_col ] += 1.0;

			}
			else { 						// this is definitely an interior edge.
				EdgeCount& ec = got->second;
				(ec.ncount)++;
			}
		}
	}

	for( DictBase< EdgeCount >::EdgeDict::iterator it = MyDict.begin(); it != MyDict.end(); ++it ) {
		edge_list.push_back( ( it->second ).orig_pair );
		if( ( it->second ).ncount == 1 )
			free_boundary.push_back( ( it->second ).orig_pair );
	}
	return std::make_tuple( edge_list, FastChainLinkSort( free_boundary ) );
}

//===================================================================================

EdgeList HSmoothTri::Triangulation::FastChainLinkSort( EdgeList& inList ) {
	std::unordered_map< int, std::vector< int > > WindingDict;
	for( int i = 0; i < inList.size(); i++ ) {
		int ltemp = std::get<0>( inList[i] );
		int rtemp = std::get<1>( inList[i] );
		std::unordered_map< int, std::vector< int > >::iterator got = WindingDict.find( ltemp );
		if( got == WindingDict.end() ) {
			std::vector< int > v{ rtemp };	// yet another way to initialize a vector!
			WindingDict.insert( { ltemp, v } );
		}
		else {
			std::vector< int >& vtemp = got->second;
			vtemp.push_back( rtemp );
		}
	}
	EdgeList outList;
	std::unordered_map< int, std::vector< int > >::iterator it = WindingDict.begin();
	while( WindingDict.size() > 0 ) {			// decimate the dictionary as chain-linked list is generated.
		int next = ( it->second ).back();	// pop backwards; as good a way to fill as any!
		EdgePair ptemp = std::make_pair( it->first, next );
		outList.push_back( ptemp );
		( it->second ).pop_back();
		if( ( it->second ).size() == 0 ) WindingDict.erase( it );
		it = WindingDict.find( next );
		if( it == WindingDict.end() ) it = WindingDict.begin();
	}
	return outList;
}

//===================================================================================

std::tuple< SpMat, matindex > HSmoothTri::Triangulation::GraphLaplacian( void ) {
	// most of the work already done in method GetEdges
	std::vector< T > tripletList;	
	tripletList.reserve( nUnique.size() + 2*Mesh.rows()*Mesh.cols() );
	for( DictBase< EdgeCount >::EdgeDict::iterator it = MyDict.begin(); it != MyDict.end(); ++it ) {
		int l = std::get<0>( it->first );
		int m = std::get<1>( it->first );
		tripletList.push_back( T( l, m, -1.0 ) );
		tripletList.push_back( T( m, l, -1.0 ) );
	}
	for( int i = 0; i < fDiagCount.size(); i++ )
		tripletList.push_back( T( i, i, fDiagCount[i] ) );

	SpMat GL = SpMat( nUnique.size(), nUnique.size() );
	GL.setFromTriplets( tripletList.begin(), tripletList.end() );
	GL.makeCompressed();

	return std::make_tuple( GL, HSmoothBase::getindex( nUnique ) );
}

//===================================================================================
