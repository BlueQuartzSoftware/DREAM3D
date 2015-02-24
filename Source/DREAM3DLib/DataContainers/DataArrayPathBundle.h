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
#ifndef _DataArrayPathBundle_H_
#define _DataArrayPathBundle_H_


#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/QMap>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

#include "DataArrayPath.h"

// our PIMPL private class
class DataArrayPathBundlePrivate;

/**
* @brief The DataArrayPathBundle class holds a complete or partial set of paths to various data arrays starting at the DataContainer
* level. The class is implemented using the PIMPL design pattern.
*/
class DREAM3DLib_EXPORT DataArrayPathBundle : public QObject
{
	Q_OBJECT
		Q_DECLARE_PRIVATE(DataArrayPathBundle)

public:
	DataArrayPathBundle();

	/**
	* @brief DataArrayPathBundle
	* @param dcName
	* @param amName
	* @param daName
	*/
	DataArrayPathBundle(const QString& dcName, const QString& amName, const QMap<QString, bool>& daNameList);

	/**
	* @brief DataArrayPathBundle
	*/
	DataArrayPathBundle(const DataArrayPathBundle& rhs);

	virtual ~DataArrayPathBundle();


	static QList<DataArrayPath> ConvertToDataArrayPaths(DataArrayPathBundle bundle);
	static QMap<QString,bool> serializeDataArrayNames(QString daNames, char token);
	static QString serializeDataArrayNames(QMap<QString,bool> daNames, char token);

	DREAM3D_PIMPL_PROPERTY_DECL(QString, DataContainerName)
	DREAM3D_PIMPL_PROPERTY_DECL(QString, AttributeMatrixName)

	void setDataArrayNameMap(QMap<QString, bool> map);
	QMap<QString, bool> getDataArrayNameMap() const;


		/**
		* @brief operator =
		*/
		void operator=(const DataArrayPathBundle& rhs);

	/**
	* @brief operator ==
	* @param rhs
	* @return
	*/
	bool operator==(const DataArrayPathBundle& rhs);

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
	* @brief Adds a data array to the bundle
	* @param daName The DataArray Name
	* @param isChecked The DataArray checkState
	*/
	void addDataArray(const QString& daName, const bool& isChecked);

	/**
	* @brief Removes a data array from the bundle
	* @param daName The DataArray Name
	*/
	void removeDataArray(const QString& daName);

	/**
	* @brief Renames a data array in the bundle
	* @param daName The DataArray Name
	*/
	void renameDataArray(const QString& oldName, const QString& newName);

	/**
	* @brief checks that two paths share the same data container
	* @param other The other path
	* @return true if the two paths share the same data container, false otherwise
	*/
	bool sameDataContainer(const DataArrayPathBundle& other) const;

	/**
	* @brief checks that two paths share the same attribute matrix
	* @param other The other path
	* @return true if the two paths share the same attribute matrix, false otherwise
	*/
	bool sameAttributeMatrix(const DataArrayPathBundle& other) const;

	/**
	* @brief checks that two paths share the same data container and attribute matrix
	* @param other The other path
	* @return true if the two paths share the same data container and attribute matrix, false otherwise
	*/
	bool sameAttributeMatrixPath(const DataArrayPathBundle& other) const;

	/**
	* @brief checks that two paths share the same data array
	* @param other The other path
	* @return true if the two paths share the same data array, false otherwise
	*/
	bool sameDataArraySet(const DataArrayPathBundle& other) const;


private:
	QScopedPointer<DataArrayPathBundlePrivate> const d_ptr;

	static void serialize(QString &str1, QString &str2, char token);


};


Q_DECLARE_METATYPE(DataArrayPathBundle)



#endif /* _DataArrayPathBundle_H_ */
