/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2010, Michael A. Groeber (US Air Force Research Laboratory)
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _H5GRAINWRITER_H_
#define _H5GRAINWRITER_H_

#include <vector>

#include "MXA/Common/MXASetGetMacros.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/EbsdColoring.hpp"
#include "DREAM3DLib/VTKUtils/VTKWriterMacros.h"
#include "DREAM3DLib/HDF5/VTKH5Constants.h"
#include "DREAM3DLib/HDF5/AIM_H5VtkDataWriter.h"



#define H5GW_DECLS() \
    int err = -1;\
  AIM_H5VtkDataWriter::Pointer h5writer = AIM_H5VtkDataWriter::New();\
  h5writer->setFileName(hdfFile);\
  err = h5writer->openFile(false);\
  std::stringstream ss;\
  std::string hdfPath;\
  std::vector<std::string > hdfPaths;\
  int numgrains = r->getTotalFields();\
  int totpoints = r->totalPoints();\
  int phase;\
  int pcount = 0;\
  unsigned char rgb[3] =  { 0, 0, 0 };\
  unsigned char hkl[3] = { 0, 0, 0 };\
  VTK_IPF_COLOR_REFDIRECTION(RefDirection);\
  int ocol, orow, oplane;\
  int col, row, plane;\
  int pid;\


#define H5GW_GRAIN_LOOP_1() \
    std::vector<int > vlist = vlists[i];\
    if (vlist.size() == 0) { continue; }\
    int vid = vlist[0];\
    ss.str("");\
    ss << "/" << i;\
    hdfPath = ss.str();\
    hdfPaths.push_back(hdfPath);\
    std::vector<float > points;\
    std::vector<int32_t > cells(vlist.size() * 9);\
    std::vector<int32_t > cell_types(vlist.size(), VTK_CELLTYPE_VOXEL);\
    std::vector<int32_t > grainName(vlist.size());\
    std::vector<unsigned char > ipfColor(vlist.size() * 3);\
    std::vector<int32_t>  phaseValues(vlist.size());\
    std::map<int, int> pointMap;\
    err = 0;\
    pcount = 0;\
    size_t cIdx = 0;

#define H5GW_VLIST_LOOP_1()\
vid = vlist[j];\
ocol = vid % r->getXPoints();\
orow = (vid / r->getXPoints()) % r->getYPoints();\
oplane = vid / (r->getXPoints() * r->getYPoints());\
cells[cIdx] = 8;\
++cIdx;\
for (int k = 0; k < 8; k++) {\
  if (k == 0) col = ocol, row = orow, plane = oplane;\
  if (k == 1) col = ocol + 1, row = orow, plane = oplane;\
  if (k == 2) col = ocol, row = orow + 1, plane = oplane;\
  if (k == 3) col = ocol + 1, row = orow + 1, plane = oplane;\
  if (k == 4) col = ocol, row = orow, plane = oplane + 1;\
  if (k == 5) col = ocol + 1, row = orow, plane = oplane + 1;\
  if (k == 6) col = ocol, row = orow + 1, plane = oplane + 1;\
  if (k == 7) col = ocol + 1, row = orow + 1, plane = oplane + 1;\
  pid = (plane * (r->getXPoints() + 1) * (r->getYPoints() + 1)) + (row * (r->getXPoints() + 1)) + col;\
  if (pointMap.find(pid) == pointMap.end())  {\
    pointMap[pid] = pcount;\
    pcount++;\
    points.push_back((col * r->getXRes()));\
    points.push_back((row * r->getYRes()));\
    points.push_back((plane * r->getZRes()));\
  }\
  cells[cIdx] = pointMap[pid];\
  ++cIdx;\
}


