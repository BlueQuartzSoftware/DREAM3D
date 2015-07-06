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


#ifndef _EMMPM_GRAPHICS_VIEW_H_
#define _EMMPM_GRAPHICS_VIEW_H_

#include <iostream>

#include <QtCore/QVector>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QRubberBand>


#include "QtSupportLib/QtSupportLib.h"

class UserInitArea;
class EmMpmWorkbench;

namespace EmMpm_Constants
{

  enum ImageDisplayType
  {
    OriginalImage = 0,
    SegmentedImage,
    CompositedImage,
    UnknownDisplayType
  };

  enum CompositeType
  {
    Exclusion,
    Difference,
    Alpha_Blend,
    Plus,
    Multiply,
    Screen,
    Darken,
    Lighten,
    ColorDodge,
    ColorBurn,
    HardLight,
    SoftLight,
    Destination,
    Source,
    DestinationOver,
    SourceIn,
    DestinationIn,
    DestinationOut,
    SourceAtop,
    DestinationAtop,
    Overlay,
    Clear,
    UnknownCompositeType
  };

}

class QtSupportLib_EXPORT DREAM3DGraphicsView : public QGraphicsView
{
    Q_OBJECT;

  public:

    DREAM3DGraphicsView( QWidget* parent = NULL);

    /**
    * @brief Over-riding implementation from base class
    * @param event QDragEnterEvent Event fired when dragging enters the QGraphicsView
    */
    void dragEnterEvent(QDragEnterEvent* event);

    /**
    * @brief Over-riding implementation from base class
    * @param event QDropEvent Event fired when object is dropped on QGraphicsView
    */
    void dropEvent(QDropEvent* event);

    /**
    * @brief Over-riding implementation from base class
    * @param event QDragLeaveEvent Event fired when dragging leaves QGraphicsView
    */
    void dragLeaveEvent(QDragLeaveEvent* event);

//    void mousePressEvent( QMouseEvent* event );
//    void mouseMoveEvent( QMouseEvent* event );
//    void mouseReleaseEvent( QMouseEvent* event );

    void setBaseImage(QImage image);
    QImage getBaseImage();

    //void setOverlayImage(QImage image);
    //QImage getOverlayImage();

    //QImage getCompositedImage();

//    void loadBaseImageFile(const QString &filename);
//    void loadOverlayImageFile(const QString &filename);

    QImage& blend(QImage& src, QImage& dst, float opacity);


    void updateColorTables(QVector<QRgb> colorTable);
    void useCustomColorTable(bool b);

  public slots:
    void zoomIn();

    void zoomOut();

    void setOverlayTransparency(float f);

    void setZoomIndex(int index);

    void setImageDisplayType(EmMpm_Constants::ImageDisplayType displayType);

    void setCompositeMode(EmMpm_Constants::CompositeType mode);

    void updateDisplay();


  signals:
    void fireBaseImageFileLoaded(const QString& filename);
    void fireOverlayImageFileLoaded(const QString& filename);

  protected:


  private:
    QGraphicsItem* m_ImageGraphicsItem;
    QImage         m_BaseImage;

    QPoint         m_MouseClickOrigin;
    float          m_ZoomFactors[10];

    EmMpm_Constants::ImageDisplayType           m_ImageDisplayType;
    bool           m_ShowOverlayImage;
    QPainter::CompositionMode m_composition_mode;
    float          m_OverlayTransparency;

    bool           m_UseColorTable;

    QVector<QRgb> m_CustomColorTable;
    QVector<QRgb> m_OriginalColorTable;

    DREAM3DGraphicsView(const DREAM3DGraphicsView&); // Copy Constructor Not Implemented
    void operator=(const DREAM3DGraphicsView&); // Operator '=' Not Implemented
};

#endif /* _EMMPM_GRAPHICS_VIEW_H_ */
