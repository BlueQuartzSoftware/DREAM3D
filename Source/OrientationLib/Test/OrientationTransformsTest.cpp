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


#include "SIMPLib/Common/FilterManager.h"

#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "OrientationLib/OrientationMath/OrientationMath.h"

#include "OrientationLib/OrientationMath/OrientationConverter.hpp"




#include "OrientationLibTestFileLocations.h"

#include "GenerateFunctionList.h"


static const QString DCName("Orientation Transforms Test");
static const QString AMName("Angles");

QString k_InputNames[7] = {"eu", "om", "qu", "ax", "ro", "ho", "cu"};

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
    ss << "The ZeissImport Plugin Requires the use of the " << filtName.toStdString() << " filter which is found in the ImageProcessing Plugin";
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
  QVector<size_t> cDims(1, 3);

  T phi1_min = static_cast<T>(0.0);
  T phi1_max = DConst::k_2Pi;
  T phi1_delta = (phi1_max - phi1_min)/static_cast<T>(nSteps);

  T phi_min = static_cast<T>(0.0);
  T phi_max = DConst::k_Pi;
  T phi_delta = (phi_max - phi_min)/static_cast<T>(nSteps);

  T phi2_min = static_cast<T>(0.0);
  T phi2_max = DConst::k_2Pi;
  T phi2_delta = (phi2_max - phi2_min)/static_cast<T>(nSteps);

  typename DataArray<T>::Pointer eulers = DataArray<T>::CreateArray(nSteps*nSteps*nSteps, cDims, k_InputNames[0]);

  size_t counter = 0;
  for(int i = 0; i < nSteps; i++)
  {
    for(int j = 1; j < nSteps+1; j++)
    {
      for(int k = 0; k < nSteps; k++)
      {
        eulers->setComponent(counter, 0, phi1_min+i*phi1_delta);
        eulers->setComponent(counter, 1, phi_min+j*phi_delta);
        eulers->setComponent(counter, 2, phi2_min+k*phi2_delta);
        counter++;
      }
    }
  }
  attrMat->addAttributeArray(eulers->getName(), eulers);



  typename EulerConverter<T>::Pointer euConv = EulerConverter<T>::New();
  euConv->setInputData(eulers);

  euConv->toOrientationMatrix();
  typename DataArray<T>::Pointer om = euConv->getOutputData();
  om->setName(k_InputNames[1]);
  attrMat->addAttributeArray(om->getName(), om);

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
void RunTestCase( GenerateFunctionList::EntryType& entry, size_t nSteps)
{
  try {
    QVector<QString> funcNames = EulerConverter<float>::GetOrientationTypeStrings();

    std::stringstream ss;
    //  std::cout << "Generating Pipeline for " ;
    for(int e = 0; e < entry.size(); e++)
    {
      //  std::cout << funcNames[entry[e]].toStdString() << " ==> ";
      ss << funcNames[entry[e]].toStdString();
      if(e != entry.size() - 1) {
        ss << " ==> ";
      }
    }
    // std::cout << std::endl;
    DREAM3D::unittest::CurrentMethod = ss.str();
    DREAM3D::unittest::numTests++;

    DataContainerArray::Pointer dca = DataContainerArray::New();

    DataContainer::Pointer m = DataContainer::New();
    m->setName(DCName);


    QVector<size_t> tDims(1, nSteps*nSteps*nSteps);
    AttributeMatrix::Pointer attrMat = AttributeMatrix::New(tDims, AMName, 3);
    m->addAttributeMatrix(AMName, attrMat);
    dca->addDataContainer(m);

    // Make all the starting data
    GenerateEulers<float>(nSteps, attrMat);

    QVariant var;
    bool propWasSet;
    QString inputName;
    QString outputName;



    for(int e = 0; e < entry.size() - 1; e++)
    {
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

      int outputType = entry[e+1];
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

      outputName = QString::number(e) + QString("_") + k_InputNames[entry[e]] + QString("2") + k_InputNames[entry[e+1]];
      var.setValue(outputName);
      propWasSet = convFilt->setProperty("OutputOrientationArrayName", var);
      if(!propWasSet)
      {
        qDebug() << "Unable to set property OutputOrientationArrayName";
      }

      convFilt->execute();
      int err=convFilt->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)

    }

    // Find Difference Map between originals and finals
    {
      AbstractFilter::Pointer diffMapFilt = InstantiateFilter("FindDifferenceMap");
      DREAM3D_REQUIRE_VALID_POINTER(diffMapFilt.get());
      diffMapFilt->setDataContainerArray(dca);

      inputName = k_InputNames[entry[0]];

      DataArrayPath dap(DCName, AMName, inputName);
      var.setValue(dap);
      propWasSet = diffMapFilt->setProperty("FirstInputArrayPath", var);
      if (!propWasSet) {
        qDebug() << "Unable to set property FirstInputArrayPath";
      }

      dap = DataArrayPath (DCName, AMName, outputName);
      var.setValue(dap);
      propWasSet = diffMapFilt->setProperty("SecondInputArrayPath", var);
      if (!propWasSet) {
        qDebug() << "Unable to set property SecondInputArrayPath";
      }


      QString diffMapArrayName = QString("%1 Difference").arg(k_InputNames[entry[0]]);
      dap = DataArrayPath (DCName, AMName, diffMapArrayName);
      var.setValue(dap);
      propWasSet = diffMapFilt->setProperty("DifferenceMapArrayPath", var);
      if (!propWasSet) {
        qDebug() << "Unable to set property DifferenceMapArrayPath";
      }

//      Observer obs;
//      obs.connect(diffMapFilt.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)),
//                  &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

      diffMapFilt->execute();
      int err=diffMapFilt->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)
    }

    #if !REMOVE_TEST_FILES
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
      out << ".dream3d";
      var.setValue(outputFile);
      propWasSet = writer->setProperty("OutputFile", var);
      if (!propWasSet) {
        qDebug() << "Unable to set property OutputFile";
      }
      writer->execute();
      int err=writer->getErrorCondition();
      DREAM3D_REQUIRED(err, >=, 0)
    }
    #endif

    TestPassed(ss.str());
    DREAM3D::unittest::CurrentMethod = "";

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
  QVector<QString> functionNames = OrientationConverter<float>::GetOrientationTypeStrings();

  GenerateFunctionList generator;
  std::vector<GenerateFunctionList::EntryType> entries = generator.GeneratePermutationsOfCombinations(6, 2);

  // Start looping on each entry in the function table.
  for(std::vector<GenerateFunctionList::EntryType>::size_type i = 0; i < entries.size(); i++)
  {
    GenerateFunctionList::EntryType entry = entries[i];
    entry.push_back(entry[0]);

    RunTestCase(entry, 20);

  }
}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char* argv[])
{
  // Register all the filters including trying to load those from Plugins
  FilterManager* fm = FilterManager::Instance();
  SIMPLibPluginLoader::LoadPluginFilters(fm);

  // Send progress messages from PipelineBuilder to this object for display
  QMetaObjectUtilities::RegisterMetaTypes();


  int err = EXIT_SUCCESS;

  StartTest();


  DREAM3D_REGISTER_TEST( RemoveTestFiles() );

  return err;
}
