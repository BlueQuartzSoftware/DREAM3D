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
#include <list>
#include <string>

//--MXA Includes
#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Utilities/MXADir.h"
#include "MXA/Utilities/StringUtils.h"

#include "H5Support/H5Lite.h"
#include "H5Support/H5Utilities.h"


#include "UnitTestSupport.hpp"
#include "TestFileLocations.h"


#define DIM 6
#define DIM0 4
#define DIM1 3
#define DIM2 2

#define RANK_1D 1
#define RANK_2D 2
#define RANK_3D 3

size_t AttrSize = 0;



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  MXADir::remove(UnitTest::H5UtilTest::FileName);
#endif
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer1DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  AttrSize++;
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "1DArrayAttribute<" + attributeKey + ">";
  int32_t rank = 1;
  T data[DIM0];
  for(int i = 0; i < DIM0; ++i)
  {
    data[i] = (T)(i);
  }
  hsize_t dims[1];
  dims[0] = DIM0;

  err = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer2DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  //std::cout << DEBUG_OUT(logTime) << "testWritePointer2DArrayAttribute" << std::endl;
  AttrSize++;
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);

  attributeKey = "2DArrayAttribute<" + attributeKey + ">";
  int32_t rank = RANK_2D;
  T data[DIM0][DIM1];
  for(int i = 0; i < DIM0; ++i)
  {
    for (int j = 0; j < DIM1; ++j) {
      data[i][j] = (T)(i * j);
    }
  }
  hsize_t dims[RANK_2D];
  dims[0] = DIM0;
  dims[1] = DIM1;
  err = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWritePointer3DArrayAttribute(hid_t file_id, const std::string &dsetName)
{
  AttrSize++;
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "3DArrayAttribute<" + attributeKey + ">";
  int32_t rank = RANK_3D;
  T data[DIM0][DIM1][DIM2];
  for(int i = 0; i < DIM0; ++i) {
    for (int j = 0; j < DIM1; ++j) {
      for (int k = 0; k < DIM2; ++k) {
        data[i][j][k] = (T)(i * j * k);
      }
    }
  }
  hsize_t dims[RANK_3D];
  dims[0] = DIM0;
  dims[1] = DIM1;
  dims[2] = DIM2;
  err = H5Lite::writePointerAttribute<T>(file_id, dsetName, attributeKey, rank, dims, (T*)data);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWriteVectorAttribute(hid_t file_id, std::string dsetName )
{
  AttrSize++;
  T value = 0x0;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "VectorAttribute<" + attributeKey + ">";

  int32_t numElements = DIM0;
  std::vector<hsize_t> dims (1, DIM0);

  /* Make dataset */
  std::vector<T> data (DIM0, 0);
  for (int i = 0; i < numElements; ++i) {
    data[i] = (T)(i);
  }
  //std::cout << "Attribute->Write: " << objName;
  err = H5Lite::writeVectorAttribute( file_id, dsetName, attributeKey, dims, data );
  DREAM3D_REQUIRE(err >= 0);
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
herr_t testWriteScalarAttribute(hid_t file_id, const std::string &dsetName)
{
  AttrSize++;
  T value = 0x0F;
  herr_t err = -1;
  std::string attributeKey = H5Lite::HDFTypeForPrimitiveAsStr(value);
  DREAM3D_REQUIRE(attributeKey.empty() == false);
  attributeKey = "ScalarAttribute<" + attributeKey + ">";
  err = H5Lite::writeScalarAttribute(file_id, dsetName, attributeKey, value);
  DREAM3D_REQUIRE(err >= 0);
  return err;
}


// -----------------------------------------------------------------------------
//  Uses Raw Pointers to save data to the data file
// -----------------------------------------------------------------------------
template <typename T>
herr_t testWritePointer2DArrayDataset(hid_t file_id)
{

  T value = 0x0;
  herr_t err = 1;
  int32_t rank = 2;
  // Create the Dimensions
  hsize_t dims[2];
  dims[0] = DIM0; dims[1] = DIM1;

  /* Make dataset char */
  int32_t tSize = dims[0] * dims[1];
 // T data[dimx*dimy];
  std::vector<T> data(tSize);
  for (int32_t i = 0; i < tSize; ++i) {
    data[i] = static_cast<T>( i * 5);
  }

  std::string dsetName = H5Lite::HDFTypeForPrimitiveAsStr(value);
  dsetName = "Pointer2DArrayDataset<" + dsetName + ">";
  std::cout << "Running " << dsetName << " ... ";
  err = H5Lite::writePointerDataset( file_id, dsetName, rank, dims, &(data.front()) );
  DREAM3D_REQUIRE(err >= 0);

  err = testWritePointer3DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int8_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int16_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int32_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<int64_t>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<float32>(file_id, dsetName);
  err = testWritePointer3DArrayAttribute<float64>(file_id, dsetName);

  err = testWritePointer1DArrayAttribute<int8_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<int16_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<int32_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<int64_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<float32>(file_id, dsetName);
  err = testWritePointer1DArrayAttribute<float64>(file_id, dsetName);

  err = testWritePointer2DArrayAttribute<int8_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint8_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<int16_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint16_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<int32_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint32_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<int64_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<uint64_t>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<float32>(file_id, dsetName);
  err = testWritePointer2DArrayAttribute<float64>(file_id, dsetName);

  err = testWriteVectorAttribute<int8_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint8_t>(file_id, dsetName);
  err = testWriteVectorAttribute<int16_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint16_t>(file_id, dsetName);
  err = testWriteVectorAttribute<int32_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint32_t>(file_id, dsetName);
  err = testWriteVectorAttribute<int64_t>(file_id, dsetName);
  err = testWriteVectorAttribute<uint64_t>(file_id, dsetName);
  err = testWriteVectorAttribute<float32>(file_id, dsetName);
  err = testWriteVectorAttribute<float64>(file_id, dsetName);

  err = testWriteScalarAttribute<int8_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint8_t>(file_id, dsetName);
  err = testWriteScalarAttribute<int16_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint16_t>(file_id, dsetName);
  err = testWriteScalarAttribute<int32_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint32_t>(file_id, dsetName);
  err = testWriteScalarAttribute<int64_t>(file_id, dsetName);
  err = testWriteScalarAttribute<uint64_t>(file_id, dsetName);
  err = testWriteScalarAttribute<float32>(file_id, dsetName);
  err = testWriteScalarAttribute<float64>(file_id, dsetName);

  std::cout << " Passed" << std::endl;
  return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void H5UtilitiesTest()
{
  // herr_t err = -1;
   hid_t   file_id;
   /* Create a new file using default properties. */
   file_id = H5Fcreate( UnitTest::H5UtilTest::FileName.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
   DREAM3D_REQUIRE(file_id > 0);

   // std::cout << logTime() << "----------- Testing Writing/Reading of Datasets using Raw Pointers -----------" << std::endl;
   DREAM3D_REQUIRE ( testWritePointer2DArrayDataset<int32_t>(file_id) >= 0);

   hid_t dsetId = H5Utilities::openHDF5Object(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>");
   DREAM3D_REQUIRE(dsetId > 0);

   DREAM3D_REQUIRE( H5Utilities::isGroup(file_id, "/") == true);
   DREAM3D_REQUIRE( H5Utilities::isGroup(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>") == false);

   std::string objName;
   int32_t err = H5Utilities::objectNameAtIndex(file_id, 0, objName);
   DREAM3D_REQUIRE(objName.compare("Pointer2DArrayDataset<H5T_NATIVE_INT32>") == 0);

   hid_t objType = -1;
   err = H5Utilities::getObjectType(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", &objType);
   DREAM3D_REQUIRE(objType == H5O_TYPE_DATASET);
   DREAM3D_REQUIRE(err >= 0);

   err = H5Utilities::getObjectType(file_id, "/", &objType);
   DREAM3D_REQUIRE(objType == H5O_TYPE_GROUP);
   DREAM3D_REQUIRE(err >= 0);

   std::string objPath = H5Utilities::getObjectPath(dsetId, false);
   DREAM3D_REQUIRE(objPath.compare("Pointer2DArrayDataset<H5T_NATIVE_INT32>") == 0);

   err = H5Utilities::closeHDF5Object(dsetId);
   DREAM3D_REQUIRE(err >= 0);

   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 1", file_id) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 2/", file_id) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("Test Path 3/", file_id) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/", file_id) < 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 4/Test Path 7", file_id) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 5/Test Path 8/", file_id) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("Test Path 6/Test Path 9/", file_id) >= 0);

   hid_t grpId = H5Utilities::openHDF5Object(file_id, "Test Path 1");
   DREAM3D_REQUIRE(grpId > 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 1", grpId) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 2/", grpId) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("Test Path 3/", grpId) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/", grpId) < 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 4/Test Path 7", grpId) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("/Test Path 5/Test Path 8/", grpId) >= 0);
   DREAM3D_REQUIRE ( H5Utilities::createGroupsFromPath("Test Path 6/Test Path 9/", grpId) >= 0);
   err = H5Gclose(grpId);
   DREAM3D_REQUIRE(err >= 0);

   hid_t gid = H5Utilities::createGroup(file_id, "test group");
   DREAM3D_REQUIRE(gid >= 0);
   err = H5Utilities::closeHDF5Object(gid);

   std::list<std::string> groups;
   err = H5Utilities::getGroupObjects(file_id, H5Utilities::H5Support_ANY, groups);
   DREAM3D_REQUIRE(err >= 0);
   DREAM3D_REQUIRE( groups.size() == 8);


   err = H5Utilities::createGroupsForDataset("/group1/group2/group3/data", file_id);
   DREAM3D_REQUIRE(err >= 0);
   gid = H5Utilities::openHDF5Object(file_id, "/group1/group2");
   DREAM3D_REQUIRE(gid >= 0);
   err = H5Utilities::closeHDF5Object(gid);

#if 0
   bool success = H5Utilities::probeForAttribute(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", "ScalarAttribute<H5T_NATIVE_INT32>" );
   DREAM3D_REQUIRE(success == true);

   success = H5Utilities::probeForAttribute(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", "ScalarAttribute<>" );
   DREAM3D_REQUIRE(success == false);
#endif

   std::list<std::string> attributes;
   err = H5Utilities::getAllAttributeNames(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", attributes);
   DREAM3D_REQUIRE(err >= 0);
   DREAM3D_REQUIRE(attributes.size() == AttrSize);

   dsetId = H5Utilities::openHDF5Object(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>");
   DREAM3D_REQUIRE(dsetId > 0);
   attributes.clear();
   err = H5Utilities::getAllAttributeNames(dsetId, attributes);
   DREAM3D_REQUIRE(err >= 0);
   DREAM3D_REQUIRE(attributes.size() == AttrSize);
   err = H5Utilities::closeHDF5Object(dsetId);
   DREAM3D_REQUIRE(err >= 0);
//
//   MXAAbstractAttributes allAttributes;
//   err = H5Utilities::readAllAttributes(file_id, "Pointer2DArrayDataset<H5T_NATIVE_INT32>", allAttributes );
//   DREAM3D_REQUIRE(err >= 0);
//   DREAM3D_REQUIRE(allAttributes.size() == AttrSize);
//   DREAM3D_REQUIRE(H5Fclose(file_id) >= 0);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StressTestCreateGroups()
{
  std::cout << logTime() << " Starting StressTestCreateGroups()" << std::endl;
  char path[64];
  ::memset(path, 0, 64);
  int err = 0;
  hid_t   file_id;
  hid_t   grpId;

  /* Create a new file using default properties. */
  file_id = H5Fcreate(UnitTest::H5UtilTest::GroupTest.c_str(), H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
  DREAM3D_REQUIRE(file_id > 0);


  for (int i = 0; i < 100; ++i) {
    std::cout << logTime() << "Outer Loop: " << i << std::endl;
//    err = H5Fclose(file_id);
//    DREAM3D_REQUIRE(err >= 0);
//    file_id = H5Fopen(MXAUnitTest::H5UtilTest::GroupTest.c_str(), H5F_ACC_RDWR, H5P_DEFAULT);
    ::memset(path, 0, 64);
    snprintf(path, 64, "/%03d", i);
    grpId = H5Utilities::createGroup(file_id, path);
    DREAM3D_REQUIRE(grpId > 0);
    err = H5Gclose(grpId);
    DREAM3D_REQUIRE(err >= 0);


    for (int j = 0; j < 100; ++j) {

      snprintf(path, 64, "/%03d/%03d", i, j);
      grpId = H5Utilities::createGroup(file_id, path);
      DREAM3D_REQUIRE(grpId > 0);
      DREAM3D_REQUIRE(grpId > 0);
      err = H5Gclose(grpId);
      DREAM3D_REQUIRE(err >= 0);

      for (int k = 0; k < 100; ++k) {

        snprintf(path, 64, "/%03d/%03d/%03d", i, j, k);
        grpId = H5Utilities::createGroup(file_id, path);
        DREAM3D_REQUIRE(grpId >= 0);
        DREAM3D_REQUIRE(grpId > 0);
        err = H5Gclose(grpId);
        DREAM3D_REQUIRE(err >= 0);
      }
    }
  }
  err = H5Fclose(file_id);
  DREAM3D_REQUIRE(err >= 0);
}

// -----------------------------------------------------------------------------
//  This function tests if we can write a data set larger than 2GB on Darwin/Apple
//  systems. There is a bug in HDF5 versions up to and including 1.8.9 where this
//  type of write will fail. This was used to report the bug to HDFGroup and should
//  NOT be used for any test. At least not yet.
// -----------------------------------------------------------------------------
#if __APPLE__
void HDF5Darwin2GBWriteTest()
{
  std::cout << "sizeof(ssize_t): " << sizeof(ssize_t) << std::endl;
  size_t dim1 = 3;
  size_t dim0 = 178956971;
  int* data = (int*)(malloc(sizeof(int) * dim0 * dim1));

  hid_t   file_id;
  /* Create a new file using default properties. */
  file_id = H5Fcreate( "/tmp/HDF5_LARGE_WRITE_BUG.h5", H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT );
  DREAM3D_REQUIRE(file_id > 0);
  int32_t rank = 2;
  hsize_t dims[2] = {dim0, dim1};

  herr_t err = H5Lite::writePointerDataset(file_id, "data", rank, dims, data);
  BOOST_ASSERT(err > -1);
}
#endif

// -----------------------------------------------------------------------------
//  Use unit test framework
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
  int err = EXIT_SUCCESS;

  DREAM3D_REGISTER_TEST( H5UtilitiesTest() )
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  PRINT_TEST_SUMMARY();

  return err;
}


