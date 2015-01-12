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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "Matrix4x4Widget.h"

#include <QtCore/QMetaProperty>
#include <QtCore/QSignalMapper>

#include "DREAM3DWidgetsLib/DREAM3DWidgetsLibConstants.h"

#include "FilterParameterWidgetsDialogs.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Matrix4x4Widget::Matrix4x4Widget(FilterParameter* parameter, AbstractFilter* filter, QWidget* parent) :
  FilterParameterWidget(parameter, filter, parent)
{
  setupUi(this);
  setupGui();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Matrix4x4Widget::~Matrix4x4Widget()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Matrix4x4Widget::setupGui()
{
  // Catch when the filter is about to execute the preflight
  connect(getFilter(), SIGNAL(preflightAboutToExecute()),
          this, SLOT(beforePreflight()));

  // Catch when the filter is finished running the preflight
  connect(getFilter(), SIGNAL(preflightExecuted()),
          this, SLOT(afterPreflight()));

  // Catch when the filter wants its values updated
  connect(getFilter(), SIGNAL(updateFilterParameters(AbstractFilter*)),
          this, SLOT(filterNeedsInputParameters(AbstractFilter*)));


  connect(v11, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v12, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v13, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v14, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v21, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v22, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v23, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v24, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v31, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v32, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v33, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v34, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v41, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v42, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v43, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );
  connect(v44, SIGNAL(textChanged(const QString&)),
          this, SLOT(widgetChanged(const QString&) ) );


  QDoubleValidator* v11Val = new QDoubleValidator(v11);
  v11->setValidator(v11Val);
  QDoubleValidator* v12Val = new QDoubleValidator(v12);
  v12->setValidator(v12Val);
  QDoubleValidator* v13Val = new QDoubleValidator(v13);
  v13->setValidator(v13Val);
  QDoubleValidator* v14Val = new QDoubleValidator(v14);
  v14->setValidator(v14Val);

  QDoubleValidator* v21Val = new QDoubleValidator(v21);
  v22->setValidator(v21Val);
  QDoubleValidator* v22Val = new QDoubleValidator(v22);
  v22->setValidator(v22Val);
  QDoubleValidator* v23Val = new QDoubleValidator(v23);
  v23->setValidator(v23Val);
  QDoubleValidator* v24Val = new QDoubleValidator(v24);
  v24->setValidator(v24Val);

  QDoubleValidator* v31Val = new QDoubleValidator(v31);
  v33->setValidator(v31Val);
  QDoubleValidator* v32Val = new QDoubleValidator(v32);
  v34->setValidator(v32Val);
  QDoubleValidator* v33Val = new QDoubleValidator(v33);
  v33->setValidator(v33Val);
  QDoubleValidator* v34Val = new QDoubleValidator(v34);
  v34->setValidator(v34Val);

  QDoubleValidator* v41Val = new QDoubleValidator(v41);
  v44->setValidator(v41Val);
  QDoubleValidator* v42Val = new QDoubleValidator(v42);
  v44->setValidator(v42Val);
  QDoubleValidator* v43Val = new QDoubleValidator(v43);
  v44->setValidator(v43Val);
  QDoubleValidator* v44Val = new QDoubleValidator(v44);
  v44->setValidator(v44Val);

  if (getFilterParameter() != NULL)
  {
    QString units = getFilterParameter()->getUnits();
    if(units.isEmpty() == false)
    {
      label->setText(getFilterParameter()->getHumanLabel() + " (" + units + ")");
    }
    else
    {
      label->setText(getFilterParameter()->getHumanLabel() );
    }

    Float4x4_t data = getFilter()->property(PROPERTY_NAME_AS_CHAR).value<Float4x4_t>();
    v11->setText( QString::number(data.v11)  );
    v12->setText( QString::number(data.v12)  );
    v13->setText( QString::number(data.v13)  );
    v14->setText( QString::number(data.v14)  );

    v21->setText( QString::number(data.v21) );
    v22->setText( QString::number(data.v22) );
    v23->setText( QString::number(data.v23) );
    v24->setText( QString::number(data.v24) );

    v31->setText( QString::number(data.v31) );
    v32->setText( QString::number(data.v32) );
    v33->setText( QString::number(data.v33) );
    v34->setText( QString::number(data.v34) );

    v41->setText( QString::number(data.v41) );
    v42->setText( QString::number(data.v42) );
    v43->setText( QString::number(data.v43) );
    v44->setText( QString::number(data.v44) );
  }

  //connect combobox to symmetry state
  connect(comboBox, SIGNAL(currentIndexChanged( int )), this, SLOT( changeSym( int ) ) );
  clearSym();
  changeSym(comboBox->currentIndex());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Matrix4x4Widget::widgetChanged(const QString& text)
{
  emit parametersChanged();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Matrix4x4Widget::filterNeedsInputParameters(AbstractFilter* filter)
{
  bool ok = false;
  Float4x4_t data;
  data.v11 = v11->text().toDouble(&ok);
  data.v12 = v12->text().toDouble(&ok);
  data.v13 = v13->text().toDouble(&ok);
  data.v14 = v14->text().toDouble(&ok);

  data.v21 = v21->text().toDouble(&ok);
  data.v22 = v22->text().toDouble(&ok);
  data.v23 = v23->text().toDouble(&ok);
  data.v24 = v24->text().toDouble(&ok);

  data.v31 = v31->text().toDouble(&ok);
  data.v32 = v32->text().toDouble(&ok);
  data.v33 = v33->text().toDouble(&ok);
  data.v34 = v34->text().toDouble(&ok);

  data.v41 = v41->text().toDouble(&ok);
  data.v42 = v42->text().toDouble(&ok);
  data.v43 = v43->text().toDouble(&ok);
  data.v44 = v44->text().toDouble(&ok);

  QVariant v;
  v.setValue(data);
  ok = filter->setProperty(PROPERTY_NAME_AS_CHAR, v);
  if(false == ok)
  {
    FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(getFilter(), getFilterParameter());
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Matrix4x4Widget::beforePreflight()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Matrix4x4Widget::afterPreflight()
{

}
