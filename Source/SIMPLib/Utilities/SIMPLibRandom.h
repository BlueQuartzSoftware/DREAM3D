/* ============================================================================
* Copyright (c) 2009-2015 BlueQuartz Software, LLC
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


/* This class uses the Mersenne Twister pseudorandom number generator code internally.
 * This class should be thread safe as long as only a single thread uses any particular
 * instance of this class.
 */

#ifndef _SIMPLibRANDOM_H_
#define _SIMPLibRANDOM_H_

#include <QtCore/QDateTime>

#include "SIMPLib/SIMPLib.h"

#define MERSENNNE_TWISTER_N 624
#define MERSENNNE_TWISTER_M 397

/**
 * @file SIMPLibRandom
 * @brief This class uses the Mersenne Twister pseudorandom number generator code internally.
 * This class should be thread safe as long as only a single thread uses any particular
 * instance of this class.
 *
 * @verbatim
 * This is a Mersenne Twister pseudorandom number generator
with period 2^19937-1 with improved initialization scheme,
modified on 2002/1/26 by Takuji Nishimura and Makoto Matsumoto.
modified on 2005/4/26 by Mutsuo Saito

Contents of this tar ball:
readme-mt.txt  this file
mt19937ar.c  the C source (ar: initialize by ARray)
mt19937ar.h      the C header file for mt19937ar
mtTest.c         the C test main program of mt19937ar.c
mt19937ar.out  Test outputs of six types generators. 1000 for each

1. Initialization
  The initialization scheme for the previous versions of MT
(e.g. 1999/10/28 version or earlier) has a tiny problem, that
the most significant bits of the seed is not well reflected
to the state vector of MT.

This version (2002/1/26) has two initialization schemes:
init_genrand(seed) and init_by_array(init_key, key_length).

init_genrand(seed) initializes the state vector by using
one unsigned 32-bit integer "seed", which may be zero.

init_by_array(init_key, key_length) initializes the state vector
by using an array init_key[] of unsigned 32-bit integers
of length key_kength. If key_length is smaller than 624,
then each array of 32-bit integers gives distinct initial
state vector. This is useful if you want a larger seed space
than 32-bit word.

2. Generation
After initialization, the following type of pseudorandom numbers
are available.

genrand_int32() generates unsigned 32-bit integers.
genrand_int31() generates unsigned 31-bit integers.
genrand_real1() generates uniform real in [0,1] (32-bit resolution).
genrand_real2() generates uniform real in [0,1) (32-bit resolution).
genrand_real3() generates uniform real in (0,1) (32-bit resolution).
genrand_res53() generates uniform real in [0,1) with 53-bit resolution.

Note: the last five functions call the first one.
if you need more speed for these five functions, you may
suppress the function call by copying genrand_int32() and
replacing the last return(), following to these five functions.

3. main()
main() is an example to initialize with an array of length 4,
then 1000 outputs of unsigned 32-bit integers,
then 1000 outputs of real [0,1) numbers.

4. The outputs
The output of the mt19937ar.c is in the file mt19937ar.out.
If you revise or translate the code, check the output
by using this file.

5. Cryptography
This generator is not cryptoraphically secure.
You need to use a one-way (or hash) function to obtain
a secure random sequence.

6. Correspondence
See:
URL http://www.math.keio.ac.jp/matumoto/emt.html
email matumoto@math.keio.ac.jp, nisimura@sci.kj.yamagata-u.ac.jp

7. Reference
M. Matsumoto and T. Nishimura,
"Mersenne Twister: A 623-Dimensionally Equidistributed Uniform
Pseudo-Random Number Generator",
ACM Transactions on Modeling and Computer Simulation,
Vol. 8, No. 1, January 1998, pp 3--30.

-------
Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
All rights reserved.
Copyright (C) 2005, Mutsuo Saito
All rights reserved.
 *
 * @endverbatim
 */
class SIMPLib_EXPORT SIMPLibRandom
{
  public:
    SIMPLibRandom();
    virtual ~SIMPLibRandom();

    /* initializes mt[N] with a seed */
    void init_genrand(unsigned long s);

    /* initialize by an array with array-length */
    /* init_key is the array for initializing keys */
    /* key_length is its length */
    /* slight change for C++, 2004/2/26 */
    void init_by_array(unsigned long init_key[], int key_length);

    /* generates a random number on [0,0xffffffff]-interval */
    unsigned long genrand_int32();

    /* generates a random number on [0,0x7fffffff]-interval */
    long genrand_int31();

    /* These real versions are due to Isaku Wada, 2002/01/09 added */
    /* generates a random number on [0,1]-real-interval */
    double genrand_real1();

    /* generates a random number on [0,1)-real-interval */
    double genrand_real2();

    /* generates a random number on (0,1)-real-interval */
    double genrand_real3();

    /* generates a random number on [0,1) with 53-bit resolution*/
    double genrand_res53();


    double genrand_beta(double a, double b);

    double genrand_norm(double a, double b);


  private:
    unsigned long mt[MERSENNNE_TWISTER_N]; /* the array for the state vector  */
    int mti; // =N+1; /* mti==N+1 means mt[N] is not initialized */
};


#ifdef CMP_WORDS_BIGENDIAN
#define AIMRNG_OFFSET 1
#else
#define AIMRNG_OFFSET 0
#endif

/* Get a seed value based off the system clock. The issue is that this will
  * be a 64 bit unsigned integer where the high 32 bits will basically not
  * change where as the lower 32 bits will. The following lines of code will
  * pull off the low 32 bits from the number. This operation depends on most
  * significant byte ordering which is different between Big Endian and
  * Little Endian machines. For Big endian machines the Most Significant Byte
  * (MSB) is the first 32 bits. For Little Endian machines the MSB is the
  * second 32 bits.
  */

#define SIMPL_RANDOMNG_NEW()\
  SIMPLibRandom rg;\
  {\
    unsigned long long int millisSinceEpoch = QDateTime::currentMSecsSinceEpoch();\
    unsigned long long int* millisSinceEpochPtr = &millisSinceEpoch;\
    unsigned int* seedPtr = reinterpret_cast<unsigned int*>(millisSinceEpochPtr);\
    rg.init_genrand(seedPtr[AIMRNG_OFFSET]);\
  }

#define SIMPL_RANDOMNG_NEW_SEEDED(seed)\
  SIMPLibRandom rg;\
  {\
    unsigned int* seedPtr = reinterpret_cast<unsigned int*> (&seed);\
    rg.init_genrand(seedPtr[AIMRNG_OFFSET]);\
  }

#endif /* SIMPLibRANDOM_H_ */

