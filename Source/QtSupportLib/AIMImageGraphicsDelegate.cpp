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

#include "AIMImageGraphicsDelegate.h"


//-- STL includes
#include <iostream>

//-- Qt Includes
#include <QtCore/QPoint>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QTableWidget>
#include <QtGui/QPixmap>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QGraphicsPixmapItem>

#include "moc_AIMImageGraphicsDelegate.cpp"

#define ZOOM_INDEX_MAX 9
#define ZOOM_INDEX_MIN 0

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImageGraphicsDelegate::AIMImageGraphicsDelegate(QObject* parent) :
  QObject(parent),
  m_MainWindow(NULL),
  m_GraphicsView(NULL),
  m_GraphicsScene(NULL),
  m_CompositeImages(false),
  m_CurrentGraphicsItem(NULL),
  _zoomFactor(1.0),
  _shouldFitToWindow(false)
{


  _zoomFactors[0] = 0.05;
  _zoomFactors[1] = 0.1;
  _zoomFactors[2] = 0.25;
  _zoomFactors[3] = 0.50;
  _zoomFactors[4] = 1.0;
  _zoomFactors[5] = 1.5;
  _zoomFactors[6] = 2.0;
  _zoomFactors[7] = 4.0;
  _zoomFactors[8] = 6.0;
  _zoomFactors[9] = -1.0;
  _zoomIndex = 4;

  m_composition_mode = QPainter::CompositionMode_Exclusion;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
AIMImageGraphicsDelegate::~AIMImageGraphicsDelegate()
{
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::resetCaches()
{
  this->m_CachedImage = QImage();
  this->m_OverlayImage = QImage();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::displayTextMessage(QString message)
{
  _displayTextMessage( message );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::increaseZoom()
{
  _shouldFitToWindow = false;
  if (m_CachedImage.isNull() == true) { return; }
  // Find the next scaling factor up from where we are currently
  QSize imageSize = this->m_CachedImage.size();
  int gvWidth = m_GraphicsView->size().width();
  int gvHeight = m_GraphicsView->size().height();
  gvWidth -= 4;
  gvHeight -= 4;
  if (imageSize.width() > imageSize.height() )
  {
    for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
    {
      if (_zoomFactor < this->_zoomFactors[i] && i > 0)
      {
        this->_zoomIndex = i;
        this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
        break;
      }
    }
  }

  for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
  {
    if (_zoomFactor < this->_zoomFactors[i] && i > 0)
    {
      this->_zoomIndex = i;
      this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
      break;
    }
  }

  updateGraphicsScene();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::decreaseZoom()
{
  _shouldFitToWindow = false;
  // Find the next scaling factor down
  if (m_CachedImage.isNull() == true) { return; }
  QSize imageSize = this->m_CachedImage.size();
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
//  gvWidth -= 4;
//  gvHeight -= 4;
  if (imageSize.width() > imageSize.height() )
  {
    for (int i = 0; i < ZOOM_INDEX_MAX - 1; ++i )
    {
      if (_zoomFactor < this->_zoomFactors[i] && i > 0)
      {
        this->_zoomIndex = i - 1;
        this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
        break;
      }
    }
  }

  for (int i = ZOOM_INDEX_MAX - 1; i >= 0; --i )
  {
    if (_zoomFactor > this->_zoomFactors[i] && i > 0)
    {
      this->_zoomIndex = i;
      this->_zoomFactor = this->_zoomFactors[this->_zoomIndex ];
      break;
    }
  }
  updateGraphicsScene();
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::fitToWindow()
{
  if (m_CachedImage.isNull() == true) { return; }
  _shouldFitToWindow = true;
  _zoomIndex = ZOOM_INDEX_MAX;
  this->setZoomFactor(_zoomFactors[_zoomIndex]);

  QSize imageSize = this->m_CachedImage.size();
  int gvWidth = m_GraphicsView->size().width();
  int gvHeight = m_GraphicsView->size().height();
  gvWidth -= 4;
  gvHeight -= 4;
  if (imageSize.width() > imageSize.height() )
  {
    double zf = (double)(gvWidth) / (double)(imageSize.width());
    this->setZoomFactor(zf);
  }
  else
  {
    double zf = (double)(gvHeight) / (double)(imageSize.height());
    this->setZoomFactor(zf);
  }
  updateGraphicsScene();
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::setZoomFactor(double zoomFactor)
{
  this->_zoomFactor = zoomFactor;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage AIMImageGraphicsDelegate::_scaleImage()
{
  //qDebug() << "  AIMImageGraphicsDelegate::_scaleImage()" << "\n";
  QSize imageSize = this->m_CachedImage.size();
// qDebug() << "    imageSize (W x H) :" << imageSize.width() << " x " << imageSize.height() << "\n";
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
// qDebug() << "    GV Size (W X H) :" << gvWidth << " x " << gvHeight << "\n";
  if (_zoomFactor > -1.0)
  {
    // qDebug() << "  _zoomFactor: " << _zoomFactor << "\n";
    imageSize *= _zoomFactor;
  }

  return this->m_CachedImage.scaled(imageSize, Qt::KeepAspectRatio );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QImage AIMImageGraphicsDelegate::_scaleImage(QImage image)
{
  //qDebug() << "  AIMImageGraphicsDelegate::_scaleImage()" << "\n";
  QSize imageSize = image.size();
// qDebug() << "    imageSize (W x H) :" << imageSize.width() << " x " << imageSize.height() << "\n";
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
// qDebug() << "    GV Size (W X H) :" << gvWidth << " x " << gvHeight << "\n";
  if (_zoomFactor > -1.0)
  {
    // qDebug() << "  _zoomFactor: " << _zoomFactor << "\n";
    imageSize *= _zoomFactor;
  }

  return image.scaled(imageSize, Qt::KeepAspectRatio );
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::on_parentResized()
{
  //qDebug() << "  AIMImageGraphicsDelegate::on_parentResized" << "\n";
//  int gvWidth = m_GraphicsView->size().width();
//  int gvHeight = m_GraphicsView->size().height();
// qDebug() << "    GV Size (W X H) :" << gvWidth << " x " << gvHeight << "\n";
  if (_shouldFitToWindow == true)
  {
    fitToWindow();
  }
  else
  {
    updateGraphicsScene();
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::updateGraphicsScene(bool update)
{

  if (this->m_CachedImage.isNull() == true)
  {
    return;
  }
  if (NULL != m_CurrentGraphicsItem)
  {
    m_GraphicsScene->removeItem(m_CurrentGraphicsItem); //Remove the image that is displaying
    m_CurrentGraphicsItem->setParentItem(NULL); // Set the parent to NULL
    delete m_CurrentGraphicsItem; // Delete the object
  }

  QImage dataImage = _scaleImage();
  QPixmap imagePixmap;
  if (m_CompositeImages == true && m_OverlayImage.isNull() == false)
  {
    QImage topImage = _scaleImage(m_OverlayImage);
    QPainter painter;
    QImage paintImage(dataImage.size(), QImage::Format_ARGB32_Premultiplied);
    QPoint point(0, 0);
    painter.begin(&paintImage);
    // Draw the fixed Image first
    painter.setPen(Qt::NoPen);
    painter.drawImage(point, topImage );
    // Draw the moving image next
    painter.setCompositionMode(m_composition_mode);
    painter.drawImage(point, dataImage);
    painter.end();
    imagePixmap = QPixmap::fromImage(paintImage);
  }
  else
  {
    imagePixmap = QPixmap::fromImage(dataImage);
  }

  m_CurrentGraphicsItem = m_GraphicsScene->addPixmap(imagePixmap); // Add the new image into the display
  QRectF rect = m_CurrentGraphicsItem->boundingRect();
  m_GraphicsScene->setSceneRect(rect);
  m_GraphicsView->setScene(m_GraphicsScene);
  m_GraphicsView->centerOn(m_CurrentGraphicsItem);
  if (update)
  {
    m_GraphicsScene->update(rect);
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void AIMImageGraphicsDelegate::_displayTextMessage(QString message)
{
  if ( NULL != m_CurrentGraphicsItem )
  {
    m_GraphicsScene->removeItem(m_CurrentGraphicsItem); //Remove the image that is displaying
    m_CurrentGraphicsItem->setParentItem(NULL); // Set the parent to NULL
    delete m_CurrentGraphicsItem; // Delete the object
  }
  m_CurrentGraphicsItem = NULL;
  QGraphicsTextItem* tItem = m_GraphicsScene->addText(message); // Add the new image into the display

  QRectF rect = tItem->boundingRect();
  m_GraphicsScene->setSceneRect(rect);
  m_GraphicsView->setScene(m_GraphicsScene);
  m_GraphicsView->centerOn(tItem);
  m_GraphicsScene->update(rect);
}

