/* ============================================================================
 * Copyright (c) 2009-2015 BlueQuartz Software, LLC
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
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#include <limits>
#include <iostream>

#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QCoreApplication>

#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"
#include "SIMPLib/Math/QuaternionMath.hpp"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/Observer.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"

#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/OrientationMath/OrientationTransforms.hpp"
#include "OrientationLib/OrientationMath/OrientationConverter.hpp"


#include "OrientationLibTestFileLocations.h"
#include "GenerateFunctionList.h"
#include "TestPrintFunctions.h"

/*
 *

 DREAM.3D Testing

 | From/To |  e   |  o   |  a   |  r   |  q   |  h   |  c   |
 |  -      |  -   |  -   |  -   |  -   |  -   |  -   |  -   |
 |  e      |  #   |  X   |  X   |  X   |  X   |  X   |      |
 |  o      |  X   |  #   |  X   |  X   |  X   |  X   |      |
 |  a      |  X   |  X   |  #   |  X   |  X   |  X   |      |
 |  r      |  X   |  X   |  X   |  #   |  X   |  X   |      |
 |  q      |  X   |  X   |  X   |  X   |  #   |  X   |      |
 |  h      |  X   |  X   |  X   |  X   |  X   |  #   |      |
 |  c      |      |      |      |      |      |      |  #   |


 */

