


#include <iostream>
#include <string>

#include <QtCore/QDebug>
#include <QtCore/QString>
#include <QtCore/QFileInfo>

#include <QtGui/QColor>


// TCLAP Includes
#include <tclap/CmdLine.h>
#include <tclap/ValueArg.h>


#include "MXA/Common/MXAEndian.h"

#include "EbsdLib/EbsdLib.h"
#include "EbsdLib/TSL/AngReader.h"
#include "EbsdLib/HKL/CtfReader.h"

#include "DREAM3DLib/Common/DREAM3DSetGetMacros.h"
#include "DREAM3DLib/Common/StatsGen.hpp"
#include "DREAM3DLib/Common/Texture.hpp"
#include "DREAM3DLib/Math/OrientationMath.h"
#include "DREAM3DLib/OrientationOps/CubicOps.h"

#include "QtSupport/PoleFigureGeneration.h"

#define SET_DIRECTION(i, j, k)\
  direction[0] = i; direction[1] = j; direction[2] = k;


class ModifiedLambertProjection
{
  public:
    DREAM3D_SHARED_POINTERS(ModifiedLambertProjection)
    DREAM3D_STATIC_NEW_MACRO(ModifiedLambertProjection)
    DREAM3D_TYPE_MACRO(ModifiedLambertProjection)

    virtual ~ModifiedLambertProjection()
    {}

    enum Square
    {
      NorthSquare = 0,
      SouthSquare = 1
    };


    DREAM3D_INSTANCE_PROPERTY(DoubleArrayType::Pointer, NorthSquare)
    DREAM3D_INSTANCE_PROPERTY(DoubleArrayType::Pointer, SouthSquare)



    DREAM3D_GET_PROPERTY(int, Dimension)
    DREAM3D_GET_PROPERTY(float, Resolution)
    DREAM3D_GET_PROPERTY(float, SphereRadius)

