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

#include "EbsdReferenceFrameDialog.h"

#include <iostream>


#include <QtCore/QPropertyAnimation>
#include <QtCore/QtEndian>
#include <QtCore/QStateMachine>

#include <QtGui/QPixmap>
#include <QtGui/QGraphicsPixmapItem>

#include "DREAM3DLib/Common/AIMArray.hpp"
#include "DREAM3DLib/Common/OIMColoring.hpp"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/EbsdReader.h"
#include "EbsdLib/EbsdLibVersion.h"
#include "EbsdLib/Utilities/MXAFileInfo.h"

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/TSL/AngReader.h"

#include "EbsdLib/HKL/CtfConstants.h"
#include "EbsdLib/HKL/CtfReader.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdReferenceFrameDialog::EbsdReferenceFrameDialog(QString filename, QWidget *parent) :
QDialog(parent),
m_EbsdFileName(filename),
m_OriginGroup(NULL),
m_PixmapGraphicsItem(NULL)
{
  setupUi(this);
  m_OriginGroup = new QButtonGroup(this);
  m_OriginGroup->addButton(m_UpperLeftBtn);
  m_OriginGroup->addButton(m_UpperRightBtn);
  m_OriginGroup->addButton(m_LowerLeftBtn);
  m_OriginGroup->addButton(m_LowerRightBtn);
  connect(m_UpperLeftBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_UpperRightBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_LowerLeftBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));
  connect(m_LowerRightBtn, SIGNAL(toggled(bool)),
          this, SLOT(originChanged(bool)));

  loadEbsdData();
  updateGraphicsView();
  m_CurrentCorner = 0;
  m_UpperLeftBtn->setChecked(true);
  m_ExplanationScrollArea->setVisible(m_ExplanationBtn->isChecked());

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EbsdReferenceFrameDialog::~EbsdReferenceFrameDialog()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdReferenceFrameDialog::setEbsdFileName(QString filename)
{
  m_EbsdFileName = filename;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

Ebsd::EbsdToSampleCoordinateMapping EbsdReferenceFrameDialog::getSelectedOrigin()
{
  if (m_UpperLeftBtn->isChecked()) { return Ebsd::UpperLeftOrigin; }
  if (m_UpperRightBtn->isChecked()) { return Ebsd::UpperRightOrigin; }
  if (m_LowerRightBtn->isChecked()) { return Ebsd::LowerRightOrigin; }
  if (m_LowerLeftBtn->isChecked()) { return Ebsd::LowerLeftOrigin; }
  return Ebsd::UnknownCoordinateMapping;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
bool EbsdReferenceFrameDialog::alignEulers()
{
  return m_AlignEulers->isChecked();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdReferenceFrameDialog::loadEbsdData()
{
  boost::shared_ptr<EbsdReader> ebsdReader(static_cast<EbsdReader*>(NULL));

  std::string ext = EbsdFileInfo::extension(m_EbsdFileName.toStdString());
  if (ext.compare(Ebsd::Ang::FileExt) == 0)
  {
    AngReader* reader = new AngReader;
    ebsdReader.reset(static_cast<EbsdReader*>(reader));
  }
  else if (ext.compare(Ebsd::Ctf::FileExt) == 0)
  {
    CtfReader* reader = new CtfReader;
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
  AIMArray<uint32_t>::Pointer rgbArray = AIMArray<uint32_t>::CreateArray(total);
  rgbArray->SetNumberOfComponents(1);
  rgbArray->WritePointer(0, total);
  // Splat 0xFF across all the data
  ::memset(rgbArray->GetPointer(0), 255, total*sizeof(uint32_t));

  float* e0 = reinterpret_cast<float*>(ebsdReader->getPointerByName("Phi1"));
  float* e1 = reinterpret_cast<float*>(ebsdReader->getPointerByName("Phi"));
  float* e2 = reinterpret_cast<float*>(ebsdReader->getPointerByName("Phi2"));




  for (uint32_t i = 0; i < total; ++i) {
    uint8_t* argb = reinterpret_cast<uint8_t*>(rgbArray->GetPointer(i)) + 1;
    OIMColoring::GenerateIPFColor(e0[i], e1[i], e2[i],
                                  RefDirection[0], RefDirection[1], RefDirection[2],
                                  argb, hkl);
//    uint32_t c = rgbArray->GetValue(i);
//    QColor color(argb[0], argb[1], argb[2]);
//    QRgb qrgb = color.rgba();
//    uint32_t ucolor = static_cast<uint32_t>(qrgb);
//    uint32_t t = rgbArray->GetValue(i);
//    qrgb = ucolor ;

  }

  QImage image(width, height, QImage::Format_ARGB32);
  size_t index = 0;
  uint32_t pixel = NULL;

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
void EbsdReferenceFrameDialog::originChanged(bool checked)
{
  if (checked == false) { return; }
  int corner = 0;
  if (m_UpperLeftBtn->isChecked()) { corner = 0; }
  if (m_UpperRightBtn->isChecked()) { corner = 1; }
  if (m_LowerRightBtn->isChecked()) { corner = 2; }
  if (m_LowerLeftBtn->isChecked()) { corner = 3; }

  switch(m_CurrentCorner)
  {

    case 0:
      if (corner == 1) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      if (corner == 2) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      if (corner == 3) {m_EbsdImage = m_EbsdImage.mirrored(false, true);}
      break;
    case 1:
      if (corner == 0) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      if (corner == 2) {m_EbsdImage = m_EbsdImage.mirrored(false, true);}
      if (corner == 3) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      break;
    case 2:
      if (corner == 0) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      if (corner == 1) {m_EbsdImage = m_EbsdImage.mirrored(false, true);}
      if (corner == 3) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
      break;
    case 3:
      if (corner == 0) {m_EbsdImage = m_EbsdImage.mirrored(false, true);}
      if (corner == 1) {m_EbsdImage = m_EbsdImage.mirrored(true, true);}
      if (corner == 2) {m_EbsdImage = m_EbsdImage.mirrored(true, false);}
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
void EbsdReferenceFrameDialog::updateGraphicsView()
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
void EbsdReferenceFrameDialog::updateDisplay()
{
   this->update();
}




// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EbsdReferenceFrameDialog::on_m_ExplanationBtn_toggled(bool b)
{
  m_ExplanationScrollArea->setVisible(m_ExplanationBtn->isChecked());
}

