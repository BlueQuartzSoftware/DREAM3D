/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef GRAINGENERATOR_H_
#define GRAINGENERATOR_H_


#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractPipeline.h"
#include "DREAM3DLib/Common/Observer.h"


#include "DREAM3DLib/Common/DataContainer.h"

/**
* @class GrainGenerator GrainGenerator.h AIM/GrainGenerator/GrainGenerator.h
* @brief This class serves as the main entry point to execute the Synthetic Grain
* generation.
* @author Michael A. Jackson for BlueQuartz Software
* @author Dr. Michael Groeber, USAFRL
* @date Nov 3, 2009
* @version 1.0
*/
class GrainGenerator : public AbstractPipeline, public Observer
{
  public:
    DREAM3D_SHARED_POINTERS(GrainGenerator);
    DREAM3D_TYPE_MACRO_SUPER(GrainGenerator, Observer);

    DREAM3D_STATIC_NEW_MACRO(GrainGenerator);

    virtual ~GrainGenerator();


    DREAM3D_INSTANCE_STRING_PROPERTY(H5StatsFile)
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputDirectory)
    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFilePrefix)
    DREAM3D_INSTANCE_PROPERTY(int, XPoints)
    DREAM3D_INSTANCE_PROPERTY(int, YPoints)
    DREAM3D_INSTANCE_PROPERTY(int, ZPoints)

    DREAM3D_INSTANCE_PROPERTY(std::vector<DREAM3D::SyntheticBuilder::ShapeType>, ShapeTypes)

    DREAM3D_INSTANCE_PROPERTY(double, XResolution)
    DREAM3D_INSTANCE_PROPERTY(double, YResolution)
    DREAM3D_INSTANCE_PROPERTY(double, ZResolution)
    DREAM3D_INSTANCE_PROPERTY(double, NeighborhoodErrorWeight)
    DREAM3D_INSTANCE_PROPERTY(double, FractionPrecipitates)

    DREAM3D_INSTANCE_PROPERTY(bool, PeriodicBoundary)
    DREAM3D_INSTANCE_STRING_PROPERTY(StructureFile)
    DREAM3D_INSTANCE_PROPERTY(bool, AlreadyFormed)

    DREAM3D_INSTANCE_PROPERTY(int, Precipitates)

    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryVTKFiles)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteVtkFile)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteSurfaceVoxel)
    DREAM3D_INSTANCE_PROPERTY(bool, WritePhaseId)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteIPFColor)

    DREAM3D_INSTANCE_PROPERTY(bool, WriteHDF5GrainFile)


    /**
    * @brief Main method to run the operation
    */
    virtual void execute();

  protected:
    GrainGenerator();

  private:
    DataContainer::Pointer m;

    GrainGenerator(const GrainGenerator&);    // Copy Constructor Not Implemented
    void operator=(const GrainGenerator&);  // Operator '=' Not Implemented
};



#endif /* GRAINGENERATOR_H_ */
