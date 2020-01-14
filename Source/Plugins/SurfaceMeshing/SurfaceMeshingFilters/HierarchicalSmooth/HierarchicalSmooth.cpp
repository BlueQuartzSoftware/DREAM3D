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
 * HierarchicalSmooth.cpp -- implementation of the routines declared in HierarchicalSmooth.h
 *
 */

#include "HierarchicalSmooth.h"


//============================================================================================

SpMat HSmoothMain::Laplacian2D( int N, std::string type ) {

	std::vector< T > tripletList;
	tripletList.reserve( 3*N ); // approx. number of nonzero elements
	for( int i = 0; i < N; i++ ) {
		tripletList.push_back( T( i, i, -1.0 ) );
		if( i != N-1 )
			tripletList.push_back( T( i, i+1, 1.0 ) );
	}
	SpMat temp( N, N );
	temp.setFromTriplets( tripletList.begin(), tripletList.end() );
	SpMat L = SpMat( temp.transpose() ) * temp;
	if( type=="serial" ){ 
		L.coeffRef( N-1, N-1 ) = 1.0;
	}
	else if( type=="cyclic" ) { 
		L.coeffRef( 0, 0 ) = 2.0;
		L.coeffRef( 0, N-1 ) = L.coeffRef( N-1, 0 ) = -1.0;
	}
	else
		std::cerr << "HSmoothMain::Laplacian2D: Unrecognized type. " << std::endl;

	L.makeCompressed();
	return L;
}

//============================================================================================

std::tuple< SpMat, std::vector< int> > HSmoothMain::GraphLaplacian( trimesh& tri ) { 

	std::vector< int > nUnique;
	for( int i = 0; i < tri.rows(); i++ ) 
		for( int j = 0; j < tri.cols(); j++ ) 
			nUnique.push_back( tri( i, j ) );
	std::sort( nUnique.begin(), nUnique.end() );
	nUnique.erase( std::unique( nUnique.begin(), nUnique.end() ), nUnique.end() );

	std::vector< T > tripletList;
	tripletList.reserve( nUnique.size() + tri.rows()*tri.cols()*2 );

	std::vector< double > fDiagCount( nUnique.size(), 0.0 );

	trimesh nSubTri = HSmoothBase::ismember( tri, nUnique );

	DictBase< int >::EdgeDict MyDict;
	for( int i = 0; i < nSubTri.rows(); i++ ) {
		for( int j = 0; j < 3; j++ ) {
			int l = ( j + 3 ) % 3;
			int m = ( j + 4 ) % 3;
			int this_row = nSubTri(i,l); 
			int this_col = nSubTri(i,m);
			EdgePair EP = std::make_pair( std::min( this_row, this_col ), std::max( this_row, this_col ) );	
			DictBase< int >::EdgeDict::const_iterator got = MyDict.find( EP );
			if( got == MyDict.end() ) {				// not found yet...
				MyDict.insert( { EP, i } );			// i.e. the edge, and one of the triangles it belongs to.
				tripletList.push_back( T( this_row, this_col, -1.0 ) );
				tripletList.push_back( T( this_col, this_row, -1.0 ) );
				fDiagCount[ this_row ] += 1.0;
				fDiagCount[ this_col ] += 1.0;
			}

		}
	}

	for( int i = 0; i < fDiagCount.size(); i++ )
		tripletList.push_back( T( i, i, fDiagCount[i] ) );

	SpMat MLap( nUnique.size(), nUnique.size() );
	MLap.setFromTriplets( tripletList.begin(), tripletList.end() );
	MLap.makeCompressed();
	return std::make_tuple( MLap, nUnique );
}

//============================================================================================

meshnode HSmoothMain::Smooth( meshnode& NodesIn, std::string type, double fThresh, int nIter ) {
	SpMat L = Laplacian2D( NodesIn.cols(), type );
	std::vector< int > vidx;
	if( type=="serial" )
		vidx = std::vector< int >{ (int)0, (int)(L.cols()-1) };
	else
		vidx = std::vector< int >{};
	matindex nFixed = HSmoothBase::getindex( vidx );
	return Smooth( NodesIn, nFixed, L, fThresh, nIter );
}

//============================================================================================

