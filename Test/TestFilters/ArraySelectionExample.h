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
#ifndef _ArraySelectionExample_H_
#define _ArraySelectionExample_H_

#include <string>

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/IDataArray.h"
#include "DREAM3DLib/Common/AbstractFilter.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
/*
 * Look at Generic/RemoveArrays class for code on how to write the values to the .dream3d file.
 */


/**
 * @class ArraySelectionExample ArraySelectionExample.h ExamplePlugin/Code/ExamplePluginFilters/ArraySelectionExample.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class ArraySelectionExample : public AbstractFilter
{
  public:
    DREAM3D_SHARED_POINTERS(ArraySelectionExample);
    DREAM3D_STATIC_NEW_MACRO(ArraySelectionExample);
    DREAM3D_TYPE_MACRO_SUPER(ArraySelectionExample, AbstractFilter);

    virtual ~ArraySelectionExample();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */

    // -----------------------------------------------------------------------------
    /* These methods are needed if you use the ArraySelectionWidget as an input */
    typedef std::set<std::string> ArrayList_t;

    virtual void setVoxelSelectedArrayNames(std::set<std::string> selectedCellArrays,
                                            std::set<std::string> selectedFieldArrays,
                                            std::set<std::string> selectedEnsembleArrays);
    virtual void setSurfaceMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                  std::set<std::string> selectedFaceArrays,
                                                  std::set<std::string> selectedEdgeArrays,
                                                  std::set<std::string> selectedFieldArrays,
                                                  std::set<std::string> selectedEnsembleArrays);
    virtual void setSolidMeshSelectedArrayNames(std::set<std::string> selectedVertexArrays,
                                                std::set<std::string> selectedFaceArrays,
                                                std::set<std::string> selectedEdgeArrays);

      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedVoxelCellArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedVoxelFieldArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedVoxelEnsembleArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshVertexArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshFaceArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshEdgeArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshFieldArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSurfaceMeshEnsembleArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSolidMeshVertexArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSolidMeshFaceArrays)
      DREAM3D_INSTANCE_PROPERTY(std::set<std::string>, SelectedSolidMeshEdgeArrays)

  public:

    // -----------------------------------------------------------------------------
    /* Each Filter can ONLY have ONE of these types of widgets */
    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, CellComparisonInputs)
    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, FieldComparisonInputs)
    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, EnsembleComparisonInputs)

    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, PointComparisonInputs)
    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, FaceComparisonInputs)
    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, EdgeComparisonInputs)

    /* And if you have one of those Widgets then you need this to support that widget */
    DREAM3D_INSTANCE_PROPERTY(std::vector<ComparisonInput_t>, ComparisonInputs)
    // -----------------------------------------------------------------------------


    DREAM3D_INSTANCE_PROPERTY(IntVec3Widget_t, Dimensions)
    DREAM3D_INSTANCE_PROPERTY(FloatVec3Widget_t, Origin)



    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    virtual const std::string getGroupName() { return "ExamplePlugin"; }

    /**
    * @brief This returns a string that is displayed in the GUI. It should be readable
    * and understandable by humans.
    */
    virtual const std::string getHumanLabel() { return "ArraySelectionExample"; }

    /**
    * @brief This returns a string that is displayed in the GUI and helps to sort the filters into
    * a subgroup. It should be readable and understandable by humans.
    */
    virtual const std::string getSubGroupName() { return "Misc"; }

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual void writeFilterParameters(AbstractFilterParametersWriter* writer);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader);

   /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  protected:
    ArraySelectionExample();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param fields The number of fields
    * @param ensembles The number of ensembles
    */
    void dataCheck(bool preflight, size_t voxels, size_t fields, size_t ensembles);

  private:

    ArraySelectionExample(const ArraySelectionExample&); // Copy Constructor Not Implemented
    void operator=(const ArraySelectionExample&); // Operator '=' Not Implemented
};

#endif /* _ArraySelectionExample_H_ */
