/* ============================================================================
 * Copyright (c) 2008, Michael A. Jackson (BlueQuartz Software)
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
#ifndef _AIMIMAGEGRAPHICSDELEGATE_H_
#define _AIMIMAGEGRAPHICSDELEGATE_H_

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

//-- Qt Includes
#include <QtCore/QObject>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtGui/QPainter>


//-- STL Includes
#include <QtCore/QString>



// --- Forward declarations of Classes ----
class QMainWindow;
class QGraphicsItem;
class QGraphicsView;
class QGraphicsScene;


/**
 * @class AIMImageGraphicsDelegate AIMImageGraphicsDelegate.h AIMImageGraphicsDelegate.h
 * @brief A Delegate class that renders HDF5 Data to various outputs
 * @author Mike Jackson
 * @date May 29, 2007
 * @version $Revision: 1.5 $
 */
class AIMImageGraphicsDelegate : public QObject
{
    Q_OBJECT


  public:
    AIMImageGraphicsDelegate(QObject* parent = 0);
    virtual ~AIMImageGraphicsDelegate();


    DREAM3D_INSTANCE_PROPERTY(QMainWindow*, MainWindow)
    DREAM3D_INSTANCE_PROPERTY(QGraphicsView*, GraphicsView)
    DREAM3D_INSTANCE_PROPERTY(QGraphicsScene*, GraphicsScene)


    DREAM3D_INSTANCE_PROPERTY(QImage, CachedImage)
    DREAM3D_INSTANCE_PROPERTY(QImage, OverlayImage)
    DREAM3D_INSTANCE_PROPERTY(bool, CompositeImages)


    /**
     * @brief Displays a Text message in the graphics view. This is typically used
     * when there is an error. You should NOT use this to display general String
     * data. Use an QHDFStringDataWindow instead
     * @param message The message to display
     */
    void displayTextMessage(QString message);

    /**
     * @brief sets all cached values to NULL or empty
     */
    void resetCaches();


  public slots:
    void on_parentResized();

    /**
     * @brief Sets the zoom value. This is expressed in a floating point number
     * where 1.0 is 100% or NO ZOOM and -1.0 represents fit to current window.
     * @param zoomFactor The value of the zoom Factor
     */
    void setZoomFactor(double zoomFactor);

    void increaseZoom();

    void decreaseZoom();

    void fitToWindow();

    void updateGraphicsScene(bool update = true);

    //  void compositeWithImage(QImage* image, bool update = true);

  public slots:
    void setClearMode() { m_composition_mode = QPainter::CompositionMode_Clear;  }
    void setSourceMode() { m_composition_mode = QPainter::CompositionMode_Source;  }
    void setDestinationMode() { m_composition_mode = QPainter::CompositionMode_Destination;  }
    void setSourceOverMode() { m_composition_mode = QPainter::CompositionMode_SourceOver;  }
    void setDestinationOverMode() { m_composition_mode = QPainter::CompositionMode_DestinationOver;  }
    void setSourceInMode() { m_composition_mode = QPainter::CompositionMode_SourceIn;  }
    void setDestInMode() { m_composition_mode = QPainter::CompositionMode_DestinationIn;  }
    void setSourceOutMode() { m_composition_mode = QPainter::CompositionMode_SourceOut;  }
    void setDestOutMode() { m_composition_mode = QPainter::CompositionMode_DestinationOut;  }
    void setSourceAtopMode() { m_composition_mode = QPainter::CompositionMode_SourceAtop;  }
    void setDestAtopMode() { m_composition_mode = QPainter::CompositionMode_DestinationAtop;  }
    void setXorMode() { m_composition_mode = QPainter::CompositionMode_Xor;  }
    void setPlusMode() { m_composition_mode = QPainter::CompositionMode_Plus;  }
    void setMultiplyMode() { m_composition_mode = QPainter::CompositionMode_Multiply;  }
    void setScreenMode() { m_composition_mode = QPainter::CompositionMode_Screen;  }
    void setOverlayMode() { m_composition_mode = QPainter::CompositionMode_Overlay;  }
    void setDarkenMode() { m_composition_mode = QPainter::CompositionMode_Darken;  }
    void setLightenMode() { m_composition_mode = QPainter::CompositionMode_Lighten;  }
    void setColorDodgeMode() { m_composition_mode = QPainter::CompositionMode_ColorDodge;  }
    void setColorBurnMode() { m_composition_mode = QPainter::CompositionMode_ColorBurn;  }
    void setHardLightMode() { m_composition_mode = QPainter::CompositionMode_HardLight;  }
    void setSoftLightMode() { m_composition_mode = QPainter::CompositionMode_SoftLight;  }
    void setDifferenceMode() { m_composition_mode = QPainter::CompositionMode_Difference;  }
    void setExclusionMode() { m_composition_mode = QPainter::CompositionMode_Exclusion;  }

  private:

    QGraphicsPixmapItem*    m_CurrentGraphicsItem;

    double            _zoomFactor;
    double            _zoomFactors[10];
    int               _zoomIndex;
    bool              _shouldFitToWindow;
    QPainter::CompositionMode m_composition_mode;

    /**
    * @brief Displays a Text message in the QGraphicsScene
    * @param message The message to display
    */
    void _displayTextMessage(QString message);

    /**
     * @brief Scales the image by a _zoomFactor
     * @return Returns a QImage that is scaled copy of the cached image
     */
    QImage _scaleImage();
    QImage _scaleImage(QImage image);

    AIMImageGraphicsDelegate(const AIMImageGraphicsDelegate&); //Copy Constructor Not Implemented
    void operator=(const AIMImageGraphicsDelegate&); //Copy Assignment Not Implemented
};

#endif /* _AIMIMAGEGRAPHICSDELEGATE_H_ */

