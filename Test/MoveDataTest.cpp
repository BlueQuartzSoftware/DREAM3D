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
    AttributeMatrix::Pointer am1 = AttributeMatrix::New(QVector<size_t>(1), "AttributeMatrix1", 0);
    AttributeMatrix::Pointer am2 = AttributeMatrix::New(QVector<size_t>(1), "AttributeMatrix2", 0);
    AttributeMatrix::Pointer am3 = AttributeMatrix::New(QVector<size_t>(1), "AttributeMatrix3", 0);
    IDataArray::Pointer da1 = DataArray<int>::CreateArray(3, "DataArray1");
    da1->setName("DataArray1");
    
    dc1->addAttributeMatrix("AttributeMatrix1", am1);
    dc1->addAttributeMatrix("AttributeMatrix2", am2);
    dc2->addAttributeMatrix("AttributeMatrix3", am3);
    
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
    
    MoveData::Pointer ptr = MoveData::New();
    ptr->setDataContainerArray(dca);
    ptr->setWhatToMove(k_MoveAttributeMatrix);
    
    // Testing "Destination Data Container Does Not Exist" (Error Code -11011)
    ptr->setDataContainerDestination("ThisDataContainerShouldNotExist");
    DataArrayPath amSource("DataContainer1", "AttributeMatrix1", "");
    ptr->setAttributeMatrixSource(amSource);
    ptr->execute();
    int err = ptr->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err, DC_DEST_NOT_FOUND)
    
    // Testing "Source Data Container Does Not Exist" (Error Code -11012)
    ptr->setDataContainerDestination("DataContainer2");
    ptr->setAttributeMatrixSource(DataArrayPath("ThisDataContainerShouldNotExist", "AttributeMatrix1", ""));
    ptr->execute();
    int err2 = ptr->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err2, DC_SRC_NOT_FOUND)
    
    // Testing "Source Attribute Matrix Does Not Exist" (Error Code -11013)
    ptr->setDataContainerDestination("DataContainer2");
    ptr->setAttributeMatrixSource(DataArrayPath("DataContainer1", "ThisAttributeMatrixShouldNotExist", ""));
    ptr->execute();
    int err3 = ptr->getErrorCondition();
    DREAM3D_REQUIRE_EQUAL(err3, AM_SRC_NOT_FOUND)
    
    DataArrayPath daSrcPath("", "", "");
    IDataArray::Pointer daSrcDataArray = dca->getExistingPrereqArrayFromPath<IDataArray,AbstractFilter>(ptr, daSrcPath);
    
/************************************Test************************************/
    
//    IDataArray::Pointer daSrcDataArray = getDataContainerArray()->getExistingPrereqArrayFromPath<IDataArray, AbstractFilter>(this, daSrcPath);
//    if (getErrorCondition() < 0)
//    {
//        return;
//    }

/************************************Test************************************/
    
//    AttributeMatrix::Pointer daDestAttributeMatrix = getDataContainerArray()->getPrereqAttributeMatrixFromPath<DataContainer>(this, amDestPath, -11016);

/************************************Test************************************/
    
//    if (daDestAttributeMatrix->getNumTuples() != daSrcDataArray->getNumberOfTuples())
//        setErrorCondition(-11019);
//        QString ss = QObject::tr("The number of tuples of source and destination do not match");

/************************************Test************************************/
    
//    else if (daSrcAttributeMatrix->getName() == daDestAttributeMatrix->getName())
//        QString ss = QObject::tr("The source and destination Attribute Matrix are the same.  Is this what you meant to do?");
}


// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( MoveDataTest() )
  PRINT_TEST_SUMMARY();

  return err;
}


