///////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2009, Michael A. Jackson. BlueQuartz Software
//  Copyright (c) 2009, Michael Groeber, US Air Force Research Laboratory
//  All rights reserved.
//  BSD License: http://www.opensource.org/licenses/bsd-license.html
//
// This code was partly written under US Air Force Contract FA8650-07-D-5800
//
///////////////////////////////////////////////////////////////////////////////

#include <MXA/Utilities/MXAFileSystemPath.h>
#include <AIM/ANG/AngDirectoryPatterns.h>
#include <DREAM3D/Common/Grain.h>

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




void test(const std::string &curAbsFile,
                 const std::string &mosDirPrefix,
                 const std::string &zeissExportDirSuffix)
{
  std::string fn = MXAFileSystemPath::filename(curAbsFile);
  //std::cout << logTime() << "Found: '" << *dir_iter << "'" << std::endl;
  std::string::size_type front = fn.find_first_of(mosDirPrefix);
  std::string::size_type back = fn.find_last_of(zeissExportDirSuffix);
  if (front != std::string::npos && back != std::string::npos)
  {
    std::cout << "FileName: " << fn << std::endl;
    std::string::size_type length = fn.length() - mosDirPrefix.size() - zeissExportDirSuffix.size();
    std::string::size_type pos = mosDirPrefix.size();

    std::cout << "Dirty String: '" << fn.substr(pos, length) << "'" << std::endl;
    const char* data = fn.data();
    while (data[pos] < '0' || data[pos] > '9' )
    {
      ++pos;
      --length;
    }
    std::cout << "Clean String: '" << fn.substr(pos, length) << "'" <<  std::endl;
    std::string sliceValue = fn.substr( pos, length );
  }

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{

  GrainTest();


  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New("/Users/Shared/Data/AIMBlades/ANG_Series", "Slice_", 0);

  std::string dirName = "/Users/Shared/Data/AIMBlades/ANG_Series";
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
