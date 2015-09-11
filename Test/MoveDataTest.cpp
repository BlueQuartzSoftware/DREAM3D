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

#include <QtCore/QObject>

#include "SIMPLib/CoreFilters/RemoveArrays.h"
#include "SIMPLib/CoreFilters/RenameDataContainer.h"
#include "SIMPLib/CoreFilters/RenameAttributeMatrix.h"
#include "SIMPLib/CoreFilters/RenameAttributeArray.h"
#include "SIMPLib/CoreFilters/MoveData.h"
#include "SIMPLib/CoreFilters/CopyDataContainer.h"
#include "SIMPLib/CoreFilters/CopyAttributeMatrix.h"
#include "SIMPLib/CoreFilters/CopyAttributeArray.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "DREAM3DTestFileLocations.h"

#include "RemoveArraysObserver.h"

static const int k_MoveAttributeMatrix = 0;
static const int k_MoveDataArray = 1;

enum ErrorCodes
{
  DC_SELECTED_NAME_EMPTY = -999,
  DC_NEW_NAME_EMPTY = -11001,
  DC_SELECTED_NOT_FOUND = -999,
  DCA_NOT_FOUND = -11003,
  AM_NEW_NAME_EMPTY = -11004,
  AM_SELECTED_PATH_EMPTY = -999,
  RENAME_ATTEMPT_FAILED = -999,
  DC_NOT_FOUND = -999,
  AM_NOT_FOUND = -307020,
  AA_NEW_NAME_EMPTY = -11009,
  AA_SELECTED_PATH_EMPTY = -999,
  DC_DEST_NOT_FOUND = -999,
  DC_SRC_NOT_FOUND = -999,
  AM_SRC_NOT_FOUND = -307020,
  AA_NOT_FOUND = -90002,
  AA_OLD_NAME_DOES_NOT_EXIST = -11016,
  AA_NEW_NAME_EXISTS = -11017,
  TUPLES_NOT_MATCH = -11019
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainerArray::Pointer createDataContainerArray()
{
  DataContainerArray::Pointer dca = DataContainerArray::New();
  DataContainer::Pointer dc1 = DataContainer::New("DataContainer1");
  DataContainer::Pointer dc2 = DataContainer::New("DataContainer2");
  DataContainer::Pointer dc3 = DataContainer::New("DataContainer3");
  AttributeMatrix::Pointer am1 = AttributeMatrix::New(QVector<size_t>(3, 2), "AttributeMatrix1", 0);
  AttributeMatrix::Pointer am2 = AttributeMatrix::New(QVector<size_t>(7, 2), "AttributeMatrix2", 0);
  AttributeMatrix::Pointer am3 = AttributeMatrix::New(QVector<size_t>(4, 3), "AttributeMatrix3", 0);
  AttributeMatrix::Pointer am4 = AttributeMatrix::New(QVector<size_t>(7, 2), "AttributeMatrix4", 0);
  AttributeMatrix::Pointer am5 = AttributeMatrix::New(QVector<size_t>(7, 2), "AttributeMatrix5", 0);
  IDataArray::Pointer da1 = DataArray<size_t>::CreateArray(8, "DataArray1");
  IDataArray::Pointer da2 = DataArray<size_t>::CreateArray(128, "DataArray2");
  IDataArray::Pointer da3 = DataArray<size_t>::CreateArray(128, "DataArray3");
  IDataArray::Pointer da4 = DataArray<size_t>::CreateArray(81, "DataArray4");
  IDataArray::Pointer da5 = DataArray<size_t>::CreateArray(81, "DataArray5");

  am1->addAttributeArray("DataArray1", da1);
  am2->addAttributeArray("DataArray2", da2);
  am2->addAttributeArray("DataArray3", da3);
  am3->addAttributeArray("DataArray4", da4);
  am3->addAttributeArray("DataArray5", da5);

  dc1->addAttributeMatrix("AttributeMatrix1", am1);
  dc1->addAttributeMatrix("AttributeMatrix2", am2);
  dc2->addAttributeMatrix("AttributeMatrix3", am3);
  dc2->addAttributeMatrix("AttributeMatrix4", am4);
  dc3->addAttributeMatrix("AttributeMatrix5", am5);

  dca->addDataContainer(dc1);
  dca->addDataContainer(dc2);
  dca->addDataContainer(dc3);

  return dca;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setDeleteSelections(DataContainerArrayProxy& proxy)
{
  proxy.setAllFlags(Qt::Unchecked);

  QMap<QString, DataContainerProxy>& dcProxies = proxy.dataContainers;

  for (QMap<QString, DataContainerProxy>::iterator dcIter = dcProxies.begin(); dcIter != dcProxies.end(); ++dcIter)
  {
    DataContainerProxy& dcProxy =  dcIter.value();
    if (dcProxy.name == "DataContainer1")
    {
      dcProxy.flag = Qt::Checked;
    }

    QMap<QString, AttributeMatrixProxy>& amProxies = dcProxy.attributeMatricies;
    for (QMap<QString, AttributeMatrixProxy>::iterator amIter = amProxies.begin(); amIter != amProxies.end(); ++amIter)
    {
      AttributeMatrixProxy& amProxy = amIter.value();
      if (amProxy.name == "AttributeMatrix5")
      {
        amProxy.flag = Qt::Checked;
      }

      QMap<QString, DataArrayProxy>& daProxies = amProxy.dataArrays;
      for (QMap<QString, DataArrayProxy>::iterator daIter = daProxies.begin(); daIter != daProxies.end(); ++daIter)
      {
        DataArrayProxy& daProxy = daIter.value();
        if (daProxy.name == "DataArray4")
        {
          daProxy.flag = Qt::Checked;
        }
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void displayErrors(RemoveArraysObserver::Pointer ptr)
{
  QList<PipelineMessage> list = ptr->getErrorList();
  QList<PipelineMessage>::iterator iter = list.begin();

  int i = 1;
  for (; iter != list.end(); iter++)
  {
    qDebug() << "Error " << i << " - " << "Error Code: " << (*iter).getCode();
    i++;
  }
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

  // "Destination Data Container Does Not Exist" Test
  moveDataPtr->setDataContainerDestination("ThisDataContainerShouldNotExist");
  DataArrayPath amSource("DataContainer1", "AttributeMatrix1", "");
  moveDataPtr->setAttributeMatrixSource(amSource);
  moveDataPtr->execute();
  DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), DC_DEST_NOT_FOUND)

  // "Source Data Container Does Not Exist" Test
  moveDataPtr->setDataContainerDestination("DataContainer2");
  moveDataPtr->setAttributeMatrixSource(DataArrayPath("ThisDataContainerShouldNotExist", "AttributeMatrix1", ""));
  moveDataPtr->execute();
  DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), DC_SRC_NOT_FOUND)

  // "Source Attribute Matrix Does Not Exist" Test
  moveDataPtr->setDataContainerDestination("DataContainer2");
  moveDataPtr->setAttributeMatrixSource(DataArrayPath("DataContainer1", "ThisAttributeMatrixShouldNotExist", ""));
  moveDataPtr->execute();
  DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), AM_SRC_NOT_FOUND)

