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

#ifndef _DataArrayPath_H_
#define _DataArrayPath_H_


#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QStringList>
#include <QtCore/QJsonObject>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

// our PIMPL private class
class DataArrayPathPrivate;

/**
 * @brief The DataArrayPath class holds a complete or partial path to a data array starting at the DataContainer
 * level. The class is implemented using the PIMPL design pattern.
 */
class SIMPLib_EXPORT DataArrayPath : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(DataArrayPath)

  public:
    DataArrayPath();

    /**
     * @brief DataArrayPath
     * @param dcName
     * @param amName
     * @param daName
     */
    DataArrayPath(const QString& dcName, const QString& amName, const QString& daName);

    /**
     * @brief DataArrayPath
     * @param path A path with the '|' delimeters
     */
    explicit DataArrayPath(const QString& path);

    /**
     * @brief DataArrayPath
     */
    DataArrayPath(const DataArrayPath& rhs);

    virtual ~DataArrayPath();


    static QVector<DataArrayPath> ConvertToQVector(QStringList& paths);

    SIMPL_PIMPL_PROPERTY_DECL(QString, DataContainerName)
    SIMPL_PIMPL_PROPERTY_DECL(QString, AttributeMatrixName)
    SIMPL_PIMPL_PROPERTY_DECL(QString, DataArrayName)


    /**
     * @brief operator =
     */
    void operator=(const DataArrayPath& rhs);

    /**
     * @brief operator ==
     * @param rhs
     * @return
     */
    bool operator==(const DataArrayPath& rhs);

    /**
     * @brief serialize Returns the path using the '|' charater by default. This can be over ridden by the programmer
     * @param delimiter
     * @return
     */
    QString serialize(QString delimiter = "|") const;

    /**
    * @brief serialize Returns the path using the '|' charater by default. This can be over ridden by the programmer
    * @param delimiter
    * @return
    */
    static DataArrayPath Deserialize(QString str, QString delimiter = "|");

    /**
     * @brief getAsStringList
     * @return
     */
    QStringList getAsStringList();

    /**
     * @brief getAsVector
     * @return
     */
    QVector<QString> getAsVector();

    /**
     * @brief isEmpty Returns if ALL of the string elements are empty. Note that a class could return FALSE for this
     * function and FALSE for isValid() also so this function is not a true indication of a valid path.
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief isValid Returns if ALL of the string components have some value stored in them so 'valid' in this sense just
     * means that all three components have a non-empty string. This does NOT necessarily mean that those strings, once
     * serialized into a path actually refer to something in your DataContainer.
     * @return
     */
    bool isValid() const;

    /**
     * @brief split
     * @param delimiter
     * @return
     */
    QStringList split(QString NOT_USED = "|") const;

    /**
     * @brief Updates the DataArrayPath with the values in the arguments
     * @param dcName The DataContainer Name
     * @param amName The AttributeMatrix Name
     * @param daName The DataArray Name
     */
    void update(const QString& dcName, const QString& amName, const QString& daName);

    /**
     * @brief checks that two paths share the same data container
     * @param other The other path
     * @return true if the two paths share the same data container, false otherwise
     */
    bool sameDataContainer(const DataArrayPath& other) const;

    /**
    * @brief checks that two paths share the same attribute matrix
    * @param other The other path
    * @return true if the two paths share the same attribute matrix, false otherwise
    */
    bool sameAttributeMatrix(const DataArrayPath& other) const;

    /**
     * @brief checks that two paths share the same data container and attribute matrix
     * @param other The other path
     * @return true if the two paths share the same data container and attribute matrix, false otherwise
     */
    bool sameAttributeMatrixPath(const DataArrayPath& other) const;

    /**
     * @brief checks that two paths share the same data array
     * @param other The other path
     * @return true if the two paths share the same data array, false otherwise
     */
    bool sameDataArray(const DataArrayPath& other) const;

    /**
    * @brief checks that a vector of paths have the same data container and attribute matrix
    * @return true if the paths in the vector have the same data container and attribute matrix, false otherwise
    */
    static bool ValidateVector(const QVector<DataArrayPath>& other);

    /**
    * @brief Gets the data array names from a QVector of DataArrayPaths.
    * @return Returns the data array names from a QVector of DataArrayPaths, in a QList.
    */
    static QList<QString> GetDataArrayNames(const QVector<DataArrayPath>& paths);

    /**
    * @brief Gets the attribute matrix path from a QVector of DataArrayPaths.
    * @return Returns the attribute matrix path as a DataArrayPath from a QVector
    * of DataArrayPaths.
    */
    static DataArrayPath GetAttributeMatrixPath(const QVector<DataArrayPath>& paths);

    /**
    * @brief Writes the contents of the proxy to the json object 'json'
    * @param json
    * @return
    */
    void writeJson(QJsonObject& json) const;

    /**
    * @brief Reads the contents of the the json object 'json' into the proxy
    * @param json
    * @return
    */
    bool readJson(QJsonObject& json) const;


  protected:



  private:
    QScopedPointer<DataArrayPathPrivate> const d_ptr;


};


Q_DECLARE_METATYPE(DataArrayPath)



#endif /* _DataArrayPath_H_ */