    void initializeSquares(int dims, float resolution, float sphereRadius)
    {
      m_Dimension = dims;
      m_Resolution = resolution;
      m_SphereRadius = sphereRadius;
      m_NorthSquare = DoubleArrayType::CreateArray(m_Dimension * m_Dimension, 1, "NorthSquare");
      m_NorthSquare->initializeWithZeros();
      m_SouthSquare = DoubleArrayType::CreateArray(m_Dimension * m_Dimension, 1, "SouthSquare");
      m_SouthSquare->initializeWithZeros();
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void addValue(unsigned int square, int index, double value)
    {
      if (square == NorthSquare)
      {
        double v = m_NorthSquare->GetValue(index) + value;
        m_NorthSquare->SetValue(index, v);
      }
      else
      {
        double v = m_SouthSquare->GetValue(index) + value;
        m_SouthSquare->SetValue(index, v);
      }
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    double getValue(unsigned int square, int index)
    {
      if (square == NorthSquare)
      {
        return m_NorthSquare->GetValue(index);
      }
      else
      {
        return m_SouthSquare->GetValue(index);
      }
    }


    /**
     * @brief getSquareCoord
     * @param xyz The input XYZ coordinate on the unit sphere.
     * @param sqCoord [output] The XY coordinate in the Modified Lambert Square
     * @return If the point was in the north or south squares
     */
    bool getSquareCoord(float* xyz, float* sqCoord)
    {
      bool nhCheck = false;
      float adjust = 1.0;
      if(xyz[2] >= 0.0)
      {
        adjust = -1.0;
        nhCheck = true;
      }
      if(fabs(xyz[0]) >= fabs(xyz[1]))
      {
        sqCoord[0] = (xyz[0]/fabs(xyz[0]))*sqrt(2.0*1.0*(1.0+(xyz[2]*adjust)))*(DREAM3D::Constants::k_SqrtPi * 0.5);
        sqCoord[1] = (xyz[0]/fabs(xyz[0]))*sqrt(2.0*1.0*(1.0+(xyz[2]*adjust)))*((0.5 * DREAM3D::Constants::k_SqrtPi)*atan(xyz[1]/xyz[0]));
      }
      else
      {
        sqCoord[0] = (xyz[1]/fabs(xyz[1]))*sqrt(2.0*1.0*(1.0+(xyz[2]*adjust)))*((0.5 * DREAM3D::Constants::k_SqrtPi)*atan(xyz[0]/xyz[1]));
        sqCoord[1] = (xyz[1]/fabs(xyz[1]))*sqrt(2.0*1.0*(1.0+(xyz[2]*adjust)))*(DREAM3D::Constants::k_SqrtPi * 0.5);
      }
      return nhCheck;
    }

    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    int getSquareIndex(float* sqCoord)
    {
      int x = (int)( (sqCoord[0] + ( ( (float)m_Dimension/2.0) * m_Resolution) ) / m_Resolution);
      int y = (int)( (sqCoord[1] + ( ( (float)m_Dimension/2.0) * m_Resolution) ) / m_Resolution);

      return y * m_Dimension + x;
    }


    // -----------------------------------------------------------------------------
    //
    // -----------------------------------------------------------------------------
    void normalizeSquares()
    {

      size_t npoints = m_NorthSquare->GetNumberOfTuples();
      double nTotal = 0;
      double sTotal = 0;

      double* north = m_NorthSquare->GetPointer(0);
      double* south = m_SouthSquare->GetPointer(0);


      for(size_t i = 0; i < npoints; ++i)
      {
        nTotal = nTotal + north[i];
        sTotal = sTotal + south[i];
      }

      for(size_t i = 0; i < npoints; ++i)
      {
        north[i] = north[i]/nTotal;
        south[i] = south[i]/sTotal;
      }

    }

  protected:
    ModifiedLambertProjection() :
      m_Dimension(0),
      m_Resolution(0.0f),
      m_SphereRadius(1.0f)
    {

    }

  private:
    int m_Dimension;
    float m_Resolution;
    float m_SphereRadius;



    ModifiedLambertProjection(const ModifiedLambertProjection&); // Copy Constructor Not Implemented
    void operator=(const ModifiedLambertProjection&); // Operator '=' Not Implemented
};



// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
ModifiedLambertProjection::Pointer createModifiedLambertProjection(FloatArrayType::Pointer coords, int dimension, float resolution)
{
  std::cout << "createModifiedLambertProjection...." << std::endl;
  size_t npoints = coords->GetNumberOfTuples();
  bool nhCheck = false;
  float sqCoord[2];

  ModifiedLambertProjection::Pointer squareProj = ModifiedLambertProjection::New();



  squareProj->initializeSquares(dimension, resolution, 1.0f);
  int sqIndex = 0;
  for(int i = 0; i < npoints; ++i)
  {
    sqCoord[0] = 0.0; sqCoord[1] = 0.0;
    //get coordinates in square projection of crystal normal parallel to boundary normal
    nhCheck = squareProj->getSquareCoord(coords->GetPointer(i * 3), sqCoord);
    sqIndex = squareProj->getSquareIndex(sqCoord);
    if (nhCheck == true)
    {
      //north increment by 1
      squareProj->addValue(ModifiedLambertProjection::NorthSquare, sqIndex, 1.0);
    }
    else
    {
      // south increment by 1
      squareProj->addValue(ModifiedLambertProjection::SouthSquare, sqIndex, 1.0);
    }
  }

  squareProj->normalizeSquares();

  return squareProj;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
DoubleArrayType::Pointer createPoleFigure(ModifiedLambertProjection::Pointer proj, int poleFigureDim)
{
  std::cout << "Creating Intensity Image...." << std::endl;
  int xpoints = poleFigureDim;
  int ypoints = poleFigureDim;

  int xpointshalf = xpoints / 2;
  int ypointshalf = ypoints / 2;

  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float xtmp, ytmp;
  float sqCoord[2];
  float xyz[3];
  bool nhCheck = false;

  DoubleArrayType::Pointer intensityPtr = DoubleArrayType::CreateArray(xpoints * ypoints, 1, "Intensity_Image");
  intensityPtr->initializeWithZeros();
  double* intensity = intensityPtr->GetPointer(0);

  for (int64_t k = 0; k < (xpoints); k++)
  {
    for (int64_t l = 0; l < (ypoints); l++)
    {
      //get (x,y) for stereographic projection pixel
      xtmp = float(k-xpointshalf)*xres+(xres/2.0);
      ytmp = float(l-ypointshalf)*yres+(yres/2.0);
      if((xtmp*xtmp+ytmp*ytmp) <= 1.0)
      {
        xyz[2] = -((xtmp*xtmp+ytmp*ytmp)-1)/((xtmp*xtmp+ytmp*ytmp)+1);
        xyz[0] = xtmp*(1+xyz[2]);
        xyz[1] = ytmp*(1+xyz[2]);

        nhCheck = proj->getSquareCoord(xyz, sqCoord);
        int sqIndex = proj->getSquareIndex(sqCoord);

        int index = l * xpoints + k;
        if (nhCheck == true)
        {
          //north increment by 1
          intensity[index] = proj->getValue(ModifiedLambertProjection::NorthSquare, sqIndex);
        }
        else
        {
          // south increment by 1
          intensity[index] = proj->getValue(ModifiedLambertProjection::SouthSquare, sqIndex);
        }

      }

    }
  }
  return intensityPtr;
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateCubicSphereCoordsFromEulers(FloatArrayType::Pointer eulers, FloatArrayType::Pointer xyz001, FloatArrayType::Pointer xyz011, FloatArrayType::Pointer xyz111)
{
  size_t nOrientations = eulers->GetNumberOfTuples();
  QuaternionMath<float>::Quaternion q1;
  CubicOps ops;
  float g[3][3];
  float* currentEuler = NULL;
  float direction[3] = {0.0, 0.0, 0.0};

  for(size_t i = 0; i < nOrientations; ++i)
  {

    currentEuler = eulers->GetPointer(i * 3);

    OrientationMath::EulertoQuat(q1, currentEuler);
    ops.getFZQuat(q1);
    OrientationMath::QuattoMat(q1, g);

    // -----------------------------------------------------------------------------
    // 001 Family

    direction[0] = 1.0; direction[1] = 0.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz001->GetPointer(i*9));
    direction[0] = 0.0; direction[1] = 1.0; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz001->GetPointer(i*9 + 3));
    direction[0] = 0.0; direction[1] = 0.0; direction[2] = 1.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz001->GetPointer(i*9 + 6));

    // -----------------------------------------------------------------------------
    // 011 Family

    direction[0] = DREAM3D::Constants::k_1OverRoot2; direction[1] = DREAM3D::Constants::k_1OverRoot2; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 0));
    direction[0] = DREAM3D::Constants::k_1OverRoot2; direction[1] = 0.0; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 3));
    direction[0] = 0.0; direction[1] = DREAM3D::Constants::k_1OverRoot2; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 6));
    direction[0] = -DREAM3D::Constants::k_1OverRoot2; direction[1] = -DREAM3D::Constants::k_1OverRoot2; direction[2] = 0.0;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 9));
    direction[0] = -DREAM3D::Constants::k_1OverRoot2; direction[1] = 0.0; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 12));
    direction[0] = 0.0; direction[1] = -DREAM3D::Constants::k_1OverRoot2; direction[2] = DREAM3D::Constants::k_1OverRoot2;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz011->GetPointer(i*18 + 15));

    // -----------------------------------------------------------------------------
    // 111 Family

    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 0));
    direction[0] = -DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 3));
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = -DREAM3D::Constants::k_1OverRoot3; direction[2] = DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 6));
    direction[0] = DREAM3D::Constants::k_1OverRoot3; direction[1] = DREAM3D::Constants::k_1OverRoot3; direction[2] = -DREAM3D::Constants::k_1OverRoot3;
    MatrixMath::Multiply3x3with3x1(g, direction, xyz111->GetPointer(i*12 + 9));
  }

}



