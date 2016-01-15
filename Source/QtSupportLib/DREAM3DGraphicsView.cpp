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



#include "DREAM3DGraphicsView.h"

#include <QtCore/QFileInfo>
#include <QtCore/QUrl>
#include <QtCore/QtDebug>
#include <QtCore/QMimeData>

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QtWidgets/QWidget>
#include <QtGui/QPixmap>
#include <QtWidgets/QGraphicsPolygonItem>
#include <QtGui/QImageReader>

#include "moc_DREAM3DGraphicsView.cpp"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DREAM3DGraphicsView::DREAM3DGraphicsView(QWidget* parent)
  : QGraphicsView(parent),
    m_ImageGraphicsItem(NULL),
    m_UseColorTable(false)
{
  setAcceptDrops(true);
  setDragMode(RubberBandDrag);

  m_ZoomFactors[0] = 0.1f;
  m_ZoomFactors[1] = 0.25f;
  m_ZoomFactors[2] = 0.5f;
  m_ZoomFactors[3] = 1.0f;
  m_ZoomFactors[4] = 1.250f;
  m_ZoomFactors[5] = 1.500f;
  m_ZoomFactors[6] = 2.000f;
  m_ZoomFactors[7] = 4.000f;
  m_ZoomFactors[8] = 6.000f;
  m_ZoomFactors[9] = -1.0f;

  m_ImageDisplayType = EmMpm_Constants::OriginalImage;
  m_composition_mode = QPainter::CompositionMode_SourceOver;
  m_OverlayTransparency = 1.0f; // Fully opaque

  m_CustomColorTable.resize(256);
  for (quint32 i = 0; i < 256; ++i)
  {
    m_CustomColorTable[i] = qRgb(i, i, i);
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::setOverlayTransparency(float f)
{
  m_OverlayTransparency = f;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::useCustomColorTable(bool b)
{
  m_UseColorTable = b;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::zoomIn()
{
  scale(1.1, 1.1);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::zoomOut()
{
  scale(1.0 / 1.1, 1.0 / 1.1);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::setZoomIndex(int index)
{
  if (index == 9)
  {
    QGraphicsScene* scenePtr = scene();
    if (NULL != scenePtr)
    {
      QRectF r = scenePtr->sceneRect();
      fitInView(r, Qt::KeepAspectRatio);
    }
  }
  else
  {
    QTransform transform;
    transform.scale(m_ZoomFactors[index], m_ZoomFactors[index]);
    setTransform(transform);
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::dragEnterEvent(QDragEnterEvent* event)
{
// qWarning("QFSDroppableGraphicsView::dragEnterEvent(QDragEnterEvent *event)");
  // accept just text/uri-list mime format
  if (event->mimeData()->hasFormat("text/uri-list"))
  {
    event->acceptProposedAction();
  }
  this->setStyleSheet("border: 1px solid green;");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::dragLeaveEvent(QDragLeaveEvent* event)
{
  Q_UNUSED(event);
//  qWarning("QFSDroppableGraphicsView::dragLeaveEvent(QDragLeaveEvent *event)");
  this->setStyleSheet("");
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::dropEvent(QDropEvent* event)
{
  this->setStyleSheet("");
//  qWarning("QFSDroppableGraphicsView::dropEvent(QDropEvent *event)");
  QList<QUrl> urlList;
  QString fName;
  QFileInfo info;

  if (event->mimeData()->hasUrls())
  {
    urlList = event->mimeData()->urls(); // returns list of QUrls
    // if just text was dropped, urlList is empty (size == 0)

    if ( urlList.size() > 0) // if at least one QUrl is present in list
    {
      fName = urlList[0].toLocalFile(); // convert first QUrl to local path
      info.setFile( fName ); // information about file
      QString ext = info.suffix().toLower();
      if (ext.compare("tif") == 0
          || ext.compare("tiff") == 0
          || ext.compare("jpg") == 0
          || ext.compare("jpeg") == 0
          || ext.compare("png") == 0
          || ext.compare("bmp") == 0)
      {
        //m_MainGui->openBaseImageFile(fName);
      }
    }
  }
  event->acceptProposedAction();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
//QImage DREAM3DGraphicsView::getCompositedImage()
//{
//  return m_CompositedImage;
//}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage& DREAM3DGraphicsView::blend(QImage& src, QImage& dst, float opacity)
{
  if (src.width() <= 0 || src.height() <= 0)
  { return dst; }
  if (dst.width() <= 0 || dst.height() <= 0)
  { return dst; }

  if (src.width() != dst.width() || src.height() != dst.height())
  {
#ifndef NDEBUG
    std::cerr << "WARNING: ImageEffect::blend : src and destination images are not the same size\n";
#endif
    return dst;
  }

  if (opacity < 0.0 || opacity > 1.0)
  {
#ifndef NDEBUG
    std::cerr << "WARNING: ImageEffect::blend : invalid opacity. Range [0, 1]\n";
#endif
    return dst;
  }

  if (src.depth() != 32) { src = src.convertToFormat(QImage::Format_ARGB32); }
  if (dst.depth() != 32) { dst = dst.convertToFormat(QImage::Format_ARGB32); }

  int pixels = src.width() * src.height();
  {
#ifdef WORDS_BIGENDIAN   // ARGB (skip alpha)
    register unsigned char* data1 = (unsigned char*)dst.bits() + 1;
    register unsigned char* data2 = (unsigned char*)src.bits() + 1;
#else                    // BGRA
    unsigned char* data1 = static_cast<unsigned char*>(dst.bits());
    unsigned char* data2 = static_cast<unsigned char*>(src.bits());
#endif

    for (int i = 0; i < pixels; i++)
    {
#ifdef WORDS_BIGENDIAN
      *data1 += (unsigned char)((*(data2++) - *data1) * opacity);
      data1++;
      *data1 += (unsigned char)((*(data2++) - *data1) * opacity);
      data1++;
      *data1 += (unsigned char)((*(data2++) - *data1) * opacity);
      data1++;
#else
      *data1 += static_cast<unsigned char>((*(data2++) - *data1) * opacity);
      data1++;
      *data1 += static_cast<unsigned char>((*(data2++) - *data1) * opacity);
      data1++;
      *data1 += static_cast<unsigned char>((*(data2++) - *data1) * opacity);
      data1++;
#endif
      data1++; // skip alpha
      data2++;
    }
  }

  return dst;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::updateDisplay()
{

//  std::cout << "DREAM3DGraphicsView::updateDisplay()" << std::endl;
  QPainter painter;
  QSize pSize(0, 0);
  if (m_BaseImage.isNull() == false)
  {
    pSize = m_BaseImage.size();
  }
  else
  {
    return;
  }

  QImage paintImage(pSize, QImage::Format_ARGB32_Premultiplied);
  QPoint point(0, 0);
  painter.begin(&paintImage);
  painter.setPen(Qt::NoPen);
  if (m_ImageDisplayType == EmMpm_Constants::OriginalImage)
  {
    painter.drawImage(point, m_BaseImage);
  }

  painter.end();

  if (paintImage.isNull() == true)
  {
    return;
  }
  QGraphicsPixmapItem* pixItem = qgraphicsitem_cast<QGraphicsPixmapItem*> (m_ImageGraphicsItem);
  pixItem->setPixmap(QPixmap::fromImage(paintImage));

  this->update();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::setImageDisplayType(EmMpm_Constants::ImageDisplayType displayType)
{
  m_ImageDisplayType = displayType;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
#if 0
void DREAM3DGraphicsView::loadBaseImageFile(const QString& filename)
{
  m_BaseImage = QImage(filename);
  if (m_BaseImage.isNull() == true)
  {
    return;
  }
  QSize pSize(0, 0);
  pSize = m_BaseImage.size();

  m_OverlayImage = m_BaseImage;
  m_CompositedImage = m_BaseImage;

  QVector<QRgb > colorTable(256);
  for (quint32 i = 0; i < 256; ++i)
  {
    colorTable[i] = qRgb(i, i, i);
  }
  m_BaseImage.setColorTable(colorTable);

  if (m_BaseImage.isNull() == true)
  {
    std::cout << "Base Image was NULL for some reason. Returning" << std::endl;
    return;
  }
  QGraphicsScene* gScene = scene();
  if (gScene == NULL)
  {
    gScene = new QGraphicsScene(this);
    setScene(gScene);
  }
  else
  {

    setScene(NULL);
    gScene->deleteLater();
    gScene = new QGraphicsScene(this);
    setScene(gScene);

    delete m_ImageGraphicsItem;
    m_ImageGraphicsItem = NULL;
  }
  if (NULL == m_ImageGraphicsItem)
  {
    QImageReader reader(filename);
    QPixmap pixmap = QPixmap::fromImageReader(&reader, 0);
    m_ImageGraphicsItem = gScene->addPixmap(pixmap);
  }
  m_ImageGraphicsItem->setAcceptDrops(true);
  m_ImageGraphicsItem->setZValue(-1);


  QRectF rect = m_ImageGraphicsItem->boundingRect();
  gScene->setSceneRect(rect);
  centerOn(m_ImageGraphicsItem);
  this->updateDisplay();
  emit fireBaseImageFileLoaded(filename);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::loadOverlayImageFile(const QString& filename)
{

  m_OverlayImage = QImage(filename);
  if (m_OverlayImage.isNull() == true)
  {
    std::cout << "Error Loading image: " << filename.toStdString() << std::endl;
    return;
  }

  m_ImageDisplayType = EmMpm_Constants::CompositedImage;
  setOverlayImage(m_OverlayImage);
  emit fireOverlayImageFileLoaded(filename);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage DREAM3DGraphicsView::getOverlayImage()
{
  return m_OverlayImage;
}

#endif
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::setBaseImage(QImage image)
{
  if (image.isNull() == true)
  {
    return;
  }
  m_BaseImage = image;

  QSize pSize(0, 0);
  pSize = m_BaseImage.size();

  QVector<QRgb > colorTable(256);
  for (quint32 i = 0; i < 256; ++i)
  {
    colorTable[i] = qRgb(i, i, i);
  }
  m_BaseImage.setColorTable(colorTable);

  if (m_BaseImage.isNull() == true)
  {
    std::cout << "Base Image was NULL for some reason. Returning" << std::endl;
    return;
  }
  QGraphicsScene* gScene = scene();
  if (gScene == NULL)
  {
    gScene = new QGraphicsScene(this);
    setScene(gScene);
  }
  else
  {

    setScene(NULL);
    gScene->deleteLater();
    gScene = new QGraphicsScene(this);
    setScene(gScene);

    delete m_ImageGraphicsItem;
    m_ImageGraphicsItem = NULL;
  }
  if (NULL == m_ImageGraphicsItem)
  {
    QPixmap pixmap = QPixmap::fromImage(image);
    m_ImageGraphicsItem = gScene->addPixmap(pixmap);
  }
  m_ImageGraphicsItem->setAcceptDrops(true);
  m_ImageGraphicsItem->setZValue(-1);


  QRectF rect = m_ImageGraphicsItem->boundingRect();
  gScene->setSceneRect(rect);
  centerOn(m_ImageGraphicsItem);
  this->updateDisplay();



}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage DREAM3DGraphicsView::getBaseImage()
{
  return m_BaseImage;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::updateColorTables( QVector<QRgb> colorTable)
{
  for (quint32 i = 0; i < 256; ++i)
  {
    m_CustomColorTable[i] = qRgb(i, i, i);
  }

  qint32 size = colorTable.size();
  for(qint32 index = 0; index < size; ++index)
  {
    m_CustomColorTable[index] = colorTable[index];
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void DREAM3DGraphicsView::setCompositeMode(EmMpm_Constants::CompositeType mode)
{
  m_ImageDisplayType = EmMpm_Constants::CompositedImage;
  switch(mode)
  {
    case EmMpm_Constants::Exclusion:
      m_composition_mode = QPainter::CompositionMode_Exclusion;
      break;
    case EmMpm_Constants::Difference:
      m_composition_mode = QPainter::CompositionMode_Difference;
      break;
    case EmMpm_Constants::Alpha_Blend:
      m_composition_mode = QPainter::CompositionMode_SourceOver;
      break;

    case EmMpm_Constants::Plus:
      m_composition_mode = QPainter::CompositionMode_Plus;
      break;
    case EmMpm_Constants::Multiply:
      m_composition_mode = QPainter::CompositionMode_Multiply;
      break;
    case EmMpm_Constants::Screen:
      m_composition_mode = QPainter::CompositionMode_Screen;
      break;
    case EmMpm_Constants::Darken:
      m_composition_mode = QPainter::CompositionMode_Darken;
      break;
    case EmMpm_Constants::Lighten:
      m_composition_mode = QPainter::CompositionMode_Lighten;
      break;
    case EmMpm_Constants::ColorDodge:
      m_composition_mode = QPainter::CompositionMode_ColorDodge;
      break;
    case EmMpm_Constants::ColorBurn:
      m_composition_mode = QPainter::CompositionMode_ColorBurn;
      break;
    case EmMpm_Constants::HardLight:
      m_composition_mode = QPainter::CompositionMode_HardLight;
      break;
    case EmMpm_Constants::SoftLight:
      m_composition_mode = QPainter::CompositionMode_SoftLight;
      break;

    case EmMpm_Constants::Destination:
      m_composition_mode = QPainter::CompositionMode_Destination;
      break;
    case EmMpm_Constants::Source:
      m_composition_mode = QPainter::CompositionMode_Source;
      break;
    case EmMpm_Constants::DestinationOver:
      m_composition_mode = QPainter::CompositionMode_DestinationOver;
      break;
    case EmMpm_Constants::SourceIn:
      m_composition_mode = QPainter::CompositionMode_SourceIn;
      break;
    case EmMpm_Constants::DestinationIn:
      m_composition_mode = QPainter::CompositionMode_DestinationIn;
      break;
    case EmMpm_Constants::DestinationOut:
      m_composition_mode = QPainter::CompositionMode_DestinationOut;
      break;
    case EmMpm_Constants::SourceAtop:
      m_composition_mode = QPainter::CompositionMode_SourceAtop;
      break;
    case EmMpm_Constants::DestinationAtop:
      m_composition_mode = QPainter::CompositionMode_DestinationAtop;
      break;
    case EmMpm_Constants::Overlay:
      m_composition_mode = QPainter::CompositionMode_Overlay;
      break;
    case EmMpm_Constants::Clear:
      m_composition_mode = QPainter::CompositionMode_Clear;
      break;

    default:
      m_composition_mode = QPainter::CompositionMode_Exclusion;
      break;
  }

  this->setImageDisplayType(m_ImageDisplayType);
}
