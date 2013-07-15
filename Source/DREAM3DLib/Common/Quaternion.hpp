/* ============================================================================
 * Copyright (c) 2012 Michael A. Jackson (BlueQuartz Software)
 * Copyright (c) 2012 Dr. Michael A. Groeber (US Air Force Research Laboratories)
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
 *                           FA8650-07-D-5800
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#ifndef _Quaternion_H_
#define _Quaternion_H_


template<typename T>
class QuaternionOps
{
public:

    typedef struct {
        T x;
        T y;
        T z;
        T w;
    } Quat_t;

    typedef struct {
        T d[4];
    } Vec4_t;

    typedef union {
        Quat_t quat;
        Vec4_t vec4;
    } Quaternion_t;

    static void Multiply(const Quaternion_t &q1, const Quaternion_t &q2, Quaternion_t &out)
    {
      out.quat.x = q2.quat.x * q1.quat.w + q2.quat.w * q1.quat.x + q2.quat.z * q1.quat.y - q2.quat.y * q1.quat.z;
      out.quat.y = q2.quat.y * q1.quat.w + q2.quat.w * q1.quat.y + q2.quat.x * q1.quat.z - q2.quat.z * q1.quat.x;
      out.quat.z = q2.quat.z * q1.quat.w + q2.quat.w * q1.quat.z + q2.quat.y * q1.quat.x - q2.quat.x * q1.quat.y;
      out.quat.w = q2.quat.w * q1.quat.w - q2.quat.x * q1.quat.x - q2.quat.y * q1.quat.y - q2.quat.z * q1.quat.z;
    }

    static void Multiply(const Quat_t &q1, const Quat_t &q2, Quat_t &out)
    {
      out.x = q2.x * q1.w + q2.w * q1.x + q2.z * q1.y - q2.y * q1.z;
      out.y = q2.y * q1.w + q2.w * q1.y + q2.x * q1.z - q2.z * q1.x;
      out.z = q2.z * q1.w + q2.w * q1.z + q2.y * q1.x - q2.x * q1.y;
      out.w = q2.w * q1.w - q2.x * q1.x - q2.y * q1.y - q2.z * q1.z;
    }

    static void Multiply(const Vec4_t &q1, const Vec4_t &q2, Vec4_t &out)
    {
      out.d[0] = q2.d[0] * q1.d[3] + q2.d[3] * q1.d[0] + q2.d[2] * q1.d[1] - q2.d[1] * q1.d[2];
      out.d[1] = q2.d[1] * q1.d[3] + q2.d[3] * q1.d[1] + q2.d[0] * q1.d[2] - q2.d[2] * q1.d[0];
      out.d[2] = q2.d[2] * q1.d[3] + q2.d[3] * q1.d[2] + q2.d[1] * q1.d[0] - q2.d[0] * q1.d[1];
      out.d[3] = q2.d[3] * q1.d[3] - q2.d[0] * q1.d[0] - q2.d[1] * q1.d[1] - q2.d[2] * q1.d[2];
    }

    static void Multiply(const T* q1, const T* q2, T* out)
    {
      out[0] = q2[0] * q1[3] + q2[3] * q1[0] + q2[2] * q1[1] - q2[1] * q1[2];
      out[1] = q2[1] * q1[3] + q2[3] * q1[1] + q2[0] * q1[2] - q2[2] * q1[0];
      out[2] = q2[2] * q1[3] + q2[3] * q1[2] + q2[1] * q1[0] - q2[0] * q1[1];
      out[3] = q2[3] * q1[3] - q2[0] * q1[0] - q2[1] * q1[1] - q2[2] * q1[2];
    }

protected:
  QuaternionOps();
};


typedef QuaternionOps<float> QuaternionOpsF;
typedef QuaternionOps<double> QuaternionOpsD;


#endif /* _Quaternion_H_ */
