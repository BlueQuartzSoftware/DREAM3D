/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Grober (US Air Force Research Laboratories
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

#ifndef _MICROSTRUCTURE_STATISTICS_H_
#define _MICROSTRUCTURE_STATISTICS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN   // Exclude rarely-used stuff from Windows headers
#endif


#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>

#include "DREAM3D/Common/Constants.h"
#include "MicrostructureStatisticsFunc.h"
#include "DREAM3D/Common/AbstractPipeline.h"

/**
* @class MicrostructureStatistics MicrostructureStatistics AIM/MicrostructureStatistics.h/MicrostructureStatistics.h
* @brief This class serves as the main entry point to execute the Microstructure Statistics codes
*
* @author Michael A. Jackson for BlueQuartz Software
* @author Dr. Michael Groeber, USAFRL
* @date May 30, 2011
* @version 1.0
*/
class DREAM3DLib_EXPORT MicrostructureStatistics  : public AbstractPipeline
{

  public:
    MXA_SHARED_POINTERS(MicrostructureStatistics);
    MXA_TYPE_MACRO(MicrostructureStatistics);
    MXA_STATIC_NEW_MACRO(MicrostructureStatistics);

    virtual ~MicrostructureStatistics();

    MXA_INSTANCE_STRING_PROPERTY(InputFile)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)

    // Options
    MXA_INSTANCE_PROPERTY(float, BinStepSize);
    MXA_INSTANCE_PROPERTY(bool, WriteH5StatsFile)

    // Compute Attributes
    MXA_INSTANCE_PROPERTY(bool, ComputeGrainSize);
    MXA_INSTANCE_PROPERTY(bool, ComputeGrainShapes);
    MXA_INSTANCE_PROPERTY(bool, ComputeNumNeighbors);
    MXA_INSTANCE_PROPERTY(bool, ComputeODF);
    MXA_INSTANCE_PROPERTY(bool, ComputeMDF);

    // Grain Data File Related
    MXA_INSTANCE_PROPERTY(bool, WriteGrainFile)
    MXA_INSTANCE_PROPERTY(bool, WriteGrainSize);
    MXA_INSTANCE_PROPERTY(bool, WriteGrainShapes);
    MXA_INSTANCE_PROPERTY(bool, WriteNumNeighbors);
    MXA_INSTANCE_PROPERTY(bool, WriteAverageOrientations);

    // VTK File Related
    MXA_INSTANCE_PROPERTY(bool, WriteVtkFile)
    MXA_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)
    MXA_INSTANCE_PROPERTY(bool, WritePhaseId)
    MXA_INSTANCE_PROPERTY(bool, WriteIPFColor)
    MXA_INSTANCE_PROPERTY(bool, WriteKernelMisorientations)
    MXA_INSTANCE_PROPERTY(bool, WriteSurfaceVoxel)


    /**
    * @brief
    * @param ostream
    */
    void printSettings(std::ostream &ostream);

    /**
     * @brief Main method to run the operation
     */
    void execute();

  protected:

    MicrostructureStatistics();


  private:
    MicrostructureStatisticsFunc::Pointer m;
    std::map<std::string, bool> m_CalcStats;


    MicrostructureStatistics(const MicrostructureStatistics&);    // Copy Constructor Not Implemented
    void operator=(const MicrostructureStatistics&);  // Operator '=' Not Implemented
};

#endif /* _MICROSTRUCTURE_STATISTICS_H_ */
