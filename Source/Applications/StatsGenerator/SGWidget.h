/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories
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
#ifndef SGWIDGET_H_
#define SGWIDGET_H_

#include <QtGui/QWidget>


#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/VolumeDataContainer.h"
#include "StatsGenerator/Presets/AbstractMicrostructurePreset.h"

class QwtPlotZoomer;
class QwtPlotPicker;
class QwtPlotPanner;
class QwtPlotGrid;
class QwtPlotCurve;
class QwtPlotMarker;

/*
 *
 */
class SGWidget : public QWidget
{

  Q_OBJECT

  public:
    SGWidget(QWidget *parent = 0);
    virtual ~SGWidget();

     DREAM3D_VIRTUAL_INSTANCE_PROPERTY(unsigned int, PhaseType)
     DREAM3D_VIRTUAL_INSTANCE_PROPERTY(unsigned int, CrystalStructure)
     DREAM3D_VIRTUAL_INSTANCE_PROPERTY(int, PhaseIndex)
     DREAM3D_VIRTUAL_INSTANCE_PROPERTY(float, PhaseFraction)
     DREAM3D_VIRTUAL_INSTANCE_PROPERTY(float, TotalPhaseFraction)
     DREAM3D_VIRTUAL_INSTANCE_PROPERTY(bool, DataHasBeenGenerated)

    virtual void extractStatsData(VolumeDataContainer::Pointer m, int index);
    virtual int gatherStatsData(VolumeDataContainer::Pointer m);

    virtual QString getComboString();
    virtual QString getTabTitle();

    virtual void on_m_GenerateDefaultData_clicked();

   public slots:


   protected slots:

     virtual void dataWasEdited();

  protected:


  private:


    SGWidget(const SGWidget&); // Copy Constructor Not Implemented
    void operator=(const SGWidget&); // Operator '=' Not Implemented

};

#endif /* SGWIDGET_H_ */
