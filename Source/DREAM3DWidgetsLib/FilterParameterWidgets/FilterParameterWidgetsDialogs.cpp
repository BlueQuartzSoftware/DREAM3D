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

#include "FilterParameterWidgetsDialogs.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtWidgets/QMessageBox>

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidgetsDialogs::FilterParameterWidgetsDialogs()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
FilterParameterWidgetsDialogs::~FilterParameterWidgetsDialogs()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidgetsDialogs::ShowCouldNotSetFilterParameter(AbstractFilter* filter, FilterParameter* filterParameter)
{
  QString ss = QString("Error occurred transferring the Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                       " Please report this issue to the developers of this filter.").arg(filterParameter->getPropertyName()).arg(filter->getHumanLabel());
  QMessageBox::StandardButton reply;
  reply = QMessageBox::critical(NULL, QString("Filter Parameter Error"), ss, QMessageBox::Ok);
  Q_UNUSED(reply);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidgetsDialogs::ShowCouldNotSetConditionalFilterParameter(AbstractFilter* filter, FilterParameter* filterParameter)
{
  QString ss = QString("Error occurred transferring the Conditional Filter Parameter '%1' in Filter '%2' to the filter instance. The pipeline may run but the underlying filter will NOT be using the values from the GUI."
                       " Please report this issue to the developers of this filter.").arg(filterParameter->getPropertyName()).arg(filter->getHumanLabel());
  QMessageBox::StandardButton reply;
  reply = QMessageBox::critical(NULL, QString("Filter Parameter Error"), ss, QMessageBox::Ok);
  Q_UNUSED(reply);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidgetsDialogs::ShowFilterPropertyCastError(AbstractFilter* filter, FilterParameter* filterParameter, const QString& filtType, const QString& paramType)
{
  QString ss = QString("Filter Parameter mismatch for parameter '%1' in Filter '%2' from the type described in the FilterParameter.getCastableType() to the type returned by the filter. Type in FilterParameter is %3. Type Widget expecting is %4"
                       " Please report this issue to the developers of this filter.").arg(filterParameter->getPropertyName()).arg(filter->getHumanLabel()).arg(filtType).arg(paramType);
  QMessageBox::StandardButton reply;
  reply = QMessageBox::critical(NULL, QString("Filter Parameter Error"), ss, QMessageBox::Ok);
  Q_UNUSED(reply);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void FilterParameterWidgetsDialogs::ShowEmptyStringError(AbstractFilter* filter, FilterParameter* filterParameter)
{
  QString ss = QString("A value must be entered for parameter '%1' in Filter '%2' to the filter instance.").arg(filterParameter->getPropertyName()).arg(filter->getHumanLabel());
  QMessageBox::StandardButton reply;
  reply = QMessageBox::critical(NULL, QString("Filter Parameter Error"), ss, QMessageBox::Ok);
  Q_UNUSED(reply);
}
