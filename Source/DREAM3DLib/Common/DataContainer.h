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
#ifndef _GrainGeneratorFunc_H
#define _GrainGeneratorFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


//-- C++ includes
#include <vector>
#include <map>
#include <sstream>
#include <list>

//-- EBSD Lib Includes
#include "EbsdLib/EbsdConstants.h"

//-- DREAM3D Includes
#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/DataContainerMacros.h"
#include "DREAM3DLib/Common/Constants.h"
#include "DREAM3DLib/Common/DataArray.hpp"
#include "DREAM3DLib/Common/Field.h"
#include "DREAM3DLib/Common/Observable.h"

namespace DREAM3D
{
  namespace VoxelData
  {
    const std::string GrainIds("GrainIds");
    const std::string Phases("Phases");
    const std::string EulerAngles("EulerAngles");
    const std::string SurfaceVoxels("SurfaceVoxels");
    const std::string Neighbors("Neighbors");
    const std::string Quats("Quats");
    const std::string AlreadyChecked("AlreadyChecked");
    const std::string GoodVoxels("GoodVoxels");
    const std::string NearestNeighbors("NearestNeighbors");
    const std::string NearestNeighborDistances("NearestNeighborDistances");
    const std::string GrainMisorientations("GrainMisorientations");
    const std::string MisorientationGradients("MisorientationGradients");
    const std::string KernelAverageMisorientations("KernelAverageMisorientations");
    const std::string ImageQuality("ImageQuality");
  }

  namespace FieldData
  {
    const std::string Active("Active");
    const std::string Phases("Phases");
    const std::string EulerAngles("EulerAngles");
    const std::string AxisEulerAngles("AxisEulerAngles");
    const std::string SurfaceFields("SurfaceFields");
    const std::string UnbiasedFields("UnbiasedFields");
    const std::string NumNeighbors("NumNeighbors");
    const std::string RGBs("RGBs");
    const std::string Centroids("Centroids");
    const std::string NumCells("NumCells");
    const std::string Volumes("Volumes");
    const std::string EquivalentDiameters("EquivalentDiameters");
    const std::string Schmids("Schmids");
    const std::string SlipSystems("SlipSystems");
    const std::string AspectRatios("AspectRatios");
    const std::string AxisLengths("AxisLengths");
    const std::string Omega3s("Omega3s");
    const std::string AvgQuats("AvgQuats");
    const std::string Poles("Poles");
    const std::string Neighborhoods("Neighborhoods");
    const std::string GrainAvgMisorientations("GrainAvgMisorientations");
    const std::string KernelAvgMisorientations("KernelAvgMisorientations");
    const std::string NeighborList("NeighborList");
    const std::string SharedSurfaceAreaList("SharedSurfaceAreaList");
  }

  namespace EnsembleData
  {
    const std::string TotalSurfaceArea("TotalSurfaceArea");
  }
}


/**
 * @class GrainGeneratorFunc GrainGeneratorFunc.h AIM/Common/GrainGeneratorFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT DataContainer : public Observable
{
  public:
    DREAM3D_SHARED_POINTERS(DataContainer)
    DREAM3D_STATIC_NEW_MACRO(DataContainer)
    DREAM3D_TYPE_MACRO_SUPER(DataContainer, Observable);

    virtual ~DataContainer();


    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Voxel)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Field)
    METHOD_DEF_TEMPLATE_INITIALIZEARRAYDATA(Ensemble)


    METHOD_DEF_TEMPLATE_GETARRAYDATA(getVoxelData);
    METHOD_DEF_TEMPLATE_GETARRAYDATA(getFieldData);
    METHOD_DEF_TEMPLATE_GETARRAYDATA(getEnsembleData);

    /* *********** These methods will eventually replace those below **********/

  /**
   * @brief Adds/overwrites the data for a named array
   * @param name The name that the array will be known by
   * @param data The IDataArray::Pointer that will hold the data
   */
    void addVoxelData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getVoxelData(const std::string &name);

    std::list<std::string> getVoxelArrayNameList();

    int getNumVoxelArrays();


    /**
    * @brief Adds/overwrites the data for a named array
    * @param name The name that the array will be known by
    * @param data The IDataArray::Pointer that will hold the data
    */
    void addFieldData(const std::string &name, IDataArray::Pointer data);

    /**
    * @brief Returns the array for a given named array or the equivelant to a
    * null pointer if the name does not exist.
    * @param name The name of the data array
    */
    IDataArray::Pointer getFieldData(const std::string &name);

    std::list<std::string> getFieldArrayNameList();

    int getNumFieldArrays();

    DREAM3D_INSTANCE_PROPERTY(int, TotalFields);


    /**
    * @brief Adds/overwrites the data for a named array
    * @param name The name that the array will be known by
    * @param data The IDataArray::Pointer that will hold the data
    */
    void addEnsembleData(const std::string &name, IDataArray::Pointer data);

    /**
     * @brief Returns the array for a given named array or the equivelant to a
     * null pointer if the name does not exist.
     * @param name The name of the data array
     */
    IDataArray::Pointer getEnsembleData(const std::string &name);

    std::list<std::string> getEnsembleArrayNameList();

    int getNumEnsembleArrays();

    /* ****************** END Map Based Methods *******************************/
    DREAM3D_INSTANCE_VEC3_PROPERTY(size_t, Dimensions);

    size_t getXPoints() { return m_Dimensions[0];}
    size_t getYPoints() { return m_Dimensions[1];}
    size_t getZPoints() { return m_Dimensions[2];}

    int64_t totalPoints() { return (int64_t)m_Dimensions[0] * (int64_t)m_Dimensions[1] * (int64_t)m_Dimensions[2]; }

// -----------------------------------------------------------------------------
//  Resolution Methods
// -----------------------------------------------------------------------------
    DREAM3D_INSTANCE_VEC3_PROPERTY(float, Resolution);

	  float getXRes() { return m_Resolution[0];}
    float getYRes() { return m_Resolution[1];}
    float getZRes() { return m_Resolution[2];}


    // -----------------------------------------------------------------------------
    //  Origin Methods
    // -----------------------------------------------------------------------------
    DREAM3D_INSTANCE_VEC3_PROPERTY(float, Origin);

    // Field Data Pointer Array
    std::vector<Field::Pointer> m_Grains;

    // Ensemble Data??
    // Phase Information (crystal structures, phase types, and shape types)
    std::vector<Ebsd::CrystalStructure> crystruct;
    std::vector<DREAM3D::Reconstruction::PhaseType> phaseType;
    std::vector<float> pptFractions;
    std::vector<DREAM3D::SyntheticBuilder::ShapeType> shapeTypes;
    std::vector<float> phasefraction;


    void resizeFieldDataArrays(size_t size)
    {
      
      for(std::map<std::string, IDataArray::Pointer>::iterator iter = m_FieldData.begin(); iter != m_FieldData.end(); ++iter)
      {
        IDataArray::Pointer d = (*iter).second;
        d->Resize(size);
      }

    }




  protected:
    DataContainer();

  private:

    std::map<std::string, IDataArray::Pointer> m_VoxelData;
    std::map<std::string, IDataArray::Pointer> m_FieldData;
    std::map<std::string, IDataArray::Pointer> m_EnsembleData;

    DataContainer(const DataContainer&);
    void operator =(const DataContainer&);
};

#endif
