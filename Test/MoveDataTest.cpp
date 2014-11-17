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
    AttributeMatrix::Pointer am3 = AttributeMatrix::New(QVector<size_t>(1), "AttributeMatrix2", 0);
    IDataArray::Pointer da1 = DataArray<int>::createArray();
    da1->setName("DataArray1");
    dca->addDataContainer(dc1);
    dca->addDataContainer(dc2);

    std::cout << "Testing";
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MoveDataTest()
{
    createDataContainerArray();
    //   MXAAbstractAttributes allAttributes;
    //   err = QH5Utilities::readAllAttributes(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", allAttributes );
    //   DREAM3D_REQUIRE(err >= 0);
    //   DREAM3D_REQUIRE(allAttributes.size() == AttrSize);
    //   DREAM3D_REQUIRE(H5Fclose(file_id) >= 0);
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


