/* ============================================================================
* Copyright (c) 2018 BlueQuartz Software, LLC
*
* Redistribution and use in source and binary forms, with or without modification,
* are permitted provided that the following conditions are met:
*
* Redistributions of source code must retain the above copyright notice, this
* list of conditions and the following disclaimer.
*
* Redistributions in binary form must reproduce the above copyright notice, this
* list of conditions and the following disclaimer in the documentation and/or
* other materials provided with the distribution.
*
* Neither the name of BlueQuartz Software, the US Air Force, nor the names of its
* contributors may be used to endorse or promote products derived from this software
* without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
* CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-15-D-5231
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataArrays/NeighborList.hpp"
#include "SIMPLib/HDF5/H5DataArrayReader.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/CoreFilters/DataContainerReader.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"

#include "H5Support/QH5Utilities.h"
#include "H5Support/QH5Lite.h"
#include "H5Support/HDF5ScopedFileSentinel.h"

#include "UnitTestSupport.hpp"

#include "GenericTestFileLocations.h"

const QString DataContainerName = "ImageDataContainer";
const QString AttributeMatrixName = "CellFeatureData";
const QString NeighborListArrayName = "NeighborList2";
const QString PhasesArrayName = "Phases";
const QString ConnectedFeaturesArrayName = "ConnectedFeatures";
const int SelectedFeatureIdValue = 5;
const int SelectedPhaseIdValue = 1;
const int FeatureIdAlgorithmValue = 0;
const int PhaseIdAlgorithmValue = 1;

class FindConnectedFeaturesTest
{

  public:
    FindConnectedFeaturesTest() {}
    virtual ~FindConnectedFeaturesTest() {}


  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
  #if REMOVE_TEST_FILES
    QFile::remove(UnitTest::FindConnectedFeaturesTest::FeatureAlgorithmOutputFile);
    QFile::remove(UnitTest::FindConnectedFeaturesTest::PhaseAlgorithmOutputFile);
  #endif
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the FindConnectedFeaturesTest Filter from the FilterManager
    QString filtName = "FindConnectedFeatures";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if (nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The Generic Requires the use of the " << filtName.toStdString() << " filter which is found in the Generic Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFindConnectedFeatures_FeatureAlgorithm()
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    DataContainerReader::Pointer dcReaderFilter = DataContainerReader::New();
    dcReaderFilter->setInputFile(UnitTest::FindConnectedFeaturesTest::InputFile);
    DataContainerArrayProxy proxy = dcReaderFilter->readDataContainerArrayStructure(UnitTest::FindConnectedFeaturesTest::InputFile);
    dcReaderFilter->setInputFileDataContainerArrayProxy(proxy);
    pipeline->pushBack(dcReaderFilter);

    QString filtName = "FindConnectedFeatures";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if (nullptr != filterFactory.get())
    {
      AbstractFilter::Pointer filter = filterFactory->create();
      bool propWasSet;
      QVariant var;

      pipeline->pushBack(filter);

      // Test "Invalid DataContainer or AttributeMatrix for NeighborList array"
      var.setValue(DataArrayPath("Foo", AttributeMatrixName, NeighborListArrayName));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30000);
      filter->setErrorCondition(0);

      var.setValue(DataArrayPath(DataContainerName, "Foo", NeighborListArrayName));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30000);
      filter->setErrorCondition(0);

      // Test "NeighborList attribute array doesn't exist"
      var.setValue(DataArrayPath(DataContainerName, AttributeMatrixName, "Foo"));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30001);
      filter->setErrorCondition(0);

      var.setValue(DataArrayPath(DataContainerName, AttributeMatrixName, NeighborListArrayName));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      // Test "Algorithm type index out of bounds"
      var.setValue(2);
      propWasSet = filter->setProperty("AlgorithmType", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30004);
      filter->setErrorCondition(0);

      var.setValue(FeatureIdAlgorithmValue);
      propWasSet = filter->setProperty("AlgorithmType", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      // Test "SelectedFeatureId doesn't exist in the NeighborList array"
      var.setValue(-1);
      propWasSet = filter->setProperty("SelectedFeatureId", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30002);
      filter->setErrorCondition(0);

      var.setValue(SelectedFeatureIdValue);
      propWasSet = filter->setProperty("SelectedFeatureId", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      DataContainerWriter::Pointer writer = DataContainerWriter::New();
      writer->setOutputFile(UnitTest::FindConnectedFeaturesTest::FeatureAlgorithmOutputFile);
      writer->setWriteXdmfFile(false);
      pipeline->pushBack(writer);

      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

      hid_t fileId = QH5Utilities::openFile(UnitTest::FindConnectedFeaturesTest::FeatureAlgorithmOutputFile, true);
      DREAM3D_REQUIRE(fileId >= 0);
      HDF5ScopedFileSentinel sentinel(&fileId, true);

      hid_t rootId = QH5Utilities::openHDF5Object(fileId, "DataContainers");
      DREAM3D_REQUIRE(rootId >= 0);
      sentinel.addGroupId(&rootId);

      hid_t dcId = QH5Utilities::openHDF5Object(rootId, DataContainerName);
      DREAM3D_REQUIRE(dcId >= 0);
      sentinel.addGroupId(&dcId);

      hid_t amId = QH5Utilities::openHDF5Object(dcId, AttributeMatrixName);
      DREAM3D_REQUIRE(amId >= 0);
      sentinel.addGroupId(&amId);

      IDataArray::Pointer iDataArray = H5DataArrayReader::ReadNeighborListData(amId, NeighborListArrayName);
      NeighborList<int32_t>::Pointer nList = std::dynamic_pointer_cast<NeighborList<int32_t> >(iDataArray);
      DREAM3D_REQUIRE(nList.get() != nullptr);

      NeighborList<int32_t>::SharedVectorType featureIdNList = nList->getList(SelectedFeatureIdValue);

      iDataArray = H5DataArrayReader::ReadIDataArray(amId, ConnectedFeaturesArrayName);
      BoolArrayType::Pointer connectedFeaturesArray = std::dynamic_pointer_cast<BoolArrayType >(iDataArray);
      DREAM3D_REQUIRE(connectedFeaturesArray.get() != nullptr);

      for (int i = 0; i < connectedFeaturesArray->getNumberOfTuples(); i++)
      {
        bool value = connectedFeaturesArray->getValue(i);
        if (value == true)
        {
          if(std::find(featureIdNList->begin(), featureIdNList->end(), i) == featureIdNList->end())
          {
            // The ConnectedFeatures array has this feature marked as a neighbor, but it does not appear in the feature's neighbor list
            DREAM3D_REQUIRE_EQUAL(0, 1)
          }
        }
        else
        {
          if(std::find(featureIdNList->begin(), featureIdNList->end(), i) != featureIdNList->end())
          {
            // The ConnectedFeatures array does not have this feature marked as a neighbor, but it still appears in the feature's neighbor list
            DREAM3D_REQUIRE_EQUAL(0, 1)
          }
        }
      }
    }
    else
    {
      std::stringstream ss;
      ss << QObject::tr("FindConnectedFeaturesTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName).toStdString();
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFindConnectedFeatures_PhaseAlgorithm()
  {
    FilterPipeline::Pointer pipeline = FilterPipeline::New();

    DataContainerReader::Pointer dcReaderFilter = DataContainerReader::New();
    dcReaderFilter->setInputFile(UnitTest::FindConnectedFeaturesTest::InputFile);
    DataContainerArrayProxy proxy = dcReaderFilter->readDataContainerArrayStructure(UnitTest::FindConnectedFeaturesTest::InputFile);
    dcReaderFilter->setInputFileDataContainerArrayProxy(proxy);
    pipeline->pushBack(dcReaderFilter);

    QString filtName = "FindConnectedFeatures";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryFromClassName(filtName);
    if (nullptr != filterFactory.get())
    {
      AbstractFilter::Pointer filter = filterFactory->create();
      bool propWasSet;
      QVariant var;

      pipeline->pushBack(filter);

      // Test "Invalid DataContainer or AttributeMatrix for NeighborList array"
      var.setValue(DataArrayPath("Foo", AttributeMatrixName, NeighborListArrayName));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30000);
      filter->setErrorCondition(0);

      var.setValue(DataArrayPath(DataContainerName, "Foo", NeighborListArrayName));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30000);
      filter->setErrorCondition(0);

      // Test "NeighborList attribute array doesn't exist"
      var.setValue(DataArrayPath(DataContainerName, AttributeMatrixName, "Foo"));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30001);
      filter->setErrorCondition(0);

      var.setValue(DataArrayPath(DataContainerName, AttributeMatrixName, NeighborListArrayName));
      propWasSet = filter->setProperty("NeighborListArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      // Test "Algorithm type index out of bounds"
      var.setValue(2);
      propWasSet = filter->setProperty("AlgorithmType", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->preflightPipeline();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30004);
      filter->setErrorCondition(0);

      var.setValue(PhaseIdAlgorithmValue);
      propWasSet = filter->setProperty("AlgorithmType", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      // Test "SelectedPhaseId doesn't exist in the NeighborList array"
      var.setValue(-1);
      propWasSet = filter->setProperty("SelectedPhaseId", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), -30003);
      filter->setErrorCondition(0);

      var.setValue(SelectedPhaseIdValue);
      propWasSet = filter->setProperty("SelectedPhaseId", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      DataContainerWriter::Pointer writer = DataContainerWriter::New();
      writer->setOutputFile(UnitTest::FindConnectedFeaturesTest::PhaseAlgorithmOutputFile);
      writer->setWriteXdmfFile(false);
      pipeline->pushBack(writer);

      var.setValue(DataArrayPath(DataContainerName, AttributeMatrixName, PhasesArrayName));
      propWasSet = filter->setProperty("PhasesArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true);

      pipeline->execute();
      DREAM3D_REQUIRE_EQUAL(filter->getErrorCondition(), 0);

      hid_t fileId = QH5Utilities::openFile(UnitTest::FindConnectedFeaturesTest::PhaseAlgorithmOutputFile, true);
      DREAM3D_REQUIRE(fileId >= 0);
      HDF5ScopedFileSentinel sentinel(&fileId, true);

      hid_t rootId = QH5Utilities::openHDF5Object(fileId, "DataContainers");
      DREAM3D_REQUIRE(rootId >= 0);
      sentinel.addGroupId(&rootId);

      hid_t dcId = QH5Utilities::openHDF5Object(rootId, DataContainerName);
      DREAM3D_REQUIRE(dcId >= 0);
      sentinel.addGroupId(&dcId);

      hid_t amId = QH5Utilities::openHDF5Object(dcId, AttributeMatrixName);
      DREAM3D_REQUIRE(amId >= 0);
      sentinel.addGroupId(&amId);

      IDataArray::Pointer iDataArray = H5DataArrayReader::ReadNeighborListData(amId, NeighborListArrayName);
      NeighborList<int32_t>::Pointer nList = std::dynamic_pointer_cast<NeighborList<int32_t> >(iDataArray);
      DREAM3D_REQUIRE(nList.get() != nullptr);

      iDataArray = H5DataArrayReader::ReadIDataArray(amId, PhasesArrayName);
      Int32ArrayType::Pointer phasesArray = std::dynamic_pointer_cast<Int32ArrayType >(iDataArray);
      DREAM3D_REQUIRE(phasesArray.get() != nullptr);

      iDataArray = H5DataArrayReader::ReadIDataArray(amId, ConnectedFeaturesArrayName);
      BoolArrayType::Pointer connectedFeaturesArray = std::dynamic_pointer_cast<BoolArrayType >(iDataArray);
      DREAM3D_REQUIRE(connectedFeaturesArray.get() != nullptr);


      // Find the feature ids that have the same phase as the selected phase
      QSet<int> featureIdSet;
      for (int i = 1; i < phasesArray->getNumberOfTuples(); i++)
      {
        int phaseNumber = phasesArray->getValue(i);
        if (phaseNumber == SelectedPhaseIdValue)
        {
          featureIdSet.insert(i);
        }
      }
      QList<int> featureList = featureIdSet.toList();

      // Find the neighbor feature ids of all these feature ids, which can otherwise be called the neighbor list for the given phase
      QSet<int> neighborIdSet;
      for (int i = 0; i < featureList.size(); i++)
      {
        int featureId = featureList[i];
        NeighborList<int32_t>::SharedVectorType featureIdNList = nList->getList(featureId);
        for (std::vector<int>::size_type j = 0; j < featureIdNList->size(); j++)
        {
          int neighborId = featureIdNList->at(j);
          if (phasesArray->getValue(neighborId) != SelectedPhaseIdValue)
          {
            neighborIdSet.insert(neighborId);
          }
        }
      }
      QList<int> phaseNeighborList = neighborIdSet.toList();

      // Check every feature id.  Ids in the phase neighbor list should be marked true, otherwise false
      for (int i = 0; i < connectedFeaturesArray->getNumberOfTuples(); i++)
      {
        if (phaseNeighborList.contains(i) == true)
        {
          DREAM3D_REQUIRE_EQUAL(connectedFeaturesArray->getValue(i), true);
        }
        else
        {
          DREAM3D_REQUIRE_EQUAL(connectedFeaturesArray->getValue(i), false);
        }
      }
    }
    else
    {
      std::stringstream ss;
      ss << QObject::tr("FindConnectedFeaturesTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName).toStdString();
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }

    return EXIT_SUCCESS;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST( TestFilterAvailability() );

    DREAM3D_REGISTER_TEST( TestFindConnectedFeatures_FeatureAlgorithm() )

    DREAM3D_REGISTER_TEST( TestFindConnectedFeatures_PhaseAlgorithm() )

    DREAM3D_REGISTER_TEST( RemoveTestFiles() )
  }

  private:
    FindConnectedFeaturesTest(const FindConnectedFeaturesTest&); // Copy Constructor Not Implemented
    void operator=(const FindConnectedFeaturesTest&); // Operator '=' Not Implemented


};

