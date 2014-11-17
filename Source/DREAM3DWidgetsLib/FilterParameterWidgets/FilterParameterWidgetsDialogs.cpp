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
#include "FilterParameterWidgetsDialogs.h"

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtGui/QMessageBox>

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/FilterParameters/FilterParameter.h"

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
