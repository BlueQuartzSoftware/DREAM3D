/*
 * AngDirectoryPatternTest.cpp
 *
 *  Created on: Oct 21, 2009
 *      Author: mjackson
 */
#include <MXA/Utilities/MXAFileSystemPath.h>
#include <AIM/ANG/AngDirectoryPatterns.h>
#include <AIM/Common/Grain.h>

//--C++ Includes
#include <string>
#include <iostream>

//-- C Includes
#include <stdlib.h>

#define FILENAME "/Users/Shared/Data/RoboMet.3D/MNML-5/MNML-5_500x_0_821.mxa"


void GPass(Grain test[])
{
  std::cout << "Grain ID: " << test[7].m_uid << std::endl;
}


void GrainTest()
{
  Grain* grains = new Grain[10];

  grains[1].euler1 = 0.0;
  std::cout << grains[5].Ixx << std::endl;

  GPass(grains);


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


  AngDirectoryPatterns::Pointer p = AngDirectoryPatterns::New();
  std::string zDName = "Slice_001.ang";
  std::string guessPrefix = "Slice_";
  p->parseDocumentName(guessPrefix, zDName);

  std::string dirName = "/Users/Shared/Data/AIMBlades/ANG_Series";
  int32 sliceCount = 1;
  int32 width = 0;
  int32 totalSlices = 150;
  while (sliceCount < totalSlices)
  {
    ++width;
    sliceCount *= 10;
  }
  std::cout << p->generateAngFileName(9, width) << std::endl;



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
