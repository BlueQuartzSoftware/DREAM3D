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


#ifndef _modifiedlambertprojection_h_
#define _modifiedlambertprojection_h_

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"

/**
 * @class ModifiedLambertProjection ModifiedLambertProjection.h DREAM3DLib/Common/ModifiedLambertProjection.h
 * @brief  This class holds a pair of Modified Lambert Projection images. Based off the paper
 * Rosca, D. (2010) New uniform grids on the sphere. Astron Astrophys A63, 520–525.
 * @date July 30, 2013
 * @version 1.0
 */
class OrientationLib_EXPORT ModifiedLambertProjection
{
  public:
    SIMPL_SHARED_POINTERS(ModifiedLambertProjection)
    SIMPL_STATIC_NEW_MACRO(ModifiedLambertProjection)
    SIMPL_TYPE_MACRO(ModifiedLambertProjection)

    virtual ~ModifiedLambertProjection();

    enum Square
    {
      NorthSquare = 0,
      SouthSquare = 1
    };

    /**
     * @brief CreateProjectionFromXYZCoords This static method creates the north and south squares based on the XYZ coordinates
     * in the 'coords' parameter. The XYZ coordinates are on the unit sphere but are true cartesian coordinates and NOT
     * spherical coordinates.
     * @param coords The XYZ cartesian coords that are all on the Unit Sphere (Radius = 1)
     * @param dimension The Dimension of the modified lambert projections images
     * @param resolution The Resolution of the modified lambert projections
     * @param sphereRadius The radius of the sphere from where the coordinates are coming from.
     * @return
     */
    static Pointer CreateProjectionFromXYZCoords(FloatArrayType* coords, int dimension, float sphereRadius);


    SIMPL_GET_PROPERTY(int, Dimension)
    SIMPL_GET_PROPERTY(float, StepSize)
    SIMPL_GET_PROPERTY(float, SphereRadius)


    SIMPL_GET_PROPERTY(DoubleArrayType::Pointer, NorthSquare)
    SIMPL_GET_PROPERTY(DoubleArrayType::Pointer, SouthSquare)

    /**
     * @brief initializeSquares
     * @param dims
     * @param resolution
     * @param sphereRadius
     */
    void initializeSquares(int dims, float sphereRadius);

    virtual int writeHDF5Data(hid_t groupId);
    virtual int readHDF5Data(hid_t groupId);

    /**
     * @brief addValue
     * @param square
     * @param sqCoord
     * @param value
     */
    void addInterpolatedValues(Square square, float* sqCoord, double value);

    /**
     * @brief addValue
     * @param square
     * @param index
     * @param value
     */
    void addValue(Square square, int index, double value);

    /**
     * @brief This function sets the value of a bin in the lambert projection
     * @param square The North or South Squares
     * @param index The index into the array
     * @param value The value to set
     */
    void setValue(Square square, int index, double value);

    /**
     * @brief getValue
     * @param square
     * @param index
     * @return
     */
    double getValue(Square square, int index);

    /**
     * @brief getInterpolatedValue
     * @param square
     * @param sqCoord
     * @return
     */
    double getInterpolatedValue(Square square, float* sqCoord);

    /**
     * @brief getSquareCoord
     * @param xyz The input XYZ coordinate on the unit sphere.
     * @param sqCoord [output] The XY coordinate in the Modified Lambert Square
     * @return If the point was in the north or south squares
     */
    bool getSquareCoord(float* xyz, float* sqCoord);

    /**
     * @brief getSquareIndex
     * @param sqCoord
     * @return
     */
    int getSquareIndex(float* sqCoord);

    /**
     * @brief This function normalizes the squares by taking the value of each square and dividing by the sum of all the
     * values in all the squares.
     */
    void normalizeSquares();

    /**
     * @brief normalizeSquaresToMRD This function first normalizes the squares then multiplies each value by the total number of squares.
     */
    void normalizeSquaresToMRD();

    /**
     * @brief createStereographicProjection
     * @param stereoGraphicProjectionDims
     */
    DoubleArrayType::Pointer createStereographicProjection(int dim);

    void createStereographicProjection(int dim, DoubleArrayType* stereoIntensity);

  protected:
    ModifiedLambertProjection();

  private:
    int m_Dimension;
    float m_StepSize; // The length of an individual grid square
    float m_SphereRadius;

    // These are some precalculated values based on the values above.
    float m_HalfDimension;
    float m_HalfDimensionTimesStepSize;
    float m_MaxCoord;
    float m_MinCoord;

    DoubleArrayType::Pointer m_NorthSquare;
    DoubleArrayType::Pointer m_SouthSquare;



    ModifiedLambertProjection(const ModifiedLambertProjection&); // Copy Constructor Not Implemented
    void operator=(const ModifiedLambertProjection&); // Operator '=' Not Implemented
};


/**
* @class GenerateIntensityMapImpl This class is a wrapper around simply generating a stereo graphically projected intensity "image" (2D Array) based
* off the intended final size of an image and a modified Lambert projection for a set of XYZ coordinates that represent
* the Coords generated from Euler Angles. This all feeds into generating a pole figure.
*/
class GenerateIntensityMapImpl
{
  public:
    GenerateIntensityMapImpl(FloatArrayType* xyzCoords, PoleFigureConfiguration_t* config, DoubleArrayType* intensity) :
      m_XYZCoords(xyzCoords),
      m_Config(config),
      m_Intensity(intensity)
    {

    }
    virtual ~GenerateIntensityMapImpl() {}

    void operator()() const
    {
      ModifiedLambertProjection::Pointer lambert = ModifiedLambertProjection::CreateProjectionFromXYZCoords(m_XYZCoords, m_Config->lambertDim, m_Config->sphereRadius);
      lambert->normalizeSquaresToMRD();
      m_Intensity->resize(m_Config->imageDim * m_Config->imageDim);
      lambert->createStereographicProjection(m_Config->imageDim, m_Intensity);
    }

  protected:
    GenerateIntensityMapImpl() {}

  private:
    FloatArrayType*     m_XYZCoords;
    PoleFigureConfiguration_t* m_Config;
    DoubleArrayType*    m_Intensity;
};

#endif /* _ModifiedLambertProjection_H_ */

