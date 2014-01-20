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
#ifndef _PoleFigureImageUtilities_H_
#define _PoleFigureImageUtilities_H_

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QtGui/QImage>

#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"

/**
 * @class PoleFigureData PoleFigureData.h StatsGenerator/PoleFigureData.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 4, 2011
 * @version 1.0
 */
class PoleFigureData : QObject
{
    Q_OBJECT

  public:
    PoleFigureData()
    {
      imageSize[0] = 0;
      imageSize[1] = 0;
      kernelRadius[0] = 3;
      kernelRadius[1] = 3;
    }

    PoleFigureData(QVector<float>& xData, QVector<float>& yData, const QString& s, qint32* kernelRad, qint32* size) :
      xData(xData),
      yData(yData),
      label(s)
    {
      imageSize[0] = size[0];
      imageSize[1] = size[1];
      kernelRadius[0] = kernelRad[0];
      kernelRadius[1] = kernelRad[1];
    }

    // We need a copy constructor
    PoleFigureData(const PoleFigureData& rhs)
    {
      this->xData = rhs.xData;
      this->yData = rhs.yData;

      this->label = rhs.label;
      this->imageSize[0] = rhs.imageSize[0];
      this->imageSize[1] = rhs.imageSize[1];
      this->kernelRadius[0] = rhs.kernelRadius[0];
      this->kernelRadius[1] = rhs.kernelRadius[1];
    }

    virtual ~PoleFigureData() {}

    QVector<float> xData;
    QVector<float> yData;

    qint32 kernelRadius[2];
    qint32 imageSize[2];
    QString label;

    void operator=(const PoleFigureData& rhs)
    {
      this->xData = rhs.xData;
      this->yData = rhs.yData;
      this->label = rhs.label;
      this->imageSize[0] = rhs.imageSize[0];
      this->imageSize[1] = rhs.imageSize[1];
      this->kernelRadius[0] = rhs.kernelRadius[0];
      this->kernelRadius[1] = rhs.kernelRadius[1];
    }

  private:

};


/**
 * @class ColorPoleFigure ColorPoleFigure.h StatsGenerator/ColorPoleFigure.h
 * @brief
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Nov 4, 2011
 * @version 1.0
 */
class PoleFigureImageUtilities
{
  public:
    PoleFigureImageUtilities();
    virtual ~PoleFigureImageUtilities();



    int countPixelNeighbors(int imageWidth, int imageHeight, int pX, int pY,
                            QVector<qint32>& data, QVector<qint32>& counts,
                            int kX, int kY, bool genmask = false);

    void generateKernelWeigths(int kernelWidth, int kernelHeight);


#if 0
    QImage generateColorPoleFigureImage(const PoleFigureData& config);

    QImage generatePoleFigureImage(const PoleFigureData& config);
#endif

    static QImage PaintPoleFigureOverlay(int imageWidth, int imageHeight, QString label, QImage image);

    static QImage CreateQImageFromRgbaArray(UInt8ArrayType* poleFigurePtr, int imageDimension, bool includeOverlay);
    /**
     * @brief Create3ImagePoleFigure
     * @param i0
     * @param i1
     * @param i2
     * @param config
     * @return
     */
    static QImage Create3ImagePoleFigure(UInt8ArrayType* i0, UInt8ArrayType* i1, UInt8ArrayType* i2,
                                         PoleFigureConfiguration_t& config);
  private:

    QVector<qint32> m_KernelWeights;
    bool m_KernelWeightsInited;


    PoleFigureImageUtilities(const PoleFigureImageUtilities&); // Copy Constructor Not Implemented
    void operator=(const PoleFigureImageUtilities&); // Operator '=' Not Implemented
};


#endif /* _PoleFigureImageUtilities_H_ */

