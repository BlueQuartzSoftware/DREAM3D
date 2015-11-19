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


#include "QEbsdReferenceFrameDialog.h"

#include <iostream>


#include <QtCore/QPropertyAnimation>
#include <QtCore/QtEndian>
#include <QtCore/QStateMachine>
#include <QtCore/QFileInfo>

#include <QtGui/QPixmap>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QPushButton>
#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QFont>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMessageBox>

#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/FilterManager.h"
#include "SIMPLib/Common/IFilterFactory.hpp"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/DataContainers/DataContainer.h"
#include "SIMPLib/FilterParameters/FilterParameter.h"
#include "SIMPLib/Geometry/ImageGeom.h"


#include "QtSupportLib/DREAM3DHelpUrlGenerator.h"

#include "OrientationAnalysis/OrientationAnalysisFilters/ReadAngData.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ReadCtfData.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/GenerateIPFColors.h"
#include "OrientationAnalysis/OrientationAnalysisFilters/ChangeAngleRepresentation.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QEbsdReferenceFrameDialog::QEbsdReferenceFrameDialog(QString filename, QWidget* parent) :
  QDialog(parent),
  m_EbsdFileName(filename),
  m_OriginGroup(NULL)
{
  setupUi(this);

  setupGui();


  loadEbsdData();
  updateGraphicsView();
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
#define ZOOM_MENU(var, menu, slot)\
  {\
    QAction* action = new QAction(menu);\
    action->setText( #var );\
    QString actionName("action_z" #var "Action");\
    action->setObjectName(actionName);\
    zoomMenu->addAction(action);\
    connect(action, SIGNAL(triggered()), this, SLOT(slot())); \
  }

#define ZOOM_MENU_SLOT_DEF(var, index)\
  void QEbsdReferenceFrameDialog::z##var##_triggered() {\
    zoomButton->setText(#var " % ");\
    m_EbsdView->setZoomIndex(index);\
  }

ZOOM_MENU_SLOT_DEF(10, 0)
ZOOM_MENU_SLOT_DEF(25, 1)
ZOOM_MENU_SLOT_DEF(50, 2)
ZOOM_MENU_SLOT_DEF(100, 3)
ZOOM_MENU_SLOT_DEF(125, 4)
ZOOM_MENU_SLOT_DEF(150, 5)
ZOOM_MENU_SLOT_DEF(200, 6)
ZOOM_MENU_SLOT_DEF(400, 7)
ZOOM_MENU_SLOT_DEF(600, 8)

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::on_fitToWindow_clicked()
{
  m_EbsdView->setZoomIndex(9);
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::setupGui()
{
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

  QMenu* zoomMenu = new QMenu(this);
  ZOOM_MENU(10, zoomMenu, z10_triggered);
  ZOOM_MENU(25, zoomMenu, z25_triggered);
  ZOOM_MENU(50, zoomMenu, z50_triggered);
  ZOOM_MENU(100, zoomMenu, z100_triggered);
  ZOOM_MENU(125, zoomMenu, z125_triggered);
  ZOOM_MENU(150, zoomMenu, z150_triggered);
  ZOOM_MENU(200, zoomMenu, z200_triggered);
  ZOOM_MENU(400, zoomMenu, z400_triggered);
  ZOOM_MENU(600, zoomMenu, z600_triggered);

  zoomButton->setMenu(zoomMenu);

  //  connect(fitToWindow, SIGNAL(clicked()),
  //          m_EbsdView->fitInView() );
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
  if (m_EbsdFileName.isEmpty() == true) { return; }
  DataContainerArray::Pointer dca = DataContainerArray::New();
  QString dcName;
  QString cellAttrMatName;
  QString cellEnsembleName;
  DataArrayPath cellPhasesArrayPath;
  DataArrayPath cellEulerAnglesArrayPath;
  DataArrayPath crystalStructuresArrayPath;

  QFileInfo fi(m_EbsdFileName);

  if(fi.suffix().compare(Ebsd::Ang::FileExtLower) == 0)
  {
    ReadAngData::Pointer reader = ReadAngData::New();
    reader->setInputFile(m_EbsdFileName);
    reader->setDataContainerArray(dca);
    reader->execute();
    int err = reader->getErrorCondition();
    if (err < 0)
    {
      QMessageBox msgBox;
      msgBox.setText("Error Reading ANG File");
      QString iText;
      QTextStream ss(&iText);
      ss << "There was an error reading the ang file.\n. The error code returned was " << err;
      msgBox.setInformativeText(iText);
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
      m_BaseImage = QImage();
      m_DisplayedImage = QImage();
      return;
    }
    dcName = reader->getDataContainerName();
    cellAttrMatName = reader->getCellAttributeMatrixName();
    cellEnsembleName = reader->getCellEnsembleAttributeMatrixName();
    cellPhasesArrayPath = DataArrayPath(dcName, cellAttrMatName, Ebsd::AngFile::Phases);
    cellEulerAnglesArrayPath = DataArrayPath(dcName, cellAttrMatName, Ebsd::AngFile::EulerAngles);
    crystalStructuresArrayPath = DataArrayPath(dcName, cellEnsembleName, Ebsd::AngFile::CrystalStructures);
  }
  else if(fi.suffix().compare(Ebsd::Ctf::FileExt) == 0)
  {
    ReadCtfData::Pointer reader = ReadCtfData::New();
    reader->setInputFile(m_EbsdFileName);
    reader->setDataContainerArray(dca);
    reader->execute();
    int err = reader->getErrorCondition();

    if (err < 0)
    {
      QMessageBox msgBox;
      msgBox.setText("Error Reading CTF File");
      QString iText;
      QTextStream ss(&iText);
      ss << "There was an error reading the ctf file.\n. The error code returned was " << err;
      msgBox.setInformativeText(iText);
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();
      m_BaseImage = QImage();
      m_DisplayedImage = QImage();
      return;
    }
    dcName = reader->getDataContainerName();
    cellAttrMatName = reader->getCellAttributeMatrixName();
    cellEnsembleName = reader->getCellEnsembleAttributeMatrixName();
    cellPhasesArrayPath = DataArrayPath(dcName, cellAttrMatName, Ebsd::CtfFile::Phases);
    cellEulerAnglesArrayPath = DataArrayPath(dcName, cellAttrMatName, Ebsd::CtfFile::EulerAngles);
    crystalStructuresArrayPath = DataArrayPath(dcName, cellEnsembleName, Ebsd::CtfFile::CrystalStructures);
  }

  //If they want to convert the Eulers to Radians
  if(degToRads->isChecked() == true)
  {
    QString filtName = ChangeAngleRepresentation::ClassName();
    FilterManager* fm = FilterManager::Instance();
    IFilterFactory::Pointer convertEulerFactory = fm->getFactoryForFilter(filtName);
    if (NULL != convertEulerFactory.get() )
    {
      // If we get this far, the Factory is good so creating the filter should not fail unless something has
      // horribly gone wrong in which case the system is going to come down quickly after this.
      AbstractFilter::Pointer convert = convertEulerFactory->create();

      DataArrayPath eulerAnglesPath(dcName, cellAttrMatName, Ebsd::CtfFile::EulerAngles);
      QVariant var;
      var.setValue(eulerAnglesPath);
      convert->setProperty("ConversionType", DREAM3D::EulerAngleConversionType::DegreesToRadians);
      convert->setProperty("CellEulerAnglesArrayPath", var);

      convert->setDataContainerArray(dca);
      convert->execute();
      int err = convert->getErrorCondition();
      if (err < 0)
      {
        m_BaseImage = QImage();
        m_DisplayedImage = QImage();
      }
    }
    else
    {
      QMessageBox msgBox;
      msgBox.setText("Missing Dependent Filter to change Degrees to Radians");
      QString iText;
      QTextStream ss(&iText);
      ss << "A filter is needed to convert the data from Degrees to Radians. That filter is missing from this instance of the application. You can proceed with the import if you wish.";
      msgBox.setInformativeText(iText);
      msgBox.setStandardButtons(QMessageBox::Ok);
      msgBox.setDefaultButton(QMessageBox::Ok);
      msgBox.exec();

    }
  }


  // We can use this filter directly because it is in the current plugin
  GenerateIPFColors::Pointer ipfColorFilter = GenerateIPFColors::New();
  FloatVec3_t ref;
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
  ipfColorFilter->setDataContainerArray(dca);
  ipfColorFilter->setCellPhasesArrayPath(cellPhasesArrayPath);
  ipfColorFilter->setCellEulerAnglesArrayPath(cellEulerAnglesArrayPath);
  ipfColorFilter->setCrystalStructuresArrayPath(crystalStructuresArrayPath);
  ipfColorFilter->execute();
  int err = ipfColorFilter->getErrorCondition();
  if (err < 0)
  {
    m_BaseImage = QImage();
    m_DisplayedImage = QImage();
    QMessageBox msgBox;
    msgBox.setText("IPF Color Filter Error");
    QString iText;
    QTextStream ss(&iText);
    ss << "Error Executing the IPF Colors Filter. The error code was " << err;
    msgBox.setInformativeText(iText);
    msgBox.setStandardButtons(QMessageBox::Ok);
    msgBox.setDefaultButton(QMessageBox::Ok);
    msgBox.exec();
    return;
  }

  DataContainer::Pointer m = dca->getDataContainer(dcName);
  size_t dims[3] = {0, 0, 0};
  m->getGeometryAs<ImageGeom>()->getDimensions(dims);
  float res[3] = {0.0f, 0.0f, 0.0f};
  m->getGeometryAs<ImageGeom>()->getResolution(res);

  m_XDim->setText(QString::number(dims[0]));
  m_YDim->setText(QString::number(dims[1]));
  m_XRes->setText(QString::number(res[0]));
  m_YRes->setText(QString::number(res[1]));

  QImage image(dims[0], dims[1], QImage::Format_ARGB32);
  size_t index = 0;

  AttributeMatrix::Pointer attrMat = m->getAttributeMatrix(cellAttrMatName);

  IDataArray::Pointer arrayPtr = attrMat->getAttributeArray(ipfColorFilter->getCellIPFColorsArrayName());
  if (NULL == arrayPtr.get())
  {
    m_BaseImage = QImage();
    m_DisplayedImage = QImage();
    return;
  }

  UInt8ArrayType* rgbArray = UInt8ArrayType::SafePointerDownCast(arrayPtr.get());
  uint8_t* ipfColors = rgbArray->getPointer(0);

  for(size_t y = 0; y < dims[1]; ++y)
  {
    uint8_t* scanLine = image.scanLine(y);
    for(size_t x = 0; x < dims[0]; ++x)
    {
#if defined (CMP_WORDS_BIGENDIAN)
#error
#else
      scanLine[x * 4 + 3] = 0xFF;
      index = y * dims[0] * 3 + x * 3;
      scanLine[x * 4 + 2] = ipfColors[index + 0];
      scanLine[x * 4 + 1] = ipfColors[index + 1];
      scanLine[x * 4 + 0] = ipfColors[index + 2];
#endif
    }

  }
  m_BaseImage = image.mirrored(false, true);
  m_DisplayedImage = m_BaseImage;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::originChanged(bool checked)
{
  if (checked == false) { return; }

  updateGraphicsView();

  if (checked == true)
  {
    updateDisplay();
  }

  // Initialize to "No Transform"
  AxisAngleInput_t sampleTrans;
  sampleTrans.angle = 0.0f;
  sampleTrans.h = 0.0f;
  sampleTrans.k = 0.0f;
  sampleTrans.l = 1.0f;
  AxisAngleInput_t eulerTrans;
  eulerTrans.angle = 0.0f;
  eulerTrans.h = 0.0f;
  eulerTrans.k = 0.0f;
  eulerTrans.l = 1.0f;


  getSampleTranformation(sampleTrans);
  getEulerTranformation(eulerTrans);

  QString sampleTransLabel = QString::number(sampleTrans.angle) + " @ <" + QString::number(sampleTrans.h) + QString::number(sampleTrans.k) + QString::number(sampleTrans.l) + ">";
  QString eulerTransLabel = QString::number(eulerTrans.angle) + " @ <" + QString::number(eulerTrans.h) + QString::number(eulerTrans.k) + QString::number(eulerTrans.l) + ">";
  m_SampleTransformationLabel->setText(sampleTransLabel);
  m_EulerTransformationLabel->setText(eulerTransLabel);

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::getSampleTranformation(AxisAngleInput_t& input)
{
  // Initialize to "No Transform"
  input.angle = 0.0f;
  input.h = 0.0f;
  input.k = 0.0f;
  input.l = 1.0f;


  if (getTSLchecked() == true)
  {
    input.angle = 180.0;
    input.h = 0.0;
    input.k = 1.0;
    input.l = 0.0;
  }
  else if (getHKLchecked() == true)
  {
    input.angle = 180.0;
    input.h = 0.0;
    input.k = 1.0;
    input.l = 0.0;
  }
  else if (getHEDMchecked() == true)
  {
    input.angle = 0.0;
    input.h = 0.0;
    input.k = 0.0;
    input.l = 1.0;
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::getEulerTranformation(AxisAngleInput_t& input)
{
  input.angle = 0.0f;
  input.h = 0.0f;
  input.k = 0.0f;
  input.l = 1.0f;

  if (getTSLchecked() == true)
  {
    input.angle = 90.0;
    input.h = 0.0;
    input.k = 0.0;
    input.l = 1.0;
  }
  else if (getHKLchecked() == true)
  {
    input.angle = 0.0;
    input.h = 0.0;
    input.k = 0.0;
    input.l = 1.0;
  }
  else if (getHEDMchecked() == true)
  {
    input.angle = 0.0;
    input.h = 0.0;
    input.k = 0.0;
    input.l = 1.0;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::updateGraphicsView()
{
  if (m_TSLdefaultBtn->isChecked())
  {
    m_DisplayedImage = m_BaseImage.mirrored(true, false);
  }
  else if (m_HKLdefaultBtn->isChecked())
  {
    m_DisplayedImage = m_BaseImage.mirrored(true, false);
  }
  else if (m_HEDMdefaultBtn->isChecked())
  {
    m_DisplayedImage = m_BaseImage;
  }
  else if (m_NoTransBtn->isChecked())
  {
    m_DisplayedImage = m_BaseImage;
  }


  QSize size = m_DisplayedImage.size();
  // If the EBSD map is larger than 1024 pixels in either height or width then rescale
  // back to 1024 pixels
  if (size.width() > 1024 || size.height() > 1024)
  {
    QSize gvSize(1024, 1024);
    m_DisplayedImage = m_DisplayedImage.scaled(gvSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  }

  m_DisplayedImage = paintImage(m_DisplayedImage);

  m_EbsdView->setBaseImage(m_DisplayedImage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage QEbsdReferenceFrameDialog::paintImage(QImage image)
{
  QSize size = image.size();
  int imageWidth = size.width();
  int imageHeight = size.height();

  int pxHigh = 0;
  int pxWide = 0;

  QFont font("Ariel", 16, QFont::Bold);
  {
    QPainter painter;
    QImage pImage(100, 100, QImage::Format_ARGB32_Premultiplied);
    pImage.fill(0xFFFFFFFF); // All white background
    painter.begin(&pImage);

    painter.setFont(font);
    QFontMetrics metrics = painter.fontMetrics();
    pxHigh = metrics.height();
    pxWide = metrics.width(QString("TD"));
    painter.end();
  }


  int pxOffset = 2 * pxWide;
  int pyOffset = 2 * pxHigh;
  // Get a QPainter object to add some more details to the image


  int pImageWidth = imageWidth + pxOffset * 2;
  int pImageHeight = imageHeight + pyOffset * 2;

  QImage pImage(pImageWidth, pImageHeight, QImage::Format_ARGB32_Premultiplied);
  pImage.fill(0xFFFFFFFF); // All white background

  // Create a Painter backed by a QImage to draw into
  QPainter painter;
  painter.begin(&pImage);
  painter.setRenderHint(QPainter::Antialiasing, true);

  painter.setFont(font);
  QFontMetrics metrics = painter.fontMetrics();
  pxHigh = metrics.height();
  pxWide = metrics.width(QString("TD"));

  QPoint point(pxOffset, pyOffset);
  painter.drawImage(point, image); // Draw the image we just generated into the QPainter's canvas

  qint32 penWidth = 2;
  painter.setPen(QPen(QColor(0, 0, 0, 255), penWidth, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));

  //  QPainterPath circle;
  //  QPointF center(pImageWidth / 2, pImageHeight / 2);
  //  circle.addEllipse(center, imageWidth / 2, imageHeight / 2);
  //  painter.drawPath(circle);

  painter.drawText(pxOffset / 2, pImageHeight / 2 + pxHigh / 3, "Y");

  pxWide = metrics.width(QString("X"));
  painter.drawText(pImageWidth / 2 - pxWide / 2, pImageHeight - pyOffset + pxHigh + 2, "X");

  painter.drawText(pxOffset / 2, pImageHeight - pyOffset + pxHigh + 2, "(0,0)");

  //  pxWide = metrics.width(config.label);
  //  painter.drawText(2, pxHigh, config.label);

  // Draw slightly transparent lines
  penWidth = 3;
  painter.setPen(QPen(QColor(0, 0, 0, 180), penWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
  // Draw the X-Axis
  painter.drawLine(pxOffset - 3, pImageHeight - pyOffset + 3, pImageWidth  / 2, pImageHeight - pyOffset + 3);
  // Draw the Y-Axis
  painter.drawLine(pxOffset - 3, pImageHeight - pyOffset + 3, pxOffset - 3, pImageHeight / 2);

  //    painter.drawLine(pxOffset, pImageHeight / 2, pImageWidth - pxOffset, pImageHeight / 2);
  //  painter.drawLine(pImageWidth / 2, pyOffset, pImageWidth / 2, pImageHeight - pyOffset);


  painter.end();
  // Scale the image down to 225 pixels
  return pImage;//.scaled(225, 225, Qt::KeepAspectRatio, Qt::SmoothTransformation);
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

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void QEbsdReferenceFrameDialog::on_showHelp_clicked()
{
  DREAM3DHelpUrlGenerator::generateAndOpenHTMLUrl("ebsdtoh5ebsd", this);
}

