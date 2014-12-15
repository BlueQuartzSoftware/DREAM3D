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
#ifndef _DataContainerBundle_H_
#define _DataContainerBundle_H_


#include <QtCore/QObject>
#include <QtCore/QVector>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataContainers/IDataContainerBundle.h"

class DREAM3DLib_EXPORT DataContainerBundle : public IDataContainerBundle
{
    Q_OBJECT
  public:
    DREAM3D_SHARED_POINTERS (DataContainerBundle)
    DREAM3D_TYPE_MACRO_SUPER(DataContainerBundle, IDataContainerBundle)

    virtual ~DataContainerBundle();

    static Pointer                                        New(const QString &name)
    {
      Pointer sharedPtr (new DataContainerBundle);
      sharedPtr->setName(name);
      return sharedPtr;
    }

    /**
     * @brief GetMetaDataName
     * @return Returns the name of the Meta Data AttributeMatrix name. This name should be used for any AttributeMatrix
     * that is used in a DataContainer to hold the type of data that is used to group DataContainers into bundles.
     */
    static QString GetMetaDataName();

    /**
    * @brief The name of this DataContainerBundle
    */
    DREAM3D_INSTANCE_STRING_PROPERTY(Name)


    /*
     * @brief This is the list of array names that reside inside the "Meta Data" attribute matrix that are the
     * pieces of data that define why the data containers were grouped into a bundle. For example, if you have a time
     * series set of data where each Data Container is a step in "time" and there are several values for time:
     * @li Time Value Real value of time
     * @li Time Step Value Time Step value in the simulation
     * @li Time Index The raw index of the data, i.e., this is the actual ordering that could also be used as then index
     * into an array of data containers
     */
    DREAM3D_INSTANCE_PROPERTY(QStringList, MetaDataArrays)

    void setDataContainers(QVector<DataContainer::Pointer> &containers);
    QVector<QString> getDataContainerNames();



    void addDataContainer(DataContainer::Pointer dc);

    void removeDataContainer(DataContainer::Pointer dc);
    void removeDataContainer(const QString &name);
    void removeDataContainer(qint32 i);


    DataContainer::Pointer getDataContainer(qint32 index);

    qint32 count();

    void clear();

    /**
     * @brief findCommonDataArrayPaths
     */
    void findCommonDataArrayPaths();

    /**
     * @brief writeH5Data Write this bundle out to an HDF5 File
     * @param file_id The hdf5 file id or other group id where the bundle is going to be serialized to.
     * @return
     */
    int writeH5Data(hid_t groupId);

    /**
     * @brief readH5Data Reads this bundle from an HDF5 file
     * @param groupId
     * @return
     */
    int readH5Data(hid_t groupId);

  protected:
    DataContainerBundle();

  private:
    QVector<DataContainer::Pointer>  m_DataContainers;
    QString m_MetaDataAMName;

    DataContainerBundle(const DataContainerBundle&); // Copy Constructor Not Implemented
    void operator=(const DataContainerBundle&); // Operator '=' Not Implemented
};

#endif /* _DataContainerBundle_H_ */
