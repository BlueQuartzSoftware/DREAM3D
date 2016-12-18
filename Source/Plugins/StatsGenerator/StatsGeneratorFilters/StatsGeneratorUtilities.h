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
* The code contained herein was partially funded by the followig contracts:
*    United States Air Force Prime Contract FA8650-07-D-5800
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _statsgeneratorutilities_h_
#define _statsgeneratorutilities_h_

#include <QtCore/QVector>

#include "SIMPLib/Common/PhaseType.h"

class StatsData;


class StatsGeneratorUtilities
{
  public:

    virtual ~StatsGeneratorUtilities();

    /**
     * @brief GenerateODFBinData
     * @param statsData
     * @param phaseType
     * @param e1s
     * @param e2s
     * @param e3s
     * @param weights
     * @param sigmas
     */
    static void GenerateODFBinData(StatsData* statsData, PhaseType::Type phaseType, unsigned int crystalStructure,
                                   QVector<float> &e1s, QVector<float> &e2s,
                                   QVector<float> &e3s, QVector<float> &weights,
                                   QVector<float> &sigmas, bool computeODF = true);

    static void GenerateAxisODFBinData(StatsData* statsData, PhaseType::Type phaseType,
                                       QVector<float> &e1s, QVector<float> &e2s,
                                       QVector<float> &e3s, QVector<float> &weights,
                                       QVector<float> &sigmas, bool computeAxisODF = true);

    static QVector<float> GenerateODFData(unsigned int crystalStructure,
                                                 QVector<float> &e1s, QVector<float> &e2s,
                                                 QVector<float> &e3s, QVector<float> &weights,
                                                 QVector<float> &sigmas, bool computeODF = true);

    static void GenerateMisorientationBinData(StatsData* statsData,
                                              PhaseType::Type phaseType, unsigned int crystalStruct,
                                              QVector<float>& odf,
                                              QVector<float>& angles, QVector<float>& axes,
                                              QVector<float>& weights, bool computeMDF = true);


  protected:
    StatsGeneratorUtilities();

  private:
    StatsGeneratorUtilities(const StatsGeneratorUtilities&); // Copy Constructor Not Implemented
    void operator=(const StatsGeneratorUtilities&); // Operator '=' Not Implemented

};


#endif /* _StatsGeneratorUtilities_h_ */

