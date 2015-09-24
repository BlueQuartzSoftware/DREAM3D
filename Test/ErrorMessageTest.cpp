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


#include <string>

#include "SIMPLib/SIMPLib.h"
#include "SIMPLib/Common/AbstractFilter.h"
#include "SIMPLib/Common/PipelineMessage.h"
#include "SIMPLib/Utilities/UnitTestSupport.hpp"

#include "DREAM3DTestFileLocations.h"

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void RemoveTestFiles()
{

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void TestErrorMessage()
{
  PipelineMessage e0;

  QString a0("Some Class Name");
  QString a1("Description");
  int eCode = -10;
  PipelineMessage e1;


  PipelineMessage e2(a0, a1, eCode);

  AbstractFilter::Pointer f = AbstractFilter::New();
  f->notifyErrorMessage("Some Test", a1, eCode);
  f->notifyErrorMessage("Another Test", "A description", -10);

  PipelineMessage pm("Joey's Test", "Testing Warning Message...", -23, PipelineMessage::Warning);
  PipelineMessage pm1("Joey's Test", "Testing Error Message...", -23, PipelineMessage::Error);
  PipelineMessage pm2("Joey's Test", "Testing Status Message...", -23, PipelineMessage::StatusMessage);
  PipelineMessage pm3("Joey's Test", "Testing Status Message...", -23, PipelineMessage::ProgressValue, 23);
  f->broadcastPipelineMessage(pm);
  f->broadcastPipelineMessage(pm1);
  f->broadcastPipelineMessage(pm2);

  if (true)
  { return; }

}

// -----------------------------------------------------------------------------
//  Use test framework
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{

  int err = EXIT_SUCCESS;

#if !REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif
  DREAM3D_REGISTER_TEST( TestErrorMessage() )

#if REMOVE_TEST_FILES
  DREAM3D_REGISTER_TEST( RemoveTestFiles() )
#endif
  PRINT_TEST_SUMMARY();
  return err;
}