QVector<QString> DataSetNames;
QVector<int> DataSetTypes;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{
#if REMOVE_TEST_FILES
  // QFile::remove();
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer InstantiateFilter(QString filtName)
{
  // Now instantiate the PhWriter Filter from the FilterManager
  FilterManager* fm = FilterManager::Instance();
  IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
  if (NULL == filterFactory.get() )
  {
    std::stringstream ss;
    ss << "The OrientationTransformTest Requires the use of the " << filtName.toStdString() << " filter. Did the plugins load?";
    DREAM3D_TEST_THROW_EXCEPTION(ss.str())
  }

  return filterFactory->create();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename T>
void GenerateEulers(size_t nSteps, AttributeMatrix::Pointer attrMat)
{
  typedef ArrayHelpers<OrientationArray<T>, T> OMHelperType;

  QVector<size_t> cDims(1, 3);

  T phi1_min = static_cast<T>(0.0);
  T phi1_max = DConst::k_2Pi;
  T phi1_delta = (phi1_max - phi1_min) / static_cast<T>(nSteps);

  T phi_min = static_cast<T>(0.0);
  T phi_max = DConst::k_Pi;
  T phi_delta = (phi_max - phi_min) / static_cast<T>(nSteps);

  T phi2_min = static_cast<T>(0.0);
  T phi2_max = DConst::k_2Pi;
  T phi2_delta = (phi2_max - phi2_min) / static_cast<T>(nSteps);

  size_t nStepsCubed = (nSteps + 1) * (nSteps + 1) * (nSteps + 1);
  typename DataArray<T>::Pointer eulers = DataArray<T>::CreateArray( nStepsCubed, cDims, k_InputNames[0]);

  size_t counter = 0;
  for(int i = 0; i <= nSteps; i++)
  {
    for(int j = 0; j <= nSteps; j++)
    {
      for(int k = 0; k <= nSteps; k++)
      {
        //        std::cout << "Euler[" << counter << "]: "
        //                  << (phi1_min+i*phi1_delta)*DConst::k_180OverPi << ", "
        //                   << (phi_min+j*phi_delta)*DConst::k_180OverPi  << ", "
        //                      << (phi2_min+k*phi2_delta)*DConst::k_180OverPi  << std::endl;



        eulers->setComponent(counter, 0, phi1_min + i * phi1_delta);
        eulers->setComponent(counter, 1, phi_min + j * phi_delta);
        eulers->setComponent(counter, 2, phi2_min + k * phi2_delta);

        T one80Check = phi1_min + i * phi1_delta + phi2_min + k * phi2_delta;
        if( SIMPLibMath::closeEnough(static_cast<T>(SIMPLib::Constants::k_Pi), one80Check, static_cast<T>(1.0E-6)) )
        {
          eulers->setComponent(counter, 0, phi1_min + i * phi1_delta + .1);
          eulers->setComponent(counter, 2, phi2_min + k * phi2_delta + .1);
        }

        one80Check = fmod(one80Check, SIMPLib::Constants::k_2Pi);
        if( SIMPLibMath::closeEnough(static_cast<T>(SIMPLib::Constants::k_Pi), one80Check, static_cast<T>(1.0E-6)) )
        {
          eulers->setComponent(counter, 0, phi1_min + i * phi1_delta + .1);
          eulers->setComponent(counter, 2, phi2_min + k * phi2_delta + .1);
        }

        counter++;
      }
    }
  }

  typename EulerConverter<T>::Pointer euConv = EulerConverter<T>::New();
  euConv->setInputData(eulers);

  euConv->toOrientationMatrix();
  typename DataArray<T>::Pointer om = euConv->getOutputData();
  om->setName(k_InputNames[1]);
  attrMat->addAttributeArray(om->getName(), om);

  // Create an Orientation matrix from the Eulers and then transform BACK to Eulers to transform
  // the values of the Eulers into the convention set forth in the Rotations Paper.
  typename OrientationMatrixConverter<T>::Pointer omConv = OrientationMatrixConverter<T>::New();
  omConv->setInputData(om);
  omConv->toEulers();
  eulers = omConv->getOutputData();
  eulers->setName(k_InputNames[0]);
  euConv->setInputData(eulers);

  attrMat->addAttributeArray(eulers->getName(), eulers);

  euConv->toQuaternion();
  typename DataArray<T>::Pointer q = euConv->getOutputData();
  q->setName(k_InputNames[2]);
  attrMat->addAttributeArray(q->getName(), q);

  euConv->toAxisAngle();
  typename DataArray<T>::Pointer ax = euConv->getOutputData();
  ax->setName(k_InputNames[3]);
  attrMat->addAttributeArray(ax->getName(), ax);

  euConv->toRodrigues();
  typename DataArray<T>::Pointer ro = euConv->getOutputData();
  ro->setName(k_InputNames[4]);
  attrMat->addAttributeArray(ro->getName(), ro);

  euConv->toHomochoric();
  typename DataArray<T>::Pointer ho = euConv->getOutputData();
  ho->setName(k_InputNames[5]);
  attrMat->addAttributeArray(ho->getName(), ho);

  euConv->toCubochoric();
  typename DataArray<T>::Pointer cu = euConv->getOutputData();
  cu->setName(k_InputNames[6]);
  attrMat->addAttributeArray(cu->getName(), cu);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString ExecuteConvertFilter(DataContainerArray::Pointer dca, GenerateFunctionList::EntryType& entry,
                             int e, QString outputName)
{

  QVariant var;
  bool propWasSet;
  QString inputName;

  AbstractFilter::Pointer convFilt = InstantiateFilter("ConvertOrientations");
  DREAM3D_REQUIRE_VALID_POINTER(convFilt.get())

  convFilt->setDataContainerArray(dca);

  int inputType = entry[e];
  var.setValue(inputType);
  propWasSet = convFilt->setProperty("InputType", var);
  if(!propWasSet)
  {
    qDebug() << "Unable to set property inputType";
  }

  int outputType = entry[e + 1];
  var.setValue(outputType);
  propWasSet = convFilt->setProperty("OutputType", var);
  if(!propWasSet)
  {
    qDebug() << "Unable to set property OutputType";
  }
  if(e == 0)
  {
    inputName = k_InputNames[entry[e]];
  }
  else
  {
    inputName = outputName;
  }
  DataArrayPath dap(DCName, AMName, inputName);
  var.setValue(dap);
  propWasSet = convFilt->setProperty("InputOrientationArrayPath", var);
  if(!propWasSet)
  {
    qDebug() << "Unable to set property InputOrientationArrayPath";
  }

  outputName = QString::number(e) + QString("_") + k_InputNames[entry[e]] + QString("2") + k_InputNames[entry[e + 1]];
  DataSetNames.push_back(outputName);
  DataSetTypes.push_back(entry[e + 1]);
  var.setValue(outputName);
  propWasSet = convFilt->setProperty("OutputOrientationArrayName", var);
  if(!propWasSet)
  {
    qDebug() << "Unable to set property OutputOrientationArrayName";
  }

  Observer obs;
  obs.connect(convFilt.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
              &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

  convFilt->execute();
  int err = convFilt->getErrorCondition();
  DREAM3D_REQUIRED(err, >=, 0)

  return outputName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename K>
void CheckRepresentation(K* data, int repType)
{
  typedef OrientationArray<K>  OrientationArrayType;
  typedef OrientationTransforms<OrientationArray<K>, K> OrTr_Type;
  typename OrTr_Type::ResultType res;

  OrientationArrayType wrapper(data, k_CompDims[repType]);
  switch (repType)
  {
    case 0:
      res = OrTr_Type::eu_check(wrapper);
      break;
    case 1:
      res = OrTr_Type::om_check(wrapper);
      break;
    case 2:
      res = OrTr_Type::qu_check(wrapper);
      break;
    case 3:
      res = OrTr_Type::ax_check(wrapper);
      break;
    case 4:
      res = OrTr_Type::ro_check(wrapper);
      break;
    case 5:
      res = OrTr_Type::ho_check(wrapper);
      break;
    case 6:
      res = OrTr_Type::cu_check(wrapper);
      break;
    default:
      break;
  }

  if(res.result <= 0)
  {
    std::cout << res.msg << std::endl;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
template<typename K>
void RunTestCase( GenerateFunctionList::EntryType& entryRef, size_t nSteps)
{

  typedef DataArray<K> DataArrayType;
  typedef typename DataArrayType::Pointer DataArrayPointerType;


  try
  {
    DataSetNames.clear();
    DataSetTypes.clear();

    GenerateFunctionList::EntryType entry = entryRef;
    QVector<QString> funcNames = EulerConverter<K>::GetOrientationTypeStrings();

    std::stringstream ss;
    for(int e = 0; e < entry.size() - 1; e++)
    {
      ss << k_InputNames[entry[e]].toStdString() << "2" << k_InputNames[entry[e + 1]].toStdString();
      if(e != entry.size() - 1)
      {
        ss << "\t";
      }
    }
    // std::cout << "####################################################################" << std::endl;
    // std::cout << ss.str() << std::endl;
    std::string testName = ss.str();

    DREAM3D::unittest::CurrentMethod = ss.str();
    DREAM3D::unittest::numTests++;
    std::cout << "Starting Test " << ss.str() << " -----------------------------------------------------" << std::endl;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New();
    m->setName(DCName);

    size_t nStepsCubed = (nSteps + 1) * (nSteps + 1) * (nSteps + 1);

    QVector<size_t> tDims(1, nStepsCubed);
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, AMName, 3);
    m->addAttributeMatrix(AMName, attrMat);
    dca->addDataContainer(m);

    // Make all the starting data
    GenerateEulers<K>(nSteps, attrMat);


    bool euCheck = false;
    QString outputName;

    if( entry[0] == 0 )
    {
      //std::cout << "CHECK EULERS!!" << std::endl;
      euCheck = true;
      entry.push_back(1); // Add an extra conversion to OM for Eulers since there can be ambiguous cases
    }
    else
    {

      for(int e = 0; e < entry.size(); e++)
      {
        if(entry[e] == 0)
        {
          GenerateFunctionList::EntryType::iterator iter = entry.begin() + e + 1;
          entry.insert(iter, 1);
          iter = entry.begin() + e + 2;
          entry.insert(iter, 0);
          e = e + 2;
        }
      }
    }

    for(int e = 0; e < entry.size() - 1; e++)
    {
      outputName = ExecuteConvertFilter(dca, entry, e, outputName);
    }

    // If we started with Eulers, then we need to convert the original Eulers and
    // the final eulers to an Orientation Matrix and back due to ambiguities when
    // transforming Eulers. Going to an Orientation Matrix with 4 degrees of freedom
    // will give us unique Eulers back which will be numerically equivalent.

    QVariant var;
    bool propWasSet;
    QString inputName;

    // Find Difference Map between originals and finals
    {
      AbstractFilter::Pointer diffMapFilt = InstantiateFilter("FindDifferenceMap");
      DREAM3D_REQUIRE_VALID_POINTER(diffMapFilt.get());
      diffMapFilt->setDataContainerArray(dca);
      size_t cDim = entry[0];
      if(euCheck)
      {
        inputName = k_InputNames[1]; // We converted the ending eulers to an Orientation Matrix so compare against the original OM
        cDim = 1; // Use the CompDim from the OM instead of what is coming in
      }
      else
      {
        inputName = k_InputNames[entry[0]];
      }

      DataArrayPath dap(DCName, AMName, inputName);
      var.setValue(dap);
      propWasSet = diffMapFilt->setProperty("FirstInputArrayPath", var);
      if (!propWasSet)
      {
        qDebug() << "Unable to set property FirstInputArrayPath";
      }

      dap = DataArrayPath (DCName, AMName, outputName);
      var.setValue(dap);
      propWasSet = diffMapFilt->setProperty("SecondInputArrayPath", var);
      if (!propWasSet)
      {
        qDebug() << "Unable to set property SecondInputArrayPath";
      }


      QString diffMapArrayName = QString("%1 Difference").arg(k_InputNames[entry[0]]);
      dap = DataArrayPath (DCName, AMName, diffMapArrayName);
      var.setValue(dap);
      propWasSet = diffMapFilt->setProperty("DifferenceMapArrayPath", var);
      if (!propWasSet)
      {
        qDebug() << "Unable to set property DifferenceMapArrayPath";
      }

      //      Observer obs;
      //      obs.connect(diffMapFilt.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
      //                  &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

      diffMapFilt->execute();
      int err = diffMapFilt->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)

      DataArrayPath daPath(DCName, AMName, diffMapArrayName);
      QVector<size_t> cDims(1, k_CompDims[cDim]);
      typename DataArray<K>::Pointer diff = dca->getPrereqArrayFromPath<DataArray<K>, AbstractFilter>(diffMapFilt.get(), daPath, cDims);
#if 1
      size_t tuples = diff->getNumberOfTuples();
      // printf("Total Tuples: %lu\n", tuples);
      size_t numErrors = 0;
      K thr = 1.0E-3;
      for(size_t t = 0; t < tuples; t++)
      {
        int nComp = diff->getNumberOfComponents();
        if(entry[0] == 4) { nComp--; } // for Rodrigues vectors we only want to compare the first 3 components.
        for(int c = 0; c < nComp; c++)
        {
          K delta = fabs(diff->getComponent(t, c));
          if(delta > thr)
          {
            numErrors++;
            std::cout << "Delta Failed: " << delta << " DataArray: '" << diff->getName().toStdString()
                      << "' Tuple[" << t << "] Comp[" << c << "] Value:"
                      << diff->getComponent(t, c)
                      << std::endl;

            // Get the AttributeMatrix:
            dap = DataArrayPath (DCName, AMName, k_InputNames[0]);
            AttributeMatrix::Pointer attrMat = dca->getAttributeMatrix(dap);

            // Print the Euler Angle that we Started with
            // cDims[0] = k_CompDims[0];
            DataArrayPointerType data = attrMat->getAttributeArrayAs<DataArrayType>(k_InputNames[0]);
            OrientationPrinters::PrintTuple<K>(data, t);
            CheckRepresentation<K>(data->getPointer(t), 0);

            // Print the starting representation
            data = attrMat->getAttributeArrayAs<DataArrayType>(k_InputNames[entry[0]]);
            OrientationPrinters::PrintTuple<K>(data, t);
            CheckRepresentation<K>(data->getPointer(t), entry[0]);

            // Now print all the intermediate Representations
            for (int q = 0; q < DataSetNames.size(); q++)
            {
              data = attrMat->getAttributeArrayAs<DataArrayType>(DataSetNames[q]);
              OrientationPrinters::PrintTuple<K>(data, t);
              CheckRepresentation<K>(data->getPointer(t), DataSetTypes[q]);
            }


#if 0
            cDims[0] = k_CompDims[entry[0]];
            dap = DataArrayPath (DCName, AMName, inputName);
            diff = dca->getPrereqArrayFromPath<DataArray<K>, AbstractFilter>(diffMapFilt.get(), dap, cDims);
            OrientationPrinters::PrintTuple<K>(diff, t);

            QString name;
            QTextStream ss(&name);
            ss << "0_" << k_InputNames[entry[0]] << "2" << k_InputNames[entry[1]];
            cDims[0] = k_CompDims[entry[1]];
            dap = DataArrayPath (DCName, AMName, name);
            diff = dca->getPrereqArrayFromPath<DataArray<K>, AbstractFilter>(diffMapFilt.get(), dap, cDims);
            OrientationPrinters::PrintTuple<K>(diff, t);

            cDims[0] = k_CompDims[entry[0]];
            dap = DataArrayPath (DCName, AMName, outputName);
            diff = dca->getPrereqArrayFromPath<DataArray<K>, AbstractFilter>(diffMapFilt.get(), dap, cDims);
            OrientationPrinters::PrintTuple<K>(diff, t);

            std::cout << "------------------------------------------" << std::endl;

#endif
            DREAM3D_REQUIRED(delta, <=, thr)
            break;
          }
        }
      }
      //   printf("numErrors: %llu\n", numErrors)
#endif
    }


    if(euCheck)
    {
      // Use original OM when we first generated the Euler Angles
      entry.pop_back();
    }

    typename DataArray<K>::Pointer junk = DataArray<K>::CreateArray(1, "Junk");
    QString typeName = junk->getTypeAsString();
#if REMOVE_TEST_FILES
    {

      AbstractFilter::Pointer writer = InstantiateFilter("DataContainerWriter");
      DREAM3D_REQUIRE_VALID_POINTER(writer.get())
      writer->setDataContainerArray(dca);

      QString outputFile;
      QTextStream out(&outputFile);

      out << UnitTest::TestTempDir << "/OrientationTransformsTest_";

      for(int e = 0; e < entry.size(); e++)
      {
        out << k_InputNames[entry[e]];
        if(e < entry.size()) { out << "_"; }
      }
      out << typeName << ".dream3d";
      var.setValue(outputFile);
      propWasSet = writer->setProperty("OutputFile", var);
      if (!propWasSet)
      {
        qDebug() << "Unable to set property OutputFile";
      }
      writer->execute();
      int err = writer->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)
    }
#endif

    {
      ss.str("");
      ss << testName << "Type: " << typeName.toStdString();
      TestPassed(ss.str());
      DREAM3D::unittest::CurrentMethod = "";
    }
  }
  catch (TestException& e)
  {
    TestFailed(DREAM3D::unittest::CurrentMethod);
    std::cout << e.what() << std::endl;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void StartTest()
{
  //  QVector<QString> functionNames = OrientationConverter<float>::GetOrientationTypeStrings();

  GenerateFunctionList generator;
  std::vector<GenerateFunctionList::EntryType> entries = generator.GeneratePermutationsOfCombinations(7, 2);

  // This outer loop will group the tests based on the first orientation representation
  for(int t = 0; t < 7; t++)
  {
    // Start looping on each entry in the function table.
    for(std::vector<GenerateFunctionList::EntryType>::size_type i = 0; i < entries.size(); i++)
    {

      GenerateFunctionList::EntryType entry = entries[i];
      if(entry[0] != t) { continue; }
      entry.push_back(entry[0]);
      //         if (entry[0] == 2
      //              && entry[1] == 1
      //             )
      {
        // RunTestCase<float>(entry, 16);
        RunTestCase<double>(entry, 16);
      }
    }
  }

  entries = generator.GeneratePermutationsOfCombinations(7, 3);
  // This outer loop will group the tests based on the first orientation representation
  for(int t = 0; t < 7; t++)
  {
    for(std::vector<GenerateFunctionList::EntryType>::size_type i = 0; i < entries.size(); i++)
    {
      GenerateFunctionList::EntryType entry = entries[i];
      if(entry[0] != t) { continue; }
      entry.push_back(entry[0]);
      //         if (entry[0] == 2
      //              && entry[1] == 4
      //              && entry[2] == 1
      //             )
      {
        //  RunTestCase<float>(entry, 16);
        RunTestCase<double>(entry, 16);
      }
    }
  }
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Instantiate the QCoreApplication that we need to get the current path and load plugins.
  QCoreApplication app(argc, argv);
  QCoreApplication::setOrganizationName("BlueQuartz Software");
  QCoreApplication::setOrganizationDomain("bluequartz.net");
  QCoreApplication::setApplicationName("OrientationTransformTest");


  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();


  int err = EXIT_SUCCESS;

  StartTest();


  DREAM3D_REGISTER_TEST( RemoveTestFiles() );
  PRINT_TEST_SUMMARY();

  return err;
}