#define H5GW_GRAIN_LOOP_2(size) \
err = h5writer->writeUnstructuredGrid(hdfPath, points, cells, cell_types);\
err = h5writer->writeFieldData<int> (hdfPath, grainName, DREAM3D::HDF5::Grain_ID.c_str(), 1);\
if (size > 0) {\
}\
err = h5writer->writeCellData<int> (hdfPath, grainName, DREAM3D::HDF5::Grain_ID.c_str(), 1);\
err = h5writer->writeCellData<unsigned char> (hdfPath, ipfColor, DREAM3D::HDF5::IPFColor.c_str(), 3);\
err = h5writer->writeCellData<int32_t> (hdfPath, phaseValues, DREAM3D::HDF5::Phase.c_str(), 1);





/**
 * @class H5GrainWriter H5GrainWriter.h DREAM3DLib/HDF5/H5GrainWriter.h
 * @brief This class will write a .h5grain file from data provided by the
 * ReconstructionFunc class
 * @author Michael A. Jackson for BlueQuartz Software
 * @date Jan 19, 2011
 * @version 1.0
 */
class  H5GrainWriter
{
  public:
    MXA_SHARED_POINTERS(H5GrainWriter)
    MXA_STATIC_NEW_MACRO(H5GrainWriter)
    MXA_TYPE_MACRO(H5GrainWriter)

    virtual ~H5GrainWriter(){};

    /**
     * @brief
     * @param hdfFile
     * @return
     */
    template<typename DataContainer>
    int writeHDF5GrainsFile(DataContainer* r, const std::string &hdfFile)
    {
      int64_t totalPoints = r->totalPoints();

      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(r, Voxel, DREAM3D::VoxelData::GrainIds, Int32ArrayType, int32_t, (totalPoints), grain_indicies);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(r, Voxel, DREAM3D::VoxelData::Phases, Int32ArrayType, int32_t, (totalPoints), phases);
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(r, Voxel, DREAM3D::VoxelData::EulerAngles, FloatArrayType, float, (3*totalPoints), eulerangles);
      
      int totalFields = r->getTotalFields();
      GET_NAMED_ARRAY_SIZE_CHK_NOMSG_RET(r, Field, DREAM3D::FieldData::NumNeighbors, Int32ArrayType, int32_t, totalFields, numNeighbors);


      H5GW_DECLS()
      err = 0;
	    std::vector<std::vector<int> > vlists;
	    vlists.resize(numgrains);
	    for (int i = 0; i < totpoints; i++)
	    {
		    vlists[grain_indicies[i]].push_back(i);
	    }
      for (int i = 1; i < numgrains; i++)
      {
        H5GW_GRAIN_LOOP_1()
        for (std::vector<int >::size_type j = 0; j < vlist.size(); j++)
        {
          H5GW_VLIST_LOOP_1()

          phase = phases[vid];
          phaseValues[j] = phase;
          if (r->crystruct[phase] == Ebsd::Cubic)
          {
            EbsdColoring::GenerateIPFColor(eulerangles[3*vid],
                                          eulerangles[3*vid + 1],
                                          eulerangles[3*vid + 2],
                                          RefDirection[0], RefDirection[1], RefDirection[2],
                                          rgb, hkl);
          }
          else if (r->crystruct[phase] == Ebsd::Hexagonal)
          {
            EbsdColoring::CalculateHexIPFColor(eulerangles[3*vid],
											  eulerangles[3*vid + 1],
											  eulerangles[3*vid + 2],
                                              RefDirection[0], RefDirection[1], RefDirection[2], rgb);
          }
          ipfColor[j * 3] = rgb[0];
          ipfColor[j * 3 + 1] = rgb[1];
          ipfColor[j * 3 + 2] = rgb[2];

          // Reconstruction Specific Assignments
          grainName[j] = grain_indicies[vid];
        }

        int numNeighborsForGrain = numNeighbors[i];
        H5GW_GRAIN_LOOP_2(numNeighborsForGrain);
      }

      err = h5writer->writeObjectIndex(hdfPaths);
      err = h5writer->closeFile();
      return err;
    }


  protected:
    H5GrainWriter(){};

  private:
    H5GrainWriter(const H5GrainWriter&); // Copy Constructor Not Implemented
    void operator=(const H5GrainWriter&); // Operator '=' Not Implemented
};

#endif /* _H5GRAINWRITER_H_ */
