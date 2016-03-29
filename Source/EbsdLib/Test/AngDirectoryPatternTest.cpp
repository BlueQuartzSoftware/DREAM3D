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


#include <MXA/Utilities/MXAFileSystemPath.h>
#include <EbsdLib/TSL/AngDirectoryPatterns.h>
#include "SIMPLib/Common/Grain.h"

//--C++ Includes
#include <string>
#include <iostream>

//-- C Includes
#include <stdlib.h>

#define FILENAME "/Users/Shared/Data/RoboMet.3D/MNML-5/MNML-5_500x_0_821.mxa"


void GrainTest()
{
  Grain* grains = new Grain[10];

  grains[1].euler1 = 0.0;
  std::cout << grains[5].Ixx << std::endl;
  std::cout << "Done" << std::endl;
}




void test(const QString& curAbsFile,
          const QString& mosDirPrefix,
          const QString& zeissExportDirSuffix)
{
  QString fn = MXAFileSystemPath::filename(curAbsFile);
  //std::cout << logTime() << "Found: '" << *dir_iter << "'" << std::endl;
  QString::size_type front = fn.find_first_of(mosDirPrefix);
  QString::size_type back = fn.find_last_of(zeissExportDirSuffix);
  if (front != QString::npos && back != QString::npos)
  {
    std::cout << "FileName: " << fn << std::endl;
    QString::size_type length = fn.length() - mosDirPrefix.size() - zeissExportDirSuffix.size();
    QString::size_type pos = mosDirPrefix.size();

    std::cout << "Dirty String: '" << fn.substr(pos, length) << "'" << std::endl;
    const char* data = fn.data();
    while (data[pos] < '0' || data[pos] > '9' )
    {
      ++pos;
      --length;
    }
    std::cout << "Clean String: '" << fn.substr(pos, length) << "'" <<  std::endl;
    QString sliceValue = fn.substr( pos, length );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char** argv)
{

  GrainTest();


  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New("/Users/Shared/Data/AIMBlades/ANG_Series", "Slice_", 0);

  QString dirName = "/Users/Shared/Data/AIMBlades/ANG_Series";
  std::cout << p->generateAngFileName(9) << std::endl;



//  std::cout << "Slice Extracted is: " << slice << std::endl;
//  std::cout << "Zeiss Meta XML : " << p->generateZeissMetaXMLName(10) << std::endl;
//  std::cout << "Zeiss Tile Image: " << p->generateZeissTileName(10, 2, 5) << std::endl;

  std::cout << "-=--------------------------" << std::endl;

//  zDName = "Section 25.tif";
//  guessPrefix = "Section ";
//  p->parseDocumentName(guessPrefix, zDName);
//   dirName = p->generateZeissDirectory(25);
//  std::cout << "Zeiss Directory: " << dirName << std::endl;
//  slice = p->extractSlice(dirName);
//  std::cout << "Slice Extracted is: " << slice << std::endl;
//  std::cout << "Zeiss Meta XML : " << p->generateZeissMetaXMLName(25) << std::endl;
//  std::cout << "Zeiss Tile Image: " << p->generateZeissTileName(25, 3, 50) << std::endl;


  return 1;
}
