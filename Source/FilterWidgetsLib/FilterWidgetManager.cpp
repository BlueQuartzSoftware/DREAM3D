/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 * Neither the name of Michael A. Groeber, Michael A. Jackson, the US Air Force,
 * BlueQuartz Software nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written
 * permission.
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "FilterWidgetManager.h"


#include "QFilterWidgetFactory.hpp"

#include "FilterWidgetsLib/Widgets/InputFileWidget.h"
#include "FilterWidgetsLib/Widgets/StringWidget.h"
#include "FilterWidgetsLib/Widgets/IntWidget.h"
#include "FilterWidgetsLib/Widgets/DoubleWidget.h"
#include "FilterWidgetsLib/Widgets/InputFileWidget.h"
#include "FilterWidgetsLib/Widgets/InputPathWidget.h"
#include "FilterWidgetsLib/Widgets/OutputFileWidget.h"
#include "FilterWidgetsLib/Widgets/OutputPathWidget.h"
#include "FilterWidgetsLib/Widgets/BooleanWidget.h"
#include "FilterWidgetsLib/Widgets/ChoiceWidget.h"
#include "FilterWidgetsLib/Widgets/IntVec3Widget.h"
#include "FilterWidgetsLib/Widgets/FloatVec3Widget.h"
#include "FilterWidgetsLib/Widgets/AxisAngleWidget.h"
#include "FilterWidgetsLib/Widgets/VolumeVertexArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VolumeEdgeArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VolumeFaceArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VolumeCellArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VolumeFeatureArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VolumeEnsembleArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/SurfaceVertexArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/SurfaceFaceArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/SurfaceEdgeArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/SurfaceFeatureArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/SurfaceEnsembleArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/EdgeVertexArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/EdgeEdgeArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/EdgeFeatureArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/EdgeEnsembleArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VertexVertexArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VertexFeatureArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VertexEnsembleArrayNameSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/ArraySelectionWidget.h"
#include "FilterWidgetsLib/Widgets/CellArrayComparisonSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/FeatureArrayComparisonSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/EnsembleArrayComparisonSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/VertexArrayComparisonSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/FaceArrayComparisonSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/EdgeArrayComparisonSelectionWidget.h"
#include "FilterWidgetsLib/Widgets/CustomWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::FilterWidgetManager()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::~FilterWidgetManager()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::Pointer FilterWidgetManager::Instance()
{
  static FilterWidgetManager::Pointer singleton;

  if (singleton.get() == NULL)
  {
    //   qDebug() << "FilterWidgetManager::Instance singleton was NULL" << "\n";
    singleton.reset (new FilterWidgetManager() );
    // qDebug() << "singleton.get(): " << singleton.get() << "\n";
  }
  return singleton;
}

