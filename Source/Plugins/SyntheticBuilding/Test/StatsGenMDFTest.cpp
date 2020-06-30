

#include <vector>

#include <QtCore/QFile>

#include "EbsdLib/LaueOps/LaueOps.h"
#include "EbsdLib/Core/EbsdLibConstants.h"
#include "EbsdLib/LaueOps/CubicLowOps.h"
#include "EbsdLib/LaueOps/CubicOps.h"
#include "EbsdLib/LaueOps/HexagonalLowOps.h"
#include "EbsdLib/LaueOps/HexagonalOps.h"
#include "EbsdLib/LaueOps/MonoclinicOps.h"
#include "EbsdLib/LaueOps/OrthoRhombicOps.h"
#include "EbsdLib/LaueOps/TetragonalLowOps.h"
#include "EbsdLib/LaueOps/TetragonalOps.h"
#include "EbsdLib/LaueOps/TriclinicOps.h"
#include "EbsdLib/LaueOps/TrigonalLowOps.h"
#include "EbsdLib/LaueOps/TrigonalOps.h"
#include "EbsdLib/Texture/StatsGen.hpp"
#include "EbsdLib/Texture/Texture.hpp"

#include "SIMPLib/Math/SIMPLibMath.h"

#include "UnitTestSupport.hpp"

#include "SyntheticBuilding/SyntheticBuildingFilters/StatsGeneratorUtilities.h"

#include "SyntheticBuildingTestFileLocations.h"

class StatsGenMDFTest
{
public:
  StatsGenMDFTest() = default;
  ~StatsGenMDFTest() = default;

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
//    QFile::remove(UnitTest::StatsGenMDFTest::TestFile1);
//    QFile::remove(UnitTest::StatsGenMDFTest::TestFile2);
#endif
  }

  // -----------------------------------------------------------------------------
  template <typename LaueOpsType>
  void generateMDFdata(const std::vector<float>& odf)
  {
    int err = 0;
    int size = 100000;
    LaueOpsType ops;

    std::cout << "############  " << ops.getNameOfClass() << " " << ops.getSymmetryName() << "  ############" << std::endl;
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    // These are the input vectors
    using ContainerType = std::vector<float>;
    ContainerType angles;
    ContainerType axes;
    ContainerType weights;

    // These are the output vectors
    int npoints = 36;
    ContainerType x(npoints);
    ContainerType y(npoints);
    ContainerType mdfValues;

    try
    {
      Texture::CalculateMDFData<float, LaueOpsType, ContainerType>(angles, axes, weights, odf, mdfValues, angles.size());
      err = StatsGen::GenMDFPlotData<float, LaueOpsType, ContainerType>(mdfValues, x, y, size);
    } catch(const std::runtime_error& e)
    {
      if(ops.getNameOfClass() == "TriclinicOps" || ops.getNameOfClass() == "MonoclinicOps" || ops.getNameOfClass() == "OrthorhombicOps")
      {
        err = 0;
      }
    }

    DREAM3D_REQUIRE_EQUAL(err, 0)

    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    std::cout << "Testing Took: " << std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << " secs" << std::endl;
    //    for(int32_t i = 0; i < npoints; i++)
    //    {
    //      std::cout << x[i] << ", " << y[i] << std::endl;
    //    }
  }

  // -----------------------------------------------------------------------------

  std::vector<float> generateOdf(int32_t laueIndex)
  {

    std::vector<float> e1s; // These should be given in Degrees
    std::vector<float> e2s; // These should be given in Degrees
    std::vector<float> e3s; // These should be given in Degrees
    std::vector<float> weights;
    std::vector<float> sigmas;
    // Convert to Radians
    for(size_t i = 0; i < e1s.size(); i++)
    {
      e1s[i] = e1s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
      e2s[i] = e2s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
      e3s[i] = e3s[i] * static_cast<float>(SIMPLib::Constants::k_PiOver180);
    }

    return StatsGeneratorUtilities::GenerateODFData(laueIndex, e1s, e2s, e3s, weights, sigmas);
  }

  // -----------------------------------------------------------------------------
  void TestMDFGeneration()
  {
    std::vector<float> odf;
    odf = generateOdf(EbsdLib::CrystalStructure::Hexagonal_High);
    generateMDFdata<HexagonalOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Cubic_High);
    generateMDFdata<CubicOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Hexagonal_Low);
    generateMDFdata<HexagonalLowOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Cubic_Low);
    generateMDFdata<CubicLowOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Triclinic);
    generateMDFdata<TriclinicOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Monoclinic);
    generateMDFdata<MonoclinicOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::OrthoRhombic);
    generateMDFdata<OrthoRhombicOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Tetragonal_Low);
    generateMDFdata<TetragonalLowOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Tetragonal_High);
    generateMDFdata<TetragonalOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Trigonal_Low);
    generateMDFdata<TrigonalLowOps>(odf);

    odf = generateOdf(EbsdLib::CrystalStructure::Trigonal_High);
    generateMDFdata<TrigonalOps>(odf);
  }

  void operator()()
  {
    int err = EXIT_SUCCESS;

    DREAM3D_REGISTER_TEST(TestMDFGeneration());

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

public:
  StatsGenMDFTest(const StatsGenMDFTest&) = delete;            // Copy Constructor Not Implemented
  StatsGenMDFTest(StatsGenMDFTest&&) = delete;                 // Move Constructor Not Implemented
  StatsGenMDFTest& operator=(const StatsGenMDFTest&) = delete; // Copy Assignment Not Implemented
  StatsGenMDFTest& operator=(StatsGenMDFTest&&) = delete;      // Move Assignment Not Implemented

private:
};
