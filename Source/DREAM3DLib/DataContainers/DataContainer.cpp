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
 #include "DataContainer.h"



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::DataContainer() :
Observable()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataContainer::~DataContainer()
{

}

template<typename T>
void DataContainer::GetPrereqArray(AbstractFilter* filter, QString AttributeMatrixName, QString AttributeArrayName, T* m_AttributeArray, int err, int Size, QVector<int> dims)
{
  QString ss; 
  if (AttributeArrayName.isEmpty() == true)
  { 
    filter->setErrorCondition(err*1000);
    ss = QObject::tr("The name of the Attribute Array - '%1' was empty. Please provide a name for this array").arg(AttributeArrayName);
    filter->addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  AttributeMatrix::Pointer matrix = getAttributeMatrix(AttributeMatrixName);
  if (matrix->doesAttributeArrayExist(AttributeArrayName) == false) 
  {
    filter->setErrorCondition(err*1000+1);
    ss = QObject::tr("An array with name '%1' does not exist and is required for this filter to execute.").arg(AttributeArrayName);
    filter->addErrorMessage(getHumanLabel(), ss, getErrorCondition());
  }
  else 
  { 
    /* QString _s(#Name); 
    addRequired##DType(_s);*/
    int NumComp = dims[0];
    for(int i=1;i<dims.size();i++)
    {
      NumComp *= dims[i];
    }
    m_AttributeArray = matrix->ArraySizeCheck<T, ArrayType, AbstractFilter>(AttributeArrayName, Size, NumComp, filter);
    if (NULL == m_AttributeArray ) 
    {
      ss = QObject::tr("\nThe current array with name '%1' is not valid for the internal array for this filter."
        "The preflight failed for one or more reasons. Check additional error messages for more details.").arg(AttributeArrayName);
      filter->setErrorCondition(err*1000+2);
      filter->addErrorMessage(getHumanLabel(), ss, getErrorCondition()); 
    }
  }
}


template<typename T>
void DataContainer::CreateNonPrereqArray(AbstractFilter* filter, QString AttributeMatrixName, QString AttributeArrayName, T* m_AttributeArray, T initValue, int Size, QVector<int> dims)
{
  if (AttributeMatrixName.isEmpty() == true)
  {
    filter->setErrorCondition(-10000);
    QString ss = QObject::tr("The name of the array was empty. Please provide a name for this array.");
    filter->addErrorMessage(getHumanLabel(), ss, -10000);
  }
  if (AttributeArrayName.isEmpty() == true)
  {
    filter->setErrorCondition(-10000);
    QString ss = QObject::tr("The name of the array was empty. Please provide a name for this array.");
    filter->addErrorMessage(getHumanLabel(), ss, -10000);
  }
  /* QString _s(#Name);*/
  int NumComp = dims[0];
  for(int i=1;i<dims.size();i++)
  {
    NumComp *= dims[i];
  }
  AttributeMatrix::Pointer matrix = getAttributeMatrix(AttributeMatrixName);
  m_AttributeArray = matrix->ArraySizeCheck<T, ArrayType, AbstractFilter>(AttributeArrayName, Size, NumComp, NULL);
  if (NULL ==  m_AttributeArray) 
  {
    ArrayType::Pointer p = ArrayType::CreateArray(Size, dims, AttributeArrayName);
    if (NULL == p.get()) 
    {
      QString ss;
      if (AttributeArrayName.isEmpty() == true) 
      {
        ss = QObject::tr("Filter %1 attempted to create array with an empty name and that is not allowed.").arg(getNameOfClass());
        filter->setErrorCondition(-501);
      } 
      else 
      {
        ss = QObject::tr("Filter %1 attempted to create array' %2' but was unsuccessful. This is most likely due to not enough contiguous memory.").arg(getNameOfClass()).arg(AttributeArrayName);
        filter->setErrorCondition(-500);
      }
      filter->addErrorMessage(getHumanLabel(), ss, -50001);
    } 
    else if (p->getPointer(0) == NULL)
    {
      filter->setErrorCondition(-11000);
      QString ss = QObject::tr("'%1' was sized to Zero which may cause the program to crash in filters that use this array, including the current filter."
        " Please add a filter before this filter that will result in a properly sized array.").arg(AttributeArrayName);
      filter->addErrorMessage(getHumanLabel(), ss, getErrorCondition());
    }
    else 
    {
      p->initializeWithValues(initValue);
      p->SetName(AttributeArrayName);
      matrix->addAttributeArray(AttributeArrayName, p);
      m_AttributeArray = p->getPointer(0);
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainer::doesAttributeMatrixExist(const QString &name)
{
  return  m_AttributeMatrices.contains(name);\
}
  
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::addAttributeMatrix(const QString &name, AttributeMatrix::Pointer data)
{
  if (data->getName().compare(name) != 0)
  {
    qDebug() << "Adding Attribute Array with different array name than key name" << "\n";
    qDebug() << "Key name: " << name << "\n";
    qDebug() << "Array Name:" << data->getName() << "\n";
    data->setName(name);
  }
  m_AttributeMatrices[name] = data;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::getAttributeMatrix(const QString &name)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    return AttributeMatrix::NullPointer();
  }
  return it.value();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AttributeMatrix::Pointer DataContainer::removeAttributeMatrix(const QString &name)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(name);
  if ( it == m_AttributeMatrices.end() )
  {
    return AttributeMatrix::NullPointer();
  }
  AttributeMatrix::Pointer p = it.value();
  m_AttributeMatrices.erase(it);
  return p;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataContainer::renameAttributeMatrix(const QString &oldname, const QString &newname)
{
  QMap<QString, AttributeMatrix::Pointer>::iterator it;
  it =  m_AttributeMatrices.find(oldname);
  if ( it == m_AttributeMatrices.end() )
  {
    return false;
  }
  AttributeMatrix::Pointer p = it.value();
  p->setName(newname);
  removeAttributeMatrix(oldname);
  addAttributeMatrix(newname, p);
  return true;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataContainer::clearAttributeMatrices()
{
  m_AttributeMatrices.clear();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<QString> DataContainer::getAttributeMatrixNameList()
{
  QList<QString> keys;
  for(QMap<QString, IDataArray::Pointer>::iterator iter = m_AttributeMatrices.begin(); iter != m_AttributeMatrices.end(); ++iter)
  {
    keys.push_back( iter.key() );
  }
  return keys;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int DataContainer::getNumAttributeMatrices()
{
  return static_cast<int>(m_AttributeMatrices.size());
}
