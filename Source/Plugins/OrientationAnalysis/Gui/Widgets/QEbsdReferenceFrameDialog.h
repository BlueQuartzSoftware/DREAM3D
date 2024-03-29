/* ============================================================================
 * Copyright (c) 2009-2016 BlueQuartz Software, LLC
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
 * The code contained herein was partially funded by the following contracts:
 *    United States Air Force Prime Contract FA8650-07-D-5800
 *    United States Air Force Prime Contract FA8650-10-D-5210
 *    United States Prime Contract Navy N00173-07-C-2068
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#pragma once

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/Common/Constants.h"
#include "SIMPLib/FilterParameters/AxisAngleInput.h"

#include "EbsdLib/Core/EbsdLibConstants.h"

#include "SVWidgetsLib/Widgets/SVControlWidgets.h"

class QGraphicsPixmapItem;
class DataArrayPath;
class DataContainerArray;
class IDataArray;
using IDataArrayShPtrType = std::shared_ptr<IDataArray>;

using DataContainerArrayShPtrType = std::shared_ptr<DataContainerArray>;

#include "ui_QEbsdReferenceFrameDialog.h"

/**
 * @class QEbsdReferenceFrameDialog QEbsdReferenceFrameDialog.h EbsdImport/UI/QEbsdReferenceFrameDialog.h
 * @brief This Dialog displays an IPF image of one slice of the users data and allows
 * the user to rotate or mirror the data so that they may rectify the reference frame
 * of the EBSD scan and the reference frame of the sample.
 *
 * @date Oct 2, 2011
 * @version 1.0
 */
class QEbsdReferenceFrameDialog : public SVDialog, private Ui::QEbsdReferenceFrameDialog
{
  Q_OBJECT

public:
  QEbsdReferenceFrameDialog(QString filename, QWidget* parent = nullptr);
  virtual ~QEbsdReferenceFrameDialog();

  void setEbsdFileName(QString filename);
  void setTSLDefault(bool checked);
  void setHKLDefault(bool checked);
  void setHEDMDefault(bool checked);
  void setNoTrans(bool checked);
  void loadEbsdData();
  void updateGraphicsView();
  void updateDisplay();

  EbsdLib::EbsdToSampleCoordinateMapping getSelectedOrigin();
  bool getTSLchecked();
  bool getHKLchecked();
  bool getHEDMchecked();
  bool getNoTranschecked();
  void getSampleTranformation(AxisAngleInput& input);
  void getEulerTranformation(AxisAngleInput& input);

protected Q_SLOTS:
  void originChanged(bool checked);

  void degToRagsChanged(int state);

  void referenceDirectionChanged();
  QImage paintImage(QImage image);

  void on_showHelp_clicked();

  void z10_triggered();
  void z25_triggered();
  void z50_triggered();
  void z100_triggered();
  void z125_triggered();
  void z150_triggered();
  void z200_triggered();
  void z400_triggered();
  void z600_triggered();

  void on_fitToWindow_clicked();

protected:
  void setupGui();

  int createIpfColors(DataContainerArrayShPtrType dca, DataArrayPath cellPhasesArrayPath, DataArrayPath cellEulerAnglesArrayPath, DataArrayPath crystalStructuresArrayPath, QString& outputArrayName);
  int createArrayColors(DataContainerArrayShPtrType dca, DataArrayPath dataArrayPath, QString outputArrayName);

  void generateImageRGB(IDataArrayShPtrType dataArray, size_t dims[3]);

private:
  QString m_EbsdFileName;
  QButtonGroup* m_OriginGroup;
  QImage m_BaseImage;
  QImage m_DisplayedImage;

public:
  QEbsdReferenceFrameDialog(const QEbsdReferenceFrameDialog&) = delete;            // Copy Constructor Not Implemented
  QEbsdReferenceFrameDialog(QEbsdReferenceFrameDialog&&) = delete;                 // Move Constructor Not Implemented
  QEbsdReferenceFrameDialog& operator=(const QEbsdReferenceFrameDialog&) = delete; // Copy Assignment Not Implemented
  QEbsdReferenceFrameDialog& operator=(QEbsdReferenceFrameDialog&&) = delete;      // Move Assignment Not Implemented
};
