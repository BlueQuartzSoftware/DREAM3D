/* ============================================================================
* Copyright (c) 2009-2016 BlueQuartz Software, LLC
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

#include <QtCore/QCoreApplication>
#include <QtCore/QString>

#include "SIMPLib/Common/FilterFactory.hpp"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/FilterPipeline.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/CoreFilters/DataContainerWriter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"
#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Utilities/QMetaObjectUtilities.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "StatisticsTestFileLocations.h"

static const QString DCName("HistogramTest");
static const QString Data_AMName("Data");
static const QString Hist_AMName("Histograms");
static const QString Duration_Name("Faithful Duration");
static const QString WaitTime_Name("Faithful Wait Time");
static const QString DurationHistogram_Name("Duration Histogram");
static const QString WaitTimeHistogram_Name("Wait Time Histogram");

static const int Faithful_Rows = 272;
static const int Faithful_Cols = 3;
double faithful[Faithful_Rows][Faithful_Cols] = {
    // Index, Duration, Wait Time
    {1, 3.6, 79},     {2, 1.8, 54},     {3, 3.333, 74},   {4, 2.283, 62},   {5, 4.533, 85},   {6, 2.883, 55},   {7, 4.7, 88},     {8, 3.6, 85},     {9, 1.95, 51},    {10, 4.35, 85},
    {11, 1.833, 54},  {12, 3.917, 84},  {13, 4.2, 78},    {14, 1.75, 47},   {15, 4.7, 83},    {16, 2.167, 52},  {17, 1.75, 62},   {18, 4.8, 84},    {19, 1.6, 52},    {20, 4.25, 79},
    {21, 1.8, 51},    {22, 1.75, 47},   {23, 3.45, 78},   {24, 3.067, 69},  {25, 4.533, 74},  {26, 3.6, 83},    {27, 1.967, 55},  {28, 4.083, 76},  {29, 3.85, 78},   {30, 4.433, 79},
    {31, 4.3, 73},    {32, 4.467, 77},  {33, 3.367, 66},  {34, 4.033, 80},  {35, 3.833, 74},  {36, 2.017, 52},  {37, 1.867, 48},  {38, 4.833, 80},  {39, 1.833, 59},  {40, 4.783, 90},
    {41, 4.35, 80},   {42, 1.883, 58},  {43, 4.567, 84},  {44, 1.75, 58},   {45, 4.533, 73},  {46, 3.317, 83},  {47, 3.833, 64},  {48, 2.1, 53},    {49, 4.633, 82},  {50, 2, 59},
    {51, 4.8, 75},    {52, 4.716, 90},  {53, 1.833, 54},  {54, 4.833, 80},  {55, 1.733, 54},  {56, 4.883, 83},  {57, 3.717, 71},  {58, 1.667, 64},  {59, 4.567, 77},  {60, 4.317, 81},
    {61, 2.233, 59},  {62, 4.5, 84},    {63, 1.75, 48},   {64, 4.8, 82},    {65, 1.817, 60},  {66, 4.4, 92},    {67, 4.167, 78},  {68, 4.7, 78},    {69, 2.067, 65},  {70, 4.7, 73},
    {71, 4.033, 82},  {72, 1.967, 56},  {73, 4.5, 79},    {74, 4, 71},      {75, 1.983, 62},  {76, 5.067, 76},  {77, 2.017, 60},  {78, 4.567, 78},  {79, 3.883, 76},  {80, 3.6, 83},
    {81, 4.133, 75},  {82, 4.333, 82},  {83, 4.1, 70},    {84, 2.633, 65},  {85, 4.067, 73},  {86, 4.933, 88},  {87, 3.95, 76},   {88, 4.517, 80},  {89, 2.167, 48},  {90, 4, 86},
    {91, 2.2, 60},    {92, 4.333, 90},  {93, 1.867, 50},  {94, 4.817, 78},  {95, 1.833, 63},  {96, 4.3, 72},    {97, 4.667, 84},  {98, 3.75, 75},   {99, 1.867, 51},  {100, 4.9, 82},
    {101, 2.483, 62}, {102, 4.367, 88}, {103, 2.1, 49},   {104, 4.5, 83},   {105, 4.05, 81},  {106, 1.867, 47}, {107, 4.7, 84},   {108, 1.783, 52}, {109, 4.85, 86},  {110, 3.683, 81},
    {111, 4.733, 75}, {112, 2.3, 59},   {113, 4.9, 89},   {114, 4.417, 79}, {115, 1.7, 59},   {116, 4.633, 81}, {117, 2.317, 50}, {118, 4.6, 85},   {119, 1.817, 59}, {120, 4.417, 87},
    {121, 2.617, 53}, {122, 4.067, 69}, {123, 4.25, 77},  {124, 1.967, 56}, {125, 4.6, 88},   {126, 3.767, 81}, {127, 1.917, 45}, {128, 4.5, 82},   {129, 2.267, 55}, {130, 4.65, 90},
    {131, 1.867, 45}, {132, 4.167, 83}, {133, 2.8, 56},   {134, 4.333, 89}, {135, 1.833, 46}, {136, 4.383, 82}, {137, 1.883, 51}, {138, 4.933, 86}, {139, 2.033, 53}, {140, 3.733, 79},
    {141, 4.233, 81}, {142, 2.233, 60}, {143, 4.533, 82}, {144, 4.817, 77}, {145, 4.333, 76}, {146, 1.983, 59}, {147, 4.633, 80}, {148, 2.017, 49}, {149, 5.1, 96},   {150, 1.8, 53},
    {151, 5.033, 77}, {152, 4, 77},     {153, 2.4, 65},   {154, 4.6, 81},   {155, 3.567, 71}, {156, 4, 70},     {157, 4.5, 81},   {158, 4.083, 93}, {159, 1.8, 53},   {160, 3.967, 89},
    {161, 2.2, 45},   {162, 4.15, 86},  {163, 2, 58},     {164, 3.833, 78}, {165, 3.5, 66},   {166, 4.583, 76}, {167, 2.367, 63}, {168, 5, 88},     {169, 1.933, 52}, {170, 4.617, 93},
    {171, 1.917, 49}, {172, 2.083, 57}, {173, 4.583, 77}, {174, 3.333, 68}, {175, 4.167, 81}, {176, 4.333, 81}, {177, 4.5, 73},   {178, 2.417, 50}, {179, 4, 85},     {180, 4.167, 74},
    {181, 1.883, 55}, {182, 4.583, 77}, {183, 4.25, 83},  {184, 3.767, 83}, {185, 2.033, 51}, {186, 4.433, 78}, {187, 4.083, 84}, {188, 1.833, 46}, {189, 4.417, 83}, {190, 2.183, 55},
    {191, 4.8, 81},   {192, 1.833, 57}, {193, 4.8, 76},   {194, 4.1, 84},   {195, 3.966, 77}, {196, 4.233, 81}, {197, 3.5, 87},   {198, 4.366, 77}, {199, 2.25, 51},  {200, 4.667, 78},
    {201, 2.1, 60},   {202, 4.35, 82},  {203, 4.133, 91}, {204, 1.867, 53}, {205, 4.6, 78},   {206, 1.783, 46}, {207, 4.367, 77}, {208, 3.85, 84},  {209, 1.933, 49}, {210, 4.5, 83},
    {211, 2.383, 71}, {212, 4.7, 80},   {213, 1.867, 49}, {214, 3.833, 75}, {215, 3.417, 64}, {216, 4.233, 76}, {217, 2.4, 53},   {218, 4.8, 94},   {219, 2, 55},     {220, 4.15, 76},
    {221, 1.867, 50}, {222, 4.267, 82}, {223, 1.75, 54},  {224, 4.483, 75}, {225, 4, 78},     {226, 4.117, 79}, {227, 4.083, 78}, {228, 4.267, 78}, {229, 3.917, 70}, {230, 4.55, 79},
    {231, 4.083, 70}, {232, 2.417, 54}, {233, 4.183, 86}, {234, 2.217, 50}, {235, 4.45, 90},  {236, 1.883, 54}, {237, 1.85, 54},  {238, 4.283, 77}, {239, 3.95, 79},  {240, 2.333, 64},
    {241, 4.15, 75},  {242, 2.35, 47},  {243, 4.933, 86}, {244, 2.9, 63},   {245, 4.583, 85}, {246, 3.833, 82}, {247, 2.083, 57}, {248, 4.367, 82}, {249, 2.133, 67}, {250, 4.35, 74},
    {251, 2.2, 54},   {252, 4.45, 83},  {253, 3.567, 73}, {254, 4.5, 73},   {255, 4.15, 88},  {256, 3.817, 80}, {257, 3.917, 71}, {258, 4.45, 83},  {259, 2, 56},     {260, 4.283, 79},
    {261, 4.767, 78}, {262, 4.533, 84}, {263, 1.85, 58},  {264, 4.25, 83},  {265, 1.983, 43}, {266, 2.25, 60},  {267, 4.75, 75},  {268, 4.117, 81}, {269, 2.15, 46},  {270, 4.417, 90},
    {271, 1.817, 46}, {272, 4.467, 74}};

static const int HistogramBins = 8;
static const int HistogramCols = 2;
static const float HistogramMinRange = 1.5;
static const float HistogramMaxRange = 5.5;
float ExpectedHistogram[HistogramBins][HistogramCols] = {{2.0f, 51.0f}, {2.5, 41.0f}, {3.0f, 5.0f}, {3.5, 7.0f}, {4.0f, 30.0f}, {4.5, 73.0f}, {5.0f, 61.0f}, {5.5, 4.0f}};

class CalculateArrayHistogramTest
{
public:
  CalculateArrayHistogramTest()
  {
  }
  virtual ~CalculateArrayHistogramTest()
  {
  }
  SIMPL_TYPE_MACRO(CalculateArrayHistogramTest)

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  int TestFilterAvailability()
  {
    // Now instantiate the CalculateArrayHistogram Filter from the FilterManager
    QString filtName = "CalculateArrayHistogram";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr == filterFactory.get())
    {
      std::stringstream ss;
      ss << "The CalculateArrayHistogramTest Requires the use of the " << filtName.toStdString() << " filter which is found in the IO Plugin";
      DREAM3D_TEST_THROW_EXCEPTION(ss.str())
    }
    return 0;
  }

  // -----------------------------------------------------------------------------
  //
  // -----------------------------------------------------------------------------
  void TestFaithful()
  {
    Observer obs; // Create an Observer to report errors/progress from the executing pipeline

    DataContainerArray::Pointer dca = DataContainerArray::New();
    DataContainer::Pointer dc = DataContainer::New(DCName);

    QVector<size_t> tDims(1, 0);
    tDims[0] = Faithful_Rows;
    QVector<size_t> cDims(1);
    cDims[0] = 1;
    AttributeMatrix::Pointer am = AttributeMatrix::New(tDims, Data_AMName, AttributeMatrix::Type::Cell);

    DoubleArrayType::Pointer duration = DoubleArrayType::CreateArray(tDims, cDims, Duration_Name);
    DoubleArrayType::Pointer wait = DoubleArrayType::CreateArray(tDims, cDims, WaitTime_Name);

    for(size_t i = 0; i < tDims[0]; i++)
    {
      duration->setComponent(i, 0, faithful[i][1]); // Duration first
      wait->setComponent(i, 0, faithful[i][2]);     // Duration first
    }

    am->addAttributeArray(duration->getName(), duration); // int array
    am->addAttributeArray(wait->getName(), wait);         // int array

    dc->addAttributeMatrix(am->getName(), am);
    dca->addDataContainer(dc);

    // Now instantiate the CalculateArrayHistogram Filter from the FilterManager
    QString filtName = "CalculateArrayHistogram";
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer filterFactory = fm->getFactoryForFilter(filtName);
    if(nullptr != filterFactory.get())
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer filter = filterFactory->create();
      filter->setDataContainerArray(dca);
      filter->connect(filter.get(), SIGNAL(filterGeneratedMessage(const PipelineMessage&)), &obs, SLOT(processPipelineMessage(const PipelineMessage&)));

      QVariant var;
      QString str;
      bool propWasSet;
      DataArrayPath path;

      path = DataArrayPath(dc->getName(), am->getName(), duration->getName());
      var.setValue(path);
      propWasSet = filter->setProperty("SelectedArrayPath", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(HistogramBins);
      propWasSet = filter->setProperty("NumberOfBins", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(HistogramMinRange);
      propWasSet = filter->setProperty("MinRange", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      var.setValue(HistogramMaxRange);
      propWasSet = filter->setProperty("MaxRange", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)
      var.setValue(true);
      propWasSet = filter->setProperty("UserDefinedRange", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(false);
      propWasSet = filter->setProperty("Normalize", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      QVariant qv(Hist_AMName);
      propWasSet = filter->setProperty("NewAttributeMatrixName", qv);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      QVariant qv1(DurationHistogram_Name);
      propWasSet = filter->setProperty("NewDataArrayName", qv1);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      var.setValue(false);
      propWasSet = filter->setProperty("NewDataContainer", var);
      DREAM3D_REQUIRE_EQUAL(propWasSet, true)

      filter->execute();
      DREAM3D_REQUIRED(filter->getErrorCondition(), >=, 0);

      DataContainerWriter::Pointer writer = DataContainerWriter::New();
      writer->setDataContainerArray(dca);
      writer->setOutputFile(UnitTest::CalculateArrayHistogramTest::OldFaithfulFile);
      writer->execute();
      DREAM3D_REQUIRED(writer->getErrorCondition(), >=, 0);

      // Now lets get the output data
      path = DataArrayPath(DCName, Hist_AMName, DurationHistogram_Name);
      cDims[0] = 2; // the output array has 2 components
      IDataArray::Pointer histArray = dca->getPrereqArrayFromPath<DoubleArrayType, AbstractFilter>(filter.get(), path, cDims);
      DoubleArrayType::Pointer durationPtr = std::dynamic_pointer_cast<DoubleArrayType>(histArray);

      // Validate the pointer before we go any further
      DREAM3D_REQUIRE_VALID_POINTER(durationPtr.get())

      DREAM3D_REQUIRE_EQUAL(durationPtr->getNumberOfComponents(), HistogramCols)
      DREAM3D_REQUIRE_EQUAL(durationPtr->getNumberOfTuples(), HistogramBins)

      for(size_t r = 0; r < 8; r++)
      {
        std::cout << durationPtr->getComponent(r, 0) << "\t" << durationPtr->getComponent(r, 1) << std::endl;
        DREAM3D_REQUIRE_EQUAL(durationPtr->getComponent(r, 0), ExpectedHistogram[r][0])
        DREAM3D_REQUIRE_EQUAL(durationPtr->getComponent(r, 1), ExpectedHistogram[r][1])
      }
    }
    else
    {
      QString ss = QObject::tr("CalculateArrayHistogramTest Error creating filter '%1'. Filter was not created/executed. Please notify the developers.").arg(filtName);
      DREAM3D_REQUIRE_EQUAL(0, 1)
    }
  }

  /**
    * @brief
  */
  void operator()()
  {
    int err = EXIT_SUCCESS;
    DREAM3D_REGISTER_TEST(TestFilterAvailability());
    // DREAM3D_REGISTER_TEST( CalculateArrayHistogramTest() )
    DREAM3D_REGISTER_TEST(TestFaithful())
  }

private:
  CalculateArrayHistogramTest(const CalculateArrayHistogramTest&); // Copy Constructor Not Implemented
  void operator=(const CalculateArrayHistogramTest&);              // Operator '=' Not Implemented
};
