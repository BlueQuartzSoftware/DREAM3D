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

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/DataArrays/DataArray.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/LaueOps/LaueOps.h"


/**
 * @class TrigonalOps TrigonalOps.h DREAM3DLib/Common/LaueOps/TrigonalOps.h
 * @brief
 *
*
 * @date May 5, 2011
 * @version 1.0
 */
class OrientationLib_EXPORT TrigonalOps : public LaueOps
{
  public:
    using Self = TrigonalOps;
    using Pointer = std::shared_ptr<Self>;
    using ConstPointer = std::shared_ptr<const Self>;
    using WeakPointer = std::weak_ptr<Self>;
    using ConstWeakPointer = std::weak_ptr<const Self>;
    static Pointer NullPointer();

    /**
    * @brief Returns the name of the class for TrigonalOps
    */
    QString getNameOfClass() const override;
    /**
    * @brief Returns the name of the class for TrigonalOps
    */
    static QString ClassName();

    static Pointer New();


    TrigonalOps();
    ~TrigonalOps() override;

    static const int k_OdfSize = 31104;
    static const int k_MdfSize = 31104;
    static const int k_NumSymQuats = 6;


    /**
     * @brief getHasInversion Returns if this Laue class has inversion
     * @return
     */
    bool getHasInversion() const override;

    /**
     * @brief getODFSize Returns the number of ODF bins
     * @return
     */
    int getODFSize() const override;

    /**
     * @brief getMDFSize Returns the number of MDF bins
     * @return
     */
    int getMDFSize() const override;

    /**
     * @brief getNumSymOps Returns the number of symmetry operators
     * @return
     */
    int getNumSymOps() const override;

    /**
     * @brief getSymmetryName Returns the name of the Laue class
     * @return
     */
    QString getSymmetryName() const override;

    double getMisoQuat(QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const override;
    float getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3) const override;

    QuatType getQuatSymOp(int i) const override;
    void getRodSymOp(int i, double* r) const override;

    void getMatSymOp(int i, double g[3][3]) const override;
    void getMatSymOp(int i, float g[3][3]) const override;

    OrientationType getODFFZRod(const OrientationType& rod) const override;
    OrientationType getMDFFZRod(const OrientationType& rod) const override;

    QuatType getNearestQuat(const QuatType& q1, const QuatType& q2) const override;
    QuatF getNearestQuat(const QuatF& q1f, const QuatF& q2f) const override;

    int getMisoBin(const OrientationType& rod) const override;
    bool inUnitTriangle(double eta, double chi) const override;
    OrientationType determineEulerAngles(uint64_t seed, int choose) const override;
    OrientationType randomizeEulerAngles(const OrientationType& euler) const override;
    OrientationType determineRodriguesVector(uint64_t seed, int choose) const override;
    int getOdfBin(const OrientationType& rod) const override;
    void getSchmidFactorAndSS(double load[3], double& schmidfactor, double angleComps[2], int& slipsys) const override;
    void getSchmidFactorAndSS(double load[3], double plane[3], double direction[3], double& schmidfactor, double angleComps[2], int& slipsys) const override;
    double getmPrime(const QuatType& q1, const QuatType& q2, double LD[3]) const override;
    double getF1(const QuatType& q1, const QuatType& q2, double LD[3], bool maxSF) const override;
    double getF1spt(const QuatType& q1, const QuatType& q2, double LD[3], bool maxSF) const override;
    double getF7(const QuatType& q1, const QuatType& q2, double LD[3], bool maxSF) const override;

    void generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* c1, FloatArrayType* c2, FloatArrayType* c3) const override;

    /**
     * @brief generateIPFColor Generates an RGB Color from a Euler Angle and Reference Direction
     * @param eulers Pointer to the 3 component Euler Angle
     * @param refDir Pointer to the 3 Component Reference Direction
     * @param convertDegrees Are the input angles in Degrees
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    SIMPL::Rgb generateIPFColor(double* eulers, double* refDir, bool convertDegrees) const override;

    /**
     * @brief generateIPFColor Generates an RGB Color from a Euler Angle and Reference Direction
     * @param e0 First component of the Euler Angle
     * @param e1 Second component of the Euler Angle
     * @param e2 Third component of the Euler Angle
     * @param dir0 First component of the Reference Direction
     * @param dir1 Second component of the Reference Direction
     * @param dir2 Third component of the Reference Direction
     * @param convertDegrees Are the input angles in Degrees
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    SIMPL::Rgb generateIPFColor(double e0, double e1, double phi2, double dir0, double dir1, double dir2, bool convertDegrees) const override;

    /**
     * @brief generateRodriguesColor Generates an RGB Color from a Rodrigues Vector
     * @param r1 First component of the Rodrigues Vector
     * @param r2 Second component of the Rodrigues Vector
     * @param r3 Third component of the Rodrigues Vector
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    SIMPL::Rgb generateRodriguesColor(double r1, double r2, double r3) const override;

    /**
     * @brief generateMisorientationColor Generates a color based on the method developed by C. Schuh and S. Patala.
     * @param q Quaternion representing the direction
     * @param refDir The sample reference direction
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    SIMPL::Rgb generateMisorientationColor(const QuatType& q, const QuatType& refFrame) const override;

    /**
     * @brief generatePoleFigure This method will generate a number of pole figures for this crystal symmetry and the Euler
     * angles that are passed in.
     * @param eulers The Euler Angles to generate the pole figure from.
     * @param imageSize The size in Pixels of the final RGB Image.
     * @param numColors The number of colors to use in the RGB Image. Less colors can give the effect of contouring.
     * @return A QVector of UInt8ArrayType pointers where each one represents a 2D RGB array that can be used to initialize
     * an image object from other libraries and written out to disk.
     */
    QVector<UInt8ArrayType::Pointer> generatePoleFigure(PoleFigureConfiguration_t& config) const override;

    /**
     * @brief generateStandardTriangle Generates an RGBA array that is a color "Standard" IPF Triangle Legend used for IPF Color Maps.
     * @return
     */
    UInt8ArrayType::Pointer generateIPFTriangleLegend(int imageDim) const;

  protected:
    double _calcMisoQuat(const QuatType quatsym[6], int numsym, QuatType& q1, QuatType& q2, double& n1, double& n2, double& n3) const;

  public:
    TrigonalOps(const TrigonalOps&) = delete;    // Copy Constructor Not Implemented
    TrigonalOps(TrigonalOps&&) = delete;         // Move Constructor Not Implemented
    TrigonalOps& operator=(const TrigonalOps&) = delete; // Copy Assignment Not Implemented
    TrigonalOps& operator=(TrigonalOps&&) = delete;      // Move Assignment Not Implemented

  private:

};


