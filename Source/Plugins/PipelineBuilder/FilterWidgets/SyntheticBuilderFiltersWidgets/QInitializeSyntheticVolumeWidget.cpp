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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#include "QInitializeSyntheticVolumeWidget.h"

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QInitializeSyntheticVolumeWidget::QInitializeSyntheticVolumeWidget(QWidget* parent) :
QFilterWidget(parent),
#if defined(Q_WS_WIN)
        m_OpenDialogLastDirectory("C:\\")
#else
        m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  m_Filter = InitializeSyntheticVolume::New();
  setupGui();
  setTitle(QString::fromStdString(m_Filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QInitializeSyntheticVolumeWidget::~QInitializeSyntheticVolumeWidget()
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QInitializeSyntheticVolumeWidget::getFilter()
{
  // Update the filter with the latest values from the GUI

  return m_Filter;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QInitializeSyntheticVolumeWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
{
//  std::cout << "outFilePath: " << outFilePath.toStdString() << std::endl;
  QFileInfo fileinfo(outFilePath);
  if (false == fileinfo.exists() )
  {
    lineEdit->setStyleSheet("border: 1px solid red;");
  }
  else
  {
    lineEdit->setStyleSheet("");
  }
  return fileinfo.exists();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::setupGui()
{
  setCheckable(true);

  changeStyle(false);

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_InputFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_InputFile_textChanged(const QString &)));



}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_InputFileBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_InputFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_InputFile->setText(p);
  on_m_InputFile_textChanged(m_InputFile->text() );
  m_InputFile->blockSignals(false);

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_InputFile_textChanged(const QString &text)
{

  if(verifyPathExists(m_InputFile->text(), m_InputFile))
  {

    QFileInfo fi(m_InputFile->text());
    if(fi.exists() && fi.isFile())
    {

    }
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_XPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_YPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_ZPoints_valueChanged(int v)
{
  estimateNumGrainsSetup();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_ZResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_YResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::on_m_XResolution_valueChanged(double v)
{
  estimateNumGrainsSetup();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int QInitializeSyntheticVolumeWidget::estimate_numgrains(int xpoints, int ypoints, int zpoints, float xres, float yres, float zres)
{
  int err = -1;


  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::estimateNumGrainsSetup()
{
  int xpoints, ypoints, zpoints;
  float xres, yres, zres;
  xpoints = m_XPoints->value();
  ypoints = m_YPoints->value();
  zpoints = m_ZPoints->value();
  xres = m_XResolution->value();
  yres = m_YResolution->value();
  zres = m_ZResolution->value();
  int est_ngrains = estimate_numgrains(xpoints, ypoints, zpoints, xres, yres, zres);
  m_EstimatedGrains->setText(QString::number(est_ngrains));
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::readOptions(QSettings &prefs)
{
  QString val;
  bool ok;
  qint32 i;
  double d;
  READ_FILEPATH_SETTING(prefs, m_, InputFile, "");
  READ_SETTING(prefs, m_, XResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, YResolution, ok, d, 0.25 , Double);
  READ_SETTING(prefs, m_, ZResolution, ok, d, 0.25 , Double);

  m_XPoints->blockSignals(true);
  READ_SETTING(prefs, m_, XPoints, ok, i, 100 , Int);
  m_XPoints->blockSignals(false);
  m_YPoints->blockSignals(true);
  READ_SETTING(prefs, m_, YPoints, ok, i, 100 , Int);
  m_YPoints->blockSignals(false);
  READ_SETTING(prefs, m_, ZPoints, ok, i, 100 , Int);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QInitializeSyntheticVolumeWidget::writeOptions(QSettings &prefs)
{
  prefs.setValue("Filter_Name", "InitializeSyntheticVolume" );
  WRITE_STRING_SETTING(prefs, m_, InputFile)


  WRITE_SETTING(prefs, m_, XResolution )
  WRITE_SETTING(prefs, m_, YResolution )
  WRITE_SETTING(prefs, m_, ZResolution )
  WRITE_SETTING(prefs, m_, XPoints )
  WRITE_SETTING(prefs, m_, YPoints )
  WRITE_SETTING(prefs, m_, ZPoints )

}
