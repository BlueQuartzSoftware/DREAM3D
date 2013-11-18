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
#ifndef _DataContainer_H_
#define _DataContainer_H_

//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QMap>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/Observable.h"
#include "DREAM3DLib/DataContainers/AttributeMatrix.h"

/**
 * @brief The DataContainer class
 */
class DREAM3DLib_EXPORT DataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS (DataContainer)
    DREAM3D_STATIC_NEW_MACRO (DataContainer)
    DREAM3D_TYPE_MACRO_SUPER(DataContainer, Observable)

    virtual ~DataContainer();

    typedef QMap<QString, AttributeMatrix::Pointer> AttributeMatrixMap_t;

    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(QString, Name)
    DREAM3D_VIRTUAL_INSTANCE_PROPERTY(AttributeMatrixMap_t, AttributeMatrices)

    template<typename T>
    void CreateNonPrereqArray(AbstractFilter* filter, QString AttributeMatrixName, QString AttributeArrayName, T* m_AttributeArray, T initValue, int Size, QVector<int> dims);

    template<typename T>
    void GetPrereqArray(AbstractFilter* filter, QString AttributeMatrixName, QString AttributeArrayName, T* m_AttributeArray, int err, int Size, QVector<int> dims);

    /**
     * @brief Returns bool of whether a named array exists
     * @param name The name of the data array
     */
    virtual bool doesAttributeMatrixExist(const QString &name);

   /**
   * @brief Adds/overwrites the data for a named array
   * @param name The name that the array will be known by
   * @param data The IDataArray::Pointer that will hold the data
   */
    virtual void addAttributeMatrix(const QString &name, AttributeMatrix::Pointer matrix);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    virtual AttributeMatrix::Pointer getAttributeMatrix(const QString &name);

    /**
    * @brief Removes the named data array from the Data Container and returns it to the calling
    * method.
    * @param name The name of the array
    * @return
    */
    virtual AttributeMatrix::Pointer removeAttributeMatrix(const QString &name);

    /**
    * @brief Renames a cell data array from the Data Container
    * @param name The name of the array
    */
    virtual bool renameAttributeMatrix(const QString &oldname, const QString &newname);

  /**
     * @brief Removes all the Cell Arrays
     */
    virtual void clearAttributeMatrices();

    /**
    * @brief Returns a list that contains the names of all the arrays currently stored in the
    * Cell (Formerly Cell) group
    * @return
    */
    virtual QList<QString> getAttributeMatrixNameList();

    /**
    * @brief Returns the total number of arrays that are stored in the Cell group
    * @return
    */
    virtual int getNumAttributeMatrices();


  protected:
    DataContainer();

  private:
    DataContainer(const DataContainer&); // Copy Constructor Not Implemented
    void operator=(const DataContainer&); // Operator '=' Not Implemented

};

#endif /* _DataContainer_H_ */