meshnode HSmoothMain::Smooth( meshnode& NodesIn, matindex& nFixed, SpMat& GL, double fThresh, int nIter ) {
	matindex nMobile = HSmoothBase::getcomplement( nFixed, GL.cols() );	
	if( nMobile.size() == 0 )
		return NodesIn;

	SpMat Data = NodesIn.transpose().sparseView();

	SpMat GLRed, fConst, D, A, AyIn, yMobile, fSmallEye, LTL, LTK, yOut;

	std::tuple< SpMat, SpMat > dbvp = GetDirichletBVP( GL, Data, nFixed, nMobile );
	GLRed = std::get<0>( dbvp );
	fConst = std::get<1>( dbvp );

	std::tuple< SpMat, SpMat > pieces = AnalyzeLaplacian( GL );
	D = std::get<0>( pieces );
	A = std::get<1>( pieces );

	AyIn = A * Data;
	Eigen::MatrixXd mtemp = Eigen::MatrixXd::Zero( nMobile.size(), nMobile.size() );
	mtemp.setIdentity();
	fSmallEye = mtemp.sparseView();
	fSmallEye.makeCompressed();
	igl::slice( Data, nMobile, 1, yMobile );
	LTL = SpMat( GLRed.transpose() * GLRed );
	LTK = SpMat( GLRed.transpose() * fConst );	// casting as SpMat to make column-major
	yOut = Data;

	Smoother smth;

	double fEps = 0.5;
	double fStep = fEps / 2.0;
	int nCount = 1;

	double fobj1, fobj2, fslope;
	fobj1 = GetObjFn( smth, fEps, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut ); 	// only the last parameter is actually modified
	fobj2 = GetObjFn( smth, fEps+fThresh, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut );
	fslope = ( fobj2 - fobj1 ) / fThresh;

	while( fabs( fslope ) < fThresh && nCount < nIter ) {
		if( fStep > 0.0 )
			fEps -= fStep;
		else
			fEps += fStep;

		fEps /= 2.0;
		fobj1 = GetObjFn( smth, fEps, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut ); 	// only the last parameter is actually modified
		fobj2 = GetObjFn( smth, fEps+fThresh, fSmallEye, LTL, LTK, Data, nMobile, yMobile, D, AyIn, yOut );
		fslope = ( fobj2 - fobj1 ) / fThresh;
		nCount++;
	}

	return Eigen::MatrixXd( yOut.transpose() );


}

//============================================================================================
	
std::tuple< SpMat, SpMat > HSmoothMain::GetDirichletBVP( SpMat& GL, SpMat& yIn, matindex& nFixed, matindex& nMobile ) {

	matindex nAll = HSmoothBase::matunion( nFixed, nMobile );
	std::vector< int > v;
	for( int i = 0; i < yIn.cols(); i++ ) v.push_back( i );
	matindex dims = HSmoothBase::getindex( v );

//	SpMat GLRed, sm1, sm2, sm3, fConst;
	SpMat	GLRed( nMobile.size(), nMobile.size() ), 
			sm1( nAll.size(), nFixed.size() ), 
			sm2( nFixed.size(), dims.size() ), 
			sm3( nAll.size(), dims.size() ), 
			fConst( nMobile.size(), dims.size() );
	
	igl::slice( GL, nMobile, nMobile, GLRed );		// thank goodness for igl::slice!
	igl::slice( GL, nAll, nFixed, sm1 );
	igl::slice( yIn, nFixed, dims, sm2 );
	sm3 = sm1*sm2;
	igl::slice( sm3, nMobile, dims, fConst );

	return std::make_tuple( GLRed, fConst );
}

//============================================================================================

std::tuple< SpMat, SpMat > HSmoothMain::AnalyzeLaplacian( SpMat& GL ) {	// NOTE: GL should be column-major

	SpMat D( GL.rows(), GL.cols() ), A( GL.rows(), GL.cols() );
	std::vector< T > DT, AT;
	
	for( int k = 0; k < GL.outerSize(); ++k ) {
		for( SpMat::InnerIterator it( GL, k ); it; ++it ) {
			if( it.value() < -0.5 )
				AT.push_back( T( it.row(), it.col(), it.value() ) );
			else if( it.value() > 0.5 )
				DT.push_back( T( it.row(), it.col(), it.value() ) );
		}
	}
	D.setFromTriplets( DT.begin(), DT.end() );
	A.setFromTriplets( AT.begin(), AT.end() );

	D.makeCompressed();
	A.makeCompressed();

	return std::make_tuple( D, A );
	
}

//============================================================================================

double HSmoothMain::GetObjFn( Smoother& smth, double feps, 
		SpMat& fSmallEye, SpMat& LTL, SpMat& LTK, 
		SpMat& Data, matindex& nMobile, SpMat& yMobile, 
		SpMat& D, SpMat& AyIn, SpMat& yOut ) {

	SpMat ySmooth; 
	
	SpMat A = ( 1.0 - feps )*fSmallEye	+ feps*LTL;
	SpMat b = ( 1.0 - feps )*yMobile	- feps*LTK;


	smth.compute( A );			// smth in general changes with each function call
	ySmooth = smth.solve( b );

	HSmoothBase::merge( ySmooth, yOut, nMobile );

	Eigen::ArrayXXd yDeltaD = Eigen::MatrixXd( D*yOut + AyIn ).array();

	return ( yDeltaD * yDeltaD ).sum();	// more efficient to calculate trace this way

}













