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


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setName(const QString& name)
{
  m_Name = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GEOM_CLASS_NAME::getName()
{
  return m_Name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int GEOM_CLASS_NAME::getGeometryType()
{
  return m_GeometryType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GEOM_CLASS_NAME::getGeometryTypeAsString()
{
  return m_GeometryTypeName;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::sendThreadSafeProgressMessage(int64_t counter, int64_t max)
{
  m_Mutex.lock();

  m_ProgressCounter += counter;

  int64_t progIncrement = max / 100;
  int64_t prog = 1;

  if (m_ProgressCounter > prog)
  {
    int64_t progressInt = static_cast<int64_t>((static_cast<float>(m_ProgressCounter) / max) * 100.0f);
    QString ss = m_MessageTitle + QObject::tr(" || %1% Complete").arg(progressInt);
    notifyStatusMessage(m_MessagePrefix, m_MessageLabel, ss);
    prog += progIncrement;
  }

  m_Mutex.unlock();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setMessagePrefix(const QString& name)
{
  m_MessagePrefix = name;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GEOM_CLASS_NAME::getMessagePrefix()
{
  return m_MessagePrefix;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setMessageTitle(const QString& title)
{
  m_MessageTitle = title;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GEOM_CLASS_NAME::getMessageTitle()
{
  return m_MessageTitle;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setMessageLabel(const QString& label)
{
  m_MessageLabel = label;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString GEOM_CLASS_NAME::getMessageLabel()
{
  return m_MessageLabel;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int GEOM_CLASS_NAME::getXdmfGridType()
{
  return m_XdmfGridType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int GEOM_CLASS_NAME::getUnitDimensionality()
{
  return m_UnitDimensionality;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void GEOM_CLASS_NAME::setSpatialDimensionality(unsigned int spatialDims)
{
  m_SpatialDimensionality = spatialDims;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
unsigned int GEOM_CLASS_NAME::getSpatialDimensionality()
{
  return m_SpatialDimensionality;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer GEOM_CLASS_NAME::getAttributeMatrix(const QString& name)
{
  AttributeMatrixMap_t::iterator it;
  it = m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    return AttributeMatrix::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer GEOM_CLASS_NAME::removeAttributeMatrix(const QString& name)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    // DO NOT return a NullPointer for any reason other than "Attribute Matrix was not found"
    return AttributeMatrix::NullPointer();
  }
  AttributeMatrix::Pointer p = it.value();
  m_AttributeMatrices.erase(it);
  return p;
}
