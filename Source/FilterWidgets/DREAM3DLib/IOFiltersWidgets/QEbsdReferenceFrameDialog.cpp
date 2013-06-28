/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#include "QEbsdReferenceFrameDialog.h"

#include <iostream>


#include <QtCore/QPropertyAnimation>
#include <QtCore/QtEndian>
#include <QtCore/QStateMachine>

#include <QtGui/QPixmap>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QPushButton>


//#include "EbsdLib/EbsdLib.h"
//#include "EbsdLib/EbsdReader.h"
//#include "EbsdLib/EbsdLibVersion.h"
//#include "EbsdLib/Utilities/MXAFileInfo.h"

//#include "EbsdLib/TSL/AngConstants.h"
//#include "EbsdLib/TSL/AngReader.h"

//#include "EbsdLib/HKL/CtfConstants.h"
//#include "EbsdLib/HKL/CtfReader.h"

//#include "EbsdLib/HEDM/MicConstants.h"
//#include "EbsdLib/HEDM/MicReader.h"

#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/IOFilters/ReadOrientationData.h"
#include "DREAM3DLib/GenericFilters/GenerateIPFColors.h"
#include "DREAM3DLib/ProcessingFilters/ConvertEulerAngles.h"




#include "DREAM3DLib/IOFiltersWidgets/moc_QEbsdReferenceFrameDialog.cxx"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEbsdReferenceFrameDialog::QEbsdReferenceFrameDialog(QString filename, QWidget *parent) :
  QDialog(parent),
  m_EbsdFileName(filename),
  m_OriginGroup(NULL),
  m_PixmapGraphicsItem(NULL)
{
  setupUi(this);
  m_OriginGroup = new QButtonGroup(this);
  m_OriginGroup->addButton(m_TSLdefaultBtn);
  m_OriginGroup->addButton(m_HKLdefaultBtn);
  m_OriginGroup->addButton(m_HEDMdefaultBtn);
  m_OriginGroup->addButton(m_NoTransBtn);
  connect(m_TSLdefaultBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_HKLdefaultBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_HEDMdefaultBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_NoTransBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));

  connect(degToRads, SIGNAL(stateChanged(int)),
          this, SLOT(degToRagsChanged(int)));


  connect(refDir, SIGNAL(currentIndexChanged(int)),
          this, SLOT(referenceDirectionChanged()));

  loadEbsdData();
  updateGraphicsView();
  m_CurrentCorner = 3;
  m_NoTransBtn->setChecked(true);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEbsdReferenceFrameDialog::~QEbsdReferenceFrameDialog()
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setEbsdFileName(QString filename)
{
  m_EbsdFileName = filename;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setTSLDefault(bool checked)
{
  m_TSLdefaultBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setHKLDefault(bool checked)
{
  m_HKLdefaultBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setHEDMDefault(bool checked)
{
  m_HEDMdefaultBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setNoTrans(bool checked)
{
  m_NoTransBtn->setChecked(checked);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Ebsd::EbsdToSampleCoordinateMapping QEbsdReferenceFrameDialog::getSelectedOrigin()
{
  if (m_TSLdefaultBtn->isChecked()) { return Ebsd::TSLdefault; }
  if (m_HKLdefaultBtn->isChecked()) { return Ebsd::HKLdefault; }
  if (m_HEDMdefaultBtn->isChecked()) { return Ebsd::HEDMdefault; }
  if (m_NoTransBtn->isChecked()) { return Ebsd::UnknownCoordinateMapping; }
  return Ebsd::UnknownCoordinateMapping;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getTSLchecked()
{
  return m_TSLdefaultBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getHKLchecked()
{
  return m_HKLdefaultBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getHEDMchecked()
{
  return m_HEDMdefaultBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool QEbsdReferenceFrameDialog::getNoTranschecked()
{
  return m_NoTransBtn->isChecked();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::loadEbsdData()
{
  m_CurrentCorner = 3;
  VoxelDataContainer::Pointer m = VoxelDataContainer::New();

  ReadOrientationData::Pointer reader = ReadOrientationData::New();
  reader->setInputFile(m_EbsdFileName.toStdString());
  reader->setVoxelDataContainer(m.get());
  reader->execute();
  int err = reader->getErrorCondition();
  if (err < 0)
  {
    ipfLabel->setText("Error Reading the file");
    m_EbsdImage = QImage();
  }

  //If they want to convert the Eulers to Radians
  if(degToRads->isChecked() == true)
  {
    ConvertEulerAngles::Pointer convert = ConvertEulerAngles::New();
    convert->setConversionType(DREAM3D::EulerAngleConversionType::DegreesToRadians);
    convert->setVoxelDataContainer(m.get());
    convert->execute();
    err = convert->getErrorCondition();
    if (err < 0)
    {
      ipfLabel->setText("Error converting angles to radians");
      m_EbsdImage = QImage();
    }
  }

  GenerateIPFColors::Pointer ipfColorFilter = GenerateIPFColors::New();
  FloatVec3Widget_t ref;
  ref.x = 0;
  ref.y = 0;
  ref.z = 0;
  if (refDir->currentIndex() == 0)
  {
    ref.x = 1;
  }
  else if(refDir->currentIndex() == 1)
  {
    ref.y = 1;
  }
  else if(refDir->currentIndex() == 2)
  {
    ref.z = 1;
  }
  ipfColorFilter->setReferenceDir(ref);
  ipfColorFilter->setVoxelDataContainer(m.get());
  ipfColorFilter->execute();
  err = ipfColorFilter->getErrorCondition();
  if (err < 0)
  {
    ipfLabel->setText("Error Generating IPF Colors" );
    m_EbsdImage = QImage();
  }

  size_t dims[3] = {0, 0, 0};
  m->getDimensions(dims);
  float res[3] = {0.0f, 0.0f, 0.0f};
  m->getResolution(res);

  // Save the Dimensions of the image
  m_ImageSize = QSize(dims[0], dims[1]);

  m_XDim->setText(QString::number(dims[0]));
  m_YDim->setText(QString::number(dims[1]));
  m_XRes->setText(QString::number(res[0]));
  m_YRes->setText(QString::number(res[1]));

  QImage image(dims[0], dims[1], QImage::Format_ARGB32);
  size_t index = 0;

  IDataArray::Pointer arrayPtr = m->getCellData(ipfColorFilter->getCellIPFColorsArrayName());
  if (NULL == arrayPtr.get())
  {
    return;
  }

  UInt8ArrayType* rgbArray = UInt8ArrayType::SafePointerDownCast(arrayPtr.get());
  uint8_t* ipfColors = rgbArray->GetPointer(0);

  for(size_t y = 0; y < dims[1]; ++y)
  {
    uint8_t* scanLine = image.scanLine(y);
    for(size_t x = 0; x < dims[0]; ++x)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
      scanLine[x*4 + 3] = 0xFF;
      index = y * dims[0] * 3 + x * 3;
      scanLine[x*4 + 2] = ipfColors[index + 0];
      scanLine[x*4 + 1] = ipfColors[index + 1];
      scanLine[x*4 + 0] = ipfColors[index + 2];
#endif
    }

  }
  m_EbsdImage = image;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::originChanged(bool checked)
{
  if (checked == false) { return; }
  int corner = 0;
  if (m_TSLdefaultBtn->isChecked()) { corner = 0; }
  else if (m_HKLdefaultBtn->isChecked()) { corner = 1; }
  else if (m_HEDMdefaultBtn->isChecked()) { corner = 2; }
  else if (m_NoTransBtn->isChecked()) { corner = 3; }

  switch(m_CurrentCorner)
  {

    case 0:
      if (corner == 1) {m_EbsdImage = m_EbsdImage.mirrored(false, false);}
      if (corner == 2) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      if (corner == 3) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      break;
    case 1:
      if (corner == 0) {m_EbsdImage = m_EbsdImage.mirrored(false, false);}
      if (corner == 2) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      if (corner == 3) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      break;
    case 2:
      if (corner == 0) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      if (corner == 1) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      if (corner == 3) {m_EbsdImage = m_EbsdImage.mirrored(false, true);}
      break;
    case 3:
      if (corner == 0) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      if (corner == 1) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      if (corner == 2) {m_EbsdImage = m_EbsdImage.mirrored(false, true);}
      break;
    default:
      break;
  }
  m_CurrentCorner = corner;

  updateGraphicsView();

  if (checked == true) {
    updateDisplay();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::updateGraphicsView()
{
  // If the EBSD map is larger than 1024 pixels in either height or width then rescale
  // back to 1024 pixels
  if (m_ImageSize.width() > 1024 || m_ImageSize.height() > 1024)
  {
    QSize gvSize(1024,1024);
    m_EbsdImage = m_EbsdImage.scaled(gvSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  ipfLabel->setText("");
  ipfLabel->setPixmap(QPixmap::fromImage(m_EbsdImage));
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::updateDisplay()
{
  this->update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::degToRagsChanged(int state)
{
  loadEbsdData();
  originChanged(true);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::referenceDirectionChanged()
{
  loadEbsdData();
  originChanged(true);
}

