/* ============================================================================
 * Copyright (c) 2011, Michael A. Jackson (BlueQuartz Software)
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
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
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
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#include <stdio.h>
#include <stdlib.h>

#include <QVector>
#include <sstream>
#include <iostream>

#include "MXA/MXA.h"
#include "MXA/Common/LogTime.h"
#include "MXA/Common/IO/MXAFileReader64.h"
#include "MXA/Common/MXAMath.h"
#include <QtCore/QDir>
#include <QtCore/QFile>
#include "MXA/Utilities/MXAFileInfo.h"

#include "TiffUtilities.h"

typedef struct
{
  int mapWidth;
  int mapHeight;
  int totalPoints;
} MapDescription_t;



typedef struct
{
    int x_index;
    int y_index;
    int var2;
    int width;
    int height;
    int var5;
    uint8_t flag;
} EBSPHeader_t;


typedef struct
{
    int16_t xIndex;
    int16_t yIndex;
    float   bc;
    uint16_t unknown2;
    float   euler1;
    float   euler2;
    float   euler3;
    uint16_t badPoint;
    uint16_t bands;
    float   mad;
} IndexResult_t;

namespace Bcf
{
  static const size_t MapDescHeaderByteSize = 25;
  static const size_t IndexResultByteSize = 30;
  static const QString FrameDescriptionFileName("FrameDescription");
  static const QString FrameDataFileName("FrameData");
  static const QString IndexingResultsFileName("IndexingResults");
}

// -----------------------------------------------------------------------------
// Returns -1 on error, 0 on Success
// -----------------------------------------------------------------------------
int parseMapSize(const QString inputDir, MapDescription_t* mapDesc )
{

    QString frameDescPath = inputDir;
    frameDescPath += MXADir::Separator + Bcf::FrameDescriptionFileName;

    MXAFileReader64 reader(frameDescPath);
    if (reader.initReader() == false ) {
        std::cout << "Could not open file " << frameDescPath << std::endl;
        return -1;
    }

    bool success = reader.readArray<int>(reinterpret_cast<int*>(mapDesc), 3); // Read the first 3 integers
    if (false == success)
    {
        std::cout << "Could not read the first 3 integers (12 bytes) of the file." << std::endl;
        return -1;
    }

    return 0;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int extractPatterns(const QString &inputDir, const QString &outputDir, MapDescription_t* mapDesc)
{
    int err = 0;
    // Generate our file path to read
    QString frameDataPath = inputDir;
    frameDataPath += MXADir::Separator + Bcf::FrameDataFileName;

    // Make sure the output path is available.
    MXADir::mkdir(outputDir, true);

    MXAFileReader64 reader(frameDataPath);
    bool isOpen = reader.initReader();
    if(isOpen == false)
    {
        std::cout << "Could not open file " << frameDataPath << std::endl;
        return -1;
    }

    EBSPHeader_t header;
    QVector<uint8_t> outImage;
    size_t ebspPixelCount = 0;
    TiffUtilities tiffUtil;
    QStringstream ss;

    QStringstream comment;
    for (int i = 0; i < mapDesc->totalPoints; ++i)
    {
        ::memset(reinterpret_cast<uint8_t*>(&header), 0xAB, Bcf::MapDescHeaderByteSize);

        // Read 25 byte header
        reader.readArray(reinterpret_cast<uint8_t*>(&header), Bcf::MapDescHeaderByteSize);

        ebspPixelCount = header.width * header.height;
        if (ebspPixelCount != outImage.size())
        {
            outImage.resize(ebspPixelCount);
        }
        // Read the pattern data into the array
        reader.readArray<uint8_t>( &(outImage.front()), ebspPixelCount);

        // Write the Tiff File for this X,Y pattern
        ss.str("");
        ss << outputDir << MXADir::Separator << header.x_index << "_" << header.y_index << "_pattern.tif";
        comment.str("");
        comment << "Kikuchi Pattern at x="<< header.x_index << ", y=" << header.y_index;
        err = tiffUtil.writeGrayScaleImage(ss.str().c_str(), header.height, header.width, comment.str().c_str(), &(outImage.front()));
        if (err < 0)
        {
            std::cout << "Error Writing Tif file for " << ss.str() << std::endl;
        }
    }
    return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int extractIndexingResults(const QString &inputDir, const QString &outputFile, MapDescription_t* mapDesc)
{
    int err = 0;
    // Generate our file path to read
    QString indexResults = inputDir;
    indexResults += MXADir::Separator + Bcf::IndexingResultsFileName;
    MXAFileReader64 reader(indexResults);

    bool isOpen = reader.initReader();
    if(isOpen == false)
    {
        std::cout << "Could not open file " << indexResults << std::endl;
        return -1;
    }


    // We are going to read everything into memory because the file is NOT actually in a consistent order.
  //  IndexResult_t* header = (IndexResult_t*)(malloc(Bcf::IndexResultByteSize * mapDesc->totalPoints));

    uint8_t* ptr = reinterpret_cast<uint8_t*>(malloc(Bcf::IndexResultByteSize * mapDesc->totalPoints));
    ::memset(ptr, 0xAB, Bcf::IndexResultByteSize * mapDesc->totalPoints);
    uint8_t curHdr[30];
    size_t index = 0;
    uint16_t* y_index = NULL;
    uint16_t* x_index = NULL;
    float* e1;
    float* e2;
    float* e3;
    float* mad;
    uint16_t* bands;

    for (int i = 0; i < mapDesc->totalPoints; ++i)
    {
        reader.readArray(reinterpret_cast<uint8_t*>(curHdr), Bcf::IndexResultByteSize);

        x_index = reinterpret_cast<uint16_t*>(curHdr);
        y_index = reinterpret_cast<uint16_t*>(curHdr + 2);

        e1 = reinterpret_cast<float*>(curHdr + 10);
        e2 = reinterpret_cast<float*>(curHdr + 14);
        e3 = reinterpret_cast<float*>(curHdr + 18);

        index = (mapDesc->mapWidth * *y_index) + *x_index;

        ::memcpy(ptr+(Bcf::IndexResultByteSize * index), curHdr, Bcf::IndexResultByteSize);
    }

#if 0
    float radToDeg = 180.0f/M_PI;
#else
    float radToDeg = 1.0f;
#endif
    
    QString parentPath = MXAFileInfo::parentPath(outputFile);
    if(!MXADir::mkdir(parentPath, true))
    {
      QStringstream ss;
      ss << "Error creating parent path '" << parentPath << "'";
      std::cout << ss.str() << std::endl;
      return -1;
    }


    FILE* f = fopen(outputFile.c_str(), "w");
    if (NULL == f)
    {
        std::cout << "Could not open file " << outputFile << " To write indexing results." << std::endl;
        return -1;
    }
    fprintf(f, "X\tY\tEuler1f\tEuler2\tEuler3\tMAD\tBands\n");
    uint8_t* header = NULL;
    float maxBC = 0.0f;
    float minBC = 1.0f;
    float* bc = NULL;


    for (int i = 0; i < mapDesc->totalPoints; ++i)
    {
        //hdrPtr = reinterpret_cast<IndexResult_t*>(ptr + (i*Bcf::IndexResultByteSize));
        header = ptr + i*Bcf::IndexResultByteSize;

        IndexResult_t row;
        void* rowPtr = &row;
        ::memcpy(rowPtr,header, Bcf::IndexResultByteSize);

//        std::cout << row.xIndex << "    " << row.yIndex << "    " << row.euler1*radToDeg << "    "
//                  << row.euler2*radToDeg << "    " << row.euler3*radToDeg << "    " << row.mad << std::endl;

        x_index = reinterpret_cast<uint16_t*>(header);
        y_index = reinterpret_cast<uint16_t*>(header + 2);
        bc = reinterpret_cast<float*>(header + 4);
        e1 = reinterpret_cast<float*>(header + 10);
        e2 = reinterpret_cast<float*>(header + 14);
        e3 = reinterpret_cast<float*>(header + 18);
        bands = reinterpret_cast<uint16_t*>(header + 24);
        mad = reinterpret_cast<float*>(header + 26);

        if (*bc > maxBC) { maxBC = *bc; }
        if (*bc < minBC) { minBC = *bc; }

        *e1 = *e1*radToDeg;
        *e2 = *e2*radToDeg;
        *e3 = *e3*radToDeg;
        fprintf(f, "%d\t%d\t%0.8f\t%0.8f\t%0.8f\t%0.8f\t%d\n", *x_index , *y_index, *e1, *e2, *e3, *mad, *bands);
    }
    fclose(f);

    std::cout << "Max BC: " << maxBC << "   MinBC: " << minBC << std::endl;

    return err;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char **argv)
{
    if (argc != 4)
    {
        std::cout << "This program needs 3 arguments:" << std::endl;
        std::cout << "  1) The input directory where all the Bcf files were extracted." << std::endl;
        std::cout << "  2) The output directory where you want all the patterns created" << std::endl;
        std::cout << "  3) The output file to generate the Index Results" << std::endl;
        return EXIT_FAILURE;
    }



    QString inputDir(argv[1]);
    QString outputDir(argv[2]);
    QString ebsdIndexOutputFile(argv[3]);

    // Read the "FrameDescription" header which tells us how many points are in the Map
    MapDescription_t mapDesc;
    ::memset(&mapDesc, 0, 12); // Initialize the structure to all zeros
    int err = parseMapSize( inputDir, &mapDesc);
    if (err < 0)
    {
        return EXIT_FAILURE;
    }

    // Extract the EBSD Scan Data
    err = extractIndexingResults(inputDir, ebsdIndexOutputFile, &mapDesc);
    if (err < 0)
    {
        return EXIT_FAILURE;
    }

    // Extract the EBSD Kikuchi Patterns to individual Tif files
    err = extractPatterns(inputDir, outputDir, &mapDesc);
    if (err < 0)
    {
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;

}

