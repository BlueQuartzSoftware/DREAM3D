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

#include "AboutDREAM3D.h"

#include "SIMPLib/SIMPLib.h"

#include <boost/version.hpp>
#include <H5public.h>
#include <qwt_global.h>

#if SIMPLib_USE_PARALLEL_ALGORITHMS
#include <tbb/tbb_stddef.h>
#endif

#if SIMPL_USE_EIGEN
#include <Eigen/src/Core/util/Macros.h>
#endif


#include <QtCore/QFile>
#include <QtCore/QTextStream>

#include "QtSupportLib/DREAM3DStyles.h"

#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/FilterManager.h"

#include "Applications/DREAM3D/DREAM3D/License/DREAM3DLicenseFiles.h"
#include "Applications/DREAM3D/DREAM3DVersion.h"

// Include the MOC generated CPP file which has all the QMetaObject methods/data
#include "moc_AboutDREAM3D.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AboutDREAM3D::AboutDREAM3D(QWidget* parent) :
  QDialog(parent)
{
  setupUi(this);
  setupGui();
  readVersions();
  setLicenseFiles(DREAM3D::LicenseList);
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
void AboutDREAM3D::setupGui()
{
  QString str;
  QTextStream out(&str);

  out << "DREAM.3D Version " << DREAM3D::Version::Package();
  versionLabel->setText(str);
  versionLabel->setFont(DREAM3DStyles::GetTitleFont());

  str.clear();
  out << "Built on " << DREAM3D::Version::BuildDate();
  buildDateLabel->setText(str);

  str.clear();
  out << "From revision " << DREAM3D::Version::Revision();
  revisionLabel->setText(str);

  setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);

#if defined (Q_OS_MAC)
  QAction* closeAction = new QAction(this);
  closeAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
  connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));
  addAction(closeAction);
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutDREAM3D::setLicenseFiles(QStringList files)
{
  m_licenseFiles = files;
  licenseCombo->clear();
  m_licenseFiles = files;
  for (int i = 0; i < m_licenseFiles.size(); ++i)
  {
    QString s = m_licenseFiles[i];
    s.remove(0, 2);
    s.remove(".license", Qt::CaseSensitive);
    licenseCombo->addItem(s);
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutDREAM3D::on_licenseCombo_currentIndexChanged(int index)
{
  //qDebug() << "on_licenseCombo_action" << "\n";
  QString resourceFile = m_licenseFiles[licenseCombo->currentIndex()];
  loadResourceFile(resourceFile);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutDREAM3D::loadResourceFile(const QString qresourceFile)
{
  QFile inputFile(qresourceFile);
  inputFile.open(QIODevice::ReadOnly);
  QTextStream in(&inputFile);
  QString line = in.readAll();
  inputFile.close();

//  appHelpText->append(line);
  appHelpText->setHtml(line);
  appHelpText->setUndoRedoEnabled(false);
  appHelpText->setUndoRedoEnabled(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AboutDREAM3D::readVersions()
{
#if 0
  QTableWidgetItem* qtwi = new QTableWidgetItem(QString("Item"), QTableWidgetItem::Type);
  version->setHorizontalHeaderItem(0, qtwi);
  QTableWidgetItem* qtwi1 = new QTableWidgetItem(QString("Version"), QTableWidgetItem::Type);
  version->setHorizontalHeaderItem(1, qtwi1);

  version->setItem(0, 1, new QTableWidgetItem(SIMPLib::Version::Package()));
  version->setItem(1, 1, new QTableWidgetItem(BOOST_LIB_VERSION));
  version->setItem(2, 1, new QTableWidgetItem(QT_VERSION_STR));

  QString strH5 = QString::number(H5_VERS_MAJOR);
  strH5.append(".");
  strH5.append(QString::number(H5_VERS_MINOR));
  strH5.append(".");
  strH5.append(QString::number(H5_VERS_RELEASE));

  version->setItem(4, 1, new QTableWidgetItem(strH5));

#if SIMPL_USE_EIGEN
  QString strEigen = QString::number(EIGEN_WORLD_VERSION);
  strEigen.append(".");
  strEigen.append(QString::number(EIGEN_MAJOR_VERSION));
  strEigen.append(".");
  strEigen.append(QString::number(EIGEN_MINOR_VERSION));
  version->setItem(5, 1, new QTableWidgetItem(strEigen));
#endif

#if SIMPLib_USE_PARALLEL_ALGORITHMS
  QString strTBB = QString::number(TBB_VERSION_MAJOR);
  strTBB.append(".");
  strTBB.append(QString::number(TBB_VERSION_MINOR));
  version->setItem(6, 1, new QTableWidgetItem(strTBB));
#endif

  version->setItem(3, 1, new QTableWidgetItem(QWT_VERSION_STR));

  labelVersion->setText(SIMPLib::Version::PackageComplete().toLatin1().data());
#endif
}

