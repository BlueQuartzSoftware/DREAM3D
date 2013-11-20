/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef _AttributeMatrix_H_
#define _AttributeMatrix_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

//-- EBSD Lib Includes
#include "EbsdLib/EbsdConstants.h"

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"


/**
 * @class AttributeMatrix AttributeMatrix DREAM3DLib/DataContainers/AttributeMatrix.h
 * @brief This data container holds data the represents a structured rectangular
 * grid of data typically referred to as a Voxel Volume
 * @author Michael A. Jackson for BlueQuartz Software
 * @author Michael A. Groeber for AFRL
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT AttributeMatrix : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(AttributeMatrix)
    DREAM3D_STATIC_NEW_MACRO(AttributeMatrix)
    DREAM3D_TYPE_MACRO_SUPER(AttributeMatrix, Observable)

    virtual ~AttributeMatrix();

#if 0
    //Initalize Array Func Here
    template<typename T, class Filter>
    typename DataArray<T>::Pointer createAttributeArray(const QString &arrayName, size_t size, QVector<int> dims, Filter* obv)
    {
      typename DataArray<T>::Pointer valuePtr = DataArray<T>::NullPointer();
      IDataArray::Pointer iDataArray = getAttributeArray(arrayName);
      if (iDataArray.get() == NULL) // There was NO AttributeArray with the given name in the AttributeMatrix
      {
        valuePtr = DataArray<T>::CreateArray(size, dims, arrayName);

        if (NULL == valuePtr.get())
        {
          QString s = QObject::tr(": Array '%1' could not allocate %2 elements.").arg(arrayName).arg(size);
          if (NULL != obv) {obv->setErrorCondition(-25);
            obv->addErrorMessage(getNameOfClass(), s, -25);
          }
          return valuePtr;
        }
        valuePtr->initializeWithZeros();
        addAttributeArray(arrayName, valuePtr);
        return valuePtr;
      }

      valuePtr = IDataArray::SafeReinterpretCast<IDataArray*, DataArrayType*, PtrType* >(iDataArray.get());
      if (NULL == valuePtr)
      {
        QString s = QObject::tr(": Array '%1' could not be cast to proper type").arg(arrayName);
        if (NULL != obv) {obv->setErrorCondition(-12);
          obv->addErrorMessage(getNameOfClass(), s, -12);
        }
        return valuePtr;
      }
      return valuePtr;
    }
#endif

    //Get Array Size Check Func Here
    template<typename T, class AbstractFilter>
    bool dataArrayCompatibility(const QString &arrayName, size_t size, int numComp, AbstractFilter* filter)
    {
      // First try checking by name
      IDataArray::Pointer iDataArray = getAttributeArray(arrayName);
      if (iDataArray.get() == 0)
      {
        if (NULL != filter)
        {
          QString s = QObject::tr("Filter '%1' requires an available array with name '%2'").arg(filter->getHumanLabel()).arg(arrayName);
          filter->setErrorCondition(-501);
          filter->addErrorMessage(filter->getHumanLabel(), s, -501);
        }
        return false;
      }
      // Make sure the sizes are equal to what is being asked for
      if (size*numComp != iDataArray->GetSize())
      {
        if (NULL != filter)
        {
          QString s = QObject::tr("Filter '%1' requires array with name '%2' to have Number of Compoenets = %3. The currently available array "
          " has %4").arg(filter->getHumanLabel()).arg(arrayName).arg((size*numComp)).arg(iDataArray->GetSize());
          filter->setErrorCondition(-501);
          filter->addErrorMessage(filter->getHumanLabel(), s, -501);
        }
        return false;
      }
      // Make sure the number of components match
      if (numComp != iDataArray->GetNumberOfComponents())
      {
        if (NULL != filter)
        {
          QString ss = QObject::tr("Filter '%1' requires an array where the number of components is %2 but the currently available array"
                                   " that was supplied has %3.").arg(filter->getHumanLabel()).arg(numComp).arg(iDataArray->GetNumberOfComponents());
          filter->addErrorMessage(filter->getHumanLabel(), ss, -503);
        }
        return false;
      }
      // Make sure we can downcast to the proper type
      DataArray<T>* array = DataArray<T>::SafePointerDownCast(iDataArray.get());
      if (NULL == array)
      {
        typename DataArray<T>::Pointer dat = DataArray<T>::CreateArray(1, "JUNK-INTERNAL-USE-ONLY");
        QString s = QObject::tr(" - The filter requested an array named '%1' with type '%2' from the %3.\n"
                                "An Array with name '%4' is stored in the %5 but is of type %6\n")
            .arg(arrayName).arg(dat->getTypeAsString()).arg(getNameOfClass()).arg(arrayName).arg(getNameOfClass()).arg(iDataArray->getTypeAsString());
        if (NULL != filter)
        {
          filter->setErrorCondition(-502);
          filter->addErrorMessage(filter->getHumanLabel(), s, -502);
        }
        return false;
      }
      return true;
    }

    DREAM3D_INSTANCE_PROPERTY(unsigned int, AMType)
    DREAM3D_INSTANCE_PROPERTY(QString, Name)

    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool doesAttributeArrayExist(const QString &name);

    /**
   * @brief Adds/overwrites the data for a named array
   * @param name The name that the array will be known by
   * @param data The IDataArray::Pointer that will hold the data
   */
    virtual void addAttributeArray(const QString &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    virtual IDataArray::Pointer getAttributeArray(const QString &name);

    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method.
    * @param name The name of the array
    * @return
    */
    virtual IDataArray::Pointer removeAttributeArray(const QString &name);

    /**
    * @brief Renames a cell data array from the Data Container
    * @param name The name of the array
    */
    virtual bool renameAttributeArray(const QString &oldname, const QString &newname);

    /**
    * @brief Resizes an array from the Data Container
    * @param size The new size of the array
    */
    void resizeAttributeArrays(size_t size);

    /**
     * @brief Removes all the Cell Arrays
     */
    virtual void clearAttributeArrays();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Cell (Formerly Cell) group
    * @return
    */
    virtual QList<QString> getAttributeArrayNameList();

    /**
    * @brief Returns the total number of arrays that are stored in the Cell group
    * @return
    */
    virtual int getNumAttributeArrays();

    /**
    * @brief Returns the number of Tuples that the feature data has. For example if there are 32 features
    * in during a set of filtering operations then the a value of '32' would be returned.
    * @return
    */
    DREAM3D_INSTANCE_PROPERTY(size_t, NumTuples)


    protected:
      AttributeMatrix();

  private:

    QMap<QString, IDataArray::Pointer> m_AttributeMatrix;

    AttributeMatrix(const AttributeMatrix&);
    void operator =(const AttributeMatrix&);
};

#endif
