
#include <stdlib.h>

#include <vector>
#include <sstream>
#include <iostream>

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Common/IO/MXAFileReader64.h"
#include "MXA/Utilities/MXADir.h"

#include "TiffUtilities.h"

typedef struct
{
    int x_index;
    int y_index;
    int var2;
    int width;
    int height;
    int var5;
    uint8_t flag;
} FrameDataHeader_t;

namespace Ctf
{
  static size_t HeaderByteSize = 25;
}

int main(int argc, char **argv)
{
  if (argc < 3)
  {
    std::cout << "This program requires 2 inputs. The input pattern file and the output directory" << std::endl;
    return EXIT_FAILURE;
  }

  uint64_t millis = MXA::getMilliSeconds();

  MXAFileReader64 reader(argv[1]);

  bool isOpen = reader.initReader();
  if(isOpen == false)
  {
    std::cout << "Could not open .bcf file " << argv[1] << std::endl;
    return EXIT_FAILURE;
  }

  FrameDataHeader_t header;
  size_t totalPatterns = 0;
  int prevYIndex = -1;
  for (int i = 0; i < 7500; ++i)
  {
    ::memset(reinterpret_cast<uint8_t*>(&header), 0xAB, Ctf::HeaderByteSize);

//    std::cout << "----------------" << std::endl;
//    std::cout << "File Pointer:" << reader.getFilePointer64() << std::endl;
    reader.readArray(reinterpret_cast<uint8_t*>(&header), Ctf::HeaderByteSize);
//    std::cout << "x_index: " << header.x_index << std::endl;
    if (header.y_index != prevYIndex) {
      std::cout << "y_index: " << header.y_index << std::endl;
      prevYIndex = header.y_index;
    }
//    std::cout << "var2: " << header.var2 << std::endl;
//    std::cout << "width: " << header.width << std::endl;
 //   std::cout << "height: " << header.height << std::endl;
//    std::cout << "var5: " << header.var5 << std::endl;
 //   std::cout << "flag:" << (int)(header.flag) << std::endl;


    size_t count = header.width * header.height;
    std::vector<uint8_t> image(count);

    reader.readArray<uint8_t>(&(image.front()), count);

    totalPatterns++;

    TiffUtilities tiffUtil;
    std::stringstream ss;
    MXADir::mkdir(argv[2], true);
    ss <<argv[2] << MXADir::Separator << header.x_index << "_" << header.y_index << "_pattern.tif";
    std::stringstream comment;
    comment << "Kikuchi Pattern at x="<< header.x_index << ", y=" << header.y_index;
    tiffUtil.writeGrayScaleImage(ss.str().c_str(), header.height, header.width, comment.str().c_str(), &(image.front()));
  }

  std::cout << "Seconds to Complete " << totalPatterns << " Patterns "
    << (MXA::getMilliSeconds() - millis) << std::endl;

  return EXIT_SUCCESS;
}