// -----------------------------------------------------------------------------
//  Static Method
// -----------------------------------------------------------------------------
void FilterWidgetManager::RegisterFilterWidgetFactory(const QString& name, IFilterWidgetFactory::Pointer factory)
{
  if (NULL != factory.get() )
  {
    // Instantiate the Instance Manager for IFilterWidgetFactory
    FilterWidgetManager::Pointer idManager = FilterWidgetManager::Instance();
    idManager->addFilterWidgetFactory( name, factory );
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterWidgetManager::Collection FilterWidgetManager::getFactories()
{
  return m_Factories;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterWidgetManager::addFilterWidgetFactory(const QString& name, IFilterWidgetFactory::Pointer factory)
{
  m_Factories[name] = factory;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterWidgetManager::RegisterKnownFilterWidgets()
{
  FilterWidgetManager::Pointer idManager = FilterWidgetManager::Instance();

{
QFilterWidgetFactory<StringWidget>::Pointer factory = QFilterWidgetFactory<StringWidget>::New();
idManager->addFilterWidgetFactory( "StringWidget", factory );
}
{
QFilterWidgetFactory<IntWidget>::Pointer factory = QFilterWidgetFactory<IntWidget>::New();
idManager->addFilterWidgetFactory( "IntWidget", factory );
}
{
QFilterWidgetFactory<DoubleWidget>::Pointer factory = QFilterWidgetFactory<DoubleWidget>::New();
idManager->addFilterWidgetFactory( "DoubleWidget", factory );
}
{
QFilterWidgetFactory<InputFileWidget>::Pointer factory = QFilterWidgetFactory<InputFileWidget>::New();
idManager->addFilterWidgetFactory( "InputFileWidget", factory );
}
{
QFilterWidgetFactory<InputPathWidget>::Pointer factory = QFilterWidgetFactory<InputPathWidget>::New();
idManager->addFilterWidgetFactory( "InputPathWidget", factory );
}
{
QFilterWidgetFactory<OutputFileWidget>::Pointer factory = QFilterWidgetFactory<OutputFileWidget>::New();
idManager->addFilterWidgetFactory( "OutputFileWidget", factory );
}
{
QFilterWidgetFactory<OutputPathWidget>::Pointer factory = QFilterWidgetFactory<OutputPathWidget>::New();
idManager->addFilterWidgetFactory( "OutputPathWidget", factory );
}
{
QFilterWidgetFactory<BooleanWidget>::Pointer factory = QFilterWidgetFactory<BooleanWidget>::New();
idManager->addFilterWidgetFactory( "BooleanWidget", factory );
}
{
QFilterWidgetFactory<ChoiceWidget>::Pointer factory = QFilterWidgetFactory<ChoiceWidget>::New();
idManager->addFilterWidgetFactory( "ChoiceWidget", factory );
}
{
QFilterWidgetFactory<IntVec3Widget>::Pointer factory = QFilterWidgetFactory<IntVec3Widget>::New();
idManager->addFilterWidgetFactory( "IntVec3Widget", factory );
}
{
QFilterWidgetFactory<FloatVec3Widget>::Pointer factory = QFilterWidgetFactory<FloatVec3Widget>::New();
idManager->addFilterWidgetFactory( "FloatVec3Widget", factory );
}
{
QFilterWidgetFactory<AxisAngleWidget>::Pointer factory = QFilterWidgetFactory<AxisAngleWidget>::New();
idManager->addFilterWidgetFactory( "AxisAngleWidget", factory );
}
{
QFilterWidgetFactory<VolumeVertexArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VolumeVertexArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VolumeVertexArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VolumeEdgeArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VolumeEdgeArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VolumeEdgeArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VolumeFaceArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VolumeFaceArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VolumeFaceArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VolumeCellArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VolumeCellArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VolumeCellArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VolumeFeatureArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VolumeFeatureArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VolumeFeatureArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VolumeEnsembleArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VolumeEnsembleArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VolumeEnsembleArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<SurfaceVertexArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<SurfaceVertexArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "SurfaceVertexArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<SurfaceFaceArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<SurfaceFaceArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "SurfaceFaceArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<SurfaceEdgeArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<SurfaceEdgeArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "SurfaceEdgeArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<SurfaceFeatureArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<SurfaceFeatureArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "SurfaceFeatureArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<SurfaceEnsembleArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<SurfaceEnsembleArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "SurfaceEnsembleArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<EdgeVertexArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<EdgeVertexArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "EdgeVertexArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<EdgeEdgeArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<EdgeEdgeArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "EdgeEdgeArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<EdgeFeatureArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<EdgeFeatureArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "EdgeFeatureArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<EdgeEnsembleArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<EdgeEnsembleArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "EdgeEnsembleArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VertexVertexArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VertexVertexArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VertexVertexArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VertexFeatureArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VertexFeatureArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VertexFeatureArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<VertexEnsembleArrayNameSelectionWidget>::Pointer factory = QFilterWidgetFactory<VertexEnsembleArrayNameSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VertexEnsembleArrayNameSelectionWidget", factory );
}
{
QFilterWidgetFactory<ArraySelectionWidget>::Pointer factory = QFilterWidgetFactory<ArraySelectionWidget>::New();
idManager->addFilterWidgetFactory( "ArraySelectionWidget", factory );
}
{
QFilterWidgetFactory<CellArrayComparisonSelectionWidget>::Pointer factory = QFilterWidgetFactory<CellArrayComparisonSelectionWidget>::New();
idManager->addFilterWidgetFactory( "CellArrayComparisonSelectionWidget", factory );
}
{
QFilterWidgetFactory<FeatureArrayComparisonSelectionWidget>::Pointer factory = QFilterWidgetFactory<FeatureArrayComparisonSelectionWidget>::New();
idManager->addFilterWidgetFactory( "FeatureArrayComparisonSelectionWidget", factory );
}
{
QFilterWidgetFactory<EnsembleArrayComparisonSelectionWidget>::Pointer factory = QFilterWidgetFactory<EnsembleArrayComparisonSelectionWidget>::New();
idManager->addFilterWidgetFactory( "EnsembleArrayComparisonSelectionWidget", factory );
}
{
QFilterWidgetFactory<VertexArrayComparisonSelectionWidget>::Pointer factory = QFilterWidgetFactory<VertexArrayComparisonSelectionWidget>::New();
idManager->addFilterWidgetFactory( "VertexArrayComparisonSelectionWidget", factory );
}
{
QFilterWidgetFactory<FaceArrayComparisonSelectionWidget>::Pointer factory = QFilterWidgetFactory<FaceArrayComparisonSelectionWidget>::New();
idManager->addFilterWidgetFactory( "FaceArrayComparisonSelectionWidget", factory );
}
{
QFilterWidgetFactory<EdgeArrayComparisonSelectionWidget>::Pointer factory = QFilterWidgetFactory<EdgeArrayComparisonSelectionWidget>::New();
idManager->addFilterWidgetFactory( "EdgeArrayComparisonSelectionWidget", factory );
}
{
QFilterWidgetFactory<CustomWidget>::Pointer factory = QFilterWidgetFactory<CustomWidget>::New();
idManager->addFilterWidgetFactory( "CustomWidget", factory );
}
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QWidget* FilterWidgetManager::createWidget(FilterParameter* parameter, AbstractFilter *filter)
{
  IFilterWidgetFactory::Pointer factory = m_Factories.value(parameter->getWidgetType());
  return factory->createWidget(parameter, filter, NULL);
}



