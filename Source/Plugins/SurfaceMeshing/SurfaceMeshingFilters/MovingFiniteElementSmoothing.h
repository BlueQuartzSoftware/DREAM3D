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

#pragma once

#include <memory>

#include <QtCore/QString>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "SurfaceMeshing/SurfaceMeshingFilters/SurfaceMeshFilter.h"

#include "SurfaceMeshing/SurfaceMeshingConstants.h"
#include "SurfaceMeshing/SurfaceMeshingVersion.h"

#include "SurfaceMeshing/SurfaceMeshingDLLExport.h"

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
class SurfaceMeshing_EXPORT MovingFiniteElementSmoothing : public SurfaceMeshFilter
{
    Q_OBJECT
    // PYB11_CREATE_BINDINGS(MovingFiniteElementSmoothing SUPERCLASS SurfaceMeshFilter)
    // PYB11_PROPERTY(int IterationSteps READ getIterationSteps WRITE setIterationSteps)
    // PYB11_PROPERTY(bool NodeConstraints READ getNodeConstraints WRITE setNodeConstraints)
    // PYB11_PROPERTY(bool ConstrainSurfaceNodes READ getConstrainSurfaceNodes WRITE setConstrainSurfaceNodes)
    // PYB11_PROPERTY(bool ConstrainQuadPoints READ getConstrainQuadPoints WRITE setConstrainQuadPoints)
    // PYB11_PROPERTY(bool SmoothTripleLines READ getSmoothTripleLines WRITE setSmoothTripleLines)
    // PYB11_PROPERTY(DataArrayPath SurfaceMeshNodeTypeArrayPath READ getSurfaceMeshNodeTypeArrayPath WRITE setSurfaceMeshNodeTypeArrayPath)
  public:
    using Self = MovingFiniteElementSmoothing;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    static std::shared_ptr<MovingFiniteElementSmoothing> New();

    /**
     * @brief Returns the name of the class for MovingFiniteElementSmoothing
     */
    QString getNameOfClass() const override;
    /**
     * @brief Returns the name of the class for MovingFiniteElementSmoothing
     */
    static QString ClassName();

    ~MovingFiniteElementSmoothing() override;

    /* Place your input parameters here. You can use some of the DREAM3D Macros if you want to */
    /**
     * @brief Setter property for IterationSteps
     */
    void setIterationSteps(int value);
    /**
     * @brief Getter property for IterationSteps
     * @return Value of IterationSteps
     */
    int getIterationSteps() const;

    Q_PROPERTY(int IterationSteps READ getIterationSteps WRITE setIterationSteps)
    /**
     * @brief Setter property for NodeConstraints
     */
    void setNodeConstraints(bool value);
    /**
     * @brief Getter property for NodeConstraints
     * @return Value of NodeConstraints
     */
    bool getNodeConstraints() const;

    Q_PROPERTY(bool NodeConstraints READ getNodeConstraints WRITE setNodeConstraints)
    /**
     * @brief Setter property for ConstrainSurfaceNodes
     */
    void setConstrainSurfaceNodes(bool value);
    /**
     * @brief Getter property for ConstrainSurfaceNodes
     * @return Value of ConstrainSurfaceNodes
     */
    bool getConstrainSurfaceNodes() const;

    Q_PROPERTY(bool ConstrainSurfaceNodes READ getConstrainSurfaceNodes WRITE setConstrainSurfaceNodes)
    /**
     * @brief Setter property for ConstrainQuadPoints
     */
    void setConstrainQuadPoints(bool value);
    /**
     * @brief Getter property for ConstrainQuadPoints
     * @return Value of ConstrainQuadPoints
     */
    bool getConstrainQuadPoints() const;

    Q_PROPERTY(bool ConstrainQuadPoints READ getConstrainQuadPoints WRITE setConstrainQuadPoints)
    /**
     * @brief Setter property for SmoothTripleLines
     */
    void setSmoothTripleLines(bool value);
    /**
     * @brief Getter property for SmoothTripleLines
     * @return Value of SmoothTripleLines
     */
    bool getSmoothTripleLines() const;

    Q_PROPERTY(bool SmoothTripleLines READ getSmoothTripleLines WRITE setSmoothTripleLines)

    /**
     * @brief This returns the group that the filter belonds to. You can select
     * a different group if you want. The string returned here will be displayed
     * in the GUI for the filter
     */
    /**
     * @brief Setter property for SurfaceMeshNodeTypeArrayPath
     */
    void setSurfaceMeshNodeTypeArrayPath(const DataArrayPath& value);
    /**
     * @brief Getter property for SurfaceMeshNodeTypeArrayPath
     * @return Value of SurfaceMeshNodeTypeArrayPath
     */
    DataArrayPath getSurfaceMeshNodeTypeArrayPath() const;

    Q_PROPERTY(DataArrayPath SurfaceMeshNodeTypeArrayPath READ getSurfaceMeshNodeTypeArrayPath WRITE setSurfaceMeshNodeTypeArrayPath)

    QString getCompiledLibraryName() const override;
    AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters) const override;
    QString getGroupName() const override;
    QString getSubGroupName() const override;

    /**
     * @brief getUuid Return the unique identifier for this filter.
     * @return A QUuid object.
     */
    QUuid getUuid() const override;

    /**
     * @brief This returns a string that is displayed in the GUI. It should be readable
     * and understandable by humans.
     */
    QString getHumanLabel() const override;

    /**
     * @brief This method will instantiate all the end user settable options/parameters
     * for this filter
     */
    void setupFilterParameters() override;

    /**
     * @brief This method will read the options from a file
     * @param reader The reader that is used to read the options from a file
     */
    void readFilterParameters(AbstractFilterParametersReader* reader, int index) override;

    /**
     * @brief Reimplemented from @see AbstractFilter class
     */
    void execute() override;

    /**
     * @brief This function runs some sanity checks on the DataContainer and inputs
     * in an attempt to ensure the filter can process the inputs.
     */
    void preflight() override;

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
    std::weak_ptr<DataArray<int8_t>> m_SurfaceMeshNodeTypePtr;
    int8_t* m_SurfaceMeshNodeType = nullptr;

    int m_IterationSteps = {};
    bool m_NodeConstraints = {};
    bool m_ConstrainSurfaceNodes = {};
    bool m_ConstrainQuadPoints = {};
    bool m_SmoothTripleLines = {};
    DataArrayPath m_SurfaceMeshNodeTypeArrayPath = {};

  public:
    MovingFiniteElementSmoothing(const MovingFiniteElementSmoothing&) = delete;            // Copy Constructor Not Implemented
    MovingFiniteElementSmoothing(MovingFiniteElementSmoothing&&) = delete;                 // Move Constructor Not Implemented
    MovingFiniteElementSmoothing& operator=(const MovingFiniteElementSmoothing&) = delete; // Copy Assignment Not Implemented
    MovingFiniteElementSmoothing& operator=(MovingFiniteElementSmoothing&&) = delete;      // Move Assignment Not Implemented
};






