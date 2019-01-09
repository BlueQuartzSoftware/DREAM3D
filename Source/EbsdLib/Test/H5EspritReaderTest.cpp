
#include <QtCore/QFile>
#include <QtCore/QString>

#include "H5Support/H5Lite.h"
#include "H5Support/H5ScopedSentinel.h"
#include "H5Support/H5Utilities.h"

#include "EbsdLib/BrukerNano/H5EspritReader.h"
#include "EbsdLib/Test/EbsdLibTestFileLocations.h"

#include "UnitTestSupport.hpp"

class H5EspritReaderTest
{
  const QString k_HDF5Path = QString("Section_435");

public:
  H5EspritReaderTest() = default;
  ~H5EspritReaderTest() = default;

  // -----------------------------------------------------------------------------
  void RemoveTestFiles()
  {
#if REMOVE_TEST_FILES
    QFile::remove(UnitTest::H5EspritReaderTest::OutputFile);
#endif
  }

  // -----------------------------------------------------------------------------
  void TestH5EspritReader()
  {
    H5EspritReader::Pointer reader = H5EspritReader::New();

    reader->readHeaderOnly();
    int32_t err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, ==, -1)

    reader->setHDF5Path(k_HDF5Path);
    reader->readHeaderOnly();
    err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, ==, -387)

    reader->setFileName("Some Random Path");
    reader->readHeaderOnly();
    err = reader->getErrorCode();
    DREAM3D_REQUIRED(err, ==, -2)

    reader->setFileName(UnitTest::H5EspritReaderTest::InputFile);
    reader->readHeaderOnly();
    err = reader->getErrorCode();
    if(err < 0)
    {
      qDebug() << reader->getErrorMessage();
    }

    int32_t ncols = reader->getNumColumns();
    DREAM3D_REQUIRED(ncols, ==, 600)

    int32_t nrows = reader->getNumRows();
    DREAM3D_REQUIRED(nrows, ==, 600)

    float xstep = reader->getXStep();
    DREAM3D_REQUIRED(xstep, ==, 0.0)
    float ystep = reader->getYStep();
    DREAM3D_REQUIRED(ystep, ==, 0.0)

    QString grid = reader->getGrid();
    DREAM3D_REQUIRED(grid, ==, Ebsd::H5Esprit::Isometric);
    DREAM3D_REQUIRED(err, >=, 0)
    int32_t patWidth = reader->getPatternWidth();
    DREAM3D_REQUIRED(patWidth, ==, 80)
    int32_t patHeight = reader->getPatternHeight();
    DREAM3D_REQUIRED(patHeight, ==, 60)

    int32_t xDim = reader->getXDimension();
    DREAM3D_REQUIRED(xDim, ==, 600)

    int32_t yDim = reader->getYDimension();
    DREAM3D_REQUIRED(yDim, ==, 600)

    QStringList scanNames;
    err = reader->readScanNames(scanNames);
    DREAM3D_REQUIRED(scanNames.size(), ==, 1)
    DREAM3D_REQUIRED(scanNames[0], ==, k_HDF5Path)

    err = reader->readFile();

    void* ptr = reader->getPointerByName(Ebsd::H5Esprit::DD);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::MAD);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::MADPhase);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::NIndexedBands);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::PCX);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::PCY);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::PHI);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::Phase);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::RadonBandCount);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::RadonQuality);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::XBEAM);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::YBEAM);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::XSAMPLE);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::YSAMPLE);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::phi1);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::phi2);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::RawPatterns);
    DREAM3D_REQUIRE_NULL_POINTER(ptr);

    Ebsd::NumType numType = reader->getPointerType(Ebsd::H5Esprit::DD);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::MAD);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::MADPhase);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Int32)
    numType = reader->getPointerType(Ebsd::H5Esprit::NIndexedBands);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Int32)
    numType = reader->getPointerType(Ebsd::H5Esprit::PCX);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::PCY);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::PHI);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::Phase);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Int32)
    numType = reader->getPointerType(Ebsd::H5Esprit::RadonBandCount);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Int32)
    numType = reader->getPointerType(Ebsd::H5Esprit::RadonQuality);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::XBEAM);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Int32)
    numType = reader->getPointerType(Ebsd::H5Esprit::YBEAM);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Int32)
    numType = reader->getPointerType(Ebsd::H5Esprit::XSAMPLE);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::YSAMPLE);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::phi1);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::phi2);
    DREAM3D_REQUIRED(numType, ==, Ebsd::Float)
    numType = reader->getPointerType(Ebsd::H5Esprit::RawPatterns);
    DREAM3D_REQUIRED(numType, ==, Ebsd::UInt8);

    float* xsamplePtr = reader->getXSAMPLEPointer();
    DREAM3D_REQUIRE_VALID_POINTER(xsamplePtr);
    reader->releaseXSAMPLEOwnership();
    ptr = reader->getPointerByName(Ebsd::H5Esprit::XSAMPLE);
    DREAM3D_REQUIRE_NULL_POINTER(ptr);
    reader->deallocateArrayData<float>(xsamplePtr);

    float* ysamplePtr = reader->getYSAMPLEPointer();
    DREAM3D_REQUIRE_VALID_POINTER(ysamplePtr);
    reader->releaseYSAMPLEOwnership();
    ptr = reader->getPointerByName(Ebsd::H5Esprit::YSAMPLE);
    DREAM3D_REQUIRE_NULL_POINTER(ptr);
    reader->deallocateArrayData<float>(ysamplePtr);

    // std::cout << "=============================================================" << std::endl;
    QSet<QString> arraysToRead;
    arraysToRead.insert(Ebsd::H5Esprit::phi1);
    arraysToRead.insert(Ebsd::H5Esprit::PHI);
    arraysToRead.insert(Ebsd::H5Esprit::phi2);
    reader->setArraysToRead(arraysToRead);
    err = reader->readFile();

    ptr = reader->getPointerByName(Ebsd::H5Esprit::phi1);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::phi2);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::PHI);
    DREAM3D_REQUIRE_VALID_POINTER(ptr);

    ptr = reader->getPointerByName(Ebsd::H5Esprit::XSAMPLE);
    DREAM3D_REQUIRE_NULL_POINTER(ptr);
    ptr = reader->getPointerByName(Ebsd::H5Esprit::YSAMPLE);
    DREAM3D_REQUIRE_NULL_POINTER(ptr);

#if 0
      reader->setReadPatternData(true);
      err = reader->readFile();
      err = reader->getErrorCode();
      if(err < 0)
      {
        qDebug() << reader->getErrorMessage();
      }
      DREAM3D_REQUIRED(err, >=, 0)
#endif
  }

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;

    std::cout << "#-- H5EspritReaderTest Starting" << std::endl;

    DREAM3D_REGISTER_TEST(TestH5EspritReader())

    DREAM3D_REGISTER_TEST(RemoveTestFiles())
  }

public:
  H5EspritReaderTest(const H5EspritReaderTest&) = delete;            // Copy Constructor Not Implemented
  H5EspritReaderTest(H5EspritReaderTest&&) = delete;                 // Move Constructor Not Implemented
  H5EspritReaderTest& operator=(const H5EspritReaderTest&) = delete; // Copy Assignment Not Implemented
  H5EspritReaderTest& operator=(H5EspritReaderTest&&) = delete;      // Move Assignment Not Implemented
};