/**
 * @brief This function writes a set of Axis coordinates to that are needed
 * for a Rectilinear Grid based data set.
 * @param f The "C" FILE* pointer to the file being written to.
 * @param axis The name of the Axis that is being written
 * @param type The type of primitive being written (float, int, ...)
 * @param npoints The total number of points in the array
 * @param min The minimum value of the axis
 * @param max The maximum value of the axis
 * @param step The step value between each point on the axis.
 */

int writeCoords(FILE* f, const char* axis, const char* type, int64_t npoints, float min, float step)
{
  int err = 0;
  fprintf(f, "%s %lld %s\n", axis, npoints, type);
  float* data = new float[npoints];
  float d;
  for (int idx = 0; idx < npoints; ++idx)
  {
    d = idx * step + min;
    MXA::Endian::FromSystemToBig::convert<float>(d);
    data[idx] = d;
  }
  size_t totalWritten = fwrite(static_cast<void*>(data), sizeof(float), static_cast<size_t>(npoints), f);
  delete[] data;
  if (totalWritten != static_cast<size_t>(npoints) )
  {
    std::cout << "Error Writing Binary VTK Data into file " << std::endl;
    fclose(f);
    return -1;
  }
  return err;
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeVtkFile(const std::string filename,  DoubleArrayType::Pointer poleFigurePtr, int dimension)
{

  std::cout << "writeVtkFile: " << filename << std::endl;
  double* poleFigure = poleFigurePtr->GetPointer(0);
  int xpoints = dimension;
  int ypoints = dimension;
  int zpoints = 1;
  float xres = 2.0 / (float)(xpoints);
  float yres = 2.0 / (float)(ypoints);
  float zres = (xres+yres)/2.0;

  std::stringstream ss;
  FILE* f = NULL;
  f = fopen(filename.c_str(), "wb");
  if(NULL == f)
  {

    ss.str("");
    ss << "Could not open GBCD viz file " << filename << " for writing. Please check access permissions and the path to the output location exists";
    //  notifyErrorMessage(ss.str(), getErrorCondition());
    std::cout << ss.str() << std::endl;
    return;
  }



  // Write the correct header
  fprintf(f, "# vtk DataFile Version 2.0\n");
  fprintf(f, "data set from DREAM3D\n");
  fprintf(f, "BINARY"); fprintf(f, "\n");
  fprintf(f, "DATASET RECTILINEAR_GRID\n");
  fprintf(f, "DIMENSIONS %d %d %d\n", xpoints+1, ypoints+1, zpoints+1);

  // Write the Coords
  writeCoords(f, "X_COORDINATES", "float", xpoints + 1, (-float(xpoints)*xres/2.0), xres);
  writeCoords(f, "Y_COORDINATES", "float", ypoints + 1, (-float(ypoints)*yres/2.0), yres);
  writeCoords(f, "Z_COORDINATES", "float", zpoints + 1, (-float(zpoints)*zres/2.0), zres);

  size_t total = xpoints * ypoints * zpoints;
  fprintf(f, "CELL_DATA %d\n", (int)total);

  fprintf(f, "SCALARS %s %s 1\n", "Intensity", "float");
  fprintf(f, "LOOKUP_TABLE default\n");
  {
    float* gn = new float[total];
    float t;
    int count = 0;
    for (int64_t j = 0; j < (ypoints); j++)
    {
      for (int64_t i = 0; i < (xpoints); i++)
      {
        t = float(poleFigure[(j*xpoints)+i]);
        MXA::Endian::FromSystemToBig::convert<float>(t);
        gn[count] = t;
        count++;
      }
    }
    int64_t totalWritten = fwrite(gn, sizeof(float), (total), f);
    delete[] gn;
    if (totalWritten != (total))  {
      std::cout << "Error Writing Binary VTK Data into file " << filename << std::endl;
      fclose(f);
    }
  }

  fclose(f);
}


///getColorCorrespondingToValue ////////////////////////////////////////////////
//
// Assumes you've already generated min and max -- the extrema for the data
// to which you're applying the color map. Then define the number of colorNodes
// and make sure there's a row of three float values (representing r, g, and b
// in a 0.0-1.0 range) for each node. Then call this method for with parameter
// val some float value between min and max inclusive. The corresponding rgb
// values will be returned in the reference-to-float parameters r, g, and b.
//
////////////////////////////////////////////////////////////////////////////////
void getColorCorrespondingTovalue(float val,
                                                   float &r, float &g, float &b,
                                                   float max, float min)
{
  static const int numColorNodes = 8;
  float color[numColorNodes][3] =
  {
    {0.0f, 1.0f/255.0f, 253.0f/255.0f},    // blue
    {105.0f/255.0f, 145.0f/255.0f, 2.0f/255.0f},    // yellow
    {1.0f/255.0f, 255.0f/255.0f, 29.0f/255.0f},    // Green
    {180.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f},
    {255.0f/255.0f, 215.0f/255.0f, 6.0f/255.0f},
    {255.0f/255.0f, 143.0f/255.0f, 1.0f/255.0f},
    {255.0f/255.0f, 69.0f/255.0f, 0.0f/255.0f},
    {253.0f/255.0f, 1.0f/255.0f, 0.0f/255.0f}     // red
  };
  float range = max - min;
  for (int i = 0; i < (numColorNodes - 1); i++)
  {
    float currFloor = min + ((float)i / (numColorNodes - 1)) * range;
    float currCeil = min + ((float)(i + 1) / (numColorNodes - 1)) * range;

    if((val >= currFloor) && (val <= currCeil))
    {
      float currFraction = (val - currFloor) / (currCeil - currFloor);
      r = color[i][0] * (1.0 - currFraction) + color[i + 1][0] * currFraction;
      g = color[i][1] * (1.0 - currFraction) + color[i + 1][1] * currFraction;
      b = color[i][2] * (1.0 - currFraction) + color[i + 1][2] * currFraction;
    }
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void writeImage(const std::string filename, DoubleArrayType::Pointer poleFigurePtr, int dimension)
{
  std::cout << "writeImage: " << filename << std::endl;
  size_t npoints = poleFigurePtr->GetNumberOfTuples();
  double max = std::numeric_limits<double>::min();
  double min = std::numeric_limits<double>::max();
  double value = 0.0;
  for(size_t i = 0; i < npoints; ++i)
  {
    value = poleFigurePtr->GetValue(i);
    if (value > max) { max = value;}
    if (value < min) { min = value;}
  }


  int imageWidth = dimension;
  int imageHeight = dimension;

  QImage image (imageWidth, imageHeight, QImage::Format_ARGB32_Premultiplied);
  image.fill(0);
  qint32 numColors = 9;

  QVector<QColor> colorTable(numColors);
  qint32 range = 8;

  float r, g, b;
  for (int i = 0; i < numColors; i++)
  {
    //int val = min + ((float)i / (float)numColors) * (float)range;
    int val = 0 + ((float)i / (float)numColors) * (float)range;
    getColorCorrespondingTovalue(val, r, g, b, max, min);
    colorTable[i] = QColor(r*255, g*255, b*255, 255);
  }
  // Index 0 is all white which is every pixel outside of the Pole Figure circle
 // colorTable[0] = QColor(255, 255, 255, 255);

  for (int yCoord = 0; yCoord < imageHeight; ++yCoord)
  {
    for (int xCoord = 0; xCoord < imageWidth; ++xCoord)
    {
      double intensity = poleFigurePtr->GetValue(yCoord * imageWidth + xCoord);
      intensity = (intensity - min) / (max - min) * (numColors-1);
      image.setPixel(xCoord, yCoord, colorTable[(int)(intensity)].rgba());
    }
  }

  // Flip the image so the (-1, -1) is in the lower left
  image = image.mirrored(true, false);
  bool saved = image.save(QString::fromStdString(filename));
  if(!saved)
  {
    std::cout << "did NOT save QImage at path: " << filename << std::endl;
  }
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateCubicPoleFigures(FloatArrayType::Pointer eulers, QString outputPath)
{
  std::cout << "generateCubicPoleFigures..." << std::endl;
  int numOrientations = eulers->GetNumberOfTuples();


  // Create an Array to hold the XYZ Coordinates which are the coords on the sphere.
  // this is size for CUBIC ONLY, <001> Family
  FloatArrayType::Pointer xyz001 = FloatArrayType::CreateArray(numOrientations * 3, 3, "TEMP_<001>_xyzCoords");
  // this is size for CUBIC ONLY, <011> Family
  FloatArrayType::Pointer xyz011 = FloatArrayType::CreateArray(numOrientations * 6, 3, "TEMP_<011>_xyzCoords");
  // this is size for CUBIC ONLY, <111> Family
  FloatArrayType::Pointer xyz111 = FloatArrayType::CreateArray(numOrientations * 4, 3, "TEMP_<111>_xyzCoords");

  int dimension = 100;
  float resolution = sqrt(M_PI*0.5) * 2.0 / (float)(dimension);
  int poleFigureDim = 512;

  // Generate the coords on the sphere
  generateCubicSphereCoordsFromEulers(eulers, xyz001, xyz011, xyz111);



  // Generate the modified Lambert projection images (Squares, 2 of them, 1 for northern hemisphere, 1 for southern hemisphere
  ModifiedLambertProjection::Pointer lambert = createModifiedLambertProjection(xyz001, dimension, resolution);
  // Now create the intensity image that will become the actual Pole figure image
  DoubleArrayType::Pointer poleFigurePtr = createPoleFigure(lambert, poleFigureDim);
  std::string filename = outputPath.toStdString() + "/" + "001.vtk";
  writeVtkFile(filename, poleFigurePtr, poleFigureDim);
  filename = outputPath.toStdString() + "/" + "001.tif";
  writeImage(filename, poleFigurePtr, poleFigureDim);

  // Generate the <011> pole figure
  lambert = createModifiedLambertProjection(xyz011, dimension, resolution);
  poleFigurePtr = createPoleFigure(lambert, poleFigureDim);
  filename = outputPath.toStdString() + "/" + "011.vtk";
  writeVtkFile(filename, poleFigurePtr, poleFigureDim);

  // Generate the <111> pole figure
  lambert = createModifiedLambertProjection(xyz111, dimension, resolution);
  poleFigurePtr = createPoleFigure(lambert, poleFigureDim);
  filename = outputPath.toStdString() + "/" + "111.vtk";
  writeVtkFile(filename, poleFigurePtr, poleFigureDim);
}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromAngFile(QString inputFile, QString outputPath)
{
  std::cout << "generateFromAngFile....." << std::endl;
  int numOrientations = 0;
  FloatArrayType::Pointer eulers = FloatArrayType::NullPointer();
  std::vector<AngPhase::Pointer> phases;
  // Scope This section so we dump the reader data soon after we copy it.
  {
    std::cout << "Reading ANG file....." << std::endl;
    // Get the ANG reader object
    AngReader reader;
    reader.setFileName(inputFile.toStdString());
    int err = reader.readFile();
    if (err < 0)
    {
      qDebug() << "Could not Read Ang file. " << err;
      qDebug() << QString::fromStdString(reader.getErrorMessage());
      return;
    }
    numOrientations = reader.getNumRows() * reader.getNumEvenCols();

    // Create an Array to hold the eulers
    eulers = FloatArrayType::CreateArray(numOrientations, 3, "TEMP_Eulers");


    float* e1s = reader.getPhi1Pointer();
    float* e2s = reader.getPhiPointer();
    float* e3s = reader.getPhi2Pointer();


    std::cout << "Copying Eulers... " << std::endl;
    // Copy the Eulers from the reader's memory into our own memory location
    for(int i = 0; i < numOrientations; ++i)
    {
      eulers->SetComponent(i, 0, e1s[i]);
      eulers->SetComponent(i, 1, e2s[i]);
      eulers->SetComponent(i, 2, e3s[i]);
    }

    phases = reader.getPhaseVector();
  }


  unsigned int symmetry = phases[0]->determineCrystalStructure();

  if (symmetry == Ebsd::CrystalStructure::Cubic_High)
  {
    generateCubicPoleFigures(eulers, outputPath);
  }
  else
  {
    std::cout << "Only Cubic Symmetry is handled for Pole Figures." << std::endl;
    return;
  }

}


// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromCtfFile(QString inputFile)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void generateFromListFile(QString inputFile)
{

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int main(int argc, char *argv[])
{

  QString inputFile;
  QString outputPath;
  try
  {

    // Handle program options passed on command line.
    TCLAP::CmdLine cmd("PoleFigureMaker", ' ', "0.1.0");

    TCLAP::ValueArg<std::string> inputFileArg( "", "file", "The input data file", true, "", "Input File");
    cmd.add(inputFileArg);

    TCLAP::ValueArg<std::string> outputPathArg( "", "outpath", "The output path", true, "", "Output Path");
    cmd.add(outputPathArg);

    // Parse the argv array.
    cmd.parse(argc, argv);
    if (argc == 1)
    {
      std::cout << "PoleFigureMaker program was not provided any arguments. Use the --help argument to show the help listing." << std::endl;
      return EXIT_FAILURE;
    }

    inputFile = QString::fromStdString(inputFileArg.getValue());
    outputPath = QString::fromStdString(outputPathArg.getValue());

    QFileInfo fi(inputFile);
    if (fi.suffix().compare("ang") == 0)
    {
      generateFromAngFile(inputFile, outputPath);
    }
    else if (fi.suffix().compare("txt") == 0)
    {
      generateFromListFile(inputFile);
    }
    else if (fi.suffix().compare("ctf") == 0)
    {
      generateFromCtfFile(inputFile);
    }


  }
  catch (TCLAP::ArgException &e) // catch any exceptions
  {
    std::cerr << " error: " << e.error() << " for arg " << e.argId() << std::endl;
    return EXIT_FAILURE;
  }




  return 0;
}
