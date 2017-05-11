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


#ifndef _findeuclideandistmap_h_
#define _findeuclideandistmap_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

/**
 * @brief The FindEuclideanDistMap class. See [Filter documentation](@ref findeuclideandistmap) for details.
 */
class FindEuclideanDistMap : public AbstractFilter
{
    Q_OBJECT
  public:
    SIMPL_SHARED_POINTERS(FindEuclideanDistMap)
    SIMPL_STATIC_NEW_MACRO(FindEuclideanDistMap)
    SIMPL_TYPE_MACRO_SUPER(FindEuclideanDistMap, AbstractFilter)

    virtual ~FindEuclideanDistMap();

    using EnumType = uint32_t;

    enum class MapType : EnumType
    {
      FeatureBoundary = 0,      //!<
      TripleJunction = 1, //!<
      QuadPoint = 2, //!<
    };


    SIMPL_FILTER_PARAMETER(DataArrayPath, FeatureIdsArrayPath)
    Q_PROPERTY(DataArrayPath FeatureIdsArrayPath READ getFeatureIdsArrayPath WRITE setFeatureIdsArrayPath)

    SIMPL_FILTER_PARAMETER(QString, GBDistancesArrayName)
    Q_PROPERTY(QString GBDistancesArrayName READ getGBDistancesArrayName WRITE setGBDistancesArrayName)

    SIMPL_FILTER_PARAMETER(QString, TJDistancesArrayName)
    Q_PROPERTY(QString TJDistancesArrayName READ getTJDistancesArrayName WRITE setTJDistancesArrayName)

    SIMPL_FILTER_PARAMETER(QString, QPDistancesArrayName)
    Q_PROPERTY(QString QPDistancesArrayName READ getQPDistancesArrayName WRITE setQPDistancesArrayName)

    SIMPL_FILTER_PARAMETER(QString, NearestNeighborsArrayName)
    Q_PROPERTY(QString NearestNeighborsArrayName READ getNearestNeighborsArrayName WRITE setNearestNeighborsArrayName)

    SIMPL_FILTER_PARAMETER(bool, DoBoundaries)
    Q_PROPERTY(bool DoBoundaries READ getDoBoundaries WRITE setDoBoundaries)

    SIMPL_FILTER_PARAMETER(bool, DoTripleLines)
    Q_PROPERTY(bool DoTripleLines READ getDoTripleLines WRITE setDoTripleLines)

    SIMPL_FILTER_PARAMETER(bool, DoQuadPoints)
    Q_PROPERTY(bool DoQuadPoints READ getDoQuadPoints WRITE setDoQuadPoints)

    SIMPL_FILTER_PARAMETER(bool, SaveNearestNeighbors)
    Q_PROPERTY(bool SaveNearestNeighbors READ getSaveNearestNeighbors WRITE setSaveNearestNeighbors)

    SIMPL_FILTER_PARAMETER(bool, CalcManhattanDist)
    Q_PROPERTY(bool CalcManhattanDist READ getCalcManhattanDist WRITE setCalcManhattanDist)

    /**
     * @brief getCompiledLibraryName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getCompiledLibraryName();

    /**
     * @brief getBrandingString Returns the branding string for the filter, which is a tag
     * used to denote the filter's association with specific plugins
     * @return Branding string
    */
    virtual const QString getBrandingString();

    /**
     * @brief getFilterVersion Returns a version string for this filter. Default
     * value is an empty string.
     * @return
     */
    virtual const QString getFilterVersion();

    /**
     * @brief newFilterInstance Reimplemented from @see AbstractFilter class
     */
    virtual AbstractFilter::Pointer newFilterInstance(bool copyFilterParameters);

    /**
     * @brief getGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getGroupName();

    /**
     * @brief getSubGroupName Reimplemented from @see AbstractFilter class
     */
    virtual const QString getSubGroupName();

    /**
     * @brief getHumanLabel Reimplemented from @see AbstractFilter class
     */
    virtual const QString getHumanLabel();

    /**
     * @brief setupFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void setupFilterParameters();

    /**
     * @brief readFilterParameters Reimplemented from @see AbstractFilter class
     */
    virtual void readFilterParameters(AbstractFilterParametersReader* reader, int index);

    /**
     * @brief execute Reimplemented from @see AbstractFilter class
     */
    virtual void execute();

    /**
    * @brief preflight Reimplemented from @see AbstractFilter class
    */
    virtual void preflight();

  signals:
    /**
     * @brief updateFilterParameters Emitted when the Filter requests all the latest Filter parameters
     * be pushed from a user-facing control (such as a widget)
     * @param filter Filter instance pointer
     */
    void updateFilterParameters(AbstractFilter* filter);

    /**
     * @brief parametersChanged Emitted when any Filter parameter is changed internally
     */
    void parametersChanged();

    /**
     * @brief preflightAboutToExecute Emitted just before calling dataCheck()
     */
    void preflightAboutToExecute();

    /**
     * @brief preflightExecuted Emitted just after calling dataCheck()
     */
    void preflightExecuted();

  protected:
    FindEuclideanDistMap();
    /**
     * @brief dataCheck Checks for the appropriate parameter values and availability of arrays
     */
    void dataCheck();

    /**
     * @brief Initializes all the private instance variables.
     */
    void initialize();


    /**
     * @brief find_euclideandistmap Provides setup for Euclidean distance map caluclation; note that the
     * actual algorithm is performed in a threaded implementation
     */
    void findDistanceMap();

  private:
    DEFINE_DATAARRAY_VARIABLE(int32_t, FeatureIds)

    DEFINE_DATAARRAY_VARIABLE(int32_t, NearestNeighbors)

    // Full Euclidean Distance Arrays
    DEFINE_DATAARRAY_VARIABLE(float, GBEuclideanDistances)
    DEFINE_DATAARRAY_VARIABLE(float, TJEuclideanDistances)
    DEFINE_DATAARRAY_VARIABLE(float, QPEuclideanDistances)

    //  Distance Arrays
    DEFINE_DATAARRAY_VARIABLE(int32_t, GBManhattanDistances)
    DEFINE_DATAARRAY_VARIABLE(int32_t, TJManhattanDistances)
    DEFINE_DATAARRAY_VARIABLE(int32_t, QPManhattanDistances)

    FindEuclideanDistMap(const FindEuclideanDistMap&); // Copy Constructor Not Implemented
    void operator=(const FindEuclideanDistMap&); // Operator '=' Not Implemented
};

#endif /* FINDEUCLIDEANDISTMAP_H_ */
