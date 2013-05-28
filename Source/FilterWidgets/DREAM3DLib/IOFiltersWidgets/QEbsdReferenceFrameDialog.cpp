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

#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/EbsdColoring.hpp"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdReader.h"
#include "EbsdLib/EbsdLibVersion.h"
#include "EbsdLib/Utilities/MXAFileInfo.h"

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngReader.h"

#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/CtfReader.h"

#include "EbsdLib/HEDM/MicConstants.h"
#include "EbsdLib/HEDM/MicReader.h"

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
  connect(buttonBox, SIGNAL(helpRequested ()),
          this, SLOT(toggleHelp()));
  connect(degToRads, SIGNAL(stateChanged(int)),
          this, SLOT(degToRagsChanged(int)));

  loadEbsdData();
  updateGraphicsView();
  m_CurrentCorner = 3;
  m_NoTransBtn->setChecked(true);
  m_ExplanationScrollArea->setVisible(false);
  buttonBox->button(QDialogButtonBox::Ok)->setDefault(true);

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
  boost::shared_ptr<EbsdReader> ebsdReader(static_cast<EbsdReader*>(NULL));
  //bool degToRads = false;
  std::string ext = EbsdFileInfo::extension(m_EbsdFileName.toStdString());
  std::string arrayNames[3];
  bool convertDegToRads = degToRads->isChecked();
  if (ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    AngReader* reader = new AngReader;
    arrayNames[0] = Ebsd::Ang::Phi1;
    arrayNames[1] = Ebsd::Ang::Phi;
    arrayNames[2] = Ebsd::Ang::Phi2;
    ebsdReader.reset(static_cast<EbsdReader*>(reader));
  }
  else if (ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    CtfReader* reader = new CtfReader;
    ebsdReader.reset(static_cast<EbsdReader*>(reader));
//    convertDegToRads = true;
    arrayNames[0] = Ebsd::Ctf::Euler1;
    arrayNames[1] = Ebsd::Ctf::Euler2;
    arrayNames[2] = Ebsd::Ctf::Euler3;
    reader->readOnlySliceIndex(0); // Some .ctf files may actually be 3D. We only need the first slice
  }
  else if (ext.compare(Ebsd::Mic::FileExt) == 0)
  {
    MicReader* reader = new MicReader;
//    convertDegToRads = true;
    arrayNames[0] = Ebsd::Mic::Euler1;
    arrayNames[1] = Ebsd::Mic::Euler2;
  arrayNames[2] = Ebsd::Mic::Euler3;
    ebsdReader.reset(static_cast<EbsdReader*>(reader));
  }
  if (NULL == ebsdReader.get())
  {
    return;
  }
  ebsdReader->setFileName(m_EbsdFileName.toStdString());
  int err = ebsdReader->readFile();
  if (err < 0)
  {
    //FIXME: Create a dialog stating the error
    std::cout << "Error reading the Ebsd File '" << m_EbsdFileName.toStdString() << "'" << std::endl;
    return;
  }


  //unsigned char* rgb;
  unsigned char hkl[3] = { 0, 0, 0 };
  float RefDirection[3] = { 0.0, 0.0, 1.0 };

  uint32_t width = ebsdReader->getXDimension();
  uint32_t height = ebsdReader->getYDimension();
  uint32_t total = width *height;
  DataArray<uint32_t>::Pointer rgbArray = DataArray<uint32_t>::CreateArray(total, "rgbArray");
  rgbArray->SetNumberOfComponents(1);
  rgbArray->Resize(total);
  // Splat 0xFF across all the data
  ::memset(rgbArray->GetPointer(0), 255, total*sizeof(uint32_t));

  float* e0 = reinterpret_cast<float*>(ebsdReader->getPointerByName(arrayNames[0]));
  float* e1 = reinterpret_cast<float*>(ebsdReader->getPointerByName(arrayNames[1]));
  float* e2 = reinterpret_cast<float*>(ebsdReader->getPointerByName(arrayNames[2]));

  if (NULL == e0 || NULL == e1 || NULL == e2)
  {
    std::cout << "Could not get raw arrays from file" << std::endl;
    return;
  }

  for (uint32_t i = 0; i < total; ++i) {
    uint8_t* argb = reinterpret_cast<uint8_t*>(rgbArray->GetPointer(i)) + 1;
    EbsdColoring::GenerateCubicIPFColor(e0[i], e1[i], e2[i],
                                  RefDirection[0], RefDirection[1], RefDirection[2],
                                  argb, hkl, convertDegToRads);
  }

  QImage image(width, height, QImage::Format_ARGB32);
  size_t index = 0;
  uint32_t pixel = 0;

  for(size_t h = 0; h < height; ++h)
  {
    for(size_t w = 0; w < width; ++w)
    {
      index = (h*width) + w;
      pixel = rgbArray->GetValue(index);
      pixel = qToBigEndian(pixel);
      image.setPixel((int)w, (int)h, pixel);
    }
  }
  m_EbsdImage = image;
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

  QGraphicsScene* gScene = m_GraphicsView->scene();
  m_GraphicsView->setScene(NULL);
  gScene->deleteLater();
  gScene = new QGraphicsScene(this);
  m_GraphicsView->setScene(gScene);
  delete m_PixmapGraphicsItem;
  m_PixmapGraphicsItem = NULL;

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
  if (m_GraphicsView == NULL)
  {
    return;
  }

  QGraphicsScene* gScene = m_GraphicsView->scene();
  if (gScene == NULL)
  {
    gScene = new QGraphicsScene(this);
    m_GraphicsView->setScene(gScene);
  }

  QRect gvRect = m_GraphicsView->geometry();
  QSize gvSize = gvRect.size();
  if(this->isVisible() == false)
  {
    gvSize.setHeight(350);
    gvSize.setWidth(350);
  }

  m_EbsdImage = m_EbsdImage.scaled(gvSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);


  if (NULL == m_PixmapGraphicsItem) {
    if (m_EbsdImage.isNull() == false) {
      m_PixmapGraphicsItem = gScene->addPixmap(QPixmap::fromImage(m_EbsdImage));
    }
  }
  if (NULL != m_PixmapGraphicsItem) {
    m_PixmapGraphicsItem->setAcceptDrops(true);
    m_PixmapGraphicsItem->setZValue(-1);
    QRectF rect = m_PixmapGraphicsItem->boundingRect();
    gScene->setSceneRect(rect);
  }


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
void QEbsdReferenceFrameDialog::toggleHelp()
{
  if (m_ExplanationScrollArea->isVisible()) {
    m_ExplanationScrollArea->setVisible(false);
  }
  else
  {
    m_ExplanationScrollArea->setVisible(true);
  }
}

