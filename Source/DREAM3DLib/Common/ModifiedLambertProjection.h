/* ============================================================================
 * Copyright (c) 2013 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2013 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-10-D-5210
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _ModifiedLambertProjection_H_
#define _ModifiedLambertProjection_H_

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/DataArrays/DataArray.hpp"
#include "DREAM3DLib/Utilities/PoleFigureUtilities.h"

/**
 * @class ModifiedLambertProjection ModifiedLambertProjection.h DREAM3DLib/Common/ModifiedLambertProjection.h
 * @brief  This class holds a pair of Modified Lambert Projection images. Based off the paper
 * Rosca, D. (2010) New uniform grids on the sphere. Astron Astrophys A63, 520–525.
 * @author Michael A. Jackson for BlueQuartz Software, Michael A. Groeber for AFRL
 * @date July 30, 2013
 * @version 1.0
 */
class DREAM3DLib_EXPORT ModifiedLambertProjection
{
  public:
    DREAM3D_SHARED_POINTERS(ModifiedLambertProjection)
    DREAM3D_STATIC_NEW_MACRO(ModifiedLambertProjection)
    DREAM3D_TYPE_MACRO(ModifiedLambertProjection)

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


    DREAM3D_GET_PROPERTY(int, Dimension)
    DREAM3D_GET_PROPERTY(float, StepSize)
    DREAM3D_GET_PROPERTY(float, SphereRadius)


    DREAM3D_GET_PROPERTY(DoubleArrayType::Pointer, NorthSquare)
    DREAM3D_GET_PROPERTY(DoubleArrayType::Pointer, SouthSquare)

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
* @class This class is a wrapper around simply generating a stereo graphically projected intensity "image" (2D Array) based
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
//        size_t dims[3] = {m_Config->lambertDim, m_Config->lambertDim, 1 };
//        float res[3] = {1.0, 1.0, 1.0};
//        DoubleArrayType::Pointer north = lambert->getNorthSquare();
//        VtkRectilinearGridWriter::WriteDataArrayToFile("/tmp/NorthSquare_" + m_Intensity->getName() + ".vtk", north.get(), dims, res, "double", true);
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
