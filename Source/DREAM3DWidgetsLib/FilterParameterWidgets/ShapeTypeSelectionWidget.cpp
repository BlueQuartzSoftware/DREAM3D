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

#include "ShapeTypeSelectionWidget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QList>

#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>

#include "DREAM3DLib/Common/ShapeType.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"
#include "DREAM3DLib/FilterParameters/ShapeTypeSelectionFilterParameter.h"
#include "DREAM3DLib/Utilities/QMetaObjectUtilities.h"
#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypeSelectionWidget::ShapeTypeSelectionWidget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent),
  m_DidCausePreflight(false)
{
  m_FilterParameter = dynamic_cast<ShapeTypeSelectionFilterParameter*>(parameter);
  Q_ASSERT_X(m_FilterParameter != NULL, "NULL Pointer", "ShapeTypeSelectionWidget can ONLY be used with a ShapeTypeSelectionFilterParameter object");

  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypeSelectionWidget::ShapeTypeSelectionWidget(QWidget* parent) :
  FilterParameterWidget(NULL, NULL, parent),
  m_DidCausePreflight(false)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ShapeTypeSelectionWidget::~ShapeTypeSelectionWidget()
{}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::setupGui()
{
  if(getFilter() == NULL)
  {
    return;
  }
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));

  if (getFilterParameter() == NULL)
  {
    return;
  }
  label->setText(getFilterParameter()->getHumanLabel() );

  updateComboBoxes();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::updateComboBoxes()
{
  bool ok = false;
  // setup the list of choices for the widget
  ShapeTypeSelectionFilterParameter* shapeType = dynamic_cast<ShapeTypeSelectionFilterParameter*>(getFilterParameter());
  QString countProp = shapeType->getPhaseTypeCountProperty();
  int size = getFilter()->property(countProp.toLatin1().constData()).toInt(&ok);

  UInt32Vector_t vectorWrapper = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<UInt32Vector_t>();
  QVector<quint32> dataFromFilter = vectorWrapper.d;

  // Get our list of predefined Shape Type Strings
  QVector<QString> shapeTypeStrings;
  ShapeType::getShapeTypeStrings(shapeTypeStrings);
  // Get our list of predefined enumeration values
  QVector<unsigned int> shapeTypeEnums;
  ShapeType::getShapeTypeEnums(shapeTypeEnums);

  // Remove all the items from the GUI and from the internal tracking Lists
  QLayoutItem* child;
  while ((formLayout_2->count() > 0) && (child = formLayout_2->takeAt(0)) != 0)
  {
    delete child;
  }
  m_ShapeTypeLabels.clear();
  m_ShapeTypeCombos.clear();

  // Create a whole new QWidget to hold everything
  m_ShapeTypeScrollContents = new QWidget();
  m_ShapeTypeScrollContents->setObjectName(QString::fromUtf8("m_ShapeTypeScrollContents"));
  formLayout_2 = new QFormLayout(m_ShapeTypeScrollContents);
  formLayout_2->setContentsMargins(4, 4, 4, 4);
  formLayout_2->setObjectName(QString::fromUtf8("formLayout_2"));
  formLayout_2->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
  formLayout_2->setHorizontalSpacing(6);
  formLayout_2->setVerticalSpacing(6);
  m_ShapeTypeScrollArea->setWidget(m_ShapeTypeScrollContents);

  // We skip the first Ensemble as it is always a dummy
  for (int i = 0; i < size - 1; i++)
  {
    QLabel* shapeTypeLabel = new QLabel(m_ShapeTypeScrollContents);
    QString str("Phase ");
    str.append(QString::number(i + 1, 10));
    str.append(":");
    shapeTypeLabel->setText(str);
    shapeTypeLabel->setObjectName(str);
    m_ShapeTypeLabels << shapeTypeLabel;

    formLayout_2->setWidget(i, QFormLayout::LabelRole, shapeTypeLabel);

    QComboBox* cb = new QComboBox(m_ShapeTypeScrollContents);
    str.append(" ComboBox");
    cb->setObjectName(str);
    for (int32_t s = 0; s < shapeTypeStrings.size(); ++s)
    {
      cb->addItem((shapeTypeStrings[s]), shapeTypeEnums[s]);
      cb->setItemData(static_cast<int>(s), shapeTypeEnums[s], Qt::UserRole);
    }
    m_ShapeTypeCombos << cb;
    formLayout_2->setWidget(i, QFormLayout::FieldRole, cb);
    if (i + 1 < dataFromFilter.size())
    {
      cb->setCurrentIndex(dataFromFilter[i + 1]);
    }
    connect(cb, SIGNAL(currentIndexChanged(int)),
            this, SLOT(comboboxChanged(int)) );

  }


}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::comboboxChanged(int index)
{
  m_DidCausePreflight = true;
  emit parametersChanged();
  m_DidCausePreflight = false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::beforePreflight()
{
  if (NULL == getFilter()) { return; }
  if(m_DidCausePreflight == true)
  {
    std::cout << "***  ShapeTypeSelectionWidget already caused a preflight, just returning" << std::endl;
    return;
  }

  updateComboBoxes();

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::afterPreflight()
{
  // std::cout << "After Preflight" << std::endl;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ShapeTypeSelectionWidget::filterNeedsInputParameters(AbstractFilter* filter)
{
  int count = m_ShapeTypeCombos.count();
  QVector<uint32_t> shapeTypes(count + 1, DREAM3D::ShapeType::UnknownShapeType);
  bool ok = false;
  for (int i = 0; i < count; ++i)
  {
    QComboBox* cb = m_ShapeTypeCombos.at(i);
    unsigned int sType = static_cast<unsigned int>(cb->itemData(cb->currentIndex(), Qt::UserRole).toUInt(&ok));
    shapeTypes[i + 1] = sType;
  }

  UInt32Vector_t data;
  data.d = shapeTypes;

  QVariant var;
  var.setValue(data);
  ok = false;
  // Set the value into the Filter
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, var);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}
