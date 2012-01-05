/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include "QLoadSlicesWidget.h"

#include <QtGui/QLabel>

#include "QtSupport/QR3DFileCompleter.h"
#include "QtSupport/DREAM3DQtMacros.h"

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/H5EbsdVolumeInfo.h"
//#include "EbsdLib/QualityMetricFilter.h"
#include "EbsdLib/H5EbsdVolumeReader.h"
#include "EbsdLib/TSL/H5AngVolumeReader.h"
//#include "EbsdLib/TSL/AngFields.h"
#include "EbsdLib/HKL/H5CtfVolumeReader.h"
//#include "EbsdLib/HKL/CtfFields.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLoadSlicesWidget::QLoadSlicesWidget(QWidget* parent) :
    QFilterWidget(parent),
    rotateslice(false),
    reorderarray(false),
    aligneulers(false),
#if defined(Q_WS_WIN)
        m_OpenDialogLastDirectory("C:\\")
#else
        m_OpenDialogLastDirectory("~/")
#endif
{
  setupUi(this);
  m_Filter = LoadSlices::New();
  setupGui();
  setTitle(QString::fromStdString(m_Filter->getHumanLabel()));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QLoadSlicesWidget::~QLoadSlicesWidget() { }

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AbstractFilter::Pointer QLoadSlicesWidget::getFilter()
{
  return m_Filter;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QLoadSlicesWidget::verifyPathExists(QString outFilePath, QLineEdit* lineEdit)
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
void QLoadSlicesWidget::setupGui()
{

  changeStyle(false);

  QR3DFileCompleter* com = new QR3DFileCompleter(this, false);
  m_H5EbsdFile->setCompleter(com);
  QObject::connect( com, SIGNAL(activated(const QString &)),
           this, SLOT(on_m_H5EbsdFile_textChanged(const QString &)));

//  m_WidgetList << m_H5EbsdFile << m_H5EbsdBtn;
//  m_WidgetList << m_ZStartIndex << m_ZEndIndex;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::on_m_H5EbsdBtn_clicked()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select Input File"),
                                                 m_OpenDialogLastDirectory,
                                                 tr("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)") );
  if ( true == file.isEmpty() ){ return; }
  QFileInfo fi (file);
  m_H5EbsdFile->blockSignals(true);
  QString p = QDir::toNativeSeparators(fi.absoluteFilePath());
  m_H5EbsdFile->setText(p);
  on_m_H5EbsdFile_textChanged(m_H5EbsdFile->text() );
  m_H5EbsdFile->blockSignals(false);

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::on_m_H5EbsdFile_textChanged(const QString &text)
{

  if (verifyPathExists(m_H5EbsdFile->text(), m_H5EbsdFile) )
  {
    QFileInfo fi(m_H5EbsdFile->text());
    H5EbsdVolumeReader::Pointer h5Reader = H5EbsdVolumeReader::New();
    h5Reader->setFileName(m_H5EbsdFile->text().toStdString() );

    // try reading the volume info from the file. Simply return if there was an error
    if (h5Reader->readVolumeInfo() < 0)
    {
      return;
    }


    // Set the Input Widgets
    m_ZStartIndex->setRange(h5Reader->getZStart(), h5Reader->getZEnd());
    m_ZStartIndex->setValue(h5Reader->getZStart());
    m_ZEndIndex->setRange(h5Reader->getZStart(), h5Reader->getZEnd());
    m_ZEndIndex->setValue(h5Reader->getZEnd());


    //Now set the information Widgets
    float xres, yres, zres;
    h5Reader->getResolution(xres, yres, zres);
    int64_t xpoints = 0;
    int64_t ypoints = 0;
    int64_t zpoints = 1;

    h5Reader->getDims(xpoints, ypoints, zpoints);
    m_XDim->setText(QString::number(xpoints));
    m_YDim->setText(QString::number(ypoints));
    m_ZDim->setText(QString::number(zpoints));

    m_XRes->setText(QString::number(xres));
    m_YRes->setText(QString::number(yres));
    m_ZRes->setText(QString::number(zres));

    m_ZMin->setText(QString::number(h5Reader->getZStart()));
    m_ZMax->setText(QString::number(h5Reader->getZEnd()));


    // Compare the Manufactureres of the current file versus the one we have cached
    // If they are different then we need to remove all the quality filters
    QString fileManufact = QString::fromStdString(h5Reader->getManufacturer());
    // Cache the Manufacturer from the File
    m_EbsdManufacturer->setText(fileManufact);


    rotateslice = h5Reader->getRotateSlice();
    reorderarray = h5Reader->getReorderArray();

    if(rotateslice == true) m_RotateSliceLabel->setText("true");
    else if(rotateslice == false) m_RotateSliceLabel->setText("false");

    if(reorderarray == true) m_ReorderArrayLabel->setText("true");
    else if(reorderarray == false) m_ReorderArrayLabel->setText("false");

    if(aligneulers == true) m_AlignEulersLabel->setText("true");
    else if(aligneulers == false) m_AlignEulersLabel->setText("false");


    aligneulers = h5Reader->getAlignEulers();
    m_StackingOrder->setText(QString::fromStdString(Ebsd::StackingOrder::Utils::getStringForEnum(h5Reader->getStackingOrder())));

  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::m_SetSliceInfo()
{
  H5EbsdVolumeInfo::Pointer reader = H5EbsdVolumeInfo::New();

  QFileInfo fi(m_H5EbsdFile->text());
  if (fi.isFile() == false)
  {
    return;
  }

  reader->setFileName(m_H5EbsdFile->text().toStdString());
  if (reader->readVolumeInfo() >= 0)
  {
    float x, y, z;
    reader->getResolution(x, y, z);
    m_ZStartIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZStartIndex->setValue(reader->getZStart());
    m_ZEndIndex->setRange(reader->getZStart(), reader->getZEnd());
    m_ZEndIndex->setValue(reader->getZEnd());
    m_ZRes->setText(QString::number(z));
    m_ZMin->setText(QString::number(reader->getZStart()));
    m_ZMax->setText(QString::number(reader->getZEnd()));
  }
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QLoadSlicesWidget::selectInputFile()
{
  QString file = QFileDialog::getOpenFileName(this, tr("Select H5 EBSD File"), m_OpenDialogLastDirectory, tr("HDF5 EBSD Files (*.h5 *.hdf5 *.h5ang *.h5ebsd)"));
  if(true == file.isEmpty())
  {
    return;
  }
  QFileInfo fi(file);
  m_OpenDialogLastDirectory = fi.path();
  m_Filter->setH5EbsdFile(file.toStdString());

  std::vector<FilterOption::Pointer> options = getFilter()->getFilterOptions();

  for (std::vector<FilterOption::Pointer>::iterator iter = options.begin(); iter != options.end(); ++iter)
  {
    FilterOption* option = (*iter).get();
    FilterOption::WidgetType wType = option->getWidgetType();
    if(wType == FilterOption::InputFileWidget)
    {
      QLabel* label = qFindChild<QLabel*>(this, QString::fromStdString(option->getHumanLabel()));
      if (label)
      {
        label->setText(file);
      }

    }
    else if (wType == FilterOption::IntConstrainedWidget)
    {
      QSpinBox* spinBox = qFindChild<QSpinBox*>(this, QString::fromStdString(option->getPropertyName()));
      if (spinBox)
      {

      }
    }

  }
}