  // "Move Attribute Matrix" Verification Test
  moveDataPtr->setAttributeMatrixSource(DataArrayPath("DataContainer1", "AttributeMatrix1", ""));
  moveDataPtr->setDataContainerDestination("DataContainer2");
  moveDataPtr->execute();
  DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), 0)

  moveDataPtr->setWhatToMove(k_MoveDataArray);

  // "Creation of Data Array Pointer" Test
  moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "AttributeMatrix1", "ThisShouldNotExist"));
  moveDataPtr->execute();
  DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)

  moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "ThisShouldNotExist", "DataArray1"));
  moveDataPtr->execute();
  DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)

  moveDataPtr->setDataArraySource(DataArrayPath("ThisShouldNotExist", "AttributeMatrix1", "DataArray1"));
  moveDataPtr->execute();
  DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)

  // "Creation of Attribute Matrix Pointer" Test
  moveDataPtr->setAttributeMatrixDestination(DataArrayPath("DataContainer2", "ThisShouldNotExist", ""));
  moveDataPtr->execute();
  DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)

  moveDataPtr->setAttributeMatrixDestination(DataArrayPath("ThisShouldNotExist", "AttributeMatrix3", ""));
  moveDataPtr->execute();
  DREAM3D_REQUIRED(moveDataPtr->getErrorCondition(), <, 0)

  // "NumTuples Comparison" Test
  moveDataPtr->setAttributeMatrixDestination(DataArrayPath("DataContainer2", "AttributeMatrix3", ""));
  moveDataPtr->setDataArraySource(DataArrayPath("DataContainer1", "AttributeMatrix2", "DataArray3"));
  moveDataPtr->execute();
  DREAM3D_REQUIRE_EQUAL(moveDataPtr->getErrorCondition(), TUPLES_NOT_MATCH)

  // "Move Data Array" Verification Test
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
  /***** Copy Data Container *****/

  // "DataContainerArray Not Found" Test
  CopyDataContainer::Pointer copyDataContainerPtr = CopyDataContainer::New();

