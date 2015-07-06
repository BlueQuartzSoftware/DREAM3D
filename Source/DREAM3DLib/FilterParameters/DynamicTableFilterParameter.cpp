/* ============================================================================
* Copyright (c) 2014 Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2014 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "DynamicTableFilterParameter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableFilterParameter::DynamicTableFilterParameter() :
  FilterParameter(),
  m_DefaultRowCount(0),
  m_DefaultColCount(0),
  m_AreRowsDynamic(false),
  m_AreColsDynamic(false),
  m_MinRowCount(0),
  m_MinColCount(0),
  m_ErrorCondition(0)
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableFilterParameter::~DynamicTableFilterParameter()
{}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DynamicTableFilterParameter::Pointer DynamicTableFilterParameter::New(const QString& humanLabel, const QString& propertyName,
    QStringList rHeaders, QStringList cHeaders, std::vector<std::vector<double> > defaultTable,
    FilterParameter::Category category, bool areRowsDynamic, bool areColsDynamic, int minRowCount, int minColCount)
{
  DynamicTableFilterParameter::Pointer ptr = DynamicTableFilterParameter::New();
  ptr->setHumanLabel(humanLabel);
  ptr->setPropertyName(propertyName);
  ptr->setCategory(category);
  ptr->setRowHeaders(rHeaders);
  ptr->setColumnHeaders(cHeaders);
  ptr->setDefaultTable(defaultTable);
  ptr->setAreRowsDynamic(areRowsDynamic);
  ptr->setAreColsDynamic(areColsDynamic);
  ptr->setMinRowCount(minRowCount);
  ptr->setMinColCount(minColCount);
  ptr->setReadOnly(false);

  // Check that all columns are initialized to the same size
  if (defaultTable.size() > 0)
  {
    QSet<int> colSizes;
    colSizes.insert(defaultTable[0].size());
    for (int i = 1; i < defaultTable.size(); i++)
    {
      colSizes.insert(defaultTable[i].size());
      if (colSizes.size() > 1)
      {
        ptr->setErrorCondition(-100);
        // Use HTML code in the error message, because this will be displayed in RichText format.
        ptr->setErrorMessage("Column " + QString::number(i) + " has a different size than the other columns.<br>Please make all columns the same size.");
        return ptr;
      }
    }
  }

  ptr->setErrorCondition(0);
  ptr->setErrorMessage("There is no error.");
  return ptr;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DynamicTableFilterParameter::getWidgetType()
{
  return QString("DynamicTableWidget");
}

