/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
 * Copyright (c) 2014 Dr. Joseph C. Tucker (UES, Inc.)
 * All rights reserved.
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
 * Neither the name of Joseph C. Tucker, Michael A. Groeber, Michael A. Jackson,
 * UES, Inc., the US Air Force, BlueQuartz Software nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 *  This code was written under United States Air Force Contract number
 *                   FA8650-07-D-5800 and FA8650-13-M-5048
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "R3DanalyticsReaderPlugin.h"

#include "PipelineBuilder/FilterWidgetManager.h"
#include "FilterWidgets/QFilterWidgetFactory.hpp"

#include "moc_R3DanalyticsReaderPlugin.cxx"

Q_EXPORT_PLUGIN2(R3DanalyticsReaderPlugin, R3DanalyticsReaderPlugin)

namespace Detail
{
   const std::string R3DanalyticsReaderPluginFile("R3DanalyticsReaderPlugin");
   const std::string R3DanalyticsReaderPluginDisplayName("R3DanalyticsReaderPlugin");
   const std::string R3DanalyticsReaderPluginBaseName("R3DanalyticsReaderPlugin");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DanalyticsReaderPlugin::R3DanalyticsReaderPlugin()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
R3DanalyticsReaderPlugin::~R3DanalyticsReaderPlugin()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString R3DanalyticsReaderPlugin::getPluginName()
{
  return QString::fromStdString(Detail::R3DanalyticsReaderPluginDisplayName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DanalyticsReaderPlugin::writeSettings(QSettings &prefs)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void R3DanalyticsReaderPlugin::readSettings(QSettings &prefs)
{

}

#include "R3DanalyticsReaderPluginFilters/RegisterKnownFilterWidgets.cpp"

