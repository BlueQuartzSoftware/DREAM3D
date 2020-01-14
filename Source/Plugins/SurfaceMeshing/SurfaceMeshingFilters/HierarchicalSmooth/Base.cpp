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
 * Base.cpp -- implementation of routines in Base.h
 *
 */

#include "Base.h"

// int is the same as unsigned long

//============================================================================================

trimesh HSmoothBase::ismember( trimesh& Array1, std::vector<int>& Array2 ) {
	
	std::unordered_map< int, int > MyDict; // dictionary lookup for faster access
	for( int i = 0; i < Array2.size(); i++ ) 
		MyDict.insert( { Array2[i], i } );
	trimesh NewTri = trimesh::Zero( Array1.rows(), Array1.cols() );
	// running through Array1 in column-major order, Eigen's default
	for( int col = 0; col < Array1.cols(); col++ ) {
		for( int row = 0; row < Array1.rows(); row++ ) {
			std::unordered_map<int,int>::const_iterator got = MyDict.find( Array1( row, col ) );
			NewTri( row, col ) = got->second;
		}
	}
	return NewTri;
}

//============================================================================================

matindex HSmoothBase::getindex( std::vector< int >& FromThis ) {
	matindex I( FromThis.size() );
	for( int i = 0; i < FromThis.size(); i++ )
		I( i ) = FromThis[i];

	return I;
}

//============================================================================================

matindex HSmoothBase::getindex( std::vector< int >& FromThis, matindex& InThis ) {
	std::unordered_map< int, int > dict;
	for( int i = 0; i < InThis.rows(); i++ ) dict.insert( { InThis( i ), i } );
	std::vector< int > vtemp;
	for( int i = 0; i < FromThis.size(); i++ ) {
		std::unordered_map< int, int >::iterator got = dict.find( FromThis[i] );
		vtemp.push_back( got->second );
	}
	return getindex( vtemp );
}

//============================================================================================

matindex HSmoothBase::getcomplement( matindex& nSet, int N ) {
	matindex nAll = -1 * matindex::Ones( N, 1 );
	for( int i = 0; i < nSet.size(); i++ )
		nAll( nSet( i ) ) = nSet( i );
	std::vector< int > nComplement;
	for( int i = 0; i < nAll.size(); i++ ) 
		if( nAll( i ) < 0 ) nComplement.push_back( i );

	return getindex( nComplement );
}

//============================================================================================

matindex HSmoothBase::matunion( matindex& mat1,  matindex& mat2 ) {
	std::vector< int > v;
	for( int i = 0; i < mat1.size(); i++ ) v.push_back( mat1( i ) );
	for( int i = 0; i < mat2.size(); i++ ) v.push_back( mat2( i ) );
	std::sort( v.begin(), v.end() );
	v.erase( std::unique( v.begin(), v.end() ), v.end() );

	return getindex( v );
}

//============================================================================================

void HSmoothBase::merge( meshnode& Source, meshnode& Target, matindex& Locations ) {
	for( int i = 0; i < Source.cols(); i++ )
		Target.col( Locations( i ) ) << Source.col( i );
	return;
}

//============================================================================================

void HSmoothBase::merge( SpMat& Source, SpMat& Target, matindex& Locations ) {
	meshnode src = Eigen::MatrixXd( Source ).transpose();
	meshnode trg = Eigen::MatrixXd( Target ).transpose();
	merge( src, trg, Locations );

	Target = trg.transpose().sparseView();
	Target.makeCompressed();
	return;
}
