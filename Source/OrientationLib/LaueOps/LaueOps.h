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

#include <vector>


#include <QtCore/QVector>
#include <QtCore/QString>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/Core/Orientation.hpp"
#include "OrientationLib/Core/OrientationTransformation.hpp"
#include "OrientationLib/Core/Quaternion.hpp"
#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"

/*
 * @class LaueOps LaueOps.h OrientationLib/LaueOps/LaueOps.h
 * @brief
 */
class OrientationLib_EXPORT LaueOps
{
  public:
    using Self = LaueOps;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    /**
     * @brief Returns the name of the class for LaueOps
     */
    virtual QString getNameOfClass() const;
    /**
     * @brief Returns the name of the class for LaueOps
     */
    static QString ClassName();

    virtual ~LaueOps();


    /**
     * @brief getOrientationOpsVector This method returns a vector of each type of LaueOps placed such that the
     * index into the vector is the value of the constant at EBSD::CrystalStructure::***
     * @return Vector of LaueOps subclasses.
     */
    static QVector<LaueOps::Pointer> getOrientationOpsQVector();

    /**
     * @brief getOrientationOpsVector This method returns a vector of each type of LaueOps placed such that the
     * index into the vector is the value of the constant at EBSD::CrystalStructure::***
     * @return Vector of LaueOps subclasses.
     */
    static std::vector<LaueOps::Pointer> getOrientationOpsVector();


    /**
     * @brief GetLaueNames Returns the names of the Laue Classes
     * @return
     */
    static std::vector<QString> GetLaueNames();

    /**
     * @brief getODFSize Returns the number of elements in the ODF array
     * @return
     */
    virtual int getODFSize() const = 0;

    /**
    * @brief getHasInversion Returns a bool whether the symmetry class is centro-symmetric
    * @return
    */
    virtual bool getHasInversion() const = 0;

    /**
     * @brief getMDFSize Returns the number of elements in the MDF Array
     * @return
     */
    virtual int getMDFSize() const = 0;

    /**
     * @brief getNumSymOps Returns the number of symmetry operators
     * @return
     */
    virtual int getNumSymOps() const = 0;

    /**
     * @brief getSymmetryName Returns the name of the symmetry
     * @return
     */
    virtual QString getSymmetryName() const = 0;

    /**
     * @brief getMisoQuat Finds the misorientation
     * @param q1
     * @param q2
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    virtual double getMisoQuat(QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const = 0;
    virtual float getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3) const = 0;

    /**
     * @brief getQuatSymOp Copies the symmetry operator at index i into q
     * @param i The index into the Symmetry operators array
     * @param q [output] The quaternion to store the value into
     */
    virtual QuatType getQuatSymOp(int i) const = 0;
    virtual void getRodSymOp(int i, double* r) const = 0;

    virtual void getMatSymOp(int i, double g[3][3]) const = 0;
    virtual void getMatSymOp(int i, float g[3][3]) const = 0;

    virtual OrientationType getODFFZRod(const OrientationType& rod) const = 0;
    virtual OrientationType getMDFFZRod(const OrientationType& rod) const = 0;

    virtual QuatType getNearestQuat(const QuatType& q1, const QuatType& q2) const = 0;
    virtual QuatF getNearestQuat(const QuatF& q1f, const QuatF& q2f) const = 0;

    virtual QuatType getFZQuat(const QuatType& qr) const;

    virtual int getMisoBin(const OrientationType& rod) const = 0;

    virtual bool inUnitTriangle(double eta, double chi) const = 0;

    virtual OrientationType determineEulerAngles(uint64_t seed, int choose) const = 0;

    virtual OrientationType randomizeEulerAngles(const OrientationType& euler) const = 0;

    virtual size_t getRandomSymmetryOperatorIndex(int numSymOps) const;

    virtual OrientationType determineRodriguesVector(uint64_t seed, int choose) const = 0;

    virtual int getOdfBin(const OrientationType& rod) const = 0;

    virtual void getSchmidFactorAndSS(double load[3], double& schmidfactor, double angleComps[2], int& slipsys) const = 0;

    virtual void getSchmidFactorAndSS(double load[3], double plane[3], double direction[3], double& schmidfactor, double angleComps[2], int& slipsys) const = 0;

    virtual double getmPrime(const QuatType& q1, const QuatType& q2, double LD[3]) const = 0;