//    DataContainerArray::Pointer dca_not_found = DataContainerArray::NullPointer();
//    copyDataContainerPtr->setDataContainerArray(dca_not_found);
//    copyDataContainerPtr->setSelectedDataContainerName("DataContainer1");
//    copyDataContainerPtr->setNewDataContainerName("DataContainer3");
//    copyDataContainerPtr->execute();
//    DREAM3D_REQUIRE_EQUAL(copyDataContainerPtr->getErrorCondition(), DCA_NOT_FOUND)

  DataContainerArray::Pointer dca = createDataContainerArray();
  copyDataContainerPtr->setDataContainerArray(dca);

  // "Selected Data Container Name is Empty" Test
  copyDataContainerPtr->setSelectedDataContainerName("");
  copyDataContainerPtr->setNewDataContainerName("DataContainer3");
  copyDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyDataContainerPtr->getErrorCondition(), DC_SELECTED_NAME_EMPTY)

  // "New Data Container Name is Empty" Test
  copyDataContainerPtr->setSelectedDataContainerName("DataContainer1");
  copyDataContainerPtr->setNewDataContainerName("");
  copyDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyDataContainerPtr->getErrorCondition(), DC_NEW_NAME_EMPTY)

  // "Selected Data Container Does Not Exist" Test
  copyDataContainerPtr->setSelectedDataContainerName("ThisShouldNotExist");
  copyDataContainerPtr->setNewDataContainerName("DataContainer3");
  copyDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyDataContainerPtr->getErrorCondition(), DC_SELECTED_NOT_FOUND)

  // Copy Data Container Verification Test
  copyDataContainerPtr->setSelectedDataContainerName("DataContainer1");
  copyDataContainerPtr->setNewDataContainerName("DataContainer3");
  copyDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyDataContainerPtr->getErrorCondition(), 0)

  /***** Copy Attribute Matrix *****/
  CopyAttributeMatrix::Pointer copyAttrMatPtr = CopyAttributeMatrix::New();
  dca = createDataContainerArray();
  copyAttrMatPtr->setDataContainerArray(dca);

  // "New Attribute Matrix Name is Empty" Test
  copyAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "AttributeMatrix1", ""));
  copyAttrMatPtr->setNewAttributeMatrix("");
  copyAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrMatPtr->getErrorCondition(), AM_NEW_NAME_EMPTY)

  // "Selected Attribute Matrix Path is Empty" Test
  copyAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("", "", ""));
  copyAttrMatPtr->setNewAttributeMatrix("Foo");
  copyAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrMatPtr->getErrorCondition(), AM_SELECTED_PATH_EMPTY)

  // "Data Container Not Found" Test
  copyAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("ThisShouldNotBeFound", "AttributeMatrix1", ""));
  copyAttrMatPtr->setNewAttributeMatrix("AttributeMatrix10");
  copyAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrMatPtr->getErrorCondition(), DC_NOT_FOUND)

  // "Attribute Matrix Not Found" Test
  copyAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "ThisShouldNotBeFound", ""));
  copyAttrMatPtr->setNewAttributeMatrix("AttributeMatrix10");
  copyAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrMatPtr->getErrorCondition(), AM_NOT_FOUND)

  // "Copy Attribute Matrix" Verification Test
  copyAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "AttributeMatrix1", ""));
  copyAttrMatPtr->setNewAttributeMatrix("AttributeMatrix10");
  copyAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrMatPtr->getErrorCondition(), 0)

  /***** Copy Attribute Array *****/
  CopyAttributeArray::Pointer copyAttrArrayPtr = CopyAttributeArray::New();
  dca = createDataContainerArray();
  copyAttrArrayPtr->setDataContainerArray(dca);

  // "New Data Array Name Empty" Test
  copyAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix1", "DataArray1"));
  copyAttrArrayPtr->setNewArrayName("");
  copyAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrArrayPtr->getErrorCondition(), AA_NEW_NAME_EMPTY)

  // "Data Array Path Empty" Test
  copyAttrArrayPtr->setSelectedArrayPath(DataArrayPath("", "", ""));
  copyAttrArrayPtr->setNewArrayName("NewDataArrayName");
  copyAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrArrayPtr->getErrorCondition(), -90000)

  // "Data Container Not Found" Test
  copyAttrArrayPtr->setSelectedArrayPath(DataArrayPath("ThisShouldNotExist", "AttributeMatrix1", "DataArray1"));
  copyAttrArrayPtr->setNewArrayName("NewDataArrayName");
  copyAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrArrayPtr->getErrorCondition(), DC_NOT_FOUND)

  // "Attribute Matrix Not Found" Test
  copyAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "ThisShouldNotExist", "DataArray1"));
  copyAttrArrayPtr->setNewArrayName("NewDataArrayName");
  copyAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrArrayPtr->getErrorCondition(), AM_NOT_FOUND)

  // "Data Array Not Found" Test
  copyAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix1", "ThisShouldNotExist"));
  copyAttrArrayPtr->setNewArrayName("NewDataArrayName");
  copyAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrArrayPtr->getErrorCondition(), AA_NOT_FOUND)

  // "Copy Data Array" Verification Test
  copyAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix1", "DataArray1"));
  copyAttrArrayPtr->setNewArrayName("NewDataArrayName");
  copyAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(copyAttrArrayPtr->getErrorCondition(), 0)
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RenameDataTest()
{
  /***** Rename Data Container *****/
  RenameDataContainer::Pointer renameDataContainerPtr = RenameDataContainer::New();

//    DataContainerArray::Pointer dca_not_found = DataContainerArray::NullPointer();
//    renameDataContainerPtr->setDataContainerArray(dca_not_found);

//    // "DataContainerArray Is Null" Test
//    renameDataContainerPtr->setNewDataContainerName("IShouldBeAbleToRenameThis");
//    renameDataContainerPtr->setSelectedDataContainerName("DataContainer1");
//    renameDataContainerPtr->execute();
//    DREAM3D_REQUIRE_EQUAL(renameDataContainerPtr->getErrorCondition(), DCA_NOT_FOUND)

  DataContainerArray::Pointer dca = createDataContainerArray();
  renameDataContainerPtr->setDataContainerArray(dca);

  // "New Data Container Name Is Empty" Test
  renameDataContainerPtr->setNewDataContainerName("");
  renameDataContainerPtr->setSelectedDataContainerName("DataContainer1");
  renameDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameDataContainerPtr->getErrorCondition(), DC_NEW_NAME_EMPTY)

  // "Rename Attempt Failed" Test
  renameDataContainerPtr->setNewDataContainerName("DataContainer2");
  renameDataContainerPtr->setSelectedDataContainerName("DataContainer1");
  renameDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameDataContainerPtr->getErrorCondition(), -11006)

  // "Selected Data Container Does Not Exist" Test
  renameDataContainerPtr->setNewDataContainerName("IShouldBeAbleToRenameThis");
  renameDataContainerPtr->setSelectedDataContainerName("ThisNameShouldNotExist");
  renameDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameDataContainerPtr->getErrorCondition(), RENAME_ATTEMPT_FAILED)

  // "Rename Data Container" Verification Test
  renameDataContainerPtr->setNewDataContainerName("IShouldBeAbleToRenameThis");
  renameDataContainerPtr->setSelectedDataContainerName("DataContainer1");
  renameDataContainerPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameDataContainerPtr->getErrorCondition(), 0)

  /***** Rename Attribute Matrix *****/
  RenameAttributeMatrix::Pointer renameAttrMatPtr = RenameAttributeMatrix::New();

