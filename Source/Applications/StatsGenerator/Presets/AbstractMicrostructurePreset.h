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

#ifndef MICROSTRUCTUREPRESET_H_
#define MICROSTRUCTUREPRESET_H_

#include "QtCore/QVector"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"

class StatsGenPlotWidget;
class StatsGenODFWidget;
class SGAxisODFWidget;
class StatsGenMDFWidget;

/**
 * @class AbstractMicrostructurePreset AbstractMicrostructurePreset.h StatsGenerator/Presets/AbstractMicrostructurePreset.h
 * @brief This class is the superclass for Microstructure Preset classes. It contains both
 * pure virtual and virtual methods. For those programmers wanting to create new
 * presets to make available to the end user they need to fully implement the methods
 * that they want to setup specific presets for. Each method has a default implementation
 * that does nothing.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date May 23, 2011
 * @version 1.0
 */
class AbstractMicrostructurePreset
{
  public:
    DREAM3D_SHARED_POINTERS(AbstractMicrostructurePreset)
    DREAM3D_TYPE_MACRO(AbstractMicrostructurePreset)
    virtual ~AbstractMicrostructurePreset();


    /**
     * @brief Displays a Dialog Box which gives the user a chance to customize
     * the preset with specific values. Although subclasses MUST implement the
     * method, nothing actually has to happen, Ie, one can simply have an empty
     * implementation if there are no customizations to be performed.
     */
    virtual void displayUserInputDialog() {}

    /**
     * @brief Initializes the Omega3 preset table values
     * @param tableModel The TableModel that will receive the preset values
     * @param binNumbers The array of Bin values which are needed for the table model
     */
    virtual void initializeOmega3TableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers) = 0;

    /**
     * @brief Initializes the B Over A preset table values
     * @param tableModel The TableModel that will receive the preset values
     * @param binNumbers The array of Bin values which are needed for the table model
     */
    virtual void initializeBOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers) = 0;

    /**
     * @brief Initializes the C Over A preset table values
     * @param tableModel The TableModel that will receive the preset values
     * @param binNumbers The array of Bin values which are needed for the table model
     */
    virtual void initializeCOverATableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers) = 0;

    /**
     * @brief Initializes the Neighbor preset table values
     * @param tableModel The TableModel that will receive the preset values
     * @param binNumbers The array of Bin values which are needed for the table model
     */
    virtual void initializeNeighborTableModel(StatsGenPlotWidget* plot, QVector<float> binNumbers) = 0;

    /**
     * @brief This method initializes the AxisODF Table with preset Euler angles,
     * Weights and Spreads.
     * @param widget Non Null SGAxisODFWidget
     */
    virtual void initializeAxisODFTableModel(SGAxisODFWidget* widget);

    /**
     * @brief This method initializes the ODF Table with preset Euler angles,
     * Weights and Spreads.
     * @param widget
     */
    virtual void initializeODFTableModel(StatsGenODFWidget* widget);

    /**
     * @brief This method initializes the MDF Table with preset Euler angles,
     * Weights and Spreads.
     * @param widget
     */
    virtual void initializeMDFTableModel(StatsGenMDFWidget* widget);

  protected:
    AbstractMicrostructurePreset();

  private:
    AbstractMicrostructurePreset(const AbstractMicrostructurePreset&); // Copy Constructor Not Implemented
    void operator=(const AbstractMicrostructurePreset&); // Operator '=' Not Implemented
};

#endif /* MICROSTRUCTUREPRESET_H_ */
