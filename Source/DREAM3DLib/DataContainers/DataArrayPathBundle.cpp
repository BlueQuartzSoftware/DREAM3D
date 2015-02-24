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
#include "DataArrayPathBundle.h"



#include "DREAM3DLib/Common/Constants.h"

/* ############## Start Private Implementation ############################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
class DataArrayPathBundlePrivate
{
	Q_DISABLE_COPY(DataArrayPathBundlePrivate)
		Q_DECLARE_PUBLIC(DataArrayPathBundle)
		DataArrayPathBundle* const q_ptr;
	QString m_DataContainerName;
	QString m_AttributeMatrixName;
	QMap<QString,bool> m_DataArrayNameMap;

	DataArrayPathBundlePrivate(DataArrayPathBundle* ptr);
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundlePrivate::DataArrayPathBundlePrivate(DataArrayPathBundle* ptr) :
q_ptr(ptr),
m_DataContainerName(""),
m_AttributeMatrixName(""),
m_DataArrayNameMap(QMap<QString,bool>())
{

}

/* ############## Start API/Public Implementation ########################### */
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundle::DataArrayPathBundle() :
d_ptr(new DataArrayPathBundlePrivate(this))
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundle::DataArrayPathBundle(const QString& dcName, const QString& amName, const QMap<QString,bool>& daNameSet) :
d_ptr(new DataArrayPathBundlePrivate(this))
{
	d_ptr->m_DataContainerName = dcName;
	d_ptr->m_AttributeMatrixName = amName;
	d_ptr->m_DataArrayNameMap = daNameSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundle::DataArrayPathBundle(const DataArrayPathBundle& rhs) :
d_ptr(new DataArrayPathBundlePrivate(this))
{
	d_ptr->m_DataContainerName = rhs.getDataContainerName();
	d_ptr->m_AttributeMatrixName = rhs.getAttributeMatrixName();
	d_ptr->m_DataArrayNameMap = rhs.getDataArrayNameMap();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundle::~DataArrayPathBundle()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QList<DataArrayPath> DataArrayPathBundle::ConvertToDataArrayPaths(DataArrayPathBundle bundle)
{
	QMap<QString,bool> daNameSet = bundle.getDataArrayNameMap();
	QString dcName = bundle.getDataContainerName();
	QString amName = bundle.getAttributeMatrixName();

	QList<DataArrayPath> paths;
	foreach(const QString daName, daNameSet)
	{
		DataArrayPath path(dcName, amName, daName);
		paths.push_back(path);
	}

	return paths;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::serialize(QString &str1, QString &str2, char token)
{
	QString fullString = str1;

	int tokenIndex = 0;

	tokenIndex = fullString.indexOf(token);
	QString result1 = fullString.mid(0, tokenIndex);
	tokenIndex++;
	QString result2 = fullString.mid(tokenIndex);

	str1 = result1;
	str2 = result2;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString,bool> DataArrayPathBundle::serializeDataArrayNames(QString daNames, char token)
{
	QMap<QString,bool> namesMap;
	int start = 0;
	int tokenIndex = 0;

	while (tokenIndex >= 0)
	{
		tokenIndex = daNames.indexOf(token, start);
		QString combo = daNames.mid(start, tokenIndex - start);
		QString name = combo;
		QString isCheckedStr = "";
		serialize(name, isCheckedStr, ':');

		bool isChecked = false;
		if (isCheckedStr == "true")
		{
			isChecked = true;
		}

		namesMap.insert(name, isChecked);
		start = tokenIndex + 1;
	}

	return namesMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathBundle::serializeDataArrayNames(QMap<QString,bool> daMap, char token)
{
	QString daNames = "";
	QString isCheckedStr = "";

	QMap<QString,bool>::iterator iter = daMap.begin();
	while ( iter != daMap.end() )
	{
		QString name = iter.key();
		bool isChecked = iter.value();

		if (isChecked == true)
		{
			isCheckedStr = "true";
		}
		else
		{
			isCheckedStr = "false";
		}
		daNames.append(name);
		daNames.append(':');
		daNames.append(isCheckedStr);

		iter++;

		if (iter != daMap.end())
		{
			daNames.append(token);
		}
	}

	return daNames;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3D_PIMPL_PROPERTY_DEF(DataArrayPathBundle, QString, DataContainerName)
DREAM3D_PIMPL_PROPERTY_DEF(DataArrayPathBundle, QString, AttributeMatrixName)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::setDataArrayNameMap(QMap<QString,bool> map) 
{
	Q_D(DataArrayPathBundle);
	d->m_DataArrayNameMap = map;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QMap<QString, bool> DataArrayPathBundle::getDataArrayNameMap() const
{
	Q_D(const DataArrayPathBundle);
	return d->m_DataArrayNameMap;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::operator=(const DataArrayPathBundle& rhs)
{
	d_ptr->m_DataContainerName = rhs.getDataContainerName();
	d_ptr->m_AttributeMatrixName = rhs.getAttributeMatrixName();
	d_ptr->m_DataArrayNameMap = rhs.getDataArrayNameMap();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::operator==(const DataArrayPathBundle& rhs)
{
	if (d_ptr->m_DataContainerName == rhs.getDataContainerName()
		&& d_ptr->m_AttributeMatrixName == rhs.getAttributeMatrixName()
		&& d_ptr->m_DataArrayNameMap == rhs.getDataArrayNameMap())
	{
		return true;
	}

	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::isEmpty() const
{
	if (d_ptr->m_DataContainerName.isEmpty() && d_ptr->m_AttributeMatrixName.isEmpty() && d_ptr->m_DataArrayNameMap.isEmpty())
	{
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::isValid() const
{
	if (d_ptr->m_DataContainerName.isEmpty() == false && d_ptr->m_AttributeMatrixName.isEmpty() == false && d_ptr->m_DataArrayNameMap.isEmpty() == false)
	{
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::addDataArray(const QString& daName, const bool& isChecked)
{
	d_ptr->m_DataArrayNameMap.insert(daName, isChecked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::removeDataArray(const QString& daName)
{
	d_ptr->m_DataArrayNameMap.remove(daName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::renameDataArray(const QString& oldName, const QString& newName)
{
	bool isChecked = d_ptr->m_DataArrayNameMap.value(oldName, false);
	removeDataArray(oldName);
	addDataArray(newName, isChecked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::sameDataContainer(const DataArrayPathBundle& other) const
{
	if (d_ptr->m_DataContainerName == other.d_ptr->m_DataContainerName)
	{
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::sameAttributeMatrix(const DataArrayPathBundle& other) const
{
	if (d_ptr->m_AttributeMatrixName == other.d_ptr->m_AttributeMatrixName)
	{
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::sameDataArraySet(const DataArrayPathBundle& other) const
{
	if ( d_ptr->m_DataArrayNameMap == other.d_ptr->m_DataArrayNameMap )
	{
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::sameAttributeMatrixPath(const DataArrayPathBundle& other) const
{
	return(sameDataContainer(other) && sameAttributeMatrix(other));
}