//    // "DataContainerArray Is Null" Test
//    renameAttrMatPtr->setDataContainerArray(dca_not_found);
//    renameAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "AttributeMatrix1", "DataArray1"));
//    renameAttrMatPtr->setNewAttributeMatrix("NewAttributeMatrixName");
//    renameAttrMatPtr->execute();
//    DREAM3D_REQUIRE_EQUAL(renameAttrMatPtr->getErrorCondition(), DCA_NOT_FOUND)

  dca = createDataContainerArray();
  renameAttrMatPtr->setDataContainerArray(dca);

  // "New Attribute Matrix Name is Empty" Test
  renameAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "AttributeMatrix1", ""));
  renameAttrMatPtr->setNewAttributeMatrix("");
  renameAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrMatPtr->getErrorCondition(), AM_NEW_NAME_EMPTY)

  // "Selected Attribute Matrix Path is Empty" Test
  renameAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("", "", ""));
  renameAttrMatPtr->setNewAttributeMatrix("Foo");
  renameAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrMatPtr->getErrorCondition(), AM_SELECTED_PATH_EMPTY)

  // "Data Container Not Found" Test
  renameAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("ThisShouldNotBeFound", "AttributeMatrix1", ""));
  renameAttrMatPtr->setNewAttributeMatrix("AttributeMatrix10");
  renameAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrMatPtr->getErrorCondition(), DC_NOT_FOUND)

  // "Attribute Matrix Not Found" Test
  renameAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "ThisShouldNotBeFound", ""));
  renameAttrMatPtr->setNewAttributeMatrix("AttributeMatrix10");
  renameAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrMatPtr->getErrorCondition(), AM_NOT_FOUND)

  // "Rename Attribute Matrix" Verification Test
  renameAttrMatPtr->setSelectedAttributeMatrixPath(DataArrayPath("DataContainer1", "AttributeMatrix1", ""));
  renameAttrMatPtr->setNewAttributeMatrix("RenamedAttributeMatrix");
  renameAttrMatPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrMatPtr->getErrorCondition(), 0)

  /***** Copy Attribute Array *****/
  RenameAttributeArray::Pointer renameAttrArrayPtr = RenameAttributeArray::New();
  dca = createDataContainerArray();
  renameAttrArrayPtr->setDataContainerArray(dca);

  // "New Data Array Name Empty" Test
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix1", "DataArray1"));
  renameAttrArrayPtr->setNewArrayName("");
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), AA_NEW_NAME_EMPTY)

  // "Data Array Path Empty" Test
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("", "", ""));
  renameAttrArrayPtr->setNewArrayName("NewDataArrayName");
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), AA_SELECTED_PATH_EMPTY)

  // "Data Container Not Found" Test
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("ThisShouldNotExist", "AttributeMatrix1", "DataArray1"));
  renameAttrArrayPtr->setNewArrayName("NewDataArrayName");
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), DC_NOT_FOUND)

  // "Attribute Matrix Not Found" Test
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "ThisShouldNotExist", "DataArray1"));
  renameAttrArrayPtr->setNewArrayName("NewDataArrayName");
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), AM_NOT_FOUND)

  // "New Array Already Exists" Test
  renameAttrArrayPtr->setNewArrayName("DataArray3");
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix2", "DataArray2"));
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), AA_NEW_NAME_EXISTS)

  // "Old Name Does Not Exist" Test
  renameAttrArrayPtr->setNewArrayName("DataArray5");
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix2", "DataArray4"));
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), AA_OLD_NAME_DOES_NOT_EXIST)

  // "Rename Data Array" Verification Test
  renameAttrArrayPtr->setSelectedArrayPath(DataArrayPath("DataContainer1", "AttributeMatrix1", "DataArray1"));
  renameAttrArrayPtr->setNewArrayName("NewDataArrayName");
  renameAttrArrayPtr->execute();
  DREAM3D_REQUIRE_EQUAL(renameAttrArrayPtr->getErrorCondition(), 0)

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveDataTest()
{
  DataContainerArray::Pointer dca = createDataContainerArray();
  RemoveArrays::Pointer removeDataPtr = RemoveArrays::New();
  removeDataPtr->setDataContainerArray(dca);

  DataContainerArrayProxy proxy(dca.get());
  setDeleteSelections(proxy);
  removeDataPtr->setDataArraysToRemove(proxy);

  removeDataPtr->execute();
  DataContainer::Pointer dc1 = dca->getDataContainer("DataContainer1");
  AttributeMatrix::Pointer am5 = dca->getDataContainer("DataContainer3")->getAttributeMatrix("AttributeMatrix5");
  IDataArray::Pointer da4 = dca->getDataContainer("DataContainer2")->getAttributeMatrix("AttributeMatrix3")->getAttributeArray("DataArray4");

  DREAM3D_REQUIRED_PTR(removeDataPtr->getErrorCondition(), ==, 0)
  DREAM3D_REQUIRED_PTR(dc1.get(), ==, NULL)
  DREAM3D_REQUIRED_PTR(am5.get(), ==, NULL)
  DREAM3D_REQUIRED_PTR(da4.get(), ==, NULL)

  RemoveArraysObserver::Pointer obs = RemoveArraysObserver::New();
  QObject foo;
  foo.connect(removeDataPtr.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
              obs.get(), SLOT(processPipelineMessage(const PipelineMessage&)));

  removeDataPtr->execute();
  QList<PipelineMessage> list = obs->getErrorList();

  DREAM3D_REQUIRE_EQUAL(list.size(), 1)

  QList<PipelineMessage>::iterator iter = list.begin();

  DREAM3D_REQUIRE_EQUAL((*iter).getCode(), DC_NOT_FOUND)
  iter++;
//    DREAM3D_REQUIRE_EQUAL((*iter).getCode(), AA_NOT_FOUND)
//    iter++;
//    DREAM3D_REQUIRE_EQUAL((*iter).getCode(), AM_NOT_FOUND)
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