    virtual double getF1(const QuatType& q1, const QuatType& q2, double LD[3], bool maxSF) const = 0;

    virtual double getF1spt(const QuatType& q1, const QuatType& q2, double LD[3], bool maxSF) const = 0;

    virtual double getF7(const QuatType& q1, const QuatType& q2, double LD[3], bool maxSF) const = 0;

    virtual void generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* c1, FloatArrayType* c2, FloatArrayType* c3) const = 0;

    /**
     * @brief generateIPFColor Generates an RGB Color from a Euler Angle and Reference Direction
     * @param eulers Pointer to the 3 component Euler Angle
     * @param refDir Pointer to the 3 Component Reference Direction
     * @param rgb [output] The pointer to store the RGB value
     * @param convertDegrees Are the input angles in Degrees
     */
    virtual SIMPL::Rgb generateIPFColor(double* eulers, double* refDir, bool convertDegrees) const = 0;

    /**
     * @brief generateIPFColor Generates an RGB Color from a Euler Angle and Reference Direction
     * @param e0 First component of the Euler Angle
     * @param e1 Second component of the Euler Angle
     * @param e2 Third component of the Euler Angle
     * @param dir0 First component of the Reference Direction
     * @param dir1 Second component of the Reference Direction
     * @param dir2 Third component of the Reference Direction
     * @param rgb [output] The pointer to store the RGB value
     * @param convertDegrees Are the input angles in Degrees
     */
    virtual SIMPL::Rgb generateIPFColor(double e0, double e1, double e2, double dir0, double dir1, double dir2, bool convertDegrees) const = 0;

    /**
     * @brief generateRodriguesColor Generates an RGB Color from a Rodrigues Vector
     * @param r1 First component of the Rodrigues Vector
     * @param r2 Second component of the Rodrigues Vector
     * @param r3 Third component of the Rodrigues Vector
     * @param rgb [output] The pointer to store the RGB value
     */
    virtual SIMPL::Rgb generateRodriguesColor(double r1, double r2, double r3) const = 0;

    /**
     * @brief generateMisorientationColor Generates a color based on the method developed by C. Schuh and S. Patala.
     * @param q A Quaternion representing the crystal direction
     * @param refDir A Quaternion representing the sample reference direction
     * @return A SIMPL::Rgb value
     */
    virtual SIMPL::Rgb generateMisorientationColor(const QuatType& q, const QuatType& refFrame) const = 0;

    /**
     * @brief generatePoleFigure This method will generate a number of pole figures for this crystal symmetry and the Euler
     * angles that are passed in.
     * @param eulers The Euler Angles to generate the pole figure from.
     * @param imageSize The size in Pixels of the final RGB Image.
     * @param numColors The number of colors to use in the RGB Image. Less colors can give the effect of contouring.
     * @return A QVector of UInt8ArrayType pointers where each one represents a 2D RGB array that can be used to initialize
     * an image object from other libraries and written out to disk.
     */
    virtual QVector<UInt8ArrayType::Pointer> generatePoleFigure(PoleFigureConfiguration_t& config) const = 0;

  protected:
    LaueOps();

    double _calcMisoQuat(const QuatType quatsym[24], int numsym, const QuatType& q1, const QuatType& q2, double& n1, double& n2, double& n3) const;

    OrientationType _calcRodNearestOrigin(const double rodsym[24][3], int numsym, const OrientationType& rod) const;
    QuatType _calcNearestQuat(const QuatType quatsym[24], int numsym, const QuatType& q1, const QuatType& q2) const;
    QuatType _calcQuatNearestOrigin(const QuatType quatsym[24], int numsym, const QuatType& qr) const;

    int _calcMisoBin(double dim[3], double bins[3], double step[3], const OrientationType& homochoric) const;
    void _calcDetermineHomochoricValues(uint64_t seed, double init[3], double step[3], int32_t phi[3], int choose, double& r1, double& r2, double& r3) const;
    int _calcODFBin(double dim[3], double bins[3], double step[3], const OrientationType& homochoric) const;

  public:
    LaueOps(const LaueOps&) = delete;        // Copy Constructor Not Implemented
    LaueOps(LaueOps&&) = delete;             // Move Constructor Not Implemented
    LaueOps& operator=(const LaueOps&) = delete; // Copy Assignment Not Implemented
    LaueOps& operator=(LaueOps&&) = delete;      // Move Assignment Not Implemented

  private:
};


