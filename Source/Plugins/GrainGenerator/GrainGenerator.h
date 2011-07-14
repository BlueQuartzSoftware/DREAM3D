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

#ifndef GRAINGENERATOR_H_
#define GRAINGENERATOR_H_

#include <MXA/Common/MXASetGetMacros.h>
#include <MXA/MXATypes.h>

#include "DREAM3D/DREAM3DConfiguration.h"
#include "DREAM3D/Common/Constants.h"
#include "DREAM3D/Common/AbstractPipeline.h"
#include "GrainGeneratorFunc.h"

/**
* @class GrainGenerator GrainGenerator.h AIM/GrainGenerator/GrainGenerator.h
* @brief This class serves as the main entry point to execute the Synthetic Grain
* generation.
* @author Michael A. Jackson for BlueQuartz Software
* @author Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class DREAM3DLib_EXPORT GrainGenerator : public AbstractPipeline
{
  public:
    MXA_SHARED_POINTERS(GrainGenerator);
    MXA_TYPE_MACRO(GrainGenerator);

    MXA_STATIC_NEW_MACRO(GrainGenerator);

    virtual ~GrainGenerator();


    MXA_INSTANCE_STRING_PROPERTY(H5StatsFile)
    MXA_INSTANCE_STRING_PROPERTY(OutputDirectory)
    MXA_INSTANCE_STRING_PROPERTY(OutputFilePrefix)
    MXA_INSTANCE_PROPERTY(int, XPoints)
    MXA_INSTANCE_PROPERTY(int, YPoints)
    MXA_INSTANCE_PROPERTY(int, ZPoints)

    MXA_INSTANCE_PROPERTY(std::vector<AIM::SyntheticBuilder::ShapeType>, ShapeTypes)

    MXA_INSTANCE_PROPERTY(double, XResolution)
    MXA_INSTANCE_PROPERTY(double, YResolution)
    MXA_INSTANCE_PROPERTY(double, ZResolution)
    MXA_INSTANCE_PROPERTY(double, FillingErrorWeight)
    MXA_INSTANCE_PROPERTY(double, NeighborhoodErrorWeight)
    MXA_INSTANCE_PROPERTY(double, SizeDistErrorWeight)
    MXA_INSTANCE_PROPERTY(double, FractionPrecipitates)

    MXA_INSTANCE_PROPERTY(bool, PeriodicBoundary)
    MXA_INSTANCE_PROPERTY(bool, WriteGrainData)
    MXA_INSTANCE_STRING_PROPERTY(StructureFile)
    MXA_INSTANCE_PROPERTY(bool, AlreadyFormed)

    MXA_INSTANCE_PROPERTY(int, Precipitates)

    MXA_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)
    MXA_INSTANCE_PROPERTY(bool, WriteVtkFile)
    MXA_INSTANCE_PROPERTY(bool, WriteSurfaceVoxel)
    MXA_INSTANCE_PROPERTY(bool, WritePhaseId)
    MXA_INSTANCE_PROPERTY(bool, WriteIPFColor)

    MXA_INSTANCE_PROPERTY(bool, WritePhFile)

    MXA_INSTANCE_PROPERTY(bool, WriteHDF5GrainFile)


    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    GrainGenerator();

  private:
    GrainGeneratorFunc::Pointer m;

    GrainGenerator(const GrainGenerator&);    // Copy Constructor Not Implemented
    void operator=(const GrainGenerator&);  // Operator '=' Not Implemented
};



#endif /* GRAINGENERATOR_H_ */
