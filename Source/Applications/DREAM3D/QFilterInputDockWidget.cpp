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

#include "QFilterInputDockWidget.h"

#include <QtCore/QFileInfo>
#include <QtGui/QLabel>
#include <QtGui/QVBoxLayout>
#include <QtGui/QFormLayout>
#include <QtGui/QLineEdit>



//#include "DREAM3DLib/Common/IFilterFactory.hpp"
//#include "DREAM3DLib/Common/FilterFactory.hpp"

//#include "FilterWidgetsLib/Widgets/InputFileWidget.h"

#include "FilterWidgetsLib/FilterWidgetManager.h"

//#include "QFilterWidget.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterInputDockWidget::QFilterInputDockWidget(QWidget* parent) :
  QWidget(parent)
{
  setupUi(this);
  setupGui();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QFilterInputDockWidget::~QFilterInputDockWidget()
{
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterInputDockWidget::setupGui()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//void QFilterInputDockWidget::initInputFileWidget(FilterParameter* option, QVBoxLayout* vertLayout, AbstractFilter* filter)
//{
//  InputFileWidget* w = new InputFileWidget(basicInputsScrollWidget);
//  vertLayout->addWidget(w);
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QFilterInputDockWidget::setSelectedFilterWidget(AbstractFilter* filter)
{

  // Remove all existing QWidgets from this Widget
  QObjectList objs = basicInputsScrollWidget->children();
  for(int i = 0; i < objs.size(); i++)
  {
    objs[i]->deleteLater();
  }


  // Remove the Layout itself
  QLayoutItem* wItem = basicInputsScrollWidget->layout()->takeAt(0);
  while (wItem != NULL) {
    delete wItem;
    wItem = basicInputsScrollWidget->layout()->takeAt(0);
  }
  //delete verticalSpacer;
  delete verticalLayout;
  delete basicInputsScrollWidget;
  basicInputsScrollWidget = new QWidget();
  basicInputsScrollWidget->setObjectName(QString::fromUtf8("basicInputsScrollWidget"));
  basicInputsScrollWidget->setGeometry(QRect(0, 0, 388, 267));
  verticalLayout = new QVBoxLayout(basicInputsScrollWidget);
  verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
  scrollArea->setWidget(basicInputsScrollWidget);

  // Add a label at the top of the Inputs Tabs to show what filter we are working on
  filterHumanLabel->setText(filter->getHumanLabel());

  QSizePolicy sizePolicy2(QSizePolicy::Expanding, QSizePolicy::Maximum);
  sizePolicy2.setHorizontalStretch(0);
  sizePolicy2.setVerticalStretch(0);
  setSizePolicy(sizePolicy2);

  FilterWidgetManager::Pointer fwm = FilterWidgetManager::Instance();

  QVector<FilterParameter::Pointer> options = filter->getFilterParameters();
  int optIndex = 0;
  for (QVector<FilterParameter::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter )
  {

    FilterParameter* option = (*iter).get();
    FilterParameter::WidgetType wType = option->getWidgetType();


    QWidget w = fwm->createWidget(option->getWidgetType());
    if (NULL == w) continue;



    switch(wType)
    {
      case FilterParameter::StringWidget:
        break;
      case FilterParameter::IntWidget:
        break;
      case FilterParameter::DoubleWidget:
        break;
      case FilterParameter::InputFileWidget:
        //initInputFileWidget(option, verticalLayout, filter);
        break;
      case FilterParameter::InputPathWidget:
        break;
      case FilterParameter::OutputFileWidget:
        break;
      case FilterParameter::OutputPathWidget:
        break;
      case FilterParameter::BooleanWidget:
        break;
      case FilterParameter::ChoiceWidget: // Generic ComboBox Drop down where the filter provides the list of strings
        break;
      case FilterParameter::IntVec3Widget:
        break;
      case FilterParameter::FloatVec3Widget:
        //initFloatVec3Widget
        break;
      case FilterParameter::AxisAngleWidget:
        /* **** DO NOT PUT ANY OTHER WIDGETS BETWEEN THIS ***** */
        break;
      case FilterParameter::VolumeVertexArrayNameSelectionWidget: // ComboBox where the Cell Array names are used to populate
        break;
      case FilterParameter::VolumeEdgeArrayNameSelectionWidget: //ComboBox where the Feature Array names are used to populate
        break;
      case FilterParameter::VolumeFaceArrayNameSelectionWidget: //ComboBox where the Ensemble Array names are used to populate
        break;
      case FilterParameter::VolumeCellArrayNameSelectionWidget: // ComboBox where the Cell Array names are used to populate
        break;
      case FilterParameter::VolumeFeatureArrayNameSelectionWidget: //ComboBox where the Feature Array names are used to populate
        break;
      case FilterParameter::VolumeEnsembleArrayNameSelectionWidget: //ComboBox where the Ensemble Array names are used to populate
        break;
      case FilterParameter::SurfaceVertexArrayNameSelectionWidget:
        break;
      case FilterParameter::SurfaceFaceArrayNameSelectionWidget:
        break;
      case FilterParameter::SurfaceEdgeArrayNameSelectionWidget:
        break;
      case FilterParameter::SurfaceFeatureArrayNameSelectionWidget:
        break;
      case FilterParameter::SurfaceEnsembleArrayNameSelectionWidget:
        break;
      case FilterParameter::EdgeVertexArrayNameSelectionWidget:
        break;
      case FilterParameter::EdgeEdgeArrayNameSelectionWidget:
        break;
      case FilterParameter::EdgeFeatureArrayNameSelectionWidget:
        break;
      case FilterParameter::EdgeEnsembleArrayNameSelectionWidget:
        break;
      case FilterParameter::VertexVertexArrayNameSelectionWidget:
        break;
      case FilterParameter::VertexFeatureArrayNameSelectionWidget:
        break;
      case FilterParameter::VertexEnsembleArrayNameSelectionWidget:
        /* ****  AND THIS LINE ******** */
        break;
      case FilterParameter::ArraySelectionWidget: // This is the generic array name selection tool where the user can select multiple arrays with checkboxes from all data containers
        /* This widget presents a blank table and the user clicks an "Add" button to add arrays and Opertors */
        break;
      case FilterParameter::CellArrayComparisonSelectionWidget:
        break;
      case FilterParameter::FeatureArrayComparisonSelectionWidget:
        break;
      case FilterParameter::EnsembleArrayComparisonSelectionWidget:
        break;
      case FilterParameter::VertexArrayComparisonSelectionWidget:
        break;
      case FilterParameter::FaceArrayComparisonSelectionWidget:
        break;
      case FilterParameter::EdgeArrayComparisonSelectionWidget:
        break;
      case FilterParameter::CustomWidget:
        break;
      default:
        break;

    }


        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

#if 0
    QString labelName = (option->getHumanLabel());
    if (option->getUnits().isEmpty() == false)
    {
      labelName.append(" (").append((option->getUnits())).append(")");
    }
    QLabel* label = new QLabel(labelName, this);

    // if (wType == FilterParameter::StringWidget)
    {
      QLineEdit* le = new QLineEdit(this);
      le->setObjectName(option->getPropertyName());

      frmLayout->setWidget(optIndex, QFormLayout::LabelRole, label);
      frmLayout->setWidget(optIndex, QFormLayout::FieldRole, le);
      //connect(le, SIGNAL(textChanged(QString)), this, SLOT(updateQLineEditStringValue(const QString &)));
      //QVariant v = property(option->getPropertyName());
      le->setText(option->getPropertyName());
    }
#endif
    ++optIndex;
  }

}
