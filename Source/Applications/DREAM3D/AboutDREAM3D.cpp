/* ============================================================================
* Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
* Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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

#include "AboutDREAM3D.h"

#include <boost/version.hpp>
#include <Eigen/src/Core/util/Macros.h>
#include <H5public.h>
#include <qwt_global.h>
#include <tbb/tbb_stddef.h>

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/FilterManager.h"


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutDREAM3D::AboutDREAM3D(QWidget* parent) :
QDialog(parent)
{
  this->setupUi(this);

  // read versions
  readVersions();

  // read plugin names
  readPlugins();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutDREAM3D::~AboutDREAM3D()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutDREAM3D::readVersions()
{
  QTableWidgetItem* qtwi = new QTableWidgetItem(QString("Item"), QTableWidgetItem::Type);
  version->setHorizontalHeaderItem(0, qtwi);
  QTableWidgetItem* qtwi1 = new QTableWidgetItem(QString("Version"), QTableWidgetItem::Type);
  version->setHorizontalHeaderItem(1, qtwi1);

	QString strH5 = QString::number(H5_VERS_MAJOR);
	strH5.append(".");
	strH5.append(QString::number(H5_VERS_MINOR));
	strH5.append(".");
	strH5.append(QString::number(H5_VERS_RELEASE));

  QString strEigen = QString::number(EIGEN_WORLD_VERSION);
  strEigen.append(".");
  strEigen.append(QString::number(EIGEN_MAJOR_VERSION));
  strEigen.append(".");
  strEigen.append(QString::number(EIGEN_MINOR_VERSION));

  QString strTBB = QString::number(TBB_VERSION_MAJOR);
  strTBB.append(".");
  strTBB.append(QString::number(TBB_VERSION_MINOR));

  version->setItem(0, 1, new QTableWidgetItem(DREAM3DLib::Version::PackageComplete().toLatin1().data()));
  version->setItem(1, 1, new QTableWidgetItem(BOOST_LIB_VERSION));
  version->setItem(2, 1, new QTableWidgetItem(QT_VERSION_STR));
  version->setItem(3, 1, new QTableWidgetItem(QWT_VERSION_STR));
  version->setItem(4, 1, new QTableWidgetItem(strH5));
  version->setItem(5, 1, new QTableWidgetItem(strEigen));
	version->setItem(6, 1, new QTableWidgetItem(strTBB));

  labelVersion->setText(DREAM3DLib::Version::PackageComplete().toLatin1().data());
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutDREAM3D::readPlugins()
{ 
	QTableWidgetItem* qtwi = new QTableWidgetItem(QString("Plugins"), QTableWidgetItem::Type);
	plugin->setHorizontalHeaderItem(0, qtwi);
	QStringList list;

	FilterManager* fm = FilterManager::Instance();
	FilterManager::Collection factories = fm->getFactories();
	QMapIterator<QString, IFilterFactory::Pointer> iter(factories);
	while (iter.hasNext())
	{
		iter.next();
		IFilterFactory::Pointer factory = iter.value();
		AbstractFilter::Pointer filter = factory->create();
		list.append(filter->getCompiledLibraryName());
	}

	list.removeDuplicates();
	list.removeOne("Core"); // for now
	list.removeOne("DREAM3DLib"); // for now
	list.removeOne("TestPlugin"); // for now
	plugin->setRowCount(list.count());
	for (int i = 0; i < list.size(); ++i){
		QTableWidgetItem* item = new QTableWidgetItem();
		item->setText(list.at(i));
		plugin->setItem(i, 0, item);
	}
}

