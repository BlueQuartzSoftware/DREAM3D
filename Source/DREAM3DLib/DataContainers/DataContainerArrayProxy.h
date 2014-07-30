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
#ifndef _DataContainerArrayProxy_H_
#define _DataContainerArrayProxy_H_



#include <QtCore/QMetaType>
#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/DataContainers/DataContainerProxy.h"
#include "DREAM3DLib/DataContainers/DataArrayPath.h"

class DataContainerArray;

/**
 * @brief DataContainerArrayProxy
 */
class DREAM3DLib_EXPORT DataContainerArrayProxy
{
  public:
    /**
     * @brief DataContainerArrayProxy
     */
    DataContainerArrayProxy();

    /**
     * @brief DataContainerArrayProxy
     * @param dca
     */
    explicit DataContainerArrayProxy(DataContainerArray* dca);

    /**
     * @brief DataContainerArrayProxy
     * @param is_valid
     */
    explicit DataContainerArrayProxy(bool is_valid);

    /**
     * @brief DataContainerArrayProxy
     * @param rhs
     */
    DataContainerArrayProxy(const DataContainerArrayProxy& rhs);

    /**
     * @brief operator =
     * @param rhs
     */
    void operator=(const DataContainerArrayProxy& rhs);

    /**
     * @brief flattenHeirarchy
     * @return
     */
    QStringList flattenHeirarchy();

    /**
     * @brief getCheckedDataArrayPaths
     * @return
     */
    QVector<DataArrayPath> getCheckedDataArrayPaths();

    /**
     * @brief Print the Heirarchy and attributes of the Proxy
     * @param out
     */
    void print(const QString header = QString(""));

    /**
     * @brief updateDataContainer This function will attempt to remove those DataContainers, AttributeMatrix and
     * DataArray items that are flagged as selected by the user/programmer.
     * @param dca
     */
    void removeSelectionsFromDataContainerArray(DataContainerArray* dca, Qt::CheckState state = Qt::Unchecked);

    /**
     * @brief setAllFlags
     * @param state
     */
    void setAllFlags(Qt::CheckState state);

    /**
     * @brief reverseFlags
     */
    void reverseFlags();

    /**
     * @brief serialize
     * @return
     */
    QStringList serialize();

    /**
     * @brief Does this contain a DataContainer for the given name
     * @param name
     * @return
     */
    bool contains(const QString& name);

    /**
     * @brief getDataContainerProxy
     * @param name
     * @return
     */
    DataContainerProxy& getDataContainerProxy(const QString& name);



    //----- Our variables, publicly available
    QList<DataContainerProxy>  list;
    bool isValid;
};

Q_DECLARE_METATYPE(DataContainerArrayProxy)

#endif /* _DataContainerArrayProxy_H_ */

