
#include <stdint.h>

#include <map>
#include <iostream>
#include <vector>



using MapType = std::map<uint64_t, float>;


typedef struct {
    int32_t scanWidth;
    int32_t scanHeight;
    int32_t patternWidth;
    int32_t patternHeight;
    int32_t zPoints;
} ScanInfo;

typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} KernelInfo;

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
float computeScalar(ScanInfo scanInfo, int32_t x, int32_t y, float* data)
{

  // Pointer Math....
  float* aPtr = data + (x * scanInfo.patternWidth * scanInfo.patternHeight);
  float* bPtr = data + (y * scanInfo.patternWidth * scanInfo.patternHeight);

  float value = -1.0f;
//  size_t patternTotal = scanInfo.patternWidth * scanInfo.patternHeight;
//  for(size_t i = 0; i < patternTotal; i++)
//  {
//    value =+ (aPtr[i] - bPtr[i]) * (aPtr[i] - bPtr[i]);
//  }
  return value;

}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void createMap(ScanInfo scanInfo, KernelInfo kernelInfo, float* data)
{
  uint64_t unique = 0;
  int32_t* p = reinterpret_cast<int32_t*>(&unique);

  MapType precomputedScalars;

  int32_t totalScanPoints = scanInfo.scanWidth * scanInfo.scanHeight;

  for(int32_t plane = 0; plane < scanInfo.zPoints; plane++)
  {

    for(int32_t row = 0; row < scanInfo.scanHeight; row++)
    {
   //   std::cout << "Row: " << row << std::endl;
      for(int32_t col = 0; col < scanInfo.scanWidth; col++)
      {
        int32_t point = (plane * scanInfo.scanWidth * scanInfo.scanHeight) + (row * scanInfo.scanWidth) + col;


        for(int32_t j = -kernelInfo.z; j < kernelInfo.z; j++)
        {
          int32_t kernelPlane = plane + j;
          if(kernelPlane < 0 || kernelPlane >= scanInfo.zPoints) { continue; }

          for(int32_t k = -kernelInfo.y; k < kernelInfo.y; k++)
          {
            int32_t kernelRow = row + k;
            if(kernelRow < 0 || kernelRow >= scanInfo.scanHeight) { continue; }
            for(int32_t l = -kernelInfo.x; l < kernelInfo.x; l++)
            {
              int32_t kernelCol = col + l;
              if(kernelCol < 0 || kernelCol >= scanInfo.scanWidth) { continue; }


              int32_t kernelPoint = (kernelPlane * scanInfo.scanWidth * scanInfo.scanHeight) + (kernelRow * scanInfo.scanWidth) + kernelCol;
                if (kernelPoint == point) { continue; }
                if (point < kernelPoint)
                {
                  p[0] = point;
                  p[1] = kernelPoint;
                }
                else
                {
                  p[1] = point;
                  p[0] = kernelPoint;
                }
                if(precomputedScalars.find(unique) == precomputedScalars.end())
                {
                 // std::cout << "Insert Key for " << point << ", " << kernelPoint << std::endl;
                  precomputedScalars[unique] = computeScalar(scanInfo, point, kernelPoint, data);
                }
            }
          }
        }
      }
    }
  }

  std::cout << "Map Size: " << precomputedScalars.size() << std::endl;
}



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
std::vector<float> CreateRandomData(ScanInfo &scanInfo)
{

  std::vector<float> v(scanInfo.scanWidth * scanInfo.scanHeight * scanInfo.patternWidth * scanInfo.patternHeight);
  // std::generate(v.begin(), v.end(), std::rand); // Using the C function rand()
  return v;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  std::cout << "Starting DRProcessing.... " << std::endl;
  std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

  ScanInfo scanInfo;
  scanInfo.scanWidth = 500;
  scanInfo.scanHeight = 500;
  scanInfo.patternWidth = 60;
  scanInfo.patternHeight = 60;
  scanInfo.zPoints = 1;

  KernelInfo kernelInfo;
  kernelInfo.x = 3;
  kernelInfo.y = 3;
  kernelInfo.z = 1;

  std::vector<float> data = CreateRandomData(scanInfo);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Allocate Pattern Data: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;

  start = std::chrono::steady_clock::now();

  createMap(scanInfo, kernelInfo, data.data());

  end = std::chrono::steady_clock::now();

  std::cout << "Create Map: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds" << std::endl;
  return 0;
}
