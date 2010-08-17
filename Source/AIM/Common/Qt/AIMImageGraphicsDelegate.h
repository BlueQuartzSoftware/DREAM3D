///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
//
///////////////////////////////////////////////////////////////////////////////
#ifndef IMAGEGRAPHICSDELEGATE_H_
#define IMAGEGRAPHICSDELEGATE_H_

#include <MXA/Common/MXASetGetMacros.h>

//-- Qt Includes
#include <QtCore/QObject>
#include <QtGui/QGraphicsPixmapItem>
#include <QtGui/QPainter>

//-- STL Includes
#include <string>



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
  Q_OBJECT;


  public:
  AIMImageGraphicsDelegate(QObject* parent = 0);
  virtual ~AIMImageGraphicsDelegate();


  MXA_INSTANCE_PROPERTY_m(QMainWindow*, MainWindow)
  MXA_INSTANCE_PROPERTY_m(QGraphicsView*, GraphicsView)
  MXA_INSTANCE_PROPERTY_m(QGraphicsScene*, GraphicsScene)


  MXA_INSTANCE_PROPERTY_m(QImage, CachedImage)
  MXA_INSTANCE_PROPERTY_m(QImage, OverlayImage)
  MXA_INSTANCE_PROPERTY_m(bool, CompositeImages)


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

#endif /* IMAGEGRAPHICSDELEGATE_H_ */
