/* ============================================================================
 * Copyright (c) 2011 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2011 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
#ifndef DecimateSolidMesh_H_
#define DecimateSolidMesh_H_

#include <vector>
#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/IDataArray.h"

#include "DREAM3DLib/Common/AbstractFilter.h"
#include "DREAM3DLib/DataContainers/VoxelDataContainer.h"
#include "DREAM3DLib/Common/SolidMeshStructs.h"

/*
 *
 */
class DREAM3DLib_EXPORT DecimateSolidMesh : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(DecimateSolidMesh)
     DREAM3D_STATIC_NEW_MACRO(DecimateSolidMesh)
     DREAM3D_TYPE_MACRO_SUPER(DecimateSolidMesh, AbstractFilter)

     virtual ~DecimateSolidMesh();

    DREAM3D_INSTANCE_PROPERTY(int, GoalElementNumber)

		virtual const std::string getGroupName() { return DREAM3D::FilterGroups::SolidMeshingFilters; }
     virtual const std::string getHumanLabel() { return "Decimate Solid Mesh"; }

    virtual void setupFilterParameters();
	virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);
    
    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

	virtual void execute();
     virtual void preflight();

   protected:
     DecimateSolidMesh();

   private:
	std::vector<int> newNodeIds;
	std::vector<bool> tetDone;
	std::vector<int> nodeGrainIds;
	std::vector<int> nodeEuclideanDistances;

	int updateNodesandTets(int currentTet, int killedNode, int newNode);

    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

    DecimateSolidMesh(const DecimateSolidMesh&); // Copy Constructor Not Implemented
    void operator=(const DecimateSolidMesh&); // Operator '=' Not Implemented

};

#endif /* DecimateSolidMesh_H_ */
