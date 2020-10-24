// -----------------------------------------------------------------------------
// Insert your license & copyright information here
// -----------------------------------------------------------------------------
#pragma once

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Filtering/FilterFactory.hpp"
#include "SIMPLib/Filtering/FilterManager.h"
#include "SIMPLib/Filtering/FilterPipeline.h"
#include "SIMPLib/Filtering/QMetaObjectUtilities.h"
#include "SIMPLib/Plugin/ISIMPLibPlugin.h"
#include "SIMPLib/Plugin/SIMPLibPluginLoader.h"

#include "UnitTestSupport.hpp"

#include "SamplingTestFileLocations.h"

class ResampleRectGridToImageGeomTest
{

public:
  ResampleRectGridToImageGeomTest() = default;
  ~ResampleRectGridToImageGeomTest() = default;
  ResampleRectGridToImageGeomTest(const ResampleRectGridToImageGeomTest&) = delete;            // Copy Constructor
  ResampleRectGridToImageGeomTest(ResampleRectGridToImageGeomTest&&) = delete;                 // Move Constructor
  ResampleRectGridToImageGeomTest& operator=(const ResampleRectGridToImageGeomTest&) = delete; // Copy Assignment
  ResampleRectGridToImageGeomTest& operator=(ResampleRectGridToImageGeomTest&&) = delete;      // Move Assignment

  // -----------------------------------------------------------------------------
  void operator()()
  {
    int err = EXIT_SUCCESS;
  }

private:
};
