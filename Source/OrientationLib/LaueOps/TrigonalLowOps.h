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
#ifndef _trigonallowops_h_
#define _trigonallowops_h_


#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"
#include "SIMPLib/DataArrays/DataArray.hpp"
#include "SIMPLib/Math/QuaternionMath.hpp"

#include "OrientationLib/OrientationLib.h"
#include "OrientationLib/LaueOps/LaueOps.h"
#include "OrientationLib/OrientationMath/OrientationArray.hpp"


/**
 * @class TrigonalLowOps TrigonalLowOps.h DREAM3DLib/Common/LaueOps/TrigonalLowOps.h
 * @brief
 *
*
 * @date May 5, 2011
 * @version 1.0
 */
class OrientationLib_EXPORT TrigonalLowOps : public LaueOps
{
  public:
    SIMPL_SHARED_POINTERS(TrigonalLowOps)
     SIMPL_TYPE_MACRO_SUPER_OVERRIDE(TrigonalLowOps, LaueOps)
    SIMPL_STATIC_NEW_MACRO(TrigonalLowOps)

    TrigonalLowOps();
    virtual ~TrigonalLowOps();

    static const int k_OdfSize = 124416;
    static const int k_MdfSize = 124416;
    static const int k_NumSymQuats = 3;

    /**
     * @brief getHasInversion Returns if this Laue class has inversion
     * @return
     */
    virtual bool getHasInversion();

    /**
     * @brief getODFSize Returns the number of ODF bins
     * @return
     */
    virtual int getODFSize();

    /**
     * @brief getMDFSize Returns the number of MDF bins
     * @return
     */
    virtual int getMDFSize();

    /**
     * @brief getNumSymOps Returns the number of symmetry operators
     * @return
     */
    virtual int getNumSymOps();

    /**
     * @brief getSymmetryName Returns the name of the Laue class
     * @return
     */
    QString getSymmetryName();


    virtual float getMisoQuat(QuatF& q1, QuatF& q2, float& n1, float& n2, float& n3);
    virtual void getQuatSymOp(int i, QuatF& q);
    virtual void getRodSymOp(int i, float* r);
    virtual void getMatSymOp(int i, float g[3][3]);
    virtual FOrientArrayType getODFFZRod(FOrientArrayType rod);
    virtual FOrientArrayType getMDFFZRod(FOrientArrayType rod);
    virtual void getNearestQuat(QuatF& q1, QuatF& q2);
    virtual int getMisoBin(FOrientArrayType rod);
    virtual bool inUnitTriangle(float eta, float chi);
    virtual FOrientArrayType determineEulerAngles(uint64_t seed, int choose);
    virtual FOrientArrayType randomizeEulerAngles(FOrientArrayType euler);
    virtual FOrientArrayType determineRodriguesVector(uint64_t seed, int choose);
    virtual int getOdfBin(FOrientArrayType rod);
    virtual void getSchmidFactorAndSS(float load[3], float& schmidfactor, float angleComps[2], int& slipsys);
    virtual void getSchmidFactorAndSS(float load[3], float plane[3], float direction[3], float& schmidfactor, float angleComps[2], int& slipsys);
    virtual void getmPrime(QuatF& q1, QuatF& q2, float LD[3], float& mPrime);
    virtual void getF1(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1);
    virtual void getF1spt(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F1spt);
    virtual void getF7(QuatF& q1, QuatF& q2, float LD[3], bool maxSF, float& F7);


    virtual void generateSphereCoordsFromEulers(FloatArrayType* eulers, FloatArrayType* c1, FloatArrayType* c2, FloatArrayType* c3);


    /**
     * @brief generateIPFColor Generates an RGB Color from a Euler Angle and Reference Direction
     * @param eulers Pointer to the 3 component Euler Angle
     * @param refDir Pointer to the 3 Component Reference Direction
     * @param convertDegrees Are the input angles in Degrees
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    virtual SIMPL::Rgb generateIPFColor(double* eulers, double* refDir, bool convertDegrees);

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
    virtual SIMPL::Rgb generateIPFColor(double e0, double e1, double phi2, double dir0, double dir1, double dir2, bool convertDegrees);

    /**
     * @brief generateRodriguesColor Generates an RGB Color from a Rodrigues Vector
     * @param r1 First component of the Rodrigues Vector
     * @param r2 Second component of the Rodrigues Vector
     * @param r3 Third component of the Rodrigues Vector
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    virtual SIMPL::Rgb generateRodriguesColor(float r1, float r2, float r3);

    /**
     * @brief generateMisorientationColor Generates a color based on the method developed by C. Schuh and S. Patala.
     * @param q Quaternion representing the direction
     * @param refDir The sample reference direction
     * @return Returns the ARGB Quadruplet SIMPL::Rgb
     */
    virtual SIMPL::Rgb generateMisorientationColor(const QuatF& q, const QuatF& refFrame);

    /**
     * @brief generatePoleFigure This method will generate a number of pole figures for this crystal symmetry and the Euler
     * angles that are passed in.
     * @param eulers The Euler Angles to generate the pole figure from.
     * @param imageSize The size in Pixels of the final RGB Image.
     * @param numColors The number of colors to use in the RGB Image. Less colors can give the effect of contouring.
     * @return A QVector of UInt8ArrayType pointers where each one represents a 2D RGB array that can be used to initialize
     * an image object from other libraries and written out to disk.
     */
    virtual QVector<UInt8ArrayType::Pointer> generatePoleFigure(PoleFigureConfiguration_t& config);

    /**
     * @brief generateStandardTriangle Generates an RGBA array that is a color "Standard" IPF Triangle Legend used for IPF Color Maps.
     * @return
     */
    virtual UInt8ArrayType::Pointer generateIPFTriangleLegend(int imageDim);

  protected:
    float _calcMisoQuat(const QuatF quatsym[6], int numsym,
                        QuatF& q1, QuatF& q2,
                        float& n1, float& n2, float& n3);

  private:
    TrigonalLowOps(const TrigonalLowOps&) = delete; // Copy Constructor Not Implemented
    void operator=(const TrigonalLowOps&) = delete; // Move assignment Not Implemented
};

#endif /* TrigonalLowOPS_H_ */

