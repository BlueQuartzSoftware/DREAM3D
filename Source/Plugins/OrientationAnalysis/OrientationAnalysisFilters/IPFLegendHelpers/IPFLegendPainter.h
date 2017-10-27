/* ============================================================================
 * Copyright (c) 2017 BlueQuartz Software, LLC
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
 * Neither the names of any of the BlueQuartz Software contributors
 * may be used to endorse or promote products derived from this software without
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
 *
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _ipflegendpainter_H_
#define _ipflegendpainter_H_

#include <QtGui/QImage>

class QFontMetrics;
class QPainter;

/**
 * @brief The IPFLegendPainter class implements the interface and a helper class that
 * allows for the painting of IPF (Inverse Pole Figure) "Triangle Legends" for IPF
 * color maps.
 */
class IPFLegendPainter
{
public:
  IPFLegendPainter();
  virtual ~IPFLegendPainter();

  virtual QImage createLegend(int pixelWidth, int pixelHeight) = 0;

  /**
 * @brief paintSymmetryDirection This method will correctly parse the string and paint the string
 * with the proper "bar" notation. Use an under_score in front of the value that you want a bar
 * to appear over. Example: [0_12] would put the bar over the 1.
 * @param text The string to parse and paint
 * @param metrics QFontMetrics that is currently being used
 * @param painter The QPainter to use to paint the text
 * @param x The X Location on the canvas
 * @param y The Y Location on the canvas
 */
  void paintSymmetryDirection(const QString& text, QFontMetrics* metrics, QPainter* painter, int x, int y);

private:
  IPFLegendPainter(const IPFLegendPainter&); // Copy Constructor Not Implemented
  void operator=(const IPFLegendPainter&);    // Operator '=' Not Implemented
};

#endif /* _ipflegendpainter_H_ */
