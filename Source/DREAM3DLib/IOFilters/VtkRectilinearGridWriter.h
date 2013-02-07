/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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

#ifndef VTKRECTILINEARGRIDWRITER_H_
#define VTKRECTILINEARGRIDWRITER_H_

#include <string>

#include "EbsdLib/EbsdConstants.h"
#include "EbsdLib/TSL/AngConstants.h"
#include "EbsdLib/HKL/CtfConstants.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/Common/VoxelDataContainer.h"
#include "DREAM3DLib/VTKUtils/VTKFileWriters.hpp"


/**
 * @class VTKRectilinearGridFileWriter VTKRectilinearGridFileWriter.h DREAM3D/Common/VTKUtils/VTKRectilinearGridFileWriter.h
 * @brief This is the main class to call when you want to write voxel based data
 * to a Rectilinear Grid based VTK data set. In order to write Scalar attributes
 * one needs to also include a vector of VtkScalarWriter objects. Each VtkScalarWriter
 * subclass knows how to write a specific scalar array to the file. When implementing
 * those subclasses keep in mind that you should be able to write both the ASCII
 * and Binary versions of your data to the VTK File.
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jun 13, 2011
 * @version 1.0
 */
class DREAM3DLib_EXPORT VtkRectilinearGridWriter : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(VtkRectilinearGridWriter)
    DREAM3D_STATIC_NEW_MACRO(VtkRectilinearGridWriter)
    DREAM3D_TYPE_MACRO_SUPER(VtkRectilinearGridWriter, AbstractFilter)

    virtual ~VtkRectilinearGridWriter();

    //------ Required Cell Data
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ParentIdsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellPhasesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GoodVoxelsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GlobAlphaArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(BCArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ConfidenceIndexArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(ImageQualityArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainReferenceRotationsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainReferenceMisorientationsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GrainReferenceCAxisMisorientationsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(KernelAverageMisorientationsArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(GBEuclideanDistancesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(TJEuclideanDistancesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(QPEuclideanDistancesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(CellEulerAnglesArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(EquivalentDiametersArrayName)
    DREAM3D_INSTANCE_STRING_PROPERTY(SchmidsArrayName)

    DREAM3D_INSTANCE_STRING_PROPERTY(OutputFile)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGrainIds)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteParentIds)
    DREAM3D_INSTANCE_PROPERTY(bool, WritePhaseIds)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteBandContrasts)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteConfidenceIndicies)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteImageQualities)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGoodVoxels)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGlobAlpha)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteRodriguesGAMColors)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGrainReferenceMisorientations)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGrainReferenceCAxisMisorientations)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteKernelAverageMisorientations)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteIPFColors)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGBEuclideanDistanceMap)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteTJEuclideanDistanceMap)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteQPEuclideanDistanceMap)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteSchmidFactors)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteGrainSizes)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteEulerAngles)
    DREAM3D_INSTANCE_PROPERTY(bool, WriteBinaryFile)


    virtual void preflight();

    virtual const std::string getGroupName() { return DREAM3D::FilterGroups::IOFilters; }
    virtual const std::string getHumanLabel() { return "Write Vtk File (Rectilinear Grid)"; }

    virtual void setupFilterParameters();
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();


  protected:
    VtkRectilinearGridWriter();

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);


  private:
    int32_t* m_GrainIds;
    int32_t* m_ParentIds;
    int32_t* m_CellPhases;
    int32_t* m_BC;
    int32_t* m_GlobAlpha;
    float* m_ImageQuality;
  float* m_ConfidenceIndex;
    bool*    m_GoodVoxels;
    float*   m_CellEulerAngles;
    float*   m_GBEuclideanDistances;
    float*   m_TJEuclideanDistances;
    float*   m_QPEuclideanDistances;
    float*   m_GrainReferenceRotations;
    float*   m_GrainReferenceMisorientations;
    float*   m_GrainReferenceCAxisMisorientations;
    float*   m_KernelAverageMisorientations;
    float*   m_EquivalentDiameters;
    float*   m_Schmids;


    VtkRectilinearGridWriter(const VtkRectilinearGridWriter&); // Copy Constructor Not Implemented
    void operator=(const VtkRectilinearGridWriter&); // Operator '=' Not Implemented

    int write(const std::string &file, VoxelDataContainer* r, std::vector<VtkScalarWriter*> &scalars);

    /**
     * @brief This function writes a set of Axis coordinates to that are needed
     * for a Rectilinear Grid based data set.
     * @param f The "C" FILE* pointer to the file being written to.
     * @param axis The name of the Axis that is being written
     * @param type The type of primitive being written (float, int, ...)
     * @param npoints The total number of points in the array
     * @param min The minimum value of the axis
     * @param max The maximum value of the axis
     * @param step The step value between each point on the axis.
     */
    template<typename T>
    int writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, T min, T max, T step)
    {
      int err = 0;
      fprintf(f, "%s %lld %s\n", axis, npoints, type);
      if (m_WriteBinaryFile == true)
      {
        T* data = new T[npoints];
        T d;
        for (int idx = 0; idx < npoints; ++idx)
        {
          d = idx * step + min;
          MXA::Endian::FromSystemToBig::convert<T>(d);
          data[idx] = d;
        }
        size_t totalWritten = fwrite(static_cast<void*>(data), sizeof(T), static_cast<size_t>(npoints), f);
        delete[] data;
        if (totalWritten != static_cast<size_t>(npoints) )
        {
          std::cout << "Error Writing Binary VTK Data into file " << std::endl;
          fclose(f);
          return -1;
        }

      }
      else
      {
        T d;
        for (int idx = 0; idx < npoints; ++idx)
        {
          d = idx * step + min;
          fprintf(f, "%f ", d);
          if (idx % 20 == 0 && idx != 0) { fprintf(f, "\n"); }
        }
        fprintf(f, "\n");
      }
      return err;
    }


};

#endif /* VTKRECTILINEARGRIDWRITER_H_ */
