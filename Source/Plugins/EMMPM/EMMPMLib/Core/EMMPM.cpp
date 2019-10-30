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

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "SIMPLib/Messages/AbstractMessageHandler.h"
#include "SIMPLib/Messages/GenericProgressMessage.h"
#include "SIMPLib/Messages/GenericStatusMessage.h"
#include "SIMPLib/Messages/GenericErrorMessage.h"
#include "SIMPLib/Messages/GenericWarningMessage.h"

#include "EMMPMLib/Common/EMMPM_Math.h"
#include "EMMPMLib/Common/EMTime.h"
#include "EMMPMLib/Common/MSVCDefines.h"
#include "EMMPMLib/Core/EMCalculation.h"
#include "EMMPMLib/Core/EMMPMUtilities.h"
#include "EMMPMLib/Core/InitializationFunctions.h"
#include "EMMPMLib/EMMPMLib.h"

/**
 * @brief This message handler is used by EMMPM instances to re-emit incoming generic messages from the
 * EMCalculation observable object as its own generic messages
 */
class EMMPMMessageHandler : public AbstractMessageHandler
{
  public:
    explicit EMMPMMessageHandler(EMMPM* emmpmObj) : m_EmmpmObject(emmpmObj) {}

    /**
     * @brief Re-emits incoming GenericProgressMessages as FilterProgressMessages.
     */
    void processMessage(const GenericProgressMessage* msg) const override
    {
      emit m_EmmpmObject->notifyProgressMessage(msg->getProgressValue(), msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericStatusMessages as FilterStatusMessages.
     */
    void processMessage(const GenericStatusMessage* msg) const override
    {
      emit m_EmmpmObject->notifyStatusMessage(msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericErrorMessages as FilterErrorMessages.
     */
    void processMessage(const GenericErrorMessage* msg) const override
    {
      emit m_EmmpmObject->setErrorCondition(msg->getCode(), msg->getMessageText());
    }

    /**
     * @brief Re-emits incoming GenericWarningMessages as FilterWarningMessages.
     */
    void processMessage(const GenericWarningMessage* msg) const override
    {
      emit m_EmmpmObject->setWarningCondition(msg->getCode(), msg->getMessageText());
    }

  private:
    EMMPM* m_EmmpmObject = nullptr;
};

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM::EMMPM()
: m_ErrorCode(0)
{
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
EMMPM::~EMMPM() = default;

#define PRINT_DATA(var) printf("%s: %d\n", #var, data->var);

#define PRINT_DATA_DOUBLE(var) printf("%s: %f\n", #var, data->var);

#define PRINT_DATA_CHAR(var) printf("%s: %s\n", #var, data->var);

#define PRINT_CHAR_ARRAY(var)                                                                                                                                                                          \
  printf("%s[MAX_CLASSES]; ", #var);                                                                                                                                                                   \
  for(i = 0; i < EMMPM_MAX_CLASSES; i++)                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    printf("%d  ", data->var[i]);                                                                                                                                                                      \
  }                                                                                                                                                                                                    \
  printf("\n");

#define PRINT_DOUBLE_ARRAY(var)                                                                                                                                                                        \
  printf("%s[MAX_CLASSES]; ", #var);                                                                                                                                                                   \
  for(i = 0; i < EMMPM_MAX_CLASSES; i++)                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    printf("%f  ", data->var[i]);                                                                                                                                                                      \
  }                                                                                                                                                                                                    \
  printf("\n");

#define PRINT_INT_ARRAY(var)                                                                                                                                                                           \
  printf("%s[MAX_CLASSES]; ", #var);                                                                                                                                                                   \
  for(i = 0; i < EMMPM_MAX_CLASSES; i++)                                                                                                                                                               \
  {                                                                                                                                                                                                    \
    printf("%d  ", data->var[i]);                                                                                                                                                                      \
  }                                                                                                                                                                                                    \
  printf("\n");

#define PRINT_UINT_ARRAY(var)                                                                                                                                                                          \
  {                                                                                                                                                                                                    \
    printf("%s[MAX_CLASSES]; ", #var);                                                                                                                                                                 \
    unsigned int ui;                                                                                                                                                                                   \
    for(i = 0; i < EMMPM_MAX_CLASSES; i++)                                                                                                                                                             \
    {                                                                                                                                                                                                  \
      ui = data->var[i];                                                                                                                                                                               \
      printf("%u  ", ui);                                                                                                                                                                              \
    }                                                                                                                                                                                                  \
    printf("\n");                                                                                                                                                                                      \
  }

#define PRINT_2D_UINT_ARRAY(var, r, c)                                                                                                                                                                 \
  {                                                                                                                                                                                                    \
    printf("%s[%s][%s];\n  ", #var, #r, #c);                                                                                                                                                           \
    for(i = 0; i < r; i++)                                                                                                                                                                             \
    {                                                                                                                                                                                                  \
      for(j = 0; j < c; j++)                                                                                                                                                                           \
      {                                                                                                                                                                                                \
        ui = data->var[i][j];                                                                                                                                                                          \
        printf("%u  ", ui);                                                                                                                                                                            \
      }                                                                                                                                                                                                \
      printf("\n  ");                                                                                                                                                                                  \
    }                                                                                                                                                                                                  \
  }

#define PRINT_POINTER(var) printf("%p\n", data->var);

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

void EMMPM::printData(EMMPM_Data::Pointer data)
{
#if 0
  int i = 0;
  int j = 0;
  unsigned int ui;
  PRINT_DATA(emIterations)
  PRINT_DATA( mpmIterations); /**<  */
  PRINT_DATA_DOUBLE( in_beta); /**<  */
// PRINT_DATA_DOUBLE( in_gamma); /**<  */
  PRINT_DATA( classes); /**<  */
  PRINT_DATA( rows); /**< The height of the image.  Applicable for both input and output images */
  PRINT_DATA( columns); /**< The width of the image. Applicable for both input and output images */
  PRINT_DATA( channels); /**< The number of color channels in the images. This should always be 1 */
  PRINT_DATA( initType); /**< The type of initialization algorithm to use  */
  PRINT_2D_UINT_ARRAY( initCoords, EMMPM_MAX_CLASSES, 4); /**<  MAX_CLASSES rows x 4 Columns  */

  PRINT_DATA( simulatedAnnealing); /**<  */

  PRINT_INT_ARRAY( grayTable);
  PRINT_DATA( verbose); /**<  */

  PRINT_DATA_CHAR( input_file_name);/**< The input file name */
  PRINT_POINTER( inputImage); /**< The raw image data that is used as input to the algorithm */
  PRINT_DATA_CHAR( output_file_name); /**< The name of the output file */
  PRINT_POINTER( outputImage); /**< The raw output image data which can be allocated by the library or the calling function. */

  PRINT_DATA_DOUBLE( y); /**<  */
  PRINT_DATA_DOUBLE( xt); /**<  */
  PRINT_DOUBLE_ARRAY( w_gamma); /**<  */

//  PRINT_DATA_DOUBLE( x); /**<  */
  PRINT_DOUBLE_ARRAY( m); /**<  */
  PRINT_DOUBLE_ARRAY( v); /**<  */
  PRINT_DOUBLE_ARRAY( N); /**<  */
  PRINT_DOUBLE_ARRAY( probs); /**<  */
  PRINT_DATA_DOUBLE( workingBeta); /**<  */

  PRINT_DATA( currentEMLoop); /**< The current EM Loop  */
  PRINT_DATA( currentMPMLoop); /**< The current MPM Loop  */
  PRINT_DATA_DOUBLE( progress); /**< A Percentage to indicate how far along the algorthm is.*/

  PRINT_DATA(userData); /**< User defined Pointer that can point to anything */
#endif
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void EMMPM::execute()
{
  EMMPM_Data* data = getData().get();

  char msgbuff[256];
  // unsigned long long int millis = 0;
  memset(msgbuff, 0, 256);

  // Copy the input image into data->y arrays
  EMMPMUtilities::ConvertInputImageToWorkingImage(m_Data);
  //  millis = EMMPM_getMilliSeconds();
  //  millis = millis << 32; // push off the high bits
  //  millis = millis >> 32; // bring back the low bits
  // data->rngVars = init_genrand( (unsigned long)(millis));

  if(data->cancel != 0)
  {
    data->progress = 100.0;
    return;
  }

  /* Initialize the Curvature Penalty variables:  */
  data->ccost = nullptr;
  if(data->useCurvaturePenalty != 0)
  {
    CurvatureInitialization::Pointer curvatureInit = CurvatureInitialization::New();
    curvatureInit->initCurvatureVariables(m_Data);

    if(data->ccost == nullptr)
    {
      setErrorCondition(-55100, "Error Allocating Curvature Variables Memory");
      return;
    }
  }

  /* Initialize the Edge Gradient Penalty variables */
  data->nw = nullptr;
  data->ew = nullptr;
  data->sw = nullptr;
  data->nw = nullptr;
  if(data->useGradientPenalty != 0)
  {
    GradientVariablesInitialization::Pointer gradientInit = GradientVariablesInitialization::New();
    gradientInit->initialize(m_Data);

    if(data->ns == nullptr || data->ew == nullptr || data->nw == nullptr || data->sw == nullptr)
    {
      setErrorCondition(-55000, "Error Allocating Gradient Variables Memory");
      return;
    }
  }

  /* Initialization of parameter estimation */
  m_InitializationFunction->initialize(m_Data);

  /* Initialize classification of each pixel randomly with a uniform disribution */
  XtArrayInitialization::Pointer xtInit = XtArrayInitialization::New();
  xtInit->initialize(m_Data);

  if(data->cancel != 0)
  {
    data->progress = 100.0;
    return;
  }

#if 0
  /* Allocate space for the output image, and copy a scaled xt
   * and then write the output image.*/
  EMMPM_ConvertXtToOutputImage(data, callbacks);
  char* oimagefname = data->output_file_name;
  data->output_file_name = "/tmp/Initial_Random_Image.tif";
  EMMPM_WriteGrayScaleTiff(data, callbacks, "Initial Random Image");
  data->output_file_name = oimagefname;
#endif

  printData(m_Data);

  // Now actually run the EM/MPM algorithm
  EMCalculation::Pointer em = EMCalculation::New();
  em->setData(getData());
  em->setStatsDelegate(getStatsDelegate());

  // Connect up the Error/Warning/Progress object so the filter can report those things
  connect(em.get(), &EMCalculation::messageGenerated, [=] (AbstractMessage::Pointer msg) {
    EMMPMMessageHandler msgHandler(this);
    msg->visit(&msgHandler);
  });

  em->execute();

  data->progress = 100.0;
  m_StatsDelegate->reportProgress(getData());
  //  notify("", data->progress, UpdateProgressValue);
  //  notify("EM/MPM Completed.", 100, UpdateProgressValueAndMessage);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
QString EMMPM::getHumanLabel() const
{
  return "EMMPM";
}

// -----------------------------------------------------------------------------
EMMPM::Pointer EMMPM::NullPointer()
{
  return Pointer(static_cast<Self*>(nullptr));
}

// -----------------------------------------------------------------------------
EMMPM::Pointer EMMPM::New()
{
  Pointer sharedPtr(new(EMMPM));
  return sharedPtr;
}

// -----------------------------------------------------------------------------
QString EMMPM::getNameOfClass() const
{
  return QString("EMMPM");
}

// -----------------------------------------------------------------------------
QString EMMPM::ClassName()
{
  return QString("EMMPM");
}

// -----------------------------------------------------------------------------
void EMMPM::setData(const EMMPM_Data::Pointer& value)
{
  m_Data = value;
}

// -----------------------------------------------------------------------------
EMMPM_Data::Pointer EMMPM::getData() const
{
  return m_Data;
}

// -----------------------------------------------------------------------------
void EMMPM::setInitializationFunction(const InitializationFunction::Pointer& value)
{
  m_InitializationFunction = value;
}

// -----------------------------------------------------------------------------
InitializationFunction::Pointer EMMPM::getInitializationFunction() const
{
  return m_InitializationFunction;
}

// -----------------------------------------------------------------------------
void EMMPM::setErrorCode(int value)
{
  m_ErrorCode = value;
}

// -----------------------------------------------------------------------------
int EMMPM::getErrorCode() const
{
  return m_ErrorCode;
}

// -----------------------------------------------------------------------------
void EMMPM::setStatsDelegate(StatsDelegate* value)
{
  m_StatsDelegate = value;
}

// -----------------------------------------------------------------------------
StatsDelegate* EMMPM::getStatsDelegate() const
{
  return m_StatsDelegate;
}
