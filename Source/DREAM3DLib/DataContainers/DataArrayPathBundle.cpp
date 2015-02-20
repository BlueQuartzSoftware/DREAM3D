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
	QSet<QString> m_DataArrayNameSet;

	DataArrayPathBundlePrivate(DataArrayPathBundle* ptr);
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundlePrivate::DataArrayPathBundlePrivate(DataArrayPathBundle* ptr) :
q_ptr(ptr),
m_DataContainerName(""),
m_AttributeMatrixName(""),
m_DataArrayNameSet(QSet<QString>())
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
DataArrayPathBundle::DataArrayPathBundle(const QString& dcName, const QString& amName, const QSet<QString>& daNameSet) :
d_ptr(new DataArrayPathBundlePrivate(this))
{
	d_ptr->m_DataContainerName = dcName;
	d_ptr->m_AttributeMatrixName = amName;
	d_ptr->m_DataArrayNameSet = daNameSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DataArrayPathBundle::DataArrayPathBundle(const DataArrayPathBundle& rhs) :
d_ptr(new DataArrayPathBundlePrivate(this))
{
	d_ptr->m_DataContainerName = rhs.getDataContainerName();
	d_ptr->m_AttributeMatrixName = rhs.getAttributeMatrixName();
	d_ptr->m_DataArrayNameSet = rhs.getDataArrayNameSet();
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
	QSet<QString> daNameSet = bundle.getDataArrayNameSet();
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
QSet<QString> DataArrayPathBundle::serializeDataArrayNames(QString daNames, char token)
{
	QSet<QString> namesSet;
	int start = 0;
	int tokenIndex = 0;
	QString name = "";

	while (tokenIndex >= 0)
	{
		tokenIndex = daNames.indexOf(token, start);
		name = daNames.mid(start, tokenIndex-start);
		namesSet.insert(name);
		start = tokenIndex + 1;
	}

	return namesSet;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString DataArrayPathBundle::serializeDataArrayNames(QSet<QString> daSet, char token)
{
	QString daNames = "";

	QSet<QString>::iterator iter = daSet.begin();
	while ( iter != daSet.end() )
	{
		QString name = *iter;
		daNames.append(name);

		iter++;

		if (iter != daSet.end())
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
DREAM3D_PIMPL_PROPERTY_DEF(DataArrayPathBundle, QSet<QString>, DataArrayNameSet)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::operator=(const DataArrayPathBundle& rhs)
{
	d_ptr->m_DataContainerName = rhs.getDataContainerName();
	d_ptr->m_AttributeMatrixName = rhs.getAttributeMatrixName();
	d_ptr->m_DataArrayNameSet = rhs.getDataArrayNameSet();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool DataArrayPathBundle::operator==(const DataArrayPathBundle& rhs)
{
	if (d_ptr->m_DataContainerName == rhs.getDataContainerName()
		&& d_ptr->m_AttributeMatrixName == rhs.getAttributeMatrixName()
		&& d_ptr->m_DataArrayNameSet == rhs.getDataArrayNameSet())
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
	if (d_ptr->m_DataContainerName.isEmpty() && d_ptr->m_AttributeMatrixName.isEmpty() && d_ptr->m_DataArrayNameSet.isEmpty())
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
	if (d_ptr->m_DataContainerName.isEmpty() == false && d_ptr->m_AttributeMatrixName.isEmpty() == false && d_ptr->m_DataArrayNameSet.isEmpty() == false)
	{
		return true;
	}
	return false;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::addDataArray(const QString& daName)
{
	d_ptr->m_DataArrayNameSet.insert(daName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::removeDataArray(const QString& daName)
{
	d_ptr->m_DataArrayNameSet.remove(daName);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DataArrayPathBundle::renameDataArray(const QString& oldName, const QString& newName)
{
	removeDataArray(oldName);
	addDataArray(newName);
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
	if ( d_ptr->m_DataArrayNameSet == other.d_ptr->m_DataArrayNameSet )
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
