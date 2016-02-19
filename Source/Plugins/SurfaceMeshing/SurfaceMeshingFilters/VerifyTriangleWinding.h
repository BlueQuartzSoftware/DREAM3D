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
*    United States Air Force Prime Contract FA8650-10-D-5210
*    United States Prime Contract Navy N00173-07-C-2068
*
* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */


#ifndef _verifytrianglewinding_h_
#define _verifytrianglewinding_h_

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/IDataArray.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"
#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @class VerifyTriangleWinding VerifyTriangleWinding.h /SurfaceMeshFilters/VerifyTriangleWinding.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class VerifyTriangleWinding : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(VerifyTriangleWinding)
    SIMPL_STATIC_NEW_MACRO(VerifyTriangleWinding)
    SIMPL_TYPE_MACRO_SUPER(VerifyTriangleWinding, SurfaceMeshFilter)

    virtual ~VerifyTriangleWinding();
    SIMPL_INSTANCE_STRING_PROPERTY(SurfaceDataContainerName)

    SIMPL_INSTANCE_STRING_PROPERTY(SurfaceMeshNodeFacesArrayName)

    typedef QMap<int, QSet<int> >                      LabelFaceMap_t;
    typedef QVector<int32_t>                               FaceList_t;

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshFaceLabelsArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshFaceLabelsArrayPath READ getSurfaceMeshFaceLabelsArrayPath WRITE setSurfaceMeshFaceLabelsArrayPath)

    virtual const QString getCompiledLibraryName();
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);
    virtual const QString getGroupName();
    virtual const QString getSubGroupName();

    /**
     * @brief This returns a string that is displayed in the GUI. It should be readable
     * and understandable by humans.
     */
    virtual const QString getHumanLabel();

    /**
    * @brief This method will instantiate all the end user settable options/parameters
    * for this filter
    */
    virtual void setupFilterParameters();

    /**
    * @brief This method will write the options to a file
    * @param writer The writer that is used to write the options to a file
    */
    virtual int writeFilterParameters(AbstractFilterParametersWriter* writer, int index);

    /**
    * @brief This method will read the options from a file
    * @param reader The reader that is used to read the options from a file
    */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
    * @brief Reimplemented from @see AbstractFilter class
    */
    virtual void execute();

    /**
    * @brief This function runs some sanity checks on the DataContainer and inputs
    * in an attempt to ensure the filter can process the inputs.
    */
    virtual void preflight();

  signals:
    void updateFilterParameters(AbstractFilter* filter);
    void parametersChanged();
    void preflightAboutToExecute();
    void preflightExecuted();

  protected:
    VerifyTriangleWinding();

    /**
    * @brief Checks for the appropriate parameter values and availability of
    * arrays in the data container
    * @param preflight
    * @param voxels The number of voxels
    * @param features The number of features
    * @param ensembles The number of ensembles
    */
    void dataCheck();

    /**
     * @brief This method generates a mapping between a "Label" (Feature Id) and the set of triangles that belong to that
     * lable value.
     * @param trianglesToLabelMap
     */
    void getLabelTriangelMap(LabelFaceMap_t& trianglesToLabelMap);

    /**
     * @brief This method verifies the winding of all the triangles and makes them consistent
     * @return
     */
    int verifyTriangleWinding();

    /**
     * @brief VerifyTriangleWinding::getSeedTriangle
     * @param label
     * @param triangleIndices
     * @return
     */
    int32_t getSeedTriangle(int32_t label, QSet<int32_t>& triangleIndices);

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, SurfaceMeshFaceLabels)

    bool m_DoNodeFaceConnectivityFilter;

    VerifyTriangleWinding(const VerifyTriangleWinding&); // Copy Constructor Not Implemented
    void operator=(const VerifyTriangleWinding&); // Operator '=' Not Implemented
};

#endif /* _VerifyTriangleWinding_H_ */



