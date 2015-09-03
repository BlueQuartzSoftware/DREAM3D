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


#ifndef EMMPM_STRUCTURES_H_
#define EMMPM_STRUCTURES_H_

// C Includes
#include <stddef.h>

// C++ Includes
#include <vector>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/SIMPLibSetGetMacros.h"

// emmpm includes
#include "EMMPMLib/EMMPMLib.h"
#include "EMMPMLib/Core/EMMPM_Constants.h"


typedef float real_t;


typedef struct
{
  int label_1;
  int label_2;
  real_t beta;
} CoupleType;

/**
 * @struct EMMPM_Data emmpm/public/EMMPM_Structures.h EMMPM_Structures.h
 * Holds variables that are needed by the various functions through out
 * the EM/MPM algorithm. Packing them into a struct allows them to be easily
 * passed between functions.
 */
class EMMPMLib_EXPORT EMMPM_Data
{
  public:
    SIMPL_SHARED_POINTERS(EMMPM_Data)
    SIMPL_STATIC_NEW_MACRO(EMMPM_Data)
    SIMPL_TYPE_MACRO(EMMPM_Data)

    virtual ~EMMPM_Data();

    int allocateDataStructureMemory();

    /**
     * @brief allocateOutputImage
     * @param width
     * @param height
     * @param samplesPerPixel
     */
    void allocateOutputImage();

    void initVariables();

    void calculateBetaMatrix(double default_beta);


    // -----------------------------------------------------------------------------
    //  Inputs from Command line or GUI program
    // -----------------------------------------------------------------------------
    int emIterations; /**<  */
    int mpmIterations; /**<  */
    real_t in_beta; /**<  */
    int classes; /**<  */
    unsigned int rows; /**< The height of the image.  Applicable for both input and output images */
    unsigned int columns; /**< The width of the image. Applicable for both input and output images */
    unsigned int dims; /**< The number of vector elements in the image.*/
    enum EMMPM_InitializationType initType;  /**< The type of initialization algorithm to use  */
    unsigned int initCoords[EMMPM_MAX_CLASSES][4];  /**<  MAX_CLASSES rows x 4 Columns  */
    //unsigned int grayTable[EMMPM_MAX_CLASSES];
    unsigned int colorTable[EMMPM_MAX_CLASSES];
    real_t min_variance[EMMPM_MAX_CLASSES]; /**< The minimum value that the variance can be for each class */
    char simulatedAnnealing; /**<  */
    char verbose; /**<  */


    // -----------------------------------------------------------------------------
    //  Input/output File names and raw storage
    // -----------------------------------------------------------------------------
    char* input_file_name;/**< The input file name */
    unsigned char* inputImage; /**< The raw image data that is used as input to the algorithm */
    char* output_file_name; /**< The name of the output file */
    unsigned char* outputImage; /**< The raw output image data which can be allocated by the library or the calling function. */
    unsigned char inputImageChannels; /**< The number of color channels in the input image */

    // -----------------------------------------------------------------------------
    //  Working Vars section - Internal Variables to the algorithm
    // -----------------------------------------------------------------------------
    unsigned char* y; /**< height*width*dims array of bytes */
    unsigned char* xt; /**< width*height array of bytes */

    real_t w_gamma[EMMPM_MAX_CLASSES]; /**<  Gamma */
    real_t* mean; /**< Mu or Mean   { classes * dims array (classes is slowest moving dimension) }*/
    real_t* variance; /**< Variance or Sigma Squared  { classes * dims array (classes is slowest moving dimension) }*/
    real_t* prev_mu; /**< Previous EM Loop Value of Mu or Mean   { classes * dims array (classes is slowest moving dimension) }*/
    real_t* prev_variance; /**< Previous EM Loop Value of Variance or Sigma Squared  { classes * dims array (classes is slowest moving dimension) }*/
    real_t  N[EMMPM_MAX_CLASSES]; /**< Dimensions { classes * dims array (classes is slowest moving dimension )}*/
    real_t* probs; /**< Probabilities for each pixel  classes * rows * cols (slowest to fastest)*/
    real_t  workingKappa; /**< Current Kappa Value being used  */
    real_t* couplingBeta; /**< Beta Matrix for Coupling */
    std::vector<CoupleType> coupleEntries; /**< The entries that map the 2 classes that will be coupled along with their value  */

    // -----------------------------------------------------------------------------
    //  Curvature Penalty Function Related variables
    // -----------------------------------------------------------------------------
    char   useCurvaturePenalty; /**<  Use the curvature Penalty function */
    char   useGradientPenalty; /**< Use the Gradient Penalty function */
    size_t ccostLoopDelay; /**<  How many em loops until the Curvature Penalty is calculated   */
    real_t beta_e; /**<  */
    real_t beta_c; /**<  */
    real_t r_max; /**<  */
    real_t* ccost; /**< classes * rows * cols (slowest to fastest)*/
    real_t* ns; /**<  */
    real_t* ew; /**<  */
    real_t* sw; /**<  */
    real_t* nw; /**<  */

    // -----------------------------------------------------------------------------
    // Variables to hold the tolerance for the stopping condition
    // -----------------------------------------------------------------------------
    char   useStoppingThreshold;
    real_t stoppingThreshold;
    real_t currentMSE; /**< Current Mean Squared Error value */

    // -----------------------------------------------------------------------------
    //  Variables that Functions may need to present progress information to the user
    // -----------------------------------------------------------------------------
    int    currentEMLoop; /**< The current EM Loop  */
    int    currentMPMLoop; /**< The current MPM Loop  */
    float  progress; /**< A Percentage to indicate how far along the algorthm is.*/
    char inside_em_loop;
    char inside_mpm_loop;


    // -----------------------------------------------------------------------------
    //  Variables to hold the histograms for each class weighted by area fraction
    // -----------------------------------------------------------------------------
    real_t* histograms; /**< classes * dims * 256 */

    // -----------------------------------------------------------------------------
    //  User defined Pointer that can point to anything
    // -----------------------------------------------------------------------------
    void*    userData; /**< User defined Pointer that can point to anything */
    char     cancel;

    // -----------------------------------------------------------------------------
    // Some Critical TIFF related fields which are taken directly from the tifftags
    //  of the input image. These really only appply for the command line version
    // -----------------------------------------------------------------------------
    char           tiffResSet;
    unsigned short xResolution;
    unsigned short yResolution;
    unsigned short resolutionUnits;

  protected:
    EMMPM_Data();
  private:





    EMMPM_Data(const EMMPM_Data&); // Copy Constructor Not Implemented
    void operator=(const EMMPM_Data&); // Operator '=' Not Implemented
};

#endif /* EMMPM_STRUCTURES_H_ */
