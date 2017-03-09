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
#ifndef _laueOps_h_
#define _laueOps_h_

#include <vector>


#include <QtCore/QVector>
#include <QtCore/QString>


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Math/QuaternionMath.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"
#include "OrientationLib/Utilities/PoleFigureUtilities.h"


/*
 * @class LaueOps LaueOps.h OrientationLib/LaueOps/LaueOps.h
 * @brief
 */
class OrientationLib_EXPORT LaueOps
{
  public:
    SIMPL_SHARED_POINTERS(LaueOps)
    SIMPL_TYPE_MACRO(LaueOps)

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
    virtual int getODFSize() = 0;

    /**
    * @brief getHasInversion Returns a bool whether the symmetry class is centro-symmetric
    * @return
    */
    virtual bool getHasInversion() = 0;

    /**
     * @brief getMDFSize Returns the number of elements in the MDF Array
     * @return
     */
    virtual int getMDFSize() = 0;

    /**
     * @brief getNumSymOps Returns the number of symmetry operators
     * @return
     */
    virtual int getNumSymOps() = 0;

    /**
     * @brief getSymmetryName Returns the name of the symmetry
     * @return
     */
    virtual QString getSymmetryName() = 0;

    /**
     * @brief getMisoQuat Finds the misorientation
     * @param q1
     * @param q2
     * @param n1
     * @param n2
     * @param n3
     * @return
     */
    virtual float getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3) = 0;

    /**
     * @brief getQuatSymOp Copies the symmetry operator at index i into q
     * @param i The index into the Symmetry operators array
     * @param q [output] The quaternion to store the value into
     */
    virtual void getQuatSymOp(int i, QuatF& q) = 0;
    virtual void getRodSymOp(int i, float* r) = 0;
    virtual void getMatSymOp(int i, float g[3][3]) = 0;
    virtual FOrientArrayType getODFFZRod(FOrientArrayType rod) = 0;
    virtual FOrientArrayType getMDFFZRod(FOrientArrayType rod) = 0;
    virtual void getNearestQuat(QuatF& q1, QuatF& q2) = 0;
    virtual void getFZQuat(QuatF& qr);
    virtual int getMisoBin(FOrientArrayType rod) = 0;
    virtual bool inUnitTriangle(float eta, float chi) = 0;
    virtual FOrientArrayType determineEulerAngles(uint64_t seed, int choose) = 0;
    virtual FOrientArrayType randomizeEulerAngles(FOrientArrayType euler) = 0;
    virtual size_t getRandomSymmetryOperatorIndex(int numSymOps);
    virtual FOrientArrayType determineRodriguesVector(uint64_t seed, int choose) = 0;
    virtual int getOdfBin(FOrientArrayType rod) = 0;
    virtual void getSchmidFactorAndSS(float load[3], float& schmidfactor, float angleComps[2], int& slipsys) = 0;
    virtual void getSchmidFactorAndSS(float load[3], float plane[3], float direction[3], float& schmidfactor, float angleComps[2], int& slipsys) = 0;
    virtual void getmPrime(QuatF& q1, QuatF& q2, float LD[3], float& mPrime) = 0;
    virtual void getF1(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1) = 0;
    virtual void getF1spt(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1spt) = 0;
    virtual void getF7(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F7) = 0;


    virtual void generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* c1, FloatArrayType* c2, FloatArrayType* c3) = 0;

    /**
     * @brief generateIPFColor Generates an RGB Color from a Euler Angle and Reference Direction
     * @param eulers Pointer to the 3 component Euler Angle
     * @param refDir Pointer to the 3 Component Reference Direction
     * @param rgb [output] The pointer to store the RGB value
     * @param convertDegrees Are the input angles in Degrees
     */
    virtual SIMPL::Rgb generateIPFColor(double* eulers, double* refDir, bool convertDegrees) = 0;

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
    virtual SIMPL::Rgb generateIPFColor(double e0, double e1, double e2, double dir0, double dir1, double dir2, bool convertDegrees) = 0;

    /**
     * @brief generateRodriguesColor Generates an RGB Color from a Rodrigues Vector
     * @param r1 First component of the Rodrigues Vector
     * @param r2 Second component of the Rodrigues Vector
     * @param r3 Third component of the Rodrigues Vector
     * @param rgb [output] The pointer to store the RGB value
     */
    virtual SIMPL::Rgb generateRodriguesColor(float r1, float r2, float r3) = 0;

    /**
     * @brief generateMisorientationColor Generates a color based on the method developed by C. Schuh and S. Patala.
     * @param q A Quaternion representing the crystal direction
     * @param refDir A Quaternion representing the sample reference direction
     * @return A SIMPL::Rgb value
     */
    virtual SIMPL::Rgb generateMisorientationColor(const QuatF& q, const QuatF& refFrame) = 0;


    /**
     * @brief generatePoleFigure This method will generate a number of pole figures for this crystal symmetry and the Euler
     * angles that are passed in.
     * @param eulers The Euler Angles to generate the pole figure from.
     * @param imageSize The size in Pixels of the final RGB Image.
     * @param numColors The number of colors to use in the RGB Image. Less colors can give the effect of contouring.
     * @return A QVector of UInt8ArrayType pointers where each one represents a 2D RGB array that can be used to initialize
     * an image object from other libraries and written out to disk.
     */
    virtual QVector<UInt8ArrayType::Pointer> generatePoleFigure(PoleFigureConfiguration_t& config) = 0;

  protected:
    LaueOps();

    float _calcMisoQuat(const QuatF quatsym[24], int numsym,
                        QuatF& q1, QuatF& q2,
                        float& n1, float& n2, float& n3);

    FOrientArrayType _calcRodNearestOrigin(const float rodsym[24][3], int numsym, FOrientArrayType rod);
    void _calcNearestQuat(const QuatF quatsym[24], int numsym, QuatF& q1, QuatF& q2);
    void _calcQuatNearestOrigin(const QuatF quatsym[24], int numsym, QuatF& qr);

    int _calcMisoBin(float dim[3], float bins[3], float step[3], const FOrientArrayType& homochoric);
    void _calcDetermineHomochoricValues(uint64_t seed, float init[3], float step[3], int32_t phi[3], int choose, float& r1, float& r2, float& r3);
    int _calcODFBin(float dim[3], float bins[3], float step[3], FOrientArrayType homochoric);

  private:
    LaueOps(const LaueOps&); // Copy Constructor Not Implemented
    void operator=(const LaueOps&); // Operator '=' Not Implemented
};

#endif /* _laueOps_H_ */

