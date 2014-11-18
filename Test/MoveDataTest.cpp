///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2007, 2010 Michael A. Jackson for BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//  This code was written under United States Air Force Contract number
//                           FA8650-04-C-5229
//
///////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#ifdef _MSC_VER
#include <cstdlib>


/*
 "It's a known, long-standing bug in the compiler system's headers.  For
 some reason the manufacturer, in its infinite wisdom, chose to #define
 macros min() and max() in violation of the upper-case convention and so
 break any legitimate functions with those names, including those in the
 standard C++ library."
 */

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define WINDOWS_LARGE_FILE_SUPPORT
#if _MSC_VER < 1400
#define snprintf _snprintf
#else
#define snprintf sprintf_s
#endif

#endif


//-- C++ includes
#include <iostream>

#include <boost/assert.hpp>

#include "DREAM3DLib/Utilities/UnitTestSupport.hpp"
#include "TestFileLocations.h"

#include "DREAM3DLib/CoreFilters/RemoveArrays.h"
#include "DREAM3DLib/CoreFilters/RenameDataContainer.h"
#include "DREAM3DLib/CoreFilters/MoveData.h"
#include "DREAM3DLib/CoreFilters/CopyDataContainer.h"

static const int k_MoveAttributeMatrix = 0;
static const int k_MoveDataArray = 1;

static const int DC_DEST_NOT_FOUND = -11011;
static const int DC_SRC_NOT_FOUND = -11012;
static const int AM_SRC_NOT_FOUND = -11013;
static const int TUPLES_NOT_MATCH = -11019;


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer createDataContainerArray()
{
    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc1 = DataContainer::New("DataContainer1");
    DataContainer::Pointer dc2 = DataContainer::New("DataContainer2");
    AttributeMatrix::Pointer am1 = AttributeMatrix::New(QVector<size_t>(3,2), "AttributeMatrix1", 0);
    AttributeMatrix::Pointer am2 = AttributeMatrix::New(QVector<size_t>(7,2), "AttributeMatrix2", 0);
    AttributeMatrix::Pointer am3 = AttributeMatrix::New(QVector<size_t>(4,3), "AttributeMatrix3", 0);
    AttributeMatrix::Pointer am4 = AttributeMatrix::New(QVector<size_t>(7,2), "AttributeMatrix4", 0);
    IDataArray::Pointer da1 = DataArray<int>::CreateArray(8, "DataArray1");
    IDataArray::Pointer da2 = DataArray<int>::CreateArray(128, "DataArray2");
    IDataArray::Pointer da3 = DataArray<int>::CreateArray(128, "DataArray3");
    IDataArray::Pointer da4 = DataArray<int>::CreateArray(81, "DataArray4");
    IDataArray::Pointer da5 = DataArray<int>::CreateArray(81, "DataArray5");
    
    am1->addAttributeArray("DataArray1", da1);
    am2->addAttributeArray("DataArray2", da2);
    am2->addAttributeArray("DataArray3", da3);
    am3->addAttributeArray("DataArray4", da4);
    am3->addAttributeArray("DataArray5", da5);
    
    dc1->addAttributeMatrix("AttributeMatrix1", am1);
    dc1->addAttributeMatrix("AttributeMatrix2", am2);
    dc2->addAttributeMatrix("AttributeMatrix3", am3);
    dc2->addAttributeMatrix("AttributeMatrix4", am4);
    
    dca->addDataContainer(dc1);
    dca->addDataContainer(dc2);
    
    return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveDataTest()
{
    DataContainerArray::Pointer dca = createDataContainerArray();
    
    MoveData::Pointer moveDataPtr = MoveData::New();
    moveDataPtr->setDataContainerArray(dca);
    moveDataPtr->setWhatToMove(k_MoveAttributeMatrix);
    
    // Testing "Destination Data Container Does Not Exist" (Error Code -11011)
    moveDataPtr->setDataContainerDestination("ThisDataContainerShouldNotExist");
    DataArrayPath amSource("DataContainer1", "AttributeMatrix1", "");
    moveDataPtr->setAttributeMatrixSource(amSource);
    moveDataPtr->execute();
    DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), DC_DEST_NOT_FOUND)
    
    // Testing "Source Data Container Does Not Exist" (Error Code -11012)
    moveDataPtr->setDataContainerDestination("DataContainer2");
    moveDataPtr->setAttributeMatrixSource(DataArrayPath("ThisDataContainerShouldNotExist", "AttributeMatrix1", ""));
    moveDataPtr->execute();
    DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), DC_SRC_NOT_FOUND)
    
    // Testing "Source Attribute Matrix Does Not Exist" (Error Code -11013)
    moveDataPtr->setDataContainerDestination("DataContainer2");
    moveDataPtr->setAttributeMatrixSource(DataArrayPath("DataContainer1", "ThisAttributeMatrixShouldNotExist", ""));
    moveDataPtr->execute();
    DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), AM_SRC_NOT_FOUND)
    
    // Testing "Move Attribute Matrix to a Destination Data Container" (Check that it works)
    moveDataPtr->setAttributeMatrixSource(DataArrayPath("DataContainer1", "AttributeMatrix1", ""));
    moveDataPtr->setDataContainerDestination("DataContainer2");
    moveDataPtr->execute();
    DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), 0)
    
    moveDataPtr->setWhatToMove(k_MoveDataArray);
    
    // Testing Creation of Data Array pointer (Error Code < 0)
    moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "AttributeMatrix1", "ThisShouldNotExist"));
    moveDataPtr->execute();
    DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)
    
    moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "ThisShouldNotExist", "DataArray1"));
    moveDataPtr->execute();
    DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)
    
    moveDataPtr->setDataArraySource(DataArrayPath("ThisShouldNotExist", "AttributeMatrix1", "DataArray1"));
    moveDataPtr->execute();
    DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)
    
    moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "AttributeMatrix1", "DataArray1"));
    
    // Testing Creation of Attribute Matrix Pointer (Error Code < 0)
    moveDataPtr->setAttributeMatrixDestination(DataArrayPath("DataContainer2", "ThisShouldNotExist", ""));
    moveDataPtr->execute();
    DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)
    
    moveDataPtr->setAttributeMatrixDestination(DataArrayPath("ThisShouldNotExist", "AttributeMatrix3", ""));
    moveDataPtr->execute();
    DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)
    
    // Testing numTuples Comparison
    moveDataPtr->setAttributeMatrixDestination(DataArrayPath("DataContainer2", "AttributeMatrix3", ""));
    moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "AttributeMatrix2", "DataArray3"));
    moveDataPtr->execute();
    DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), TUPLES_NOT_MATCH)
    
    // Testing "Move Data Array to a Destination Attribute Matrix" (Check that it works)
    moveDataPtr->setAttributeMatrixDestination(DataArrayPath("DataContainer2", "AttributeMatrix4", ""));
    moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "AttributeMatrix2", "DataArray3"));
    moveDataPtr->execute();
    DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void CopyDataTest()
{
    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataTest()
{
    
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveDataTest()
{
    
}

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
    int err = EXIT_SUCCESS;
    
    DREAM3D_REGISTER_TEST( MoveDataTest() )
    DREAM3D_REGISTER_TEST( CopyDataTest() )
    DREAM3D_REGISTER_TEST( RenameDataTest() )
    DREAM3D_REGISTER_TEST( RemoveDataTest() )
    
    PRINT_TEST_SUMMARY();
    
    return err;
}


