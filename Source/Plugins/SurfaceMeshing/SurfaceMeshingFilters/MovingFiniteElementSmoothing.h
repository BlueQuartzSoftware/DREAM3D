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

#ifndef _movingfiniteelementsmoothing_h_
#define _movingfiniteelementsmoothing_h_

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"

/**
 * @class MovingFiniteElementSmoothing MovingFiniteElementSmoothing.h /Code/Filters/MovingFiniteElementSmoothing.h
 * @brief The Moving Finite Element (MFE) algorithm is based on Kuprat's work on Gradient Weighted Moving
 * Finite Element method [SIAM].  A conformal triangular surface mesh represents the feature
 *  boundary network.  The curvature at each node is computed and used as the driving
 *  force for nodal motion. Additional forces are computed based on element
 *  quality. The user can choose a balance between the curvature and element quality
 *  forces to find a compromise between smoothness and final element quality.  Certain
 *  node types can be constrained in their motion.
 * @author
 * @date
 * @version 1.0
 */
class MovingFiniteElementSmoothing : public SurfaceMeshFilter
{
    Q_OBJECT /* Need this for Qt's signals and slots mechanism to work */
  public:
    SIMPL_SHARED_POINTERS(MovingFiniteElementSmoothing)
    SIMPL_STATIC_NEW_MACRO(MovingFiniteElementSmoothing)
    SIMPL_TYPE_MACRO_SUPER(MovingFiniteElementSmoothing, SurfaceMeshFilter)

    virtual ~MovingFiniteElementSmoothing();

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    SIMPL_FILTER_PARAMETER(int, IterationSteps)
    Q_PROPERTY(int IterationSteps READ getIterationSteps WRITE setIterationSteps)
    SIMPL_FILTER_PARAMETER(bool, NodeConstraints)
    Q_PROPERTY(bool NodeConstraints READ getNodeConstraints WRITE setNodeConstraints)
    SIMPL_FILTER_PARAMETER(bool, ConstrainSurfaceNodes)
    Q_PROPERTY(bool ConstrainSurfaceNodes READ getConstrainSurfaceNodes WRITE setConstrainSurfaceNodes)
    SIMPL_FILTER_PARAMETER(bool, ConstrainQuadPoints)
    Q_PROPERTY(bool ConstrainQuadPoints READ getConstrainQuadPoints WRITE setConstrainQuadPoints)
    SIMPL_FILTER_PARAMETER(bool, SmoothTripleLines)
    Q_PROPERTY(bool SmoothTripleLines READ getSmoothTripleLines WRITE setSmoothTripleLines)

    /**
    * @brief This returns the group that the filter belonds to. You can select
    * a different group if you want. The string returned here will be displayed
    * in the GUI for the filter
    */
    SIMPL_FILTER_PARAMETER(DataArrayPath, SurfaceMeshNodeTypeArrayPath)
    Q_PROPERTY(DataArrayPath SurfaceMeshNodeTypeArrayPath READ getSurfaceMeshNodeTypeArrayPath WRITE setSurfaceMeshNodeTypeArrayPath)

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
    MovingFiniteElementSmoothing();

    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


  private:
    DEFINE_DATAARRAY_VARIABLE(int8_t, SurfaceMeshNodeType)

    MovingFiniteElementSmoothing(const MovingFiniteElementSmoothing&); // Copy Constructor Not Implemented
    void operator=(const MovingFiniteElementSmoothing&); // Operator '=' Not Implemented
};

#endif /* _MovingFiniteElementSmoothing_H_ */





