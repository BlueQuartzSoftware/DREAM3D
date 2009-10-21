#include "MicroGen3D.h"

#if 0
#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>
#include <time.h>
#include <stdlib.h>

#include <vector>
#include <algorithm>
#include <numeric>
#include <assert.h>
#include <stdio.h>
#endif

#ifndef M_PI
#define M_PI 3.1415926535897;
#endif

#include <sstream>

using namespace std;

MicroGen3D::MicroGen3D() :
  voxels(NULL),
  tempgrain(NULL),
  grains(NULL),
  goodgrain(NULL),
  packedgrain(NULL),
  gsizes(NULL),
  gremovals(NULL),
  diambin(NULL),
  boverabin(NULL),
  coverabin(NULL),
  coverbbin(NULL),
  seNbin(NULL),
  eulerbin(NULL),
  orient(NULL),
  gridfine(NULL),
  gridcourse(NULL),
  actualmisobin(NULL),
  simmisobin(NULL),
  actualmicrobin(NULL),
  simmicrobin(NULL)
{

}

MicroGen3D::~MicroGen3D()
{

  delete [] voxels;
  delete [] tempgrain;
  delete [] grains;
  delete [] goodgrain;
  delete [] packedgrain;
  delete [] gsizes;
  delete [] gremovals;
  delete [] diambin;
  delete [] boverabin;
  delete [] coverabin;
  delete [] coverbbin;
  delete [] seNbin;
  delete [] eulerbin;
  delete [] orient;
  delete [] gridfine;
  delete [] gridcourse;
  delete [] actualmisobin;
  delete [] simmisobin;
  delete [] actualmicrobin;
  delete [] simmicrobin;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicroGen3D::initialize(double v_sizex,
                  double v_sizey,
                  double v_sizez,
                  double v_resx,
                  double v_resy,
                  double v_resz,
                  bool v_mergetwinsoption,
                  int32 v_minallowedgrainsize,
                  double v_minseedconfidence,
                  double v_misorientationtolerance,
                  int32 v_crystruct,
                  bool v_alreadyformed)
{
  sizex = v_sizex;
  sizey = v_sizey;
  sizez = v_sizez;
  resx = v_resx;
  resy = v_resy;
  resz = v_resz;
  mergetwinsoption = (v_mergetwinsoption == true) ? 1 : 0;
  minallowedgrainsize = v_minallowedgrainsize;
  minseedconfidence = v_minseedconfidence;
  misorientationtolerance = v_misorientationtolerance;
  crystruct = v_crystruct;
  alreadyformed = (v_alreadyformed == true) ? 1 : 0;

  xpoints = (sizex / resx) + 3;
  ypoints = (sizey / resy) + 3;
  zpoints = (sizez / resz) + 3;
  totalpoints = xpoints * ypoints * zpoints;

  voxels = new Voxel[totalpoints];
  tempgrain = new Grain[totalpoints];
  grains = new Grain[totalpoints];
  goodgrain = new Grain[totalpoints];
  packedgrain = new Grain[totalpoints];
  gsizes = new int[totalpoints];
  gremovals = new int[totalpoints];
  diambin = new Bin[totalpoints];
  boverabin = new Bin[totalpoints];
  coverabin = new Bin[totalpoints];
  coverbbin = new Bin[totalpoints];
  seNbin = new Bin[totalpoints];
  eulerbin = new Bin[totalpoints];
  orient = new Orient[totalpoints];
  gridfine = new Voxel[totalpoints];
  gridcourse = new Voxel[totalpoints];
  actualmisobin = new Bin[totalpoints];
  simmisobin = new Bin[totalpoints];
  actualmicrobin = new Bin[totalpoints];
  simmicrobin = new Bin[totalpoints];

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void MicroGen3D::loadSlices()
{

  int count = 0;
  double z;
  double junk1;
  double junk2;
  double junk3;
  double junk4;
  int zero = 0;
  int checked = 1;
  int badgrain = -1;
  for(int k = 0; k < zpoints; k++)
    {
    if(k == 0 || k == zpoints-1)
    {
      for(int j = 0; j < ypoints; j++)
      {
        for(int i = 0; i < xpoints; i++)
        {
          voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler1 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler2 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler3 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].confidence = zero;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].alreadychecked = checked;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].grainname = badgrain;
        }
      }
    }
    if(k > 0 && k < zpoints-1)
    {
#if 0
      char filename[15] = "TXT\\1";
      char extension[15] = ".txt";
      char cindex[5];
      int fileindex = k+1001;
      cout << "Loading Slice - " << k << endl;
      _itoa_s(fileindex,cindex,10);
      strcat(filename,cindex);
      strcat(filename,extension);
#endif
      int fileindex = k+1001;
      std::stringstream ss;
      ss << "TXT/1" << fileindex << ".txt";
      std::string filename = ss.str();

      ifstream inputFile;
      inputFile.open(filename.c_str() );
      for(int j = 0; j < ypoints; j++)
      {
        if(j==0 || j == ypoints-1)
        {
          for(int i = 0; i < xpoints; i++)
          {
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler1 = -1;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler2 = -1;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler3 = -1;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].confidence = zero;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].alreadychecked = checked;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].grainname = badgrain;
          }
        }
        if(j > 0 && j < ypoints-1)
        {
          voxels[((k*xpoints*ypoints)+(j*xpoints)+0)].euler1 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+0)].euler2 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+0)].euler3 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+0)].confidence = zero;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+0)].alreadychecked = checked;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+0)].grainname = badgrain;
          for(int i = 1; i < xpoints-1; i++)
          {
            inputFile >> voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler1 >> voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler2 >> voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler3 >> voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].xc >> voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].xc >> junk1 >> voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].confidence >> junk2 >> junk3 >> junk4;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].zc = (k-1)*resz;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].alreadychecked = zero;
            voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].grainname = badgrain;
            if(voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler1 == 12.566 && voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler2 == 12.566 && voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].euler3 == 12.566)
            {
              voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].confidence = zero;
              voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].alreadychecked = zero;
              voxels[((k*xpoints*ypoints)+(j*xpoints)+i)].grainname = badgrain;
            }
          }
          voxels[((k*xpoints*ypoints)+(j*xpoints)+xpoints-1)].euler1 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+xpoints-1)].euler2 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+xpoints-1)].euler3 = -1;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+xpoints-1)].confidence = zero;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+xpoints-1)].alreadychecked = checked;
          voxels[((k*xpoints*ypoints)+(j*xpoints)+xpoints-1)].grainname = badgrain;
        }
      }
      inputFile.close();
    }
    }
}

int  MicroGen3D::form_grains()
{
  int totalsize = 0;
  int noseeds = 0;
  int checked = 1;
  int graincount = 0;
  double n1;
  double n2;
  double n3;
  int size = 0;
  int voxelslist[100000];
  int neighborhood[6];
  neighborhood[0] = -1;
  neighborhood[1] = 1;
  neighborhood[2] = -xpoints;
  neighborhood[3] = xpoints;
  neighborhood[4] = -(xpoints*ypoints);
  neighborhood[5] = (xpoints*ypoints);
  while(noseeds == 0)
  {
    int seed = -1;
    double maxconfidence = 0;
    for(int k = 1; k < zpoints-1; k++)
    {
      for(int j = 1; j < ypoints-1; j++)
      {
        for(int i = 1; i < xpoints-1; i++)
        {
          int point = (k*xpoints*ypoints)+(j*xpoints)+i;
          double confidence = voxels[point].confidence;
          if(confidence > MicroGen3D().minseedconfidence && voxels[point].alreadychecked == 0)
          {
            seed = point;
          }
          if(seed > -1) {break;}
        }
      }
    }
    if(seed == -1) noseeds = 1;
    if(seed >= 0)
    {
      size = 0;
      voxels[seed].alreadychecked = checked;
      voxels[seed].grainname = graincount;
      cout << "Making Grain - " << graincount << endl;
      voxelslist[size] = seed;
      size++;
      for(int j = 0; j < size; j ++)
      {
        int currentpoint = voxelslist[j];
        double currentx = voxels[currentpoint].xc;
        double currenty = voxels[currentpoint].yc;
        double currentz = voxels[currentpoint].zc;
        double v1ea1 = voxels[currentpoint].euler1;
        double v1ea2 = voxels[currentpoint].euler2;
        double v1ea3 = voxels[currentpoint].euler3;
        for(int i = 0; i < 6; i++)
        {
          int neighbor = currentpoint+neighborhood[i];
/*          if(i == 0 && currentx > 0) neighbor = currentpoint-1;
          if(i == 1 && currentx < sizex) neighbor = currentpoint+1;
          if(i == 2 && currenty > 0) neighbor = currentpoint-xpoints;
          if(i == 3 && currenty < sizey) neighbor = currentpoint+xpoints;
          if(i == 4 && currentz > 0) neighbor = currentpoint-(xpoints*ypoints);
          if(i == 5 && currentz < sizez) neighbor = currentpoint+(xpoints*ypoints);
          if(i == 6 && currentx > 0 && currenty > 0) neighbor = currentpoint-1-xpoints;
          if(i == 7 && currentx > 0 && currenty < sizey) neighbor = currentpoint-1+xpoints;
          if(i == 8 && currentx > 0 && currentz > 0) neighbor = currentpoint-1-(xpoints*ypoints);
          if(i == 9 && currentx > 0 && currentz < sizez) neighbor = currentpoint-1+(xpoints*ypoints);
          if(i == 10 && currentx < sizex && currenty > 0) neighbor = currentpoint+1-xpoints;
          if(i == 11 && currentx < sizex && currenty < sizey) neighbor = currentpoint+1+xpoints;
          if(i == 12 && currentx < sizex && currentz > 0) neighbor = currentpoint+1-(xpoints*ypoints);
          if(i == 13 && currentx < sizex && currentz < sizez) neighbor = currentpoint+1+(xpoints*ypoints);
          if(i == 14 && currenty > 0 && currentz > 0) neighbor = currentpoint-xpoints-(xpoints*ypoints);
          if(i == 15 && currenty > 0 && currentz < sizez) neighbor = currentpoint-xpoints+(xpoints*ypoints);
          if(i == 16 && currenty < sizey && currentz > 0) neighbor = currentpoint+xpoints-(xpoints*ypoints);
          if(i == 17 && currenty < sizey && currentz < sizez) neighbor = currentpoint+xpoints+(xpoints*ypoints);*/
          if(voxels[neighbor].alreadychecked == 0)
          {
            double v2ea1 = voxels[neighbor].euler1;
            double v2ea2 = voxels[neighbor].euler2;
            double v2ea3 = voxels[neighbor].euler3;
            double w = getmisoquat(crystruct,misorientationtolerance,v1ea1,v1ea2,v1ea3,v2ea1,v2ea2,v2ea3,n1,n2,n3);
//            double w2 = GetMisorientationOnly(crystruct,misorientationtolerance,v1ea1,v1ea2,v1ea3,v2ea1,v2ea2,v2ea3,n1,n2,n3);
            if(w < misorientationtolerance)
            {
              voxels[neighbor].alreadychecked = checked;
              voxels[neighbor].grainname = graincount;
              voxelslist[size] = neighbor;
              size++;
            }
          }
        }
      }
      grains[graincount].numvoxels = size;
      totalsize = totalsize+size;
      graincount++;
    }
  }
return graincount;
}


void  MicroGen3D::remove_smallgrains()
{
  int zero = 0;
  int badgrain = -1;
  int sizemerged = 1;
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    if(grainname != -1)
    {
      int size = grains[grainname].numvoxels;
      if(size < minallowedgrainsize)
      {
        voxels[i].alreadychecked = zero;
        voxels[i].grainname = badgrain;
        grains[grainname].gotsizemerged = sizemerged;
      }
    }
  }
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
int  MicroGen3D::renumber_grains1()
{
  int graincount = 0;
  for(int i=0;i<numgrains;i++)
  {
    int gotsizemerged = grains[i].gotsizemerged;
    if(gotsizemerged != 1)
    {
      grains[i].newgrainname = graincount;
      int size = grains[i].numvoxels;
      grains[graincount].numvoxels = size;
      graincount++;
    }
  }
  for(int j=0;j<(xpoints*ypoints*zpoints);j++)
  {
    int grainname = voxels[j].grainname;
    if(grainname != -1)
    {
      int newgrainname = grains[grainname].newgrainname;
      voxels[j].grainname = newgrainname;
    }
  }
  return graincount;
}
void  MicroGen3D::write_volume1(string writename10)
{
    ofstream outFile;
    outFile.open(writename10.c_str());
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    double ea1 = voxels[i].euler1;
    double ea2 = voxels[i].euler2;
    double ea3 = voxels[i].euler3;
    double x = voxels[i].xc;
    double y = voxels[i].yc;
    double z = voxels[i].zc;
    double ci = voxels[i].confidence;
    outFile << grainname << " " << ea1 << " " << ea2 << " " << ea3 << " " << x << " " << y << " " << z << " " << ci << endl;
  }
  outFile.close();
}
void  MicroGen3D::assign_badpoints()
{
  vector<int> neighs;
  vector<int> remove;
  int checked = 1;
  int count = 1;
  double x = 0;
  double y = 0;
  double z = 0;
  int *n;
  n = new int[numgrains];
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int grainname = voxels[i].grainname;
      if(grainname == -1)
      {
        for(int c = 0; c < numgrains; c++)
        {
          n[c] = 0;
        }
        x = voxels[i].xc;
        y = voxels[i].yc;
        z = voxels[i].zc;
        count++;
        if(x > 0)
        {
          int grain1 = voxels[i-1].grainname;
          if(grain1 != -1)
          {
            neighs.push_back(grain1);
          }
        }
        if(x < sizex)
        {
          int grain2 = voxels[i+1].grainname;
          if(grain2 != -1)
          {
            neighs.push_back(grain2);
          }
        }
        if(y > 0)
        {
          int grain3 = voxels[i-(xpoints)].grainname;
          if(grain3 != -1)
          {
            neighs.push_back(grain3);
          }
        }
        if(y < sizey)
        {
          int grain4 = voxels[i+(xpoints)].grainname;
          if(grain4 != -1)
          {
            neighs.push_back(grain4);
          }
        }
        if(z > 0)
        {
          int grain5 = voxels[i-(xpoints*ypoints)].grainname;
          if(grain5 != -1)
          {
            neighs.push_back(grain5);
          }
        }
        if(z < sizez)
        {
          int grain6 = voxels[i+(xpoints*ypoints)].grainname;
          if(grain6 != -1)
          {
            neighs.push_back(grain6);
          }
        }
        int current = 0;
        int most = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for(int k=0;k<size;k++)
        {
          int neighbor = neighs[k];
          n[neighbor]++;
          current = n[neighbor];
          if(current > most)
          {
            most = current;
            curgrain = neighbor;
          }
        }
        if(size > 0)
        {
          int bordered = 1;
          voxels[i].hasneighbor = bordered;
          voxels[i].neighbor = curgrain;
          neighs.clear();
        }
      }
    }
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int grainname = voxels[j].grainname;
      int hasneighbor = voxels[j].hasneighbor;
      if(grainname == -1 && hasneighbor == 1)
      {
        int neighbor = voxels[j].neighbor;
        voxels[j].grainname = neighbor;
        voxels[j].alreadychecked = checked;
      }
    }
  }
}
void  MicroGen3D::find_neighbors()
{
  int count = 0;
  int dup = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  vector<int> nlist;
  for(int i = 0; i < numgrains; i++)
  {
    if(grains[i].gotsizemerged != 1)
    {
      for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
      {
        int onsurf = 0;
        int gnum = voxels[j].grainname;
        if(gnum == i)
        {
          x = voxels[j].xc;
          y = voxels[j].yc;
          z = voxels[j].zc;
          int first = voxels[j].grainname;
          if(x > 0)
          {
              int grain1 = voxels[j-1].grainname;
              if(grain1 != i)
            {
              nlist.push_back(grain1);
            }
          }
          if(x < sizex)
          {
            int grain2 = voxels[j+1].grainname;
            if(grain2 != i)
            {
              nlist.push_back(grain2);
            }
          }
          if(y > 0)
          {
            int grain3 = voxels[j-(xpoints)].grainname;
            if(grain3 != i)
            {
              nlist.push_back(grain3);
            }
          }
          if(y < sizey)
          {
            int grain4 = voxels[j+(xpoints)].grainname;
            if(grain4 != i)
            {
              nlist.push_back(grain4);
            }
          }
          if(z > 0)
          {
            int grain5 = voxels[j-(xpoints*ypoints)].grainname;
            if(grain5 != i)
            {
              nlist.push_back(grain5);
            }
          }
          if(z < sizez)
          {
            int grain6 = voxels[j+(xpoints*ypoints)].grainname;
            if(grain6 != i)
            {
              nlist.push_back(grain6);
            }
          }
        }
      }
      vector<int>::iterator newend;
      sort(nlist.begin(),nlist.end());
      newend = unique(nlist.begin(),nlist.end());
      nlist.erase(newend,nlist.end());
      int numneighs = int(nlist.size());
      grains[i].numneighbors = numneighs;
      grains[i].neighborlist = nlist;
      nlist.clear();
    }
  }
}
void  MicroGen3D::merge_containedgrains()
{
  int containedmerged = 1;
  vector<int> nlistgood;
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    int numneighbors = grains[grainname].numneighbors;
    if(numneighbors == 1)
    {
      vector<int> neighborlist = grains[grainname].neighborlist;
      int neighbor = neighborlist[0];
      voxels[i].grainname = neighbor;
      grains[grainname].gotcontainedmerged = containedmerged;
    }
  }
  for(int j = 0; j < numgrains; j++)
  {
    if(grains[j].gotsizemerged != 1 && grains[j].gotcontainedmerged != 1)
    {
      vector<int> neighborlist = grains[j].neighborlist;
      int size = int(neighborlist.size());
      for(int k=0;k<size;k++)
      {
        int firstneighbor = neighborlist[k];
        int gotcontainedmerged = grains[firstneighbor].gotcontainedmerged;
        int gotsizemerged = grains[firstneighbor].gotsizemerged;
        if(gotcontainedmerged != 1 && gotsizemerged != 1) nlistgood.push_back(firstneighbor);
      }
      int neighcount = int(nlistgood.size());
      grains[j].numneighbors = neighcount;
      grains[j].neighborlist = nlistgood;
      nlistgood.clear();
    }
  }
}
int  MicroGen3D::renumber_grains2()
{
  int graincount = 0;
  for(int i=0;i<numgrains;i++)
  {
    int gotcontainedmerged = grains[i].gotcontainedmerged;
    if(gotcontainedmerged != 1)
    {
      grains[i].newgrainname = graincount;
      int size = grains[i].numvoxels;
      int numneighbors = grains[i].numneighbors;
      vector<int> nlist = grains[i].neighborlist;
      grains[graincount].numvoxels = size;
      grains[graincount].numneighbors = numneighbors;
      grains[graincount].neighborlist = nlist;
      graincount++;
    }
  }
  for(int j=0;j<(xpoints*ypoints*zpoints);j++)
  {
    int grainname = voxels[j].grainname;
    if(grainname != -1)
    {
      int newgrainname = grains[grainname].newgrainname;
      voxels[j].grainname = newgrainname;
    }
  }
  return graincount;
}
void  MicroGen3D::homogenize_grains()
{
  for(int i = 0; i < numgrains; i++)
  {
    double ea1good = 0;
    double ea2good = 0;
    double ea3good = 0;
    for(int temp1 = 0; temp1 < 180; temp1++)
    {
      for(int temp2 = 0; temp2 < 180; temp2++)
      {
        for(int temp3 = 0; temp3 < 180; temp3++)
        {
          eulerrank[temp1][temp2][temp3] = 0;
        }
      }
    }
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int gnum = voxels[j].grainname;
      double ci = voxels[j].confidence;
      if(gnum == i && ci > 0.25)
      {
        double g1ea1 = voxels[j].euler1;
        double g1ea2 = voxels[j].euler2;
        double g1ea3 = voxels[j].euler3;
        for(int k = 0; k < 24; k++)
        {
          double o[3][3];
          double ga[3][3];
          double m1[3][3];
          if (k == 0)
          {
            o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
            o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
            o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
          }
          else if (k == 1)
          {
            o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] =  0.0;
            o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = -1.0;
            o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] =  0.0;
          }
          else if (k == 2)
          {
            o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] =  0.0;
            o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
            o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
          }
          else if (k == 3)
          {
            o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
            o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = 1.0;
            o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] = 0.0;
          }
          else if (k == 4)
          {
            o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = -1.0;
            o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] =  0.0;
            o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] =  0.0;
          }
          else if (k == 5)
          {
            o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = 1.0;
            o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] = 0.0;
            o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] = 0.0;
          }
          else if (k == 6)
          {
            o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] =  0.0;
            o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] =  0.0;
            o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
          }
          else if (k == 7)
          {
            o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] = 0.0;
            o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
            o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = 1.0;
          }
          else if (k == 8)
          {
            o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] = 0.0;
            o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] = 0.0;
            o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = 1.0;
          }
          else if (k == 9)
          {
            o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] = 0.0;
            o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] = 0.0;
            o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
          }
          else if (k == 10)
          {
            o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] = 0.0;
            o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = 1.0;
            o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] = 0.0;
          }
          else if (k == 11)
          {
            o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = 1.0;
            o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] = 0.0;
            o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] = 0.0;
          }
          else if (k == 12)
          {
            o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] =  0.0;
            o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = -1.0;
            o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] =  0.0;
          }
          else if (k == 13)
          {
            o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = -1.0;
            o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] =  0.0;
            o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] =  0.0;
          }
          else if (k == 14)
          {
            o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] =  0.0;
            o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = -1.0;
            o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] =  0.0;
          }
          else if (k == 15)
          {
            o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = -1.0;
            o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] =  0.0;
            o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] =  0.0;
          }
          else if (k == 16)
          {
            o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] = 0.0;
            o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = 1.0;
            o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] = 0.0;
          }
          else if (k == 17)
          {
            o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = 1.0;
            o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] = 0.0;
            o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] = 0.0;
          }
          else if (k == 18)
          {
            o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] =  0.0;
            o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] =  0.0;
            o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
          }
          else if (k == 19)
          {
            o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
            o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = 1.0;
            o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] = 0.0;
          }
          else if (k == 20)
          {
            o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = 1.0;
            o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
            o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] = 0.0;
          }
          else if (k == 21)
          {
            o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] =  0.0;
            o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = -1.0;
            o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] =  0.0;
          }
          else if (k == 22)
          {
            o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = -1.0;
            o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] =  0.0;
            o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] =  0.0;
          }
          else if (k == 23)
          {
            o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] =  0.0;
            o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] =  0.0;
            o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
          }
          ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
          ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
          ga[0][2] = sin(g1ea3)*sin(g1ea2);
          ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
          ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
          ga[1][2] =  cos(g1ea3)*sin(g1ea2);
          ga[2][0] =  sin(g1ea1)*sin(g1ea2);
          ga[2][1] = -cos(g1ea1)*sin(g1ea2);
          ga[2][2] =  cos(g1ea2);
          m1[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
          m1[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
          m1[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];
          m1[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
          m1[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
          m1[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];
          m1[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
          m1[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
          m1[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];
          double ea2 = acos(m1[2][2]);
          double cosine3 = (m1[1][2]/sin(ea2));
          double sine3 = (m1[0][2]/sin(ea2));
          double cosine1 = (-m1[2][1]/sin(ea2));
          double sine1 = (m1[2][0]/sin(ea2));
          double ea3 = acos(cosine3);
          double ea1 = acos(cosine1);
          if(sine3 < 0) ea3 = (2*3.1415926535897)-ea3;
          if(sine1 < 0) ea1 = (2*3.1415926535897)-ea1;
          int ea1bin = int(ea1/0.0087266);
          int ea2bin = int(ea2/0.0087266);
          int ea3bin = int(ea3/0.0087266);
          if(ea1bin <= 179 && ea2bin <= 179 && ea3bin <= 179) eulerrank[ea1bin][ea2bin][ea3bin]++;
        }
      }
    }
    double current = 0;
    for(int temp4 = 0; temp4 < 180; temp4++)
    {
      for(int temp5 = 0; temp5 < 180; temp5++)
      {
        for(int temp6 = 0; temp6 < 180; temp6++)
        {
          if(eulerrank[temp4][temp5][temp6] > current)
          {
            ea1good = (temp4*0.0087266)+(0.0087266/2.0);
            ea2good = (temp5*0.0087266)+(0.0087266/2.0);
            ea3good = (temp6*0.0087266)+(0.0087266/2.0);
            current = eulerrank[temp4][temp5][temp6];
          }
        }
      }
    }
    grains[i].avgeuler1 = ea1good;
    grains[i].avgeuler2 = ea2good;
    grains[i].avgeuler3 = ea3good;
    double avgmiso = 0;
    double totalcount = 0;
    double n1,n2,n3;
    for(int iter = 0; iter < (xpoints*ypoints*zpoints); iter++)
    {
      int gnum = voxels[iter].grainname;
      double ci = voxels[iter].confidence;
      if(gnum == i)
      {
        double angcur = 1000000;
        double g1ea1 = grains[i].avgeuler1;
        double g1ea2 = grains[i].avgeuler2;
        double g1ea3 = grains[i].avgeuler3;
        double g2ea1 = voxels[iter].euler1;
        double g2ea2 = voxels[iter].euler2;
        double g2ea3 = voxels[iter].euler3;
        double wmin = getmisoquat(crystruct,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
        voxels[iter].misorientation = wmin;
        avgmiso = avgmiso + angcur;
        totalcount++;
      }
    }
    avgmiso = avgmiso/totalcount;
    grains[i].averagemisorientation = avgmiso;
  }
}
void  MicroGen3D::load_data(string readname)
{
  ifstream inputFile;
  inputFile.open(readname.c_str());
  int gnum;
  double ea1;
  double ea2;
  double ea3;
  double x;
  double y;
  double z;
  double ci;
  int xsize;
  int ysize;
  int zsize;
  numgrains = 0;
  int graincount = 0;
  int gnums[1000];
  int euler1[1000];
  int euler2[1000];
  int euler3[1000];
  for(int g=0;g<1000;g++)
  {
    gnums[g] = -1;
  }
  inputFile >> xsize >> ysize >> zsize;
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    inputFile >> gnum;
    ea1 = euler1[gnum];
    ea2 = euler2[gnum];
    ea3 = euler3[gnum];
    x = i%xpoints;
    y = (i/xpoints)%ypoints;
    z = i/(xpoints*ypoints);
    ci = 1;
    if(gnums[gnum] == -1) gnums[gnum] = graincount, graincount++;
    gnum = gnums[gnum];
    voxels[i].grainname = gnum;
    voxels[i].euler1 = ea1;
    voxels[i].euler2 = ea2;
    voxels[i].euler3 = ea3;
    voxels[i].xc = x;
    voxels[i].yc = y;
    voxels[i].zc = z;
    voxels[i].confidence = ci;
//    if(gnum > numgrains) numgrains = gnum;
  }
  numgrains = graincount;
  inputFile.close();
}
void  MicroGen3D::merge_twins ()
{
  int twinmerged = 1;
  int beenset = 1;
  int mergedcount = 0;
  double angcur = 180;
  vector<int> twinlist;
  double n1,n2,n3;
  for(int i = 0; i < numgrains; i++)
  {
    if(grains[i].gotsizemerged != 1 && grains[i].gotcontainedmerged != 1 && grains[i].twinnewnumberbeenset != 1)
    {
      twinlist.push_back(i);
      int tsize = int(twinlist.size());
      for(int m=0;m<tsize;m++)
      {
        tsize = int(twinlist.size());
        int firstgrain = twinlist[m];
        vector<int> nlist = grains[firstgrain].neighborlist;
        int size = int(nlist.size());
        for(int l=0;l<size;l++)
        {
          angcur = 180;
          int twin = 0;
          int neigh = nlist[l];
          if(neigh != i && grains[neigh].twinnewnumberbeenset != 1)
          {
            double g1ea1 = grains[firstgrain].avgeuler1;
            double g1ea2 = grains[firstgrain].avgeuler2;
            double g1ea3 = grains[firstgrain].avgeuler3;
            double g2ea1 = grains[neigh].avgeuler1;
            double g2ea2 = grains[neigh].avgeuler2;
            double g2ea3 = grains[neigh].avgeuler3;
            double w = getmisoquat(crystruct,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
            double tanhalfang = tan(w/2.0);
            double rodvect1 = tanhalfang*n1;
            double rodvect2 = tanhalfang*n2;
            double rodvect3 = tanhalfang*n3;
            double vecttol = 1.0/3.0;
            double vecttol2 = 0.2;
            double vecttol3 = 0.25;
            double rodvectdiff1 = fabs(fabs(rodvect1)-vecttol);
            double rodvectdiff2 = fabs(fabs(rodvect2)-vecttol);
            double rodvectdiff3 = fabs(fabs(rodvect3)-vecttol);
            double rodvectdiff12 = fabs(fabs(rodvect1)-vecttol2);
            double rodvectdiff22 = fabs(fabs(rodvect2)-vecttol2);
            double rodvectdiff32 = fabs(fabs(rodvect3)-vecttol2);
            double rodvectdiff13 = fabs(fabs(rodvect1)-vecttol3);
            double rodvectdiff23 = fabs(fabs(rodvect2)-vecttol3);
            double rodvectdiff33 = fabs(fabs(rodvect3)-vecttol3);
            if(rodvectdiff1 < 0.03 && rodvectdiff2 < 0.03 && rodvectdiff3 < 0.03) twin = 1;
            if(rodvectdiff1 < 0.03 && fabs(rodvect2) < 0.03 && fabs(rodvect3) < 0.03) twin = 1;
            if(rodvectdiff2 < 0.03 && fabs(rodvect1) < 0.03 && fabs(rodvect3) < 0.03) twin = 1;
            if(rodvectdiff3 < 0.03 && fabs(rodvect1) < 0.03 && fabs(rodvect2) < 0.03) twin = 1;
            if(rodvectdiff1 < 0.03 && rodvectdiff2 < 0.03 && fabs(rodvect3) < 0.03) twin = 1;
            if(rodvectdiff1 < 0.03 && rodvectdiff3 < 0.03 && fabs(rodvect2) < 0.03) twin = 1;
            if(rodvectdiff2 < 0.03 && rodvectdiff3 < 0.03 && fabs(rodvect1) < 0.03) twin = 1;
            if(rodvectdiff12 < 0.03 && rodvectdiff22 < 0.03 && rodvectdiff32 < 0.03) twin = 1;
            if(rodvectdiff13 < 0.03 && rodvectdiff23 < 0.03 && fabs(rodvect3) < 0.03) twin = 1;
            if(rodvectdiff13 < 0.03 && rodvectdiff33 < 0.03 && fabs(rodvect2) < 0.03) twin = 1;
            if(rodvectdiff23 < 0.03 && rodvectdiff33 < 0.03 && fabs(rodvect1) < 0.03) twin = 1;
            if(w < angcur)
            {
              angcur = w;
            }
            if(twin == 1)
            {
              grains[neigh].gottwinmerged = twinmerged;
              grains[neigh].twinnewnumberbeenset = beenset;
              grains[neigh].twinnewnumber = i;
              twinlist.push_back(neigh);
            }
            if(angcur <= 5)
            {
              grains[neigh].twinnewnumberbeenset = beenset;
              grains[neigh].twinnewnumber = i;
              twinlist.push_back(neigh);
            }
          }
        }
      }
    }
    twinlist.clear();
  }
  for(int k = 0; k < (xpoints*ypoints*zpoints); k++)
  {
    int grainname = voxels[k].grainname;
    int gottwinmerged = grains[grainname].gottwinmerged;
    if(gottwinmerged == 1)
    {
      int twinnewnumber = grains[grainname].twinnewnumber;
      voxels[k].grainname = twinnewnumber;
    }
  }
}

void  MicroGen3D::characterize_twins()
{
  for(int i=0;i<numgrains;i++)
  {

  }
}

int  MicroGen3D::renumber_grains3()
{
  int graincount = 0;
  for(int i = 0; i < numgrains; i++)
  {
    int gottwinmerged = grains[i].gottwinmerged;
    if(gottwinmerged != 1)
    {
      grains[i].newgrainname = graincount;
      double ea1good = grains[i].avgeuler1;
      double ea2good = grains[i].avgeuler2;
      double ea3good = grains[i].avgeuler3;
      int size = grains[i].numvoxels;
      int numneighbors = grains[i].numneighbors;
      vector<int> nlist = grains[i].neighborlist;
      goodgrain[graincount].numvoxels = size;
      goodgrain[graincount].numneighbors = numneighbors;
      goodgrain[graincount].neighborlist = nlist;
      goodgrain[graincount].avgeuler1 = ea1good;
      goodgrain[graincount].avgeuler2 = ea2good;
      goodgrain[graincount].avgeuler3 = ea3good;
      graincount++;
    }
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int grainname = voxels[j].grainname;
    int newgrainname = grains[grainname].newgrainname;
    voxels[j].grainname = newgrainname;
  }
  return graincount;
}
void  MicroGen3D::find_goodneighbors()
{
  int count = 0;
  int dup = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  vector<int> nlist;
  int neighcount = 0;
  for(int i = 0; i < numgrains; i++)
  {
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int onsurf = 0;
      int gnum = voxels[j].grainname;
      if(gnum == i)
      {
        x = voxels[j].xc;
        y = voxels[j].yc;
        z = voxels[j].zc;
        int first = voxels[j].grainname;
        if(x > 0)
        {
          int grain1 = voxels[j-1].grainname;
          if(grain1 != i)
          {
            onsurf = 1;
            nlist.push_back(grain1);
          }
        }
        if(x < sizex)
        {
          int grain2 = voxels[j+1].grainname;
          if(grain2 != i)
          {
            onsurf = 1;
            nlist.push_back(grain2);
          }
        }
        if(y > 0)
        {
          int grain3 = voxels[j-(xpoints)].grainname;
          if(grain3 != i)
          {
            onsurf = 1;
            nlist.push_back(grain3);
          }
        }
        if(y < sizey)
        {
          int grain4 = voxels[j+(xpoints)].grainname;
          if(grain4 != i)
          {
            onsurf = 1;
            nlist.push_back(grain4);
          }
        }
        if(z > 0)
        {
          int grain5 = voxels[j-(xpoints*ypoints)].grainname;
          if(grain5 != i)
          {
            onsurf = 1;
            nlist.push_back(grain5);
          }
        }
        if(z < sizez)
        {
          int grain6 = voxels[j+(xpoints*ypoints)].grainname;
          if(grain6 != i)
          {
            onsurf = 1;
            nlist.push_back(grain6);
          }
        }
      }
      voxels[j].surfacevoxel = onsurf;
    }
    vector<int>::iterator newend;
    sort(nlist.begin(),nlist.end());
    newend = unique(nlist.begin(),nlist.end());
    nlist.erase(newend,nlist.end());
    neighcount = int(nlist.size());
    goodgrain[i].numneighbors = neighcount;
    goodgrain[i].neighborlist = nlist;
    nlist.clear();
  }
}
void  MicroGen3D::find_centroids()
{
  int count = 0;
  int size = 0;
  int onedge = 0;
  double sumx = 0;
  double sumy = 0;
  double sumz = 0;
  double centerx = 0;
  double centery = 0;
  double centerz = 0;
  for(int i = 0; i < numgrains; i++)
  {
    size = 0;
    sumx = 0;
    sumy = 0;
    sumz = 0;
    onedge = 0;
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int gnum = voxels[j].grainname;
      if(gnum == i)
      {
        size++;
        double x = voxels[j].xc;
        double y = voxels[j].yc;
        double z = voxels[j].zc;
        if(x <= 0) onedge = 1;
        if(x >= sizex) onedge = 1;
        if(y <= 0) onedge = 1;
        if(y >= sizey) onedge = 1;
        if(z <= 0) onedge = 1;
        if(z >= sizez) onedge = 1;
        sumx = sumx + x;
        sumy = sumy + y;
        sumz = sumz + z;
      }
    }
    centerx = sumx/size;
    centery = sumy/size;
    centerz = sumz/size;
    goodgrain[i].centroidx = centerx;
    goodgrain[i].centroidy = centery;
    goodgrain[i].centroidz = centerz;
    goodgrain[i].numvoxels = size;
    goodgrain[i].surfacegrain = onedge;
  }
}


void  MicroGen3D::find_moments ()
{
  int count = 0;
  double sumxx = 0;
  double sumyy = 0;
  double sumzz = 0;
  double sumxy = 0;
  double sumxz = 0;
  double sumyz = 0;
  for(int i = 0; i < numgrains; i++)
  {
    sumxx = 0;
    sumyy = 0;
    sumzz = 0;
    sumxy = 0;
    sumxz = 0;
    sumyz = 0;
    double xc = goodgrain[i].centroidx;
    double yc = goodgrain[i].centroidy;
    double zc = goodgrain[i].centroidz;
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int gnum = voxels[j].grainname;
      if(gnum == i)
      {
        double x = voxels[j].xc;
        double y = voxels[j].yc;
        double z = voxels[j].zc;
        double x1 = x+(resx/2);
        double x2 = x-(resx/2);
        double y1 = y+(resy/2);
        double y2 = y-(resy/2);
        double z1 = z+(resz/2);
        double z2 = z-(resz/2);
        double xdist1 = (x1-xc);
        double ydist1 = (y1-yc);
        double zdist1 = (z1-zc);
        double xdist2 = (x1-xc);
        double ydist2 = (y1-yc);
        double zdist2 = (z2-zc);
        double xdist3 = (x1-xc);
        double ydist3 = (y2-yc);
        double zdist3 = (z1-zc);
        double xdist4 = (x1-xc);
        double ydist4 = (y2-yc);
        double zdist4 = (z2-zc);
        double xdist5 = (x2-xc);
        double ydist5 = (y1-yc);
        double zdist5 = (z1-zc);
        double xdist6 = (x2-xc);
        double ydist6 = (y1-yc);
        double zdist6 = (z2-zc);
        double xdist7 = (x2-xc);
        double ydist7 = (y2-yc);
        double zdist7 = (z1-zc);
        double xdist8 = (x2-xc);
        double ydist8 = (y2-yc);
        double zdist8 = (z2-zc);
        sumxx = sumxx + ((ydist1)*(ydist1))+((zdist1)*(zdist1)) + ((ydist2)*(ydist2))+((zdist2)*(zdist2)) + ((ydist3)*(ydist3))+((zdist3)*(zdist3)) + ((ydist4)*(ydist4))+((zdist4)*(zdist4)) + ((ydist5)*(ydist5))+((zdist5)*(zdist5)) + ((ydist6)*(ydist6))+((zdist6)*(zdist6)) + ((ydist7)*(ydist7))+((zdist7)*(zdist7)) + ((ydist8)*(ydist8))+((zdist8)*(zdist8));
        sumyy = sumyy + ((xdist1)*(xdist1))+((zdist1)*(zdist1)) + ((xdist2)*(xdist2))+((zdist2)*(zdist2)) + ((xdist3)*(xdist3))+((zdist3)*(zdist3)) + ((xdist4)*(xdist4))+((zdist4)*(zdist4)) + ((xdist5)*(xdist5))+((zdist5)*(zdist5)) + ((xdist6)*(xdist6))+((zdist6)*(zdist6)) + ((xdist7)*(xdist7))+((zdist7)*(zdist7)) + ((xdist8)*(xdist8))+((zdist8)*(zdist8));
        sumzz = sumzz + ((xdist1)*(xdist1))+((ydist1)*(ydist1)) + ((xdist2)*(xdist2))+((ydist2)*(ydist2)) + ((xdist3)*(xdist3))+((ydist3)*(ydist3)) + ((xdist4)*(xdist4))+((ydist4)*(ydist4)) + ((xdist5)*(xdist5))+((ydist5)*(ydist5)) + ((xdist6)*(xdist6))+((ydist6)*(ydist6)) + ((xdist7)*(xdist7))+((ydist7)*(ydist7)) + ((xdist8)*(xdist8))+((ydist8)*(ydist8));
        sumxy = sumxy + ((xdist1)*(ydist1)) + ((xdist2)*(ydist2)) + ((xdist3)*(ydist3)) + ((xdist4)*(ydist4)) + ((xdist5)*(ydist5)) + ((xdist6)*(ydist6)) + ((xdist7)*(ydist7)) + ((xdist8)*(ydist8));
        sumyz = sumyz + ((ydist1)*(zdist1)) + ((ydist2)*(zdist2)) + ((ydist3)*(zdist3)) + ((ydist4)*(zdist4)) + ((ydist5)*(zdist5)) + ((ydist6)*(zdist6)) + ((ydist7)*(zdist7)) + ((ydist8)*(zdist8));
        sumxz = sumxz + ((xdist1)*(zdist1)) + ((xdist2)*(zdist2)) + ((xdist3)*(zdist3)) + ((xdist4)*(zdist4)) + ((xdist5)*(zdist5)) + ((xdist6)*(zdist6)) + ((xdist7)*(zdist7)) + ((xdist8)*(zdist8));
      }
    }
    sumxx = sumxx*(resx/2)*(resy/2)*(resz/2);
    sumyy = sumyy*(resx/2)*(resy/2)*(resz/2);
    sumzz = sumzz*(resx/2)*(resy/2)*(resz/2);
    sumxy = sumxy*(resx/2)*(resy/2)*(resz/2);
    sumyz = sumyz*(resx/2)*(resy/2)*(resz/2);
    sumxz = sumxz*(resx/2)*(resy/2)*(resz/2);
    goodgrain[i].Ixx = sumxx;
    goodgrain[i].Iyy = sumyy;
    goodgrain[i].Izz = sumzz;
    goodgrain[i].Ixy = -sumxy;
    goodgrain[i].Ixz = -sumxz;
    goodgrain[i].Iyz = -sumyz;
  }
}
void  MicroGen3D::find_axes ()
{
  for (int i = 0; i < numgrains; i++)
  {
    double Ixx = goodgrain[i].Ixx;
    double Iyy = goodgrain[i].Iyy;
    double Izz = goodgrain[i].Izz;
    double Ixy = goodgrain[i].Ixy;
    double Iyz = goodgrain[i].Iyz;
    double Ixz = goodgrain[i].Ixz;
    double a = 1;
    double b = -Ixx-Iyy-Izz;
    double c = ((Ixx*Izz)+(Ixx*Iyy)+(Iyy*Izz)-(Ixz*Ixz)-(Ixy*Ixy)-(Iyz*Iyz));
    double d = ((Ixz*Iyy*Ixz)+(Ixy*Izz*Ixy)+(Iyz*Ixx*Iyz)-(Ixx*Iyy*Izz)-(Ixy*Iyz*Ixz)-(Ixy*Iyz*Ixz));
    double f = ((3*c/a)-((b/a)*(b/a)))/3;
    double g = ((2*(b/a)*(b/a)*(b/a))-(9*b*c/(a*a))+(27*(d/a)))/27;
    double h = (g*g/4)+(f*f*f/27);
    double rsquare = (g*g/4)-h;
    double r = pow(rsquare,0.5);
    double theta = 0;
    if(r == 0)
    {
      theta = 0;
    }
    if(r != 0)
    {
      theta = acos(-g/(2*r));
    }
    double r1 = 2*pow(r,0.33333333333)*cos(theta/3)-(b/(3*a));
    double r2 = -pow(r,0.33333333333)*(cos(theta/3)-(1.7320508*sin(theta/3)))-(b/(3*a));
    double r3 = -pow(r,0.33333333333)*(cos(theta/3)+(1.7320508*sin(theta/3)))-(b/(3*a));
    goodgrain[i].axis1 = r1;
    goodgrain[i].axis2 = r2;
    goodgrain[i].axis3 = r3;
  }
}
void  MicroGen3D::find_vectors ()
{
  for(int i = 0; i < numgrains; i++)
  {
    int size = goodgrain[i].numvoxels;
    if(size > 1)
    {
      double Ixx = goodgrain[i].Ixx;
      double Iyy = goodgrain[i].Iyy;
      double Izz = goodgrain[i].Izz;
      double Ixy = goodgrain[i].Ixy;
      double Iyz = goodgrain[i].Iyz;
      double Ixz = goodgrain[i].Ixz;
      double axis1 = goodgrain[i].axis1;
      double axis2 = goodgrain[i].axis2;
      double axis3 = goodgrain[i].axis3;
      double m[3][3];
      double e[3][1];
      double uber[3][3];
      double bmat[3][1];
      double vect[3][3];
      m[0][0] = Ixx;
      m[0][1] = Ixy;
      m[0][2] = Ixz;
      m[1][0] = Ixy;
      m[1][1] = Iyy;
      m[1][2] = Iyz;
      m[2][0] = Ixz;
      m[2][1] = Iyz;
      m[2][2] = Izz;
      e[0][0] = axis1;
      e[1][0] = axis2;
      e[2][0] = axis3;
      bmat[0][0] = 0.0000001;
      bmat[1][0] = 0.0000001;
      bmat[2][0] = 0.0000001;
      for(int j = 0; j < 3; j++)
      {
        uber[0][0] = Ixx-e[j][0];
        uber[0][1] = Ixy;
        uber[0][2] = Ixz;
        uber[1][0] = Ixy;
        uber[1][1] = Iyy-e[j][0];
        uber[1][2] = Iyz;
        uber[2][0] = Ixz;
        uber[2][1] = Iyz;
        uber[2][2] = Izz-e[j][0];
        double **uberelim;
        double **uberbelim;
        uberelim = new double *[3];
        uberbelim = new double *[3];
        for(int d = 0; d < 3; d++)
        {
          uberelim[d] = new double [3];
          uberbelim[d] = new double [1];
        }
        int elimcount = 0;
        int elimcount1 = 0;
        double q = 0;
        double sum = 0;
        double c = 0;
        for(int a = 0; a < 3; a++)
        {
          elimcount1 = 0;
          for(int b = 0; b < 3; b++)
          {
            uberelim[elimcount][elimcount1] = uber[a][b];
            elimcount1++;
          }
          uberbelim[elimcount][0] = bmat[a][0];
          elimcount++;
        }
        for(int k = 0; k < elimcount-1; k++)
        {
          for(int l = k+1; l < elimcount; l++)
          {
            c = uberelim[l][k]/uberelim[k][k];
            for(int m = k+1; m < elimcount; m++)
            {
              uberelim[l][m] = uberelim[l][m] - c*uberelim[k][m];
            }
            uberbelim[l][0] = uberbelim[l][0] - c*uberbelim[k][0];
          }
        }
        uberbelim[elimcount-1][0] = uberbelim[elimcount-1][0]/uberelim[elimcount-1][elimcount-1];
        for(int l = 1; l < elimcount; l++)
        {
          int m = (elimcount-1)-l;
          sum = 0;
          for(int n = m+1; n < elimcount; n++)
          {
            sum = sum + (uberelim[m][n]*uberbelim[n][0]);
          }
          uberbelim[m][0] = (uberbelim[m][0]-sum)/uberelim[m][m];
        }
        for(int p = 0; p < elimcount; p++)
        {
          q = uberbelim[p][0];
          vect[j][p] = q;
        }
      }
      double n1x = vect[0][0];
      double n1y = vect[0][1];
      double n1z = vect[0][2];
      double n2x = vect[1][0];
      double n2y = vect[1][1];
      double n2z = vect[1][2];
      double n3x = vect[2][0];
      double n3y = vect[2][1];
      double n3z = vect[2][2];
      goodgrain[i].axis1x = n1x;
      goodgrain[i].axis1y = n1y;
      goodgrain[i].axis1z = n1z;
      goodgrain[i].axis2x = n2x;
      goodgrain[i].axis2y = n2y;
      goodgrain[i].axis2z = n2z;
      goodgrain[i].axis3x = n3x;
      goodgrain[i].axis3y = n3y;
      goodgrain[i].axis3z = n3z;
    }
    if(size == 1)
    {
      double n1x = 1;
      double n1y = 0;
      double n1z = 0;
      double n2x = 0;
      double n2y = 1;
      double n2z = 0;
      double n3x = 0;
      double n3y = 0;
      double n3z = 1;
      goodgrain[i].axis1x = n1x;
      goodgrain[i].axis1y = n1y;
      goodgrain[i].axis1z = n1z;
      goodgrain[i].axis2x = n2x;
      goodgrain[i].axis2y = n2y;
      goodgrain[i].axis2z = n2z;
      goodgrain[i].axis3x = n3x;
      goodgrain[i].axis3y = n3y;
      goodgrain[i].axis3z = n3z;
    }
  }
}
void  MicroGen3D::measure_misorientations ()
{
  vector<double> misolist;
  double n1;
  double n2;
  double n3;
  for (int i = 0; i < numgrains; i++)
  {
    vector<int> nlist = goodgrain[i].neighborlist;
    double g1ea1 = goodgrain[i].avgeuler1;
    double g1ea2 = goodgrain[i].avgeuler2;
    double g1ea3 = goodgrain[i].avgeuler3;
    int size = int(nlist.size());
    for(int j=0;j<size;j++)
    {
      int nname = nlist[j];
      double g2ea1 = goodgrain[nname].avgeuler1;
      double g2ea2 = goodgrain[nname].avgeuler2;
      double g2ea3 = goodgrain[nname].avgeuler3;
      double w = getmisoquat(crystruct,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      misolist.push_back(w);
    }
    goodgrain[i].misorientationlist = misolist;
    misolist.clear();
  }
}
double MicroGen3D::getmisoquat(double crystruct,double misorientationtolerance,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wmin=9999999,na,nb,nc;
  double q1[4];
  double q2[4];
  double qc[4];
  double qco[4];
  q1[0]=sin((g1ea2/2.0))*cos(((g1ea1-g1ea3)/2.0));
  q1[1]=sin((g1ea2/2.0))*sin(((g1ea1-g1ea3)/2.0));
  q1[2]=cos((g1ea2/2.0))*sin(((g1ea1+g1ea3)/2.0));
  q1[3]=cos((g1ea2/2.0))*cos(((g1ea1+g1ea3)/2.0));
  q2[0]=sin((g2ea2/2.0))*cos(((g2ea1-g2ea3)/2.0));
  q2[1]=sin((g2ea2/2.0))*sin(((g2ea1-g2ea3)/2.0));
  q2[2]=cos((g2ea2/2.0))*sin(((g2ea1+g2ea3)/2.0));
  q2[3]=cos((g2ea2/2.0))*cos(((g2ea1+g2ea3)/2.0));
  q2[0]=q2[0];
  q2[1]=-q2[1];
  q2[2]=-q2[2];
  q2[3]=-q2[3];
  qc[0]=q1[0]*q2[0]-q1[1]*q2[1]-q1[2]*q2[2]-q1[3]*q2[3];
  qc[1]=q1[0]*q2[1]+q1[1]*q2[0]+q1[2]*q2[3]-q1[3]*q2[2];
  qc[2]=q1[0]*q2[2]-q1[1]*q2[3]+q1[2]*q2[0]+q1[3]*q2[1];
  qc[3]=q1[0]*q2[3]+q1[1]*q2[2]-q1[2]*q2[1]+q1[3]*q2[0];
  qc[0]=fabs(qc[0]);
  qc[1]=fabs(qc[1]);
  qc[2]=fabs(qc[2]);
  qc[3]=fabs(qc[3]);
  for(int i=0;i<4;i++)
  {
    qco[i]=100000;
    for(int j=0;j<4;j++)
    {
      if((qc[j] < qco[i] && i == 0) || (qc[j] < qco[i] && qc[j] > qco[i-1]))
      {
        qco[i] = qc[j];
      }
    }
  }
  wmin = qco[3];
  if(((qco[2]+qco[3])/(pow(2,0.5))) > wmin) wmin = ((qco[2]+qco[3])/(pow(2,0.5)));
  if(((qco[0]+qco[1]+qco[2]+qco[3])/2) > wmin) wmin = ((qco[0]+qco[1]+qco[2]+qco[3])/2);
  if(wmin < -1) wmin = -1;
  if(wmin > 1) wmin = 1;
  wmin = acos(wmin);
  wmin = (360.0/3.1415926535897)*wmin;
  return wmin;
}
void  MicroGen3D::find_colors()
{
  for(int i = 0; i < numgrains; i++)
  {
    double g1ea1 = goodgrain[i].avgeuler1;
    double g1ea2 = goodgrain[i].avgeuler2;
    double g1ea3 = goodgrain[i].avgeuler3;
    double go[3][3];
    double cd[3][1];
    double sd[3][1];
    go[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
    go[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
    go[0][2] = sin(g1ea3)*sin(g1ea2);
    go[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
    go[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
    go[1][2] =  cos(g1ea3)*sin(g1ea2);
    go[2][0] =  sin(g1ea1)*sin(g1ea2);
    go[2][1] = -cos(g1ea1)*sin(g1ea2);
    go[2][2] =  cos(g1ea2);
    cd[0][0] = go[0][0]*0+go[0][1]*0+go[0][2]*1;
    cd[1][0] = go[1][0]*0+go[1][1]*0+go[1][2]*1;
    cd[2][0] = go[2][0]*0+go[2][1]*0+go[2][2]*1;
    sd[0][0] = fabs(cd[0][0]);
    sd[1][0] = fabs(cd[1][0]);
    sd[2][0] = fabs(cd[2][0]);
    double smallest = 100000;
    double biggest = 100000;
    double middle = 100000;
    int place = 0;
    for(int j = 0; j < 3; j++)
    {
      place = 0;
      for(int k = 0; k < 3; k++)
      {
        if(sd[j][0] > sd[k][0]) place++;
        if((sd[j][0] == sd[k][0]) && (j != k) && (j < k)) place++;
      }
      if(place == 0) smallest = sd[j][0];
      if(place == 1) middle = sd[j][0];
      if(place == 2) biggest = sd[j][0];
    }
    double b = smallest;
    double g = middle-b;
    double r = biggest-g-b;
    double cur = r;
    if(g > cur) cur = g;
    if(b > cur) cur = b;
    r = r/cur;
    g = g/cur;
    b = b/cur;
    r = 0.75*r+0.25;
    g = 0.75*g+0.25;
    b = 0.75*b+0.25;
    goodgrain[i].red = r;
    goodgrain[i].green = g;
    goodgrain[i].blue = b;
    double loadx = cd[0][0];
    double loady = cd[1][0];
    double loadz = cd[2][0];
    double mag = loadx*loadx+loady*loady+loadz*loadz;
    mag = pow(mag,0.5);
    double theta1 = (loadx+loady+loadz)/(mag*1.732);
    theta1 = fabs(theta1);
    double theta2 = (loadx+loady-loadz)/(mag*1.732);
    theta2 = fabs(theta2);
    double theta3 = (loadx-loady+loadz)/(mag*1.732);
    theta3 = fabs(theta3);
    double theta4 = (-loadx+loady+loadz)/(mag*1.732);
    theta4 = fabs(theta4);
    double lambda1 = (loadx+loady)/(mag*1.414);
    lambda1 = fabs(lambda1);
    double lambda2 = (loadx+loadz)/(mag*1.414);
    lambda2 = fabs(lambda2);
    double lambda3 = (loadx-loady)/(mag*1.414);
    lambda3 = fabs(lambda3);
    double lambda4 = (loadx-loadz)/(mag*1.414);
    lambda4 = fabs(lambda4);
    double lambda5 = (loady+loadz)/(mag*1.414);
    lambda5 = fabs(lambda5);
    double lambda6 = (loady-loadz)/(mag*1.414);
    lambda6 = fabs(lambda6);
    double schmid1 = theta1*lambda6;
    double schmid2 = theta1*lambda4;
    double schmid3 = theta1*lambda3;
    double schmid4 = theta2*lambda3;
    double schmid5 = theta2*lambda2;
    double schmid6 = theta2*lambda5;
    double schmid7 = theta3*lambda1;
    double schmid8 = theta3*lambda5;
    double schmid9 = theta3*lambda4;
    double schmid10 = theta4*lambda1;
    double schmid11 = theta4*lambda2;
    double schmid12 = theta4*lambda6;
    double schmid = schmid1;
    if(schmid2 > schmid) schmid = schmid2;
    if(schmid3 > schmid) schmid = schmid3;
    if(schmid4 > schmid) schmid = schmid4;
    if(schmid5 > schmid) schmid = schmid5;
    if(schmid6 > schmid) schmid = schmid6;
    if(schmid7 > schmid) schmid = schmid7;
    if(schmid8 > schmid) schmid = schmid8;
    if(schmid9 > schmid) schmid = schmid9;
    if(schmid10 > schmid) schmid = schmid10;
    if(schmid11 > schmid) schmid = schmid11;
    if(schmid12 > schmid) schmid = schmid12;
    goodgrain[i].schmidfactor = schmid;
  }
}
void  MicroGen3D::find_convexities()
{
  for(int i = 0; i < numgrains; i++)
  {
    int size = goodgrain[i].numvoxels;
    if(size > 1)
    {
      double insidecount = 0;
      int vol = goodgrain[i].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double xc = goodgrain[i].centroidx;
      double yc = goodgrain[i].centroidy;
      double zc = goodgrain[i].centroidz;
      double rad1x = goodgrain[i].axis1x;
      double rad1y = goodgrain[i].axis1y;
      double rad1z = goodgrain[i].axis1z;
      double rad2x = goodgrain[i].axis2x;
      double rad2y = goodgrain[i].axis2y;
      double rad2z = goodgrain[i].axis2z;
      double rad3x = goodgrain[i].axis3x;
      double rad3y = goodgrain[i].axis3y;
      double rad3z = goodgrain[i].axis3z;
      double mag1 = (rad1x*rad1x)+(rad1y*rad1y)+(rad1z*rad1z);
      mag1 = pow(mag1,0.5);
      double mag2 = (rad2x*rad2x)+(rad2y*rad2y)+(rad2z*rad2z);
      mag2 = pow(mag2,0.5);
      double mag3 = (rad3x*rad3x)+(rad3y*rad3y)+(rad3z*rad3z);
      mag3 = pow(mag3,0.5);
      rad1x = rad1x/mag1;
      rad1y = rad1y/mag1;
      rad1z = rad1z/mag1;
      rad2x = rad2x/mag2;
      rad2y = rad2y/mag2;
      rad2z = rad2z/mag2;
      rad3x = rad3x/mag3;
      rad3y = rad3y/mag3;
      rad3z = rad3z/mag3;
      double I1 = goodgrain[i].axis1;
      double I2 = goodgrain[i].axis2;
      double I3 = goodgrain[i].axis3;
      I1 = (15*I1)/(4*3.1415926535897);
      I2 = (15*I2)/(4*3.1415926535897);
      I3 = (15*I3)/(4*3.1415926535897);
      double A = (I1+I2-I3)/2;
      double B = (I1+I3-I2)/2;
      double C = (I2+I3-I1)/2;
      double a = (A*A*A*A)/(B*C);
      a = pow(a,0.1);
      double b = B/A;
      b = pow(b,0.5)*a;
      double c = A/(a*a*a*b);
      double bovera = b/a;
      double covera = c/a;
      double rad1 = voxvol/(1.333333*3.1415926535897*bovera*covera);
      rad1 = pow(rad1,0.3333333);
      double rad2 = rad1*bovera;
      double rad3 = rad1*covera;
      for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
      {
        int gnum = voxels[j].grainname;
        if(gnum == i)
        {
          double x = voxels[j].xc;
          double y = voxels[j].yc;
          double z = voxels[j].zc;
          double axis[3][3];
          double diff[3][1];
          double axiselim[3][3];
          double diffelim[3][1];
          double constmat[3][1];
          axis[0][0] = rad1x;
          axis[0][1] = rad2x;
          axis[0][2] = rad3x;
          axis[1][0] = rad1y;
          axis[1][1] = rad2y;
          axis[1][2] = rad3y;
          axis[2][0] = rad1z;
          axis[2][1] = rad2z;
          axis[2][2] = rad3z;
          diff[0][0] = x-xc;
          diff[1][0] = y-yc;
          diff[2][0] = z-zc;
          int elimcount = 0;
          int elimcount1 = 0;
          double q = 0;
          double sum = 0;
          double c = 0;
          for(int a = 0; a < 3; a++)
          {
            elimcount1 = 0;
            for(int b = 0; b < 3; b++)
            {
              axiselim[elimcount][elimcount1] = axis[a][b];
              if(axiselim[elimcount][elimcount1] == 0)
              {
                axiselim[elimcount][elimcount1] = 0.000001;
              }
              elimcount1++;
            }
            diffelim[elimcount][0] = diff[a][0];
            elimcount++;
          }
          for(int k = 0; k < elimcount-1; k++)
          {
            for(int l = k+1; l < elimcount; l++)
            {
              c = axiselim[l][k]/axiselim[k][k];
              for(int m = k+1; m < elimcount; m++)
              {
                axiselim[l][m] = axiselim[l][m] - c*axiselim[k][m];
              }
              diffelim[l][0] = diffelim[l][0] - c*diffelim[k][0];
            }
          }
          diffelim[elimcount-1][0] = diffelim[elimcount-1][0]/axiselim[elimcount-1][elimcount-1];
          for(int l = 1; l < elimcount; l++)
          {
            int m = (elimcount-1)-l;
            sum = 0;
            for(int n = m+1; n < elimcount; n++)
            {
              sum = sum + (axiselim[m][n]*diffelim[n][0]);
            }
            diffelim[m][0] = (diffelim[m][0]-sum)/axiselim[m][m];
          }
          for(int p = 0; p < elimcount; p++)
          {
            q = diffelim[p][0];
            constmat[p][0] = q;
          }
          double inside = 1-(((constmat[0][0])/rad3)*((constmat[0][0])/rad3))-(((constmat[1][0])/rad2)*((constmat[1][0])/rad2))-(((constmat[2][0])/rad1)*((constmat[2][0])/rad1));
          if(inside >= 0)
          {
            insidecount++;
          }
        }
      }
      double convex = insidecount/size;
      goodgrain[i].convexity = convex;
    }
    if(size == 1)
    {
      double convex = 1;
      goodgrain[i].convexity = convex;
    }
  }
}
void MicroGen3D::volume_stats(string writename1,
                              string writename2,
                              string writename3,
                              string writename4,
                              string writename5,
                              string writename6,
                              string writename7,
                              string writename8,
                              string writename9)
{
  double actualgrains = 0;
  double misocount = 0;
  double avgvol = 0;
  double avglnvol = 0;
  double avgneigh = 0;
  double avglnneigh = 0;
  double avgbovera = 0;
  double avgcovera = 0;
  double avgcoverb = 0;
  double avgdiam = 0;
  double avglogdiam = 0;
  double avgdiam2 = 0;
  double avgschmid = 0;
  double avgem = 0;
  double neighcount = 0;
  double maxvol = 0;
  double maxneigh = 0;
  double maxdiam = 0;
  double maxlogdiam = 0;
  double maxbovera = 0;
  double maxcovera = 0;
  double maxcoverb = 0;
  double maxschmid = 0;
  double maxem = 0;
  double svn[100][5];
  double svs[100][5];
  double svbovera[100][5];
  double svcovera[100][5];
  double svcoverb[100][5];
  double svschmid[100][5];
  double svem[100][5];
  double eulercount[12][10][10][10];
  for(int temp = 0; temp < 100; temp++)
  {
    for(int temp2 = 0; temp2 < 5; temp2++)
    {
      svn[temp][temp2] = 0;
      svs[temp][temp2] = 0;
      svbovera[temp][temp2] = 0;
      svcovera[temp][temp2] = 0;
      svcoverb[temp][temp2] = 0;
      svschmid[temp][temp2] = 0;
      svem[temp][temp2] = 0;
    }
  }
  for(int iter0 = 0; iter0 < 12; iter0++)
  {
    for(int iter1 = 0; iter1 < 10; iter1++)
    {
      for(int iter2 = 0; iter2 < 10; iter2++)
      {
        for(int iter3 = 0; iter3 < 10; iter3++)
        {
          eulercount[iter0][iter1][iter2][iter3] = 0;
        }
      }
    }
  }
  for(int i = 0; i < numgrains; i++)
  {
    int onedge = goodgrain[i].surfacegrain;
    if(onedge == 0)
    {
      double g1ea1 = goodgrain[i].avgeuler1;
      double g1ea2 = goodgrain[i].avgeuler2;
      double g1ea3 = goodgrain[i].avgeuler3;
      actualgrains++;
      int vol = goodgrain[i].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double logvol = log(voxvol);
      double rad3 = 0.75*(1/3.1415926535897)*voxvol;
      double diam = 2*pow(rad3,0.333333333);
      int diamint = int(diam);
      double logdiam = log(diam);
      for(int k = 0; k < 24; k++)
      {
        double o[3][3];
        double ga[3][3];
        double m1[3][3];
        if (k == 0)
        {
          o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
          o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
          o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
        }
        else if (k == 1)
        {
          o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] =  0.0;
          o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = -1.0;
          o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] =  0.0;
        }
        else if (k == 2)
        {
          o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] =  0.0;
          o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
          o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
        }
        else if (k == 3)
        {
          o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
          o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = 1.0;
          o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] = 0.0;
        }
        else if (k == 4)
        {
          o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = -1.0;
          o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] =  0.0;
          o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] =  0.0;
        }
        else if (k == 5)
        {
          o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = 1.0;
          o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] = 0.0;
          o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] = 0.0;
        }
        else if (k == 6)
        {
          o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] =  0.0;
          o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] =  0.0;
          o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
        }
        else if (k == 7)
        {
          o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] = 0.0;
          o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
          o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = 1.0;
        }
        else if (k == 8)
        {
          o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] = 0.0;
          o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] = 0.0;
          o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = 1.0;
        }
        else if (k == 9)
        {
          o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] = 0.0;
          o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] = 0.0;
          o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
        }
        else if (k == 10)
        {
          o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] = 0.0;
          o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = 1.0;
          o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] = 0.0;
        }
        else if (k == 11)
        {
          o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = 1.0;
          o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] = 0.0;
          o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] = 0.0;
        }
        else if (k == 12)
        {
          o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] =  0.0;
          o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = -1.0;
          o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] =  0.0;
        }
        else if (k == 13)
        {
          o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = -1.0;
          o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] =  0.0;
          o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] =  0.0;
        }
        else if (k == 14)
        {
          o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] =  0.0;
          o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = -1.0;
          o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] =  0.0;
        }
        else if (k == 15)
        {
          o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = -1.0;
          o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] =  0.0;
          o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] =  0.0;
        }
        else if (k == 16)
        {
          o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] = 0.0;
          o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = 1.0;
          o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] = 0.0;
        }
        else if (k == 17)
        {
          o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = 1.0;
          o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] = 0.0;
          o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] = 0.0;
        }
        else if (k == 18)
        {
          o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] =  0.0;
          o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] =  0.0;
          o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
        }
        else if (k == 19)
        {
          o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
          o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = 1.0;
          o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] = 0.0;
        }
        else if (k == 20)
        {
          o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = 1.0;
          o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
          o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] = 0.0;
        }
        else if (k == 21)
        {
          o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] =  0.0;
          o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = -1.0;
          o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] =  0.0;
        }
        else if (k == 22)
        {
          o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = -1.0;
          o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] =  0.0;
          o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] =  0.0;
        }
        else if (k == 23)
        {
          o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] =  0.0;
          o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] =  0.0;
          o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
        }
        ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
        ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
        ga[0][2] = sin(g1ea3)*sin(g1ea2);
        ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
        ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
        ga[1][2] =  cos(g1ea3)*sin(g1ea2);
        ga[2][0] =  sin(g1ea1)*sin(g1ea2);
        ga[2][1] = -cos(g1ea1)*sin(g1ea2);
        ga[2][2] =  cos(g1ea2);
        m1[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
        m1[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
        m1[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];
        m1[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
        m1[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
        m1[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];
        m1[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
        m1[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
        m1[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];
        double ea2 = acos(m1[2][2]);
        double cosine3 = (m1[1][2]/sin(ea2));
        double sine3 = (m1[0][2]/sin(ea2));
        double cosine1 = (-m1[2][1]/sin(ea2));
        double sine1 = (m1[2][0]/sin(ea2));
        double ea3 = acos(cosine3);
        double ea1 = acos(cosine1);
        if(sine3 < 0) ea3 = (2*3.1415926535897)-ea3;
        if(sine1 < 0) ea1 = (2*3.1415926535897)-ea1;
        int ea1bin = int(0.5+ea1/(3.1415926535897/18));
        int ea2bin = int(0.5+ea2/(3.1415926535897/18));
        int ea3bin = int(0.5+ea3/(3.1415926535897/18));
        if(ea1 >= 0 && ea2 >= 0 && ea3 >= 0 && ea1 <= (3.1415926535897/2) && ea2 <= (3.1415926535897/2) && ea3 <= (3.1415926535897/2))
        {
          if(diamint > 11) diamint = 11;
          eulercount[diamint][ea1bin][ea2bin][ea3bin]++;
        }
      }
      double I1 = goodgrain[i].axis1;
      double I2 = goodgrain[i].axis2;
      double I3 = goodgrain[i].axis3;
      I1 = (15*I1)/(4*3.1415926535897);
      I2 = (15*I2)/(4*3.1415926535897);
      I3 = (15*I3)/(4*3.1415926535897);
      double A = (I1+I2-I3)/2;
      double B = (I1+I3-I2)/2;
      double C = (I2+I3-I1)/2;
      double a = (A*A*A*A)/(B*C);
      a = pow(a,0.1);
      double b = B/A;
      b = pow(b,0.5)*a;
      double c = A/(a*a*a*b);
      double bovera = b/a;
      double covera = c/a;
      double coverb = c/b;
      int nnum = goodgrain[i].numneighbors;
      double lognnum = log(double(nnum));
      double schmid = goodgrain[i].schmidfactor;
      double em = goodgrain[i].convexity;
      vector<int> nlist = goodgrain[i].neighborlist;
      avgvol = avgvol+voxvol;
      avglnvol = avglnvol+logvol;
      avgneigh = avgneigh+nnum;
      avglnneigh = avglnneigh+lognnum;
      avgbovera = avgbovera+bovera;
      avgcovera = avgcovera+covera;
      avgcoverb = avgcoverb+coverb;
      avgdiam = avgdiam + diam;
      avglogdiam = avglogdiam+logdiam;
      avgschmid = avgschmid+schmid;
      avgem = avgem+em;
      svn[diamint][0]++;
      svbovera[diamint][0]++;
      svcovera[diamint][0]++;
      svcoverb[diamint][0]++;
      svschmid[diamint][0]++;
      svem[diamint][0]++;
      svn[diamint][1] = svn[diamint][1] + nnum;
      svn[diamint][3] = svn[diamint][3] + lognnum;
      svbovera[diamint][1] = svbovera[diamint][1] + bovera;
      svcovera[diamint][1] = svcovera[diamint][1] + covera;
      svcoverb[diamint][1] = svcoverb[diamint][1] + coverb;
      svschmid[diamint][1] = svschmid[diamint][1] + schmid;
      svem[diamint][1] = svem[diamint][1] + em;
      int size = int(nlist.size());
      for(int k=0;k<size;k++)
      {
        int neigh = nlist[k];
        int neighvol = goodgrain[neigh].numvoxels;
        double neighvoxvol = neighvol*resx*resy*resz;
        double neighlogvol = log(neighvoxvol);
        double neighrad3 = 0.75*(1/3.1415926535897)*neighvoxvol;
        double neighdiam = 2*pow(neighrad3,0.333333333);
        int neighdiamint = int(neighdiam);
        svs[diamint][0]++;
        svs[diamint][1] = svs[diamint][1] + neighdiam;
        avgdiam2 = avgdiam2 + neighdiam;
        neighcount++;
      }
      if(voxvol > maxvol) maxvol = voxvol;
      if(nnum > maxneigh) maxneigh = nnum;
      if(bovera > maxbovera) maxbovera = bovera;
      if(covera > maxcovera) maxcovera = covera;
      if(coverb > maxcoverb) maxcoverb = coverb;
      if(diam > maxdiam) maxdiam = diam;
      if(logdiam > maxlogdiam) maxlogdiam = logdiam;
      if(schmid > maxschmid) maxschmid = schmid;
      if(em > maxem) maxem = em;
    }
  }
  int maxdiamint = int(maxdiam);
  for(int temp3 = 0; temp3 < (maxdiamint+1); temp3++)
  {
    if(svn[temp3][0] != 0)
    {
      svn[temp3][1] = svn[temp3][1]/svn[temp3][0];
      svn[temp3][3] = svn[temp3][3]/svn[temp3][0];
      svs[temp3][1] = svs[temp3][1]/svs[temp3][0];
      svbovera[temp3][1] = svbovera[temp3][1]/svbovera[temp3][0];
      svcovera[temp3][1] = svcovera[temp3][1]/svcovera[temp3][0];
      svcoverb[temp3][1] = svcoverb[temp3][1]/svcoverb[temp3][0];
      svschmid[temp3][1] = svschmid[temp3][1]/svschmid[temp3][0];
      svem[temp3][1] = svem[temp3][1]/svem[temp3][0];
    }
  }
  avgvol = avgvol/actualgrains;
  avglnvol = avglnvol/actualgrains;
  avgneigh = avgneigh/actualgrains;
  avglnneigh = avglnneigh/actualgrains;
  avgbovera = avgbovera/actualgrains;
  avgcovera = avgcovera/actualgrains;
  avgcoverb = avgcoverb/actualgrains;
  avgdiam = avgdiam/actualgrains;
  avglogdiam = avglogdiam/actualgrains;
  avgdiam2 = avgdiam2/neighcount;
  avgschmid = avgschmid/actualgrains;
  avgem = avgem/actualgrains;
  maxvol = maxvol/avgvol;
  int vbw = int((maxvol/25)+1);
  int nbw = int((maxneigh/25)+1);
  double sbw1 = 1.0/25.0;
  double sbw2 = 1.0/25.0;
  double sbw3 = 1.0/25.0;
  double schw = 0.5/25.0;
  double emw = 1.0/25.0;
  double vbin[25];
  double nbin[25];
  double sbin1[25];
  double sbin2[25];
  double sbin3[25];
  double schbin[25];
  double embin[25];
  double svnbin[100][25];
  double svsbin[100][100];
  double svboverabin[100][25];
  double svcoverabin[100][25];
  double svcoverbbin[100][25];
  double svschmidbin[100][25];
  double svembin[100][25];
  double misobin[10];
  double microbin[10];
  for(int a = 0; a < 100; a++)
  {
    for(int b = 0; b < 25; b++)
    {
      svnbin[a][b] = 0;
      svboverabin[a][b] = 0;
      svcoverabin[a][b] = 0;
      svcoverbbin[a][b] = 0;
      svschmidbin[a][b] = 0;
      svembin[a][b] = 0;
    }
    for(int d = 0; d < 100; d++)
    {
      svsbin[a][d] = 0;
    }
  }
  for(int c = 0; c < 25; c++)
  {
    vbin[c] = 0;
    nbin[c] = 0;
    sbin1[c] = 0;
    sbin2[c] = 0;
    sbin3[c] = 0;
    schbin[c] = 0;
    embin[c] = 0;
  }
  for(int e = 0; e < 10; e++)
  {
    misobin[e] = 0;
    microbin[e] = 0;
  }
  double sdvol = 0;
  double sdlnvol = 0;
  double sdneigh = 0;
  double sdlnneigh = 0;
  double sdbovera = 0;
  double sdcovera = 0;
  double sdcoverb = 0;
  double sddiam = 0;
  double sdlogdiam = 0;
  double sddiam2 = 0;
  double sdschmid = 0;
  double sdem = 0;
  for(int j = 0; j < numgrains; j++)
  {
    int onedge = goodgrain[j].surfacegrain;
    if(onedge == 0)
    {
      int vol = goodgrain[j].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double logvol = log(voxvol);
      double rad3 = 0.75*(1/3.1415926535897)*voxvol;
      double diam = 2*pow(rad3,0.333333333);
      int diamint = int(diam);
      double logdiam = log(diam);
      double I1 = goodgrain[j].axis1;
      double I2 = goodgrain[j].axis2;
      double I3 = goodgrain[j].axis3;
      I1 = (15*I1)/(4*3.1415926535897);
      I2 = (15*I2)/(4*3.1415926535897);
      I3 = (15*I3)/(4*3.1415926535897);
      double A = (I1+I2-I3)/2;
      double B = (I1+I3-I2)/2;
      double C = (I2+I3-I1)/2;
      double a = (A*A*A*A)/(B*C);
      a = pow(a,0.1);
      double b = B/A;
      b = pow(b,0.5)*a;
      double c = A/(a*a*a*b);
      double bovera = b/a;
      double covera = c/a;
      double coverb = c/b;
      int nnum = goodgrain[j].numneighbors;
      double lognnum = log(double(nnum));
      double schmid = goodgrain[j].schmidfactor;
      double em = goodgrain[j].convexity;
      vector<int> nlist = goodgrain[j].neighborlist;
      sdvol = sdvol + ((voxvol-avgvol)*(voxvol-avgvol));
      sdlnvol = sdlnvol + ((logvol-avglnvol)*(logvol-avglnvol));
      sdneigh = sdneigh + ((nnum-avgneigh)*(nnum-avgneigh));
      sdlnneigh = sdlnneigh + ((lognnum-avglnneigh)*(lognnum-avglnneigh));
      sdbovera = sdbovera + ((bovera-avgbovera)*(bovera-avgbovera));
      sdcovera = sdcovera + ((covera-avgcovera)*(covera-avgcovera));
      sdcoverb = sdcoverb + ((coverb-avgcoverb)*(coverb-avgcoverb));
      sddiam = sddiam + ((diam-avgdiam)*(diam-avgdiam));
      sdlogdiam = sdlogdiam + ((logdiam-avglogdiam)*(logdiam-avglogdiam));
      sdschmid = sdschmid + ((schmid-avgschmid)*(schmid-avgschmid));
      sdem = sdem + ((em-avgem)*(em-avgem));
      svn[diamint][2] = svn[diamint][2] + ((nnum-svn[diamint][1])*(nnum-svn[diamint][1]));
      svn[diamint][4] = svn[diamint][4] + ((lognnum-svn[diamint][3])*(lognnum-svn[diamint][3]));
      svbovera[diamint][2] = svbovera[diamint][2] + ((bovera-svbovera[diamint][1])*(bovera-svbovera[diamint][1]));
      svcovera[diamint][2] = svcovera[diamint][2] + ((covera-svcovera[diamint][1])*(covera-svcovera[diamint][1]));
      svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb-svcoverb[diamint][1])*(coverb-svcoverb[diamint][1]));
      svschmid[diamint][2] = svschmid[diamint][2] + ((schmid-svschmid[diamint][1])*(schmid-svschmid[diamint][1]));
      svem[diamint][2] = svem[diamint][2] + ((em-svem[diamint][1])*(em-svem[diamint][1]));
      int size = int(nlist.size());
      for(int k=0;k<size;k++)
      {
        int neigh = nlist[k];
        int neighvol = goodgrain[neigh].numvoxels;
        double neighvoxvol = neighvol*resx*resy*resz;
        double neighlogvol = log(neighvoxvol);
        double neighrad3 = 0.75*(1/3.1415926535897)*neighvoxvol;
        double neighdiam = 2*pow(neighrad3,0.333333333);
        int neighdiamint = int(neighdiam);
        svs[diamint][2] = svs[diamint][2] + ((neighdiam-svs[diamint][1])*(neighdiam-svs[diamint][1]));
        svsbin[diamint][neighdiamint]++;
        sddiam2 = sddiam2 + ((neighdiam-avgdiam2)*(neighdiam-avgdiam2));
      }
      voxvol = voxvol/avgvol;
      int vcur = int(voxvol/vbw);
      if(vcur == 25) vcur = 24;
      int ncur = int(nnum/nbw);
      if(ncur == 25) ncur = 24;
      int s1cur = int(bovera/sbw1);
      if(s1cur == 25) s1cur = 24;
      int s2cur = int(covera/sbw2);
      if(s2cur == 25) s2cur = 24;
      int s3cur = int(coverb/sbw3);
      if(s3cur == 25) s3cur = 24;
      int schcur = int(schmid/schw);
      if(schcur == 25) schcur = 24;
      int emcur = int(em/emw);
      if(emcur == 25) emcur = 24;
      vbin[vcur]++;
      nbin[ncur]++;
      sbin1[s1cur]++;
      sbin2[s2cur]++;
      sbin3[s3cur]++;
      schbin[schcur]++;
      embin[emcur]++;
      svnbin[diamint][ncur]++;
      svboverabin[diamint][s1cur]++;
      svcoverabin[diamint][s2cur]++;
      svcoverbbin[diamint][s3cur]++;
      svschmidbin[diamint][schcur]++;
      svembin[diamint][emcur]++;
    }
  }
  for(int temp4 = 0; temp4 < (maxdiamint+1); temp4++)
  {
    if(svn[temp4][0] != 0)
    {
      svn[temp4][2] = svn[temp4][2]/svn[temp4][0];
      svn[temp4][4] = svn[temp4][4]/svn[temp4][0];
      svs[temp4][2] = svs[temp4][2]/svs[temp4][0];
      svbovera[temp4][2] = svbovera[temp4][2]/svbovera[temp4][0];
      svcovera[temp4][2] = svcovera[temp4][2]/svcovera[temp4][0];
      svcoverb[temp4][2] = svcoverb[temp4][2]/svcoverb[temp4][0];
      svschmid[temp4][2] = svschmid[temp4][2]/svschmid[temp4][0];
      svbovera[temp4][3] = svbovera[temp4][1]*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
      svbovera[temp4][4] = (1-svbovera[temp4][1])*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
      svcovera[temp4][3] = svcovera[temp4][1]*(((svcovera[temp4][1]*(1-svcovera[temp4][1]))/svcovera[temp4][2])-1);
      svcovera[temp4][4] = (1-svcovera[temp4][1])*(((svcovera[temp4][1]*(1-svcovera[temp4][1]))/svcovera[temp4][2])-1);
      svcoverb[temp4][3] = svcoverb[temp4][1]*(((svcoverb[temp4][1]*(1-svcoverb[temp4][1]))/svcoverb[temp4][2])-1);
      svcoverb[temp4][4] = (1-svcoverb[temp4][1])*(((svcoverb[temp4][1]*(1-svcoverb[temp4][1]))/svcoverb[temp4][2])-1);
      svn[temp4][2] = pow(svn[temp4][2],0.5);
      svn[temp4][4] = pow(svn[temp4][4],0.5);
      svs[temp4][2] = pow(svs[temp4][2],0.5);
      svbovera[temp4][2] = pow(svbovera[temp4][2],0.5);
      svcovera[temp4][2] = pow(svcovera[temp4][2],0.5);
      svcoverb[temp4][2] = pow(svcoverb[temp4][2],0.5);
      svschmid[temp4][2] = pow(svschmid[temp4][2],0.5);
      svem[temp4][2] = pow(svem[temp4][2],0.5);
    }
  }
  sdvol = sdvol/actualgrains;
  sdlnvol = sdlnvol/actualgrains;
  sdneigh = sdneigh/actualgrains;
  sdlnneigh = sdlnneigh/actualgrains;
  sdbovera = sdbovera/actualgrains;
  sdcovera = sdcovera/actualgrains;
  sdcoverb = sdcoverb/actualgrains;
  sddiam = sddiam/actualgrains;
  sdlogdiam = sdlogdiam/actualgrains;
  sddiam2 = sddiam2/neighcount;
  sdschmid = sdschmid/actualgrains;
  sdem = sdem/actualgrains;
  double volvar = sdvol;
  double vollnvar = sdlnvol;
  double neighvar = sdneigh;
  double neighlnvar = sdlnneigh;
  double boveravar = sdbovera;
  double coveravar = sdcovera;
  double coverbvar = sdcoverb;
  double diamvar = sddiam;
  double logdiamvar = sdlogdiam;
  double diamvar2 = sddiam2;
  double schmidvar = sdschmid;
  double emvar = sdem;
  double pbovera = avgbovera*(((avgbovera*(1-avgbovera))/boveravar)-1);
  double qbovera = (1-avgbovera)*(((avgbovera*(1-avgbovera))/boveravar)-1);
  double pcovera = avgcovera*(((avgcovera*(1-avgcovera))/coveravar)-1);
  double qcovera = (1-avgcovera)*(((avgcovera*(1-avgcovera))/coveravar)-1);
  double pcoverb = avgcoverb*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
  double qcoverb = (1-avgcoverb)*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
  sdvol = pow(sdvol,0.5);
  sdlnvol = pow(sdlnvol,0.5);
  sdneigh = pow(sdneigh,0.5);
  sdlnneigh = pow(sdlnneigh,0.5);
  sdbovera = pow(sdbovera,0.5);
  sdcovera = pow(sdcovera,0.5);
  sdcoverb = pow(sdcoverb,0.5);
  sddiam = pow(sddiam,0.5);
  sdlogdiam = pow(sdlogdiam,0.5);
  sddiam2 = pow(sddiam2,0.5);
  sdschmid = pow(sdschmid,0.5);
  sdem = pow(sdem,0.5);
  double svncr = 0;
  double svscr = 0;
  double svboveracr = 0;
  double svcoveracr = 0;
  double svcoverbcr = 0;
  double svschmidcr = 0;
  double svemcr = 0;
  for(int temp5 = 0; temp5 < (maxdiamint+1); temp5++)
  {
    svncr = svncr + (svn[temp5][0]*((svn[temp5][1]-avgneigh)*(svn[temp5][1]-avgneigh)));
    svscr = svscr + (svs[temp5][0]*((svs[temp5][1]-avgdiam2)*(svs[temp5][1]-avgdiam2)));
    svboveracr = svboveracr + (svbovera[temp5][0]*((svbovera[temp5][1]-avgbovera)*(svbovera[temp5][1]-avgbovera)));
    svcoveracr = svcoveracr + (svcovera[temp5][0]*((svcovera[temp5][1]-avgcovera)*(svcovera[temp5][1]-avgcovera)));
    svcoverbcr = svcoverbcr + (svcoverb[temp5][0]*((svcoverb[temp5][1]-avgcoverb)*(svcoverb[temp5][1]-avgcoverb)));
    svschmidcr = svschmidcr + (svschmid[temp5][0]*((svschmid[temp5][1]-avgschmid)*(svschmid[temp5][1]-avgschmid)));
    svemcr = svemcr + (svem[temp5][0]*((svem[temp5][1]-avgem)*(svem[temp5][1]-avgem)));
  }
  svncr = svncr/(actualgrains*neighvar);
  svscr = svscr/(neighcount*diamvar2);
  svboveracr = svboveracr/(actualgrains*boveravar);
  svcoveracr = svcoveracr/(actualgrains*coveravar);
  svcoverbcr = svcoverbcr/(actualgrains*coverbvar);
  svschmidcr = svschmidcr/(actualgrains*schmidvar);
  svemcr = svemcr/(actualgrains*emvar);
  for(int l = 0; l < numgrains; l++)
  {
    int onedge = goodgrain[l].surfacegrain;
    if(onedge == 0)
    {
      double microcount = 0;
      vector<double> misolist = goodgrain[l].misorientationlist;
      int nummisos = int(misolist.size());
      int size = int(misolist.size());
      for(int k=0;k<size;k++)
      {
        misocount++;
        double firstmiso = misolist[k];
        int misocur = int(firstmiso/6.5);
        if(misocur == 10) misocur = 9;
        misobin[misocur]++;
        if(firstmiso < 15) microcount++;
      }
      microcount = microcount/nummisos;
      int microcur = int(microcount/0.25);
      if(microcur == 4) microcur = 3;
      microbin[microcur]++;
    }
  }
  double orand[15][2];
  double delta = 3.1415926535897/18;
  for(int temp50 = 0; temp50 < 12; temp50++)
  {
    double texindex = 0;
    double texstrength = 0;
    for(int iter51 = 0; iter51 < 10; iter51++)
    {
      for(int iter52 = 0; iter52 < 10; iter52++)
      {
        for(int iter53 = 0; iter53 < 10; iter53++)
        {
          double f = 0;
          if(iter52 == 0)
          {
            f = (3.1415926535897*3.1415926535897/4)*(double(eulercount[temp50][iter51][iter52][iter53])/double(svn[temp50][0]))/((delta)*(delta)*cos(double((iter52*delta)+(delta/2))));
          }
          if(iter52 == 10)
          {
            f = (3.1415926535897*3.1415926535897/4)*(double(eulercount[temp50][iter51][iter52][iter53])/double(svn[temp50][0]))/((delta)*(delta)*cos(double((iter52*delta)-(delta/2))));
          }
          if(iter52 != 0 && iter52 != 10)
          {
            f = (3.1415926535897*3.1415926535897/4)*(double(eulercount[temp50][iter51][iter52][iter53])/double(svn[temp50][0]))/((delta)*(delta)*(cos(double((iter52*delta)-(delta/2)))-cos(double((iter52*delta)+(delta/2)))));
          }
          texindex = texindex + (f*f);
        }
      }
    }
    texindex = texindex/(10*10*10);
    texstrength = pow(texindex,0.5);
    orand[temp50][0] = texindex;
    orand[temp50][1] = texstrength;
  }
    ofstream outFile;
    outFile.open(writename1.c_str());
  outFile << "INDIVIDUAL DISTRIBUTIONS" << endl;
  outFile << endl;
  outFile << "Parameter" << " " << "Avg." << "  " << "Std. Dev." << endl;
  outFile << "Volume" << "  " << avgvol << "  " << sdvol << endl;
  outFile << "Volume" << "  " << avglnvol << "  " << sdlnvol << endl;
  outFile << "Neighbors" << " " << avgneigh << "  " << sdneigh << endl;
  outFile << "Neighbors" << " " << avglnneigh << "  " << sdlnneigh << endl;
  outFile << "b/a" << " " << avgbovera << " " << sdbovera << endl;
  outFile << "c/a" << " " << avgcovera << " " << sdcovera << endl;
  outFile << "c/b" << " " << avgcoverb << " " << sdcoverb <<  endl;
  outFile << "Diameter" << "  " << avgdiam << " " << sddiam << endl;
  outFile << "Schmid Factor" << " " << avgschmid << " " << sdschmid << endl;
  outFile << "Ellipsoidal Misfit" << "  " << avgem << " " << sdem << endl;
  outFile << endl;
  outFile << "Volume Bin" << "  " << "Count" << " " << "Neighbor Bin" << "  " << "Count" << " " << "b/a Bin" << " " << "Count" << " " << "c/a Bin" << " " << "Count" << " " << "c/b Bin" << " " << "Count" << " " << "Schmid Factor Bin" << " " << "Count" << " " << "Ellipsoidal Misfit Bin" << "  " << "Count" << endl;
  for(int temp6 = 0; temp6 < 25; temp6++)
  {
    outFile << (vbw*temp6) << " " << vbin[temp6]/actualgrains << "  " << (nbw*temp6) << " " << nbin[temp6]/actualgrains << "  " << (sbw1*temp6) << "  " << sbin1[temp6]/actualgrains << " " << (sbw2*temp6) << "  " << sbin2[temp6]/actualgrains << " " << (sbw3*temp6) << "  " << sbin3[temp6]/actualgrains << " " << (schw*temp6) << "  " << schbin[temp6]/actualgrains << "  " << (emw*temp6) << " " << embin[temp6]/actualgrains << endl;
  }
  outFile << endl;
  outFile << "CORRELATIONS" << endl;
  outFile << endl;
  outFile << "Size V Neighbors" << "  " << svncr << endl;
  outFile << "Size V Neighbor Size" << "  " << svscr << endl;
  outFile << "Size V b/a" << "  " << svboveracr << endl;
  outFile << "Size V c/a" << "  " << svcoveracr << endl;
  outFile << "Size V c/b" << "  " << svcoverbcr << endl;
  outFile << "Size V Schmid Factor" << "  " << svschmidcr << endl;
  outFile << "Size V Ellipsoidal Misfit" << " " << svemcr << endl;
  outFile << endl;
  outFile << "Diameter" << "  " << "Count" << " " << "Avg. Neighbors" << "  " << "Std. Dev. Neighbors" << " " << "Avg. Ln Neighbors" << " " << "Std. Dev.  Ln Neighbors" << " " << "Avg. Neighbor Diameter" << "  " << "Std. Dev. Neighbor Diameter" << " " <<"Avg. b/a" << " " << "Std. Dev. b/a" << " " << "Avg. c/a" << "  " << "Std. Dev. c/a" << " " << "Avg. c/b" << "  " << "Std. Dev. c/b" << " " <<"P b/a" << "  " << "Q Dev. b/a" << "  " << "P c/a" << " " << "Q Dev. c/a" << "  " << "P c/b" << " " << "Q Dev. c/b" << "  " << "Avg. Schmid Factor" << "  " << "Std. Dev. Schmid Factor" << " " << "Avg. Ellipsoidal misfot" << " " << "Std. Dev. Ellipsoidal misfot" << endl;
  for(int temp7 = 0; temp7 < (maxdiamint+1); temp7++)
  {
    outFile << temp7 << " " << svn[temp7][0] << " " << svn[temp7][1] << " " << svn[temp7][2] << " " << svn[temp7][3] << " " << svn[temp7][4] << " " << svs[temp7][1] << " " << svs[temp7][2] << " " << svbovera[temp7][1] << "  " << svbovera[temp7][2] << "  " << svcovera[temp7][1] << "  " << svcovera[temp7][2] << "  " << svcoverb[temp7][1] << "  " << svcoverb[temp7][2] << "  " << svbovera[temp7][3] << "  " << svbovera[temp7][4] << "  " << svcovera[temp7][3] << "  " << svcovera[temp7][4] << "  " << svcoverb[temp7][3] << "  " << svcoverb[temp7][4] << "  " << svschmid[temp7][1] << "  " << svschmid[temp7][2] << "  " << svem[temp7][1] << "  " << svem[temp7][2] << endl;
  }
  outFile << endl;
  for(int temp8 = 0; temp8 < 25; temp8++)
  {
    for(int temp9 = 0; temp9 < (maxdiamint+1); temp9++)
    {
      outFile << (svnbin[temp9][temp8]/svn[temp9][0]) << "  ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp10 = 0; temp10 < (maxdiamint+1); temp10++)
  {
    for(int temp11 = 0; temp11 < (maxdiamint+1); temp11++)
    {
      outFile << (svsbin[temp11][temp10]/svs[temp11][0]) << " ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp12 = 0; temp12 < 25; temp12++)
  {
    for(int temp13 = 0; temp13 < (maxdiamint+1); temp13++)
    {
      outFile << (svboverabin[temp13][temp12]/svn[temp13][0]) << "  ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp14 = 0; temp14 < 25; temp14++)
  {
    for(int temp15 = 0; temp15 < (maxdiamint+1); temp15++)
    {
      outFile << (svcoverabin[temp15][temp14]/svn[temp15][0]) << "  ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp16 = 0; temp16 < 25; temp16++)
  {
    for(int temp17 = 0; temp17 < (maxdiamint+1); temp17++)
    {
      outFile << (svcoverbbin[temp17][temp16]/svn[temp17][0]) << "  ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp20 = 0; temp20 < 25; temp20++)
  {
    for(int temp21 = 0; temp21 < (maxdiamint+1); temp21++)
    {
      outFile << (svschmidbin[temp21][temp20]/svn[temp21][0]) << "  ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp22 = 0; temp22 < 25; temp22++)
  {
    for(int temp23= 0; temp23< (maxdiamint+1); temp23++)
    {
      outFile << (svembin[temp23][temp22]/svn[temp23][0]) << "  ";
    }
    outFile << endl;
  }
  outFile << endl;
  for(int temp24 = 0; temp24 < 10; temp24++)
  {
    outFile << (misobin[temp24]/misocount) << endl;
  }
  outFile << endl;
  for(int temp25 = 0; temp25 < 10; temp25++)
  {
    outFile << (microbin[temp25]/actualgrains) << endl;
  }
  outFile << endl;
  for(int temp26 = 0; temp26 < 12; temp26++)
  {
    outFile << orand[temp26][0] << "  " << orand[temp26][1] << endl;
  }
  outFile.close();
    ofstream outFile1;
    outFile1.open(writename2.c_str());
  double probability = 0;
  double binwidth = (maxdiam*1.5)/double(numdiambins);
  double cummprobability = 0;
  for(int i = 0; i < numdiambins; i++)
  {
    double diameter = (binwidth*double(i))+(binwidth/2.0);
    double binval = log(diameter);
    probability = ((1/(sdlogdiam*pow((2.0*3.1415926535897),0.5)*diameter))*exp(-pow((binval-avglogdiam),2.0)/(2.0*pow(sdlogdiam,2.0))))*binwidth;
    cummprobability = cummprobability+probability;
    outFile1 << cummprobability << "  " << diameter << endl;
  }
  outFile1.close();
    ofstream outFile2;
    outFile2.open(writename3.c_str());
  probability = 0;
  cummprobability = 0;
  for(int i = 0; i < numsizebins; i++)
  {
    double p = svbovera[i][3];
    double q = svbovera[i][4];
    double pq = p+q;
    double inradicalpq = pq*sinh(1/pq)+(1/(810*pq*pq*pq*pq*pq*pq));
    double inradicalp = p*sinh(1/p)+(1/(810*p*p*p*p*p*p));
    double inradicalq = q*sinh(1/q)+(1/(810*q*q*q*q*q*q));
    inradicalpq = pow(inradicalpq,0.5);
    inradicalp = pow(inradicalp,0.5);
    inradicalq = pow(inradicalq,0.5);
    double e = exp(1.0);
    double gammapq = pow((6.28318/pq),0.5)*pow(((pq/e)*inradicalpq),pq);
    double gammap = pow((6.28318/p),0.5)*pow(((p/e)*inradicalp),p);
    double gammaq = pow((6.28318/q),0.5)*pow(((q/e)*inradicalq),q);
    cummprobability = 0;
    for(int j = 0; j < numshapebins; j++)
    {
      double bovera = (double(j)/double(numshapebins))+(1.0/double(numshapebins*2.0));
      probability = ((gammapq/(gammap*gammaq))*pow(bovera,(p-1.0))*pow((1.0-bovera),(q-1.0)))/double(numshapebins);
      cummprobability = cummprobability+probability;
      outFile2 << (double(j)/double(numshapebins)) << " " << cummprobability << endl;
    }
  }
  outFile2.close();
    ofstream outFile3;
    outFile3.open(writename4.c_str());
  probability = 0;
  for(int i = 0; i < numsizebins; i++)
  {
    double p = svcovera[i][3];
    double q = svcovera[i][4];
    double pq = p+q;
    double inradicalpq = pq*sinh(1/pq)+(1/(810*pq*pq*pq*pq*pq*pq));
    double inradicalp = p*sinh(1/p)+(1/(810*p*p*p*p*p*p));
    double inradicalq = q*sinh(1/q)+(1/(810*q*q*q*q*q*q));
    inradicalpq = pow(inradicalpq,0.5);
    inradicalp = pow(inradicalp,0.5);
    inradicalq = pow(inradicalq,0.5);
    double e = exp(1.0);
    double gammapq = pow((6.28318/pq),0.5)*pow(((pq/e)*inradicalpq),pq);
    double gammap = pow((6.28318/p),0.5)*pow(((p/e)*inradicalp),p);
    double gammaq = pow((6.28318/q),0.5)*pow(((q/e)*inradicalq),q);
    cummprobability = 0;
    for(int j = 0; j < numshapebins; j++)
    {
      double covera = (double(j)/double(numshapebins))+(1.0/double(numshapebins*2.0));
      probability = ((gammapq/(gammap*gammaq))*pow(covera,(p-1.0))*pow((1.0-covera),(q-1.0)))/double(numshapebins);
      cummprobability = cummprobability+probability;
      outFile3 << (double(j)/double(numshapebins)) << " " << cummprobability << endl;
    }
  }
  outFile3.close();
    ofstream outFile4;
    outFile4.open(writename5.c_str());
  probability = 0;
  for(int i = 0; i < numsizebins; i++)
  {
    double p = svcoverb[i][3];
    double q = svcoverb[i][4];
    double pq = p+q;
    double inradicalpq = pq*sinh(1/pq)+(1/(810*pq*pq*pq*pq*pq*pq));
    double inradicalp = p*sinh(1/p)+(1/(810*p*p*p*p*p*p));
    double inradicalq = q*sinh(1/q)+(1/(810*q*q*q*q*q*q));
    inradicalpq = pow(inradicalpq,0.5);
    inradicalp = pow(inradicalp,0.5);
    inradicalq = pow(inradicalq,0.5);
    double gammapq = pow((6.28318/pq),0.5)*pow(((pq/exp(1.0))*inradicalpq),pq);
    double gammap = pow((6.28318/p),0.5)*pow(((p/exp(1.0))*inradicalp),p);
    double gammaq = pow((6.28318/q),0.5)*pow(((q/exp(1.0))*inradicalq),q);
    cummprobability = 0;
    for(int j = 0; j < numshapebins; j++)
    {
      double coverb = (double(j)/double(numshapebins))+(1.0/double(numshapebins*2.0));
      probability = ((gammapq/(gammap*gammaq))*pow(coverb,(p-1.0))*pow((1.0-coverb),(q-1.0)))/double(numshapebins);
      cummprobability = cummprobability+probability;
      outFile4 << (double(j)/double(numshapebins)) << " " << cummprobability << endl;
    }
  }
  outFile4.close();
    ofstream outFile5;
    outFile5.open(writename6.c_str());
  probability = 0;
  cummprobability = 0;
  for(int i = 0; i < numsizebins; i++)
  {
    for(int j = 0; j < numneighbins; j++)
    {
      double numneigh = double(j)+double(1.0/2.0);
      double binval = log(numneigh);
      probability = (1/(svn[i][4]*pow((2.0*3.1415926535897),0.5)*numneigh))*exp(-pow((binval-svn[i][3]),2.0)/(2.0*pow(svn[i][4],2.0)));
      cummprobability = cummprobability+probability;
      outFile5 << j << "  " << probability << endl;
    }
  }
  outFile5.close();
    ofstream outFile6;
    outFile6.open(writename7.c_str());
  probability = 0;
  for(int i = 0; i < numsizebins; i++)
  {
    outFile6 << i;
    for(int j = 0; j < numsizebins; j++)
    {
      outFile6 << " " << (svsbin[i][j]/svs[i][0]);
    }
    outFile6 << endl;
  }
  outFile6.close();
    ofstream outFile7;
    outFile7.open(writename8.c_str());
  probability = 0;
  for(int i = 0; i < nummisobins; i++)
  {
    outFile7 << (misobin[i]/misocount) << endl;
  }
  outFile7.close();
    ofstream outFile8;
    outFile8.open(writename9.c_str());
  probability = 0;
  for(int i = 0; i < nummicrobins; i++)
  {
    outFile8 << (microbin[i]/actualgrains) << endl;
  }
  outFile8.close();
}
void  MicroGen3D::write_volume2(string writename10)
{
    ofstream outFile;
    outFile.open(writename10.c_str());
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    double ea1 = goodgrain[grainname].avgeuler1;
    double ea2 = goodgrain[grainname].avgeuler2;
    double ea3 = goodgrain[grainname].avgeuler3;
    double x = voxels[i].xc;
    double y = voxels[i].yc;
    double z = voxels[i].zc;
    double ci = voxels[i].confidence;
    outFile << grainname << " " << ea1 << " " << ea2 << " " << ea3 << " " << x << " " << y << " " << z << " " << ci << endl;
  }
  outFile.close();
}
void  MicroGen3D::create_visualization(string writename11)
{
    ofstream outFile;
    outFile.open(writename11.c_str());
  double x = 0;
  double y = 0;
  double z = 0;
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET STRUCTURED_POINTS" << endl;
  outFile << "DIMENSIONS " << xpoints << " " << ypoints << " " << zpoints << endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << endl;
  outFile << "SPACING " << resx << " " << -resy << " " << -resz << endl;
  outFile << "POINT_DATA " << xpoints*ypoints*zpoints << endl;
  outFile << endl;
  outFile << endl;
  outFile << "SCALARS GrainID int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    if(i%20 == 0 && i > 0) outFile << endl;
    int grainname = voxels[i].grainname;
    outFile << "   ";
    if(grainname < 10000) outFile << " ";
    if(grainname < 1000) outFile << " ";
    if(grainname < 100) outFile << " ";
    if(grainname < 10) outFile << " ";
    outFile << grainname;
  }
  outFile << endl;
  if(mergetwinsoption == 1)
  {
    outFile << "SCALARS WasTwin int  1" << endl;
    outFile << "LOOKUP_TABLE default" << endl;
    for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      if(i%20 == 0 && i > 0) outFile << endl;
      int grainname = voxels[i].grainname;
      int twinmerged = grains[grainname].gottwinmerged;
      outFile << "       ";
      outFile << twinmerged;
    }
  }
  outFile.close();
}


void  MicroGen3D::write_grains(string writename12)
{
    ofstream outFile;
    outFile.open(writename12.c_str());
  outFile << numgrains<< endl;
  for(int i=0;i<numgrains;i++)
  {
    double xc = goodgrain[i].centroidx;
    double yc = goodgrain[i].centroidy;
    double zc = goodgrain[i].centroidz;
    double ea1 = goodgrain[i].avgeuler1;
    double ea2 = goodgrain[i].avgeuler2;
    double ea3 = goodgrain[i].avgeuler3;
    int nnum = goodgrain[i].numneighbors;
    vector<int> nlist = goodgrain[i].neighborlist;
    outFile << i << " " << xc << "  " << yc << "  " << zc << "  " << ea1 << " " << ea2 << " " << ea3 << " " << nnum;
    int size = int(nlist.size());
    for(int k=0;k<size;k++)
    {
      int neigh = nlist[k];
      outFile << "  " << neigh;
    }
    outFile << endl;
  }
}

void  MicroGen3D::write_axisorientations(string writename14)
{
    ofstream outFile;
    outFile.open(writename14.c_str());
  outFile << numgrains << endl;
  for(int i = 0; i < numgrains; i++)
  {
    double r1x = goodgrain[i].axis1x;
    double r1y = goodgrain[i].axis1y;
    double r1z = goodgrain[i].axis1z;
    double r2x = goodgrain[i].axis2x;
    double r2y = goodgrain[i].axis2y;
    double r2z = goodgrain[i].axis2z;
    double r3x = goodgrain[i].axis3x;
    double r3y = goodgrain[i].axis3y;
    double r3z = goodgrain[i].axis3z;
    outFile << r1x << " " << r1y << " " << r1z << " " << r2x << " " << r2y << " " << r2z << " " << r3x << " " << r3y << " " << r3z << endl;
  }
  outFile.close();
}
void  MicroGen3D::write_eulerangles(string writename15)
{
    ofstream outFile;
    outFile.open(writename15.c_str());
  outFile << numgrains <<endl;
  for(int i = 0; i < numgrains; i++)
  {
    double ea1 = goodgrain[i].avgeuler1;
    double ea2 = goodgrain[i].avgeuler2;
    double ea3 = goodgrain[i].avgeuler3;
    outFile << ea1 << " " << ea2 << " " << ea3 << endl;
  }
  outFile.close();
}
void  MicroGen3D::find_boundarycenters(string writename13)
{
  int count = 0;
  double tempcount = 0;
  for(int a = 0; a < 250000; a++)
  {
    for(int b = 0; b < 5; b++)
    {
      bcent[a][b] = 0;
    }
  }
  for(int i = 0; i < numgrains; i++)
  {
    vector<int> nlist = goodgrain[i].neighborlist;
    int size = int(nlist.size());
    for(int k=0;k<size;k++)
    {
      tempcount = 0;
      int firstneigh = nlist[k];
      for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
      {
        int gnum = voxels[j].grainname;
        if(gnum == i)
        {
          int surfacevoxel = voxels[j].surfacevoxel;
          if(surfacevoxel == 1)
          {
            double x = voxels[j].xc;
            double y = voxels[j].yc;
            double z = voxels[j].zc;
            for(int temp = 0; temp < 6; temp++)
            {
              int firstvoxneigh = -1;
              if(temp == 0 && x > 0)
              {
                firstvoxneigh = voxels[j-1].grainname;
              }
              if(temp == 1 && x < sizex)
              {
                firstvoxneigh = voxels[j+1].grainname;
              }
              if(temp == 2 && y > 0)
              {
                firstvoxneigh = voxels[j-xpoints].grainname;
              }
              if(temp == 3 && y <sizey)
              {
                firstvoxneigh = voxels[j+xpoints].grainname;
              }
              if(temp == 4 && z > 0)
              {
                firstvoxneigh = voxels[j-(xpoints*ypoints)].grainname;
              }
              if(temp == 5 && z < sizez)
              {
                firstvoxneigh = voxels[j+(xpoints*ypoints)].grainname;
              }
              if(firstvoxneigh == firstneigh)
              {
                bcent[count][2] = bcent[count][2] + x;
                bcent[count][3] = bcent[count][3] + y;
                bcent[count][4] = bcent[count][4] + z;
                tempcount++;
                {break;}
              }
            }
          }
        }
      }
      bcent[count][0] = i;
      bcent[count][1] = firstneigh;
      bcent[count][2] = bcent[count][2]/tempcount;
      bcent[count][3] = bcent[count][3]/tempcount;
      bcent[count][4] = bcent[count][4]/tempcount;
      count++;
    }
  }
  ofstream outFile;
  outFile.open(writename13.c_str());
  outFile << count << endl;
  for(int l = 0; l < count; l++)
  {
    int grain1 = int(bcent[l][0]);
    int grain2 = int(bcent[l][1]);
    double xc = bcent[l][2];
    double yc = bcent[l][3];
    double zc = bcent[l][4];
    if(xc != 0 || yc != 0 || zc != 0)
    {
      outFile << grain1 << "  " << grain2 << "  " << xc << "  " << yc << "  " << zc << endl;
    }
  }
  outFile.close();
}


void  MicroGen3D::loadVolData(string inname1, int numvolbins)
{
    ifstream inputFile;
    inputFile.open(inname1.c_str());
  int count = 0;
  double dprob;
  double diam;
  for (long k = 0; k < numvolbins; k++)
    {
    inputFile >> dprob >> diam;
    diambin[k].dprobability = dprob;
    diambin[k].diameter = diam;
        count++;
    }
    inputFile.close();
}


void  MicroGen3D::loadboveraData(string inname2, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname2.c_str());
  int count = 0;
  double sprob;
  double r;
  int bnum=0;
  for (long j = 0; j < numsizebins; j++)
  {
    for (long k = 0; k < numshapebins; k++)
    {
      inputFile >> r >> sprob;
      bnum = (j*numshapebins)+k;
      boverabin[bnum].sprobability = sprob;
      boverabin[bnum].rad = r;
          count++;
    }
  }
   inputFile.close();
}

void  MicroGen3D::loadcoveraData(string inname3, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname3.c_str());
  int count = 0;
  double sprob;
  double r;
  int bnum=0;
  for (long j = 0; j < numsizebins; j++)
  {
    for (long k = 0; k < numshapebins; k++)
    {
      inputFile >> r >> sprob;
      bnum = (j*numshapebins)+k;
      coverabin[bnum].sprobability = sprob;
      coverabin[bnum].rad = r;
          count++;
    }
  }
   inputFile.close();
}

void  MicroGen3D::loadcoverbData(string inname4, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname4.c_str());
  int count = 0;
  double sprob;
  double r;
  int bnum=0;
  for (long j = 0; j < numsizebins; j++)
  {
    for (long k = 0; k < numshapebins; k++)
    {
      inputFile >> r >> sprob;
      bnum = (j*numshapebins)+k;
      coverbbin[bnum].sprobability = sprob;
      coverbbin[bnum].rad = r;
          count++;
    }
  }
   inputFile.close();
}

void  MicroGen3D::loadNData(string inname5, int numshapebins)
{
    ifstream inputFile;
    inputFile.open(inname5.c_str());
  int count = 0;
  double Nprob;
  double N;
  double junk;
  int bnum=0;
  for (long j = 0; j < numsizebins; j++)
  {
    for (long k = 0; k < numseNbins; k++)
    {
      inputFile >> junk >> Nprob >> N;
      bnum = (j*numseNbins)+k;
      seNbin[bnum].Nprobability = Nprob;
      seNbin[bnum].Nvalue = N;
          count++;
    }
  }
   inputFile.close();
}

void  MicroGen3D::loadorientData(string inname6, int numorients)
{
    ifstream inputFile;
    inputFile.open(inname6.c_str());
  int count = 0;
  double r1x;
  double r1y;
  double r1z;
  double r2x;
  double r2y;
  double r2z;
  double r3x;
  double r3y;
  double r3z;
  inputFile >> numorients;
  for (long k = 0; k < numorients; k++)
    {
    inputFile >> r1x >> r1y >> r1z >> r2x >> r2y >> r2z >> r3x >> r3y >> r3z;
    double denom1 = pow((r1x*r1x+r1y*r1y+r1z*r1z),0.5);
    double denom2 = pow((r2x*r2x+r2y*r2y+r2z*r2z),0.5);
    double denom3 = pow((r3x*r3x+r3y*r3y+r3z*r3z),0.5);
    r1x = r1x/denom1;
    r1y = r1y/denom1;
    r1z = r1z/denom1;
    r2x = r2x/denom2;
    r2y = r2y/denom2;
    r2z = r2z/denom2;
    r3x = r3x/denom3;
    r3y = r3y/denom3;
    r3z = r3z/denom3;
    orient[k].rad1x = r1x;
    orient[k].rad1y = r1y;
    orient[k].rad1z = r1z;
    orient[k].rad2x = r2x;
    orient[k].rad2y = r2y;
    orient[k].rad2z = r2z;
    orient[k].rad3x = r3x;
    orient[k].rad3y = r3y;
    orient[k].rad3z = r3z;
        count++;
    }
    inputFile.close();
}

void  MicroGen3D::loadeulerData(string inname7, int numeulers)
{
    ifstream inputFile;
    inputFile.open(inname7.c_str());
  int count = 0;
  double g1ea1;
  double g1ea2;
  double g1ea3;
  double ea1;
  double ea2;
  double ea3;
  int totalcount = 0;
  int bnum=0;
  inputFile >> numeulers;
  for (long iter = 0; iter < numeulers; iter++)
    {
    inputFile >> g1ea1 >> g1ea2 >> g1ea3;
    for(int k = 0; k < 24; k++)
    {
      double o[3][3];
      double ga[3][3];
      double m1[3][3];
      double mo[3][3];
      if (k == 0)
      {
        o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
        o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
        o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
      }
      else if (k == 1)
      {
        o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] =  0.0;
        o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = -1.0;
        o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] =  0.0;
      }
      else if (k == 2)
      {
        o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] =  0.0;
        o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] =  0.0;
        o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = -1.0;
      }
      else if (k == 3)
      {
        o[0][0] = 1.0; o[0][1] =  0.0; o[0][2] = 0.0;
        o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = 1.0;
        o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] = 0.0;
      }
      else if (k == 4)
      {
        o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = -1.0;
        o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] =  0.0;
        o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] =  0.0;
      }
      else if (k == 5)
      {
        o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = 1.0;
        o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] = 0.0;
        o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] = 0.0;
      }
      else if (k == 6)
      {
        o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] =  0.0;
        o[1][0] =  0.0; o[1][1] = 1.0; o[1][2] =  0.0;
        o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = -1.0;
      }
      else if (k == 7)
      {
        o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] = 0.0;
        o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] = 0.0;
        o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = 1.0;
      }
      else if (k == 8)
      {
        o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] = 0.0;
        o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] = 0.0;
        o[2][0] =  0.0; o[2][1] = 0.0; o[2][2] = 1.0;
      }
      else if (k == 9)
      {
        o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] = 0.0;
        o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] = 0.0;
        o[2][0] = 0.0; o[2][1] =  0.0; o[2][2] = 1.0;
      }
      else if (k == 10)
      {
        o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] = 0.0;
        o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = 1.0;
        o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] = 0.0;
      }
      else if (k == 11)
      {
        o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = 1.0;
        o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] = 0.0;
        o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] = 0.0;
      }
      else if (k == 12)
      {
        o[0][0] = 0.0; o[0][1] = -1.0; o[0][2] =  0.0;
        o[1][0] = 0.0; o[1][1] =  0.0; o[1][2] = -1.0;
        o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] =  0.0;
      }
      else if (k == 13)
      {
        o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = -1.0;
        o[1][0] = 1.0; o[1][1] =  0.0; o[1][2] =  0.0;
        o[2][0] = 0.0; o[2][1] = -1.0; o[2][2] =  0.0;
      }
      else if (k == 14)
      {
        o[0][0] =  0.0; o[0][1] = 1.0; o[0][2] =  0.0;
        o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = -1.0;
        o[2][0] = -1.0; o[2][1] = 0.0; o[2][2] =  0.0;
      }
      else if (k == 15)
      {
        o[0][0] =  0.0; o[0][1] = 0.0; o[0][2] = -1.0;
        o[1][0] = -1.0; o[1][1] = 0.0; o[1][2] =  0.0;
        o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] =  0.0;
      }
      else if (k == 16)
      {
        o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] = 0.0;
        o[1][0] = 0.0; o[1][1] = 0.0; o[1][2] = 1.0;
        o[2][0] = 1.0; o[2][1] = 0.0; o[2][2] = 0.0;
      }
      else if (k == 17)
      {
        o[0][0] = 0.0; o[0][1] = 0.0; o[0][2] = 1.0;
        o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] = 0.0;
        o[2][0] = 0.0; o[2][1] = 1.0; o[2][2] = 0.0;
      }
      else if (k == 18)
      {
        o[0][0] = 0.0; o[0][1] = 1.0; o[0][2] =  0.0;
        o[1][0] = 1.0; o[1][1] = 0.0; o[1][2] =  0.0;
        o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
      }
      else if (k == 19)
      {
        o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
        o[1][0] =  0.0; o[1][1] = 0.0; o[1][2] = 1.0;
        o[2][0] =  0.0; o[2][1] = 1.0; o[2][2] = 0.0;
      }
      else if (k == 20)
      {
        o[0][0] = 0.0; o[0][1] =  0.0; o[0][2] = 1.0;
        o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
        o[2][0] = 1.0; o[2][1] =  0.0; o[2][2] = 0.0;
      }
      else if (k == 21)
      {
        o[0][0] = -1.0; o[0][1] =  0.0; o[0][2] =  0.0;
        o[1][0] =  0.0; o[1][1] =  0.0; o[1][2] = -1.0;
        o[2][0] =  0.0; o[2][1] = -1.0; o[2][2] =  0.0;
      }
      else if (k == 22)
      {
        o[0][0] =  0.0; o[0][1] =  0.0; o[0][2] = -1.0;
        o[1][0] =  0.0; o[1][1] = -1.0; o[1][2] =  0.0;
        o[2][0] = -1.0; o[2][1] =  0.0; o[2][2] =  0.0;
      }
      else if (k == 23)
      {
        o[0][0] =  0.0; o[0][1] = -1.0; o[0][2] =  0.0;
        o[1][0] = -1.0; o[1][1] =  0.0; o[1][2] =  0.0;
        o[2][0] =  0.0; o[2][1] =  0.0; o[2][2] = -1.0;
      }
      for(int p = 0; p < 4; p++)
      {
        double o2[3][3];
        if (p == 0)
        {
          o2[0][0] = 1.0; o2[0][1] = 0.0; o2[0][2] = 0.0;
          o2[1][0] = 0.0; o2[1][1] = 1.0; o2[1][2] = 0.0;
          o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] = 1.0;
        }
        else if (p == 1)
        {
          o2[0][0] = 1.0; o2[0][1] = 0.0; o2[0][2] =  0.0;
          o2[1][0] = 0.0; o2[1][1] = -1.0; o2[1][2] = 0.0;
          o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] =  -1.0;
        }
        else if (p == 2)
        {
          o2[0][0] = -1.0; o2[0][1] =  0.0; o2[0][2] =  0.0;
          o2[1][0] = 0.0; o2[1][1] = 1.0; o2[1][2] =  0.0;
          o2[2][0] = 0.0; o2[2][1] =  0.0; o2[2][2] = -1.0;
        }
        else if (p == 3)
        {
          o2[0][0] = -1.0; o2[0][1] =  0.0; o2[0][2] = 0.0;
          o2[1][0] = 0.0; o2[1][1] = -1.0; o2[1][2] = 0.0;
          o2[2][0] = 0.0; o2[2][1] = 0.0; o2[2][2] = 1.0;
        }
        ga[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
        ga[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
        ga[0][2] = sin(g1ea3)*sin(g1ea2);
        ga[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
        ga[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
        ga[1][2] =  cos(g1ea3)*sin(g1ea2);
        ga[2][0] =  sin(g1ea1)*sin(g1ea2);
        ga[2][1] = -cos(g1ea1)*sin(g1ea2);
        ga[2][2] =  cos(g1ea2);

        mo[0][0] = o[0][0]*ga[0][0] + o[0][1]*ga[1][0] + o[0][2]*ga[2][0];
        mo[0][1] = o[0][0]*ga[0][1] + o[0][1]*ga[1][1] + o[0][2]*ga[2][1];
        mo[0][2] = o[0][0]*ga[0][2] + o[0][1]*ga[1][2] + o[0][2]*ga[2][2];
        mo[1][0] = o[1][0]*ga[0][0] + o[1][1]*ga[1][0] + o[1][2]*ga[2][0];
        mo[1][1] = o[1][0]*ga[0][1] + o[1][1]*ga[1][1] + o[1][2]*ga[2][1];
        mo[1][2] = o[1][0]*ga[0][2] + o[1][1]*ga[1][2] + o[1][2]*ga[2][2];
        mo[2][0] = o[2][0]*ga[0][0] + o[2][1]*ga[1][0] + o[2][2]*ga[2][0];
        mo[2][1] = o[2][0]*ga[0][1] + o[2][1]*ga[1][1] + o[2][2]*ga[2][1];
        mo[2][2] = o[2][0]*ga[0][2] + o[2][1]*ga[1][2] + o[2][2]*ga[2][2];

        m1[0][0] = mo[0][0]*o2[0][0] + mo[0][1]*o2[1][0] + mo[0][2]*o2[2][0];
        m1[0][1] = mo[0][0]*o2[0][1] + mo[0][1]*o2[1][1] + mo[0][2]*o2[2][1];
        m1[0][2] = mo[0][0]*o2[0][2] + mo[0][1]*o2[1][2] + mo[0][2]*o2[2][2];
        m1[1][0] = mo[1][0]*o2[0][0] + mo[1][1]*o2[1][0] + mo[1][2]*o2[2][0];
        m1[1][1] = mo[1][0]*o2[0][1] + mo[1][1]*o2[1][1] + mo[1][2]*o2[2][1];
        m1[1][2] = mo[1][0]*o2[0][2] + mo[1][1]*o2[1][2] + mo[1][2]*o2[2][2];
        m1[2][0] = mo[2][0]*o2[0][0] + mo[2][1]*o2[1][0] + mo[2][2]*o2[2][0];
        m1[2][1] = mo[2][0]*o2[0][1] + mo[2][1]*o2[1][1] + mo[2][2]*o2[2][1];
        m1[2][2] = mo[2][0]*o2[0][2] + mo[2][1]*o2[1][2] + mo[2][2]*o2[2][2];

        ea2 = acos(m1[2][2]);
        double cosine3 = (m1[1][2]/sin(ea2));
        double sine3 = (m1[0][2]/sin(ea2));
        double cosine1 = (-m1[2][1]/sin(ea2));
        double sine1 = (m1[2][0]/sin(ea2));
        ea3 = acos(cosine3);
        ea1 = acos(cosine1);
        if(sine3 < 0) ea3 = (2*3.1415926535897)-ea3;
        if(sine1 < 0) ea1 = (2*3.1415926535897)-ea1;
        int ea1bin = int(ea1/(3.1415926535897/36));
        int ea2bin = int(ea2/(3.1415926535897/36));
        int ea3bin = int(ea3/(3.1415926535897/36));
        if(ea1 >= 0 && ea2 >= 0 && ea3 >= 0 && ea1 <= (3.1415926535897/2) && ea2 <= (3.1415926535897/2) && ea3 <= (3.1415926535897/2))
        {
          int curcount = eulercount[ea1bin][ea2bin][ea3bin];
          int bnum = (ea1bin*18*18)+(ea2bin*18)+ea3bin;
          eulerbin[bnum].euler1 = ea1;
          eulerbin[bnum].euler2 = ea2;
          eulerbin[bnum].euler3 = ea3;
          eulercount[ea1bin][ea2bin][ea3bin]++;
          totalcount++;
        }
      }
    }
    }
    inputFile.close();
}

void  MicroGen3D::generate_grains(int numgrains)
{
  int good = 1;
  double totvol = 0;
  int curbin1 = 0;
  int curbin2 = 0;
  int curbin3 = 0;
  int curbin4 = 0;
  int curbin5 = 0;
  double r1 = 1;
  int bnum=0;
  int takencheck[1000];
  srand(static_cast<unsigned int>(time(NULL)));
  for(int l = 0; l < numgrains; l++)
  {
    curbin1 = 0;
    good = 1;
    double random = rg.Random();
    double random1 = random;
    for(int i = 0; i < numdiambins; i++)
    {
      double dprob = diambin[i].dprobability;
      if(dprob < random1) curbin1 = i;
    }
    double diam = diambin[curbin1].diameter;
    double vol = (4.0/3.0)*(3.1415926535897)*((diam/2.0)*(diam/2.0)*(diam/2.0));
    int diameter = int(diam);
    if(diameter >= numsizebins) diameter = numsizebins-1;
    int good = 0;
    while(good == 0)
    {
      random = rg.Random();
      double random2 = random;
      random = rg.Random();
      double random3 = random;
      for(int j = 0; j < numshapebins; j++)
      {
        bnum = (diameter*numshapebins)+j;
        double sprob1 = boverabin[bnum].sprobability;
        double sprob2 = coverabin[bnum].sprobability;
        if(sprob1 < random2) curbin2 = j;
        if(sprob2 < random3) curbin3 = j;
      }
      random = rg.Random();
      double random4 = 0.01*random;
      bnum = (diameter*numshapebins)+curbin2;
      double tempr2 = boverabin[bnum].rad;
      bnum = (diameter*numshapebins)+curbin3;
      double tempr3 = coverabin[bnum].rad;
      double coverb = tempr3/tempr2;
      double sprob3 = 0;
      for(int k = 0; k < numshapebins; k++)
      {
        bnum = (diameter*numshapebins)+k;
        double checkcoverb = coverbbin[bnum].rad;
        if(checkcoverb < coverb)
        {
          sprob3 = coverbbin[bnum].sprobability;
        }
      }
      if(sprob3 > random4) good = 1;
      if(coverb > 1) good = 0;
    }
    bnum = (diameter*numshapebins)+curbin2;
    double r2 = boverabin[bnum].rad;
    bnum = (diameter*numshapebins)+curbin3;
    double r3 = coverabin[bnum].rad;
    random = rg.Random();
    double random5 = random;
    int onum = int(random5*numorients);
    double r1x = orient[onum].rad1x;
    double r1y = orient[onum].rad1y;
    double r1z = orient[onum].rad1z;
    double r2x = orient[onum].rad2x;
    double r2y = orient[onum].rad2y;
    double r2z = orient[onum].rad2z;
    double r3x = orient[onum].rad3x;
    double r3y = orient[onum].rad3y;
    double r3z = orient[onum].rad3z;
    random = rg.Random();
    double random6 = random;
    for(int p = 0; p < numseNbins; p++)
    {
      bnum = (diameter*numseNbins)+p;
      double Nprob = seNbin[bnum].Nprobability;
      if(Nprob < random6) curbin5 = p;
    }
    bnum = (diameter*numshapebins)+curbin5;
    double N = seNbin[bnum].Nvalue;
    int gnum = l;
    tempgrain[l].grainname = gnum;
    tempgrain[l].volume = vol;
    tempgrain[l].axis1 = r1;
    tempgrain[l].axis2 = r2;
    tempgrain[l].axis3 = r3;
    tempgrain[l].axis1x = r1x;
    tempgrain[l].axis1y = r1y;
    tempgrain[l].axis1z = r1z;
    tempgrain[l].axis2x = r2x;
    tempgrain[l].axis2y = r2y;
    tempgrain[l].axis2z = r2z;
    tempgrain[l].axis3x = r3x;
    tempgrain[l].axis3y = r3y;
    tempgrain[l].axis3z = r3z;
    tempgrain[l].Nvalue = N;
    totvol = totvol + vol;
  }
  sizex = int(pow((totvol*0.9),0.33333));
  sizey = int(pow((totvol*0.9),0.33333));
  sizez = int(pow((totvol*0.9),0.33333));
  for(int i=0;i<numgrains;i++)
  {
    int maxgrain = 0;
    double maxvol = 0;
    for(int j=0;j<numgrains;j++)
    {
      double vol = tempgrain[j].volume;
      if(vol > maxvol && takencheck[j] != 1)
      {
        maxvol = vol;
        maxgrain = j;
      }
    }
    takencheck[maxgrain] = 1;
    double vol = tempgrain[maxgrain].volume;
    double r1 = tempgrain[maxgrain].axis1;
    double r2 = tempgrain[maxgrain].axis2;
    double r3 = tempgrain[maxgrain].axis3;
    double r1x = tempgrain[maxgrain].axis1x;
    double r1y = tempgrain[maxgrain].axis1y;
    double r1z = tempgrain[maxgrain].axis1z;
    double r2x = tempgrain[maxgrain].axis2x;
    double r2y = tempgrain[maxgrain].axis2y;
    double r2z = tempgrain[maxgrain].axis2z;
    double r3x = tempgrain[maxgrain].axis3x;
    double r3y = tempgrain[maxgrain].axis3y;
    double r3z = tempgrain[maxgrain].axis3z;
    double N = tempgrain[maxgrain].Nvalue;
    grains[i].grainname = i;
    grains[i].volume = vol;
    grains[i].axis1 = r1;
    grains[i].axis2 = r2;
    grains[i].axis3 = r3;
    grains[i].axis1x = r1x;
    grains[i].axis1y = r1y;
    grains[i].axis1z = r1z;
    grains[i].axis2x = r2x;
    grains[i].axis2y = r2y;
    grains[i].axis2z = r2z;
    grains[i].axis3x = r3x;
    grains[i].axis3y = r3y;
    grains[i].axis3z = r3z;
    grains[i].Nvalue = N;
  }
}

void  MicroGen3D::assign_eulers(int numgrains)
{
  int count = 0;
  double grainssleft = numgrains-count;
  double density = 0;
  double synea1=0,synea2=0,synea3=0;
  for(int j = 0; j < 18; j++)
  {
    for(int k = 0; k < 18; k++)
    {
      for(int l = 0; l < 18; l++)
      {
        double num = (numgrains)*(double(eulercount[j][k][l]))/numeulers;
        density = density + (eulercount[j][k][l]*eulercount[j][k][l]);
        double adjust = double(numgrains)/double(numeulers);
        num = num;
        int numint = int(num);
        for(int m = 0; m < numint; m++)
        {
          double random = rg.Random();
          double random1 = random;
          random = rg.Random();
          double random2 = random;
          random = rg.Random();
          double random3 = random;
          synea1 = ((3.1415926535897/36)*j)+((3.1415926535897/36)*random1);
          synea2 = ((3.1415926535897/36)*k)+((3.1415926535897/36)*random2);
          synea3 = ((3.1415926535897/36)*l)+((3.1415926535897/36)*random3);
          grains[count].euler1 = synea1;
          grains[count].euler2 = synea2;
          grains[count].euler3 = synea3;
          count++;
          if(count == numgrains) {break;}
        }
        if(count == numgrains) {break;}
      }
      if(count == numgrains) {break;}
    }
    if(count == numgrains) {break;}
  }
  double lastcur = 1;
  while(count < numgrains)
  {
    double cur = 0;
    for(int m = 0; m < 18; m++)
    {
      for(int n = 0; n < 18; n++)
      {
        for(int p = 0; p < 18; p++)
        {
          double num = eulercount[m][n][p]/numeulers;
          if(num > cur && num < lastcur)
          {
            cur = num;
            double random = rg.Random();
            double random4 = random;
            random = rg.Random();
            double random5 = random;
            random = rg.Random();
            double random6 = random;
            synea1 = ((3.1415926535897/36)*m)+((3.1415926535897/36)*random4);
            synea2 = ((3.1415926535897/36)*n)+((3.1415926535897/36)*random5);
            synea3 = ((3.1415926535897/36)*p)+((3.1415926535897/36)*random6);
          }
        }
      }
    }
    lastcur = cur;
    synea1 = rg.Random()*2*3.141592;
    synea2 = rg.Random()*1*3.141592;
    synea3 = rg.Random()*2*3.141592;
    grains[count].euler1 = synea1;
    grains[count].euler2 = synea2;
    grains[count].euler3 = synea3;
    count++;
  }
}

void  MicroGen3D::loadSVNData(string inname8)
{
    ifstream inputFile;
    inputFile.open(inname8.c_str());
  double prob;
  int nnum;
    for (int k = 0; k < numsizebins; k++)
    {
    for (int l = 0; l < numneighbins; l++)
    {
      inputFile >> nnum >> prob;
      svn[k][l] = prob;
    }
    }
    inputFile.close();
}

void  MicroGen3D::loadSVSData(string inname9)
{
    ifstream inputFile;
    inputFile.open(inname9.c_str());
    int bin;
  double prob;

    for (int k = 0; k < numsizebins; k++)
    {
    inputFile >> bin;
    for (int l = 0; l < numsizebins; l++)
    {
      inputFile >> prob;
      svs[k][l] = prob;
    }
    }
    inputFile.close();
}

void  MicroGen3D::make_points(int numgrains)
{
  int ncount = 0;
  int count = 0;
  int good = 0;
  int column = 0;
  int row = 0;
  int plane = 0;
  int columncourse = 0;
  int rowcourse = 0;
  int planecourse = 0;
  int tempsurf = 0;
  int xmin = 0;
  int xmax = 0;
  int ymin = 0;
  int ymax = 0;
  int zmin = 0;
  int zmax = 0;
  long j = 0;
  double xc = 0;
  double yc = 0;
  double zc = 0;
  double x = 0;
  double y = 0;
  double z = 0;
  int init = 1;
  int in = 1;
  int out = 0;
  int taken90 = 0;
  long pointsleft = 0;
  double insidecount = 0;
  double badcount = 0;
  long triedcount = 0;
  int uniquecursize = 0;
  int totalcursize = 0;
  int uniquecurcoursesize = 0;
  vector<long> alreadytriedlist;
  srand(static_cast<unsigned int>(time(NULL)));
  int xpoints = int(sizex/resx);
  int ypoints = int(sizey/resy);
  int zpoints = int(sizez/resz);
  int xpointscourse = int(sizex/resx1);
  int ypointscourse = int(sizey/resy1);
  int zpointscourse = int(sizez/resz1);
  long nottakencount = (xpoints*ypoints*zpoints);
  long nottakencountcourse = (xpointscourse*ypointscourse*zpointscourse);
  for (long a = 0; a < nottakencountcourse; a++)
  {
    gridcourse[a].available = init;
    gridcourse[a].available90 = init;
  }
  for (long b = 0; b < nottakencount; b++)
  {
    gridfine[b].available = init;
    gridfine[b].available90 = init;
  }
  for (int i = 0; i < numgrains; i++)
  {
    good = 0;
    triedcount = 0;
    int size = int(alreadytriedlist.size());
    for(int k=0;k<size;k++)
    {
      int firstgridpoint = alreadytriedlist[k];
      gridcourse[firstgridpoint].available = init;
    }
    double volcur = grains[i].volume;
    double aovera = grains[i].axis1;
    double bovera = grains[i].axis2;
    double covera = grains[i].axis3;
    double coverb = covera/bovera;
    double Nvalue = grains[i].Nvalue;
    double beta1 = (gamma((1.0/Nvalue))*gamma((1/Nvalue)))/gamma((2/Nvalue));
    double beta2 = (gamma((2.0/Nvalue))*gamma((1/Nvalue)))/gamma((3/Nvalue));
    double radcur1cube = 1;
    if(shapeclass == 3)
    {
      if(Nvalue >= 0 && Nvalue <= 1)
      {
        radcur1cube = (volcur*6.0)/(6-(Nvalue*Nvalue*Nvalue));
      }
      if(Nvalue > 1 && Nvalue <= 2)
      {
        radcur1cube = (volcur*6.0)/(3+(9*Nvalue)-(9*Nvalue*Nvalue)+(2*Nvalue*Nvalue*Nvalue));
      }
    }
    if(shapeclass == 2)
    {
      radcur1cube = (volcur*(3.0/2.0)*(1.0/bovera)*(1.0/covera)*((Nvalue*Nvalue)/4.0)*(1.0/beta1)*(1.0/beta2));
    }
    if(shapeclass == 1)
    {
      radcur1cube = (volcur*(3.0/4.0)*(1.0/3.1415926535897)*(1.0/bovera)*(1.0/covera));
    }
    double rad = pow(radcur1cube,0.333333333333);
    if(shapeclass == 3) rad = rad/2.0;
    double radcur1 = rad;
    double radcur2 = (rad*bovera);
    double radcur3 = (rad*covera);
    double rad1x = grains[i].axis1x;
    double rad1y = grains[i].axis1y;
    double rad1z = grains[i].axis1z;
    double rad2x = grains[i].axis2x;
    double rad2y = grains[i].axis2y;
    double rad2z = grains[i].axis2z;
    double rad3x = grains[i].axis3x;
    double rad3y = grains[i].axis3y;
    double rad3z = grains[i].axis3z;
    while(good == 0)
    {
      double random = rg.Random();
      double random1 = random;
      pointsleft = nottakencountcourse-triedcount;
      int remainder = int(random1*pointsleft-1);
      int selector = -1;
      int tempcounter = 0;
      while(selector != remainder)
      {
        int picked = gridcourse[tempcounter].available;
        int noway2 = gridcourse[tempcounter].available90;
        if(picked == 1 && noway2 == 1)
        {
          selector++;
          j = tempcounter;
        }
        tempcounter++;
      }
      int alreadypicked = 0;
      int tried = gridcourse[j].available;
      gridcourse[j].available = alreadypicked;
      int noway = gridcourse[j].available90;
      if(noway == 1 && tried == 1)
      {
        for(int c=0;c<i;c++)
        {
          gremovals[c]=0;
        }
        alreadytriedlist.push_back(j);
        good = 1;
        columncourse = (j%xpointscourse);
        rowcourse = ((j/xpointscourse)%ypointscourse);
        planecourse = (j/(xpointscourse*ypointscourse));
        xc = (columncourse*resx1)+(resx1/2);
        yc = (rowcourse*resy1)+(resy1/2);
        zc = (planecourse*resz1)+(resz1/2);
        column = int((xc-(resx1/2))/resx);
        row = int((yc-(resy1/2))/resy);
        plane = int((zc-(resz1/2))/resz);
        xc = (column*resx)+(resx/2);
        yc = (row*resy)+(resy/2);
        zc = (plane*resz)+(resz/2);
        insidecount = 0;
        badcount = 0;
        xmin = 0;
        xmax = int((sizex/resx)-1);
        ymin = 0;
        ymax = int((sizey/resy)-1);
        zmin = 0;
        zmax = int((sizez/resz)-1);
        if(column-((radcur1/resx)+1) > 0)
        {
          xmin = int(column-((radcur1/resx)+1));
        }
        if(column+((radcur1/resx)+1) < (sizex/resx)-1)
        {
          xmax = int(column+((radcur1/resx)+1));
        }
        if(row-((radcur1/resy)+1) > 0)
        {
          ymin = int(row-((radcur1/resy)+1));
        }
        if(row+((radcur1/resy)+1) < (sizey/resy)-1)
        {
          ymax = int(row+((radcur1/resy)+1));
        }
        if(plane-((radcur1/resz)+1) > 0)
        {
          zmin = int(plane-((radcur1/resz)+1));
        }
        if(plane+((radcur1/resz)+1) < (sizez/resz)-1)
        {
          zmax = int(plane+((radcur1/resz)+1));
        }
        for(int iter1 = xmin; iter1 < xmax+1; iter1++)
        {
          for(int iter2 = ymin; iter2 < ymax+1; iter2++)
          {
            for(int iter3 = zmin; iter3 < zmax+1; iter3++)
            {
              double inside = -1;
              column = iter1;
              row = iter2;
              plane = iter3;
              x = (column*resx)+(resx/2);
              y = (row*resy)+(resy/2);
              z = (plane*resz)+(resz/2);
              double axis[3][3];
              double diff[3][1];
              double axiselim[3][3];
              double diffelim[3][1];
              double constmat[3][1];
              axis[0][0] = rad1x;
              axis[0][1] = rad2x;
              axis[0][2] = rad3x;
              axis[1][0] = rad1y;
              axis[1][1] = rad2y;
              axis[1][2] = rad3y;
              axis[2][0] = rad1z;
              axis[2][1] = rad2z;
              axis[2][2] = rad3z;
              diff[0][0] = x-xc;
              diff[1][0] = y-yc;
              diff[2][0] = z-zc;
              int elimcount = 0;
              int elimcount1 = 0;
              double q = 0;
              double sum = 0;
              double c = 0;
              for(int a = 0; a < 3; a++)
              {
                elimcount1 = 0;
                for(int b = 0; b < 3; b++)
                {
                  axiselim[elimcount][elimcount1] = axis[a][b];
                  if(axiselim[elimcount][elimcount1] == 0)
                  {
                    axiselim[elimcount][elimcount1] = 0.0001;
                  }
                  elimcount1++;
                }
                diffelim[elimcount][0] = diff[a][0];
                elimcount++;
              }
              for(int k = 0; k < elimcount-1; k++)
              {
                for(int l = k+1; l < elimcount; l++)
                {
                  c = axiselim[l][k]/axiselim[k][k];
                  for(int m = k+1; m < elimcount; m++)
                  {
                    axiselim[l][m] = axiselim[l][m] - c*axiselim[k][m];
                  }
                  diffelim[l][0] = diffelim[l][0] - c*diffelim[k][0];
                }
              }
              diffelim[elimcount-1][0] = diffelim[elimcount-1][0]/axiselim[elimcount-1][elimcount-1];
              for(int l = 1; l < elimcount; l++)
              {
                int m = (elimcount-1)-l;
                sum = 0;
                for(int n = m+1; n < elimcount; n++)
                {
                  sum = sum + (axiselim[m][n]*diffelim[n][0]);
                }
                diffelim[m][0] = (diffelim[m][0]-sum)/axiselim[m][m];
              }
              for(int p = 0; p < elimcount; p++)
              {
                q = diffelim[p][0];
                constmat[p][0] = q;
              }
              double axis1comp = constmat[0][0]/radcur1;
              double axis2comp = constmat[1][0]/radcur2;
              double axis3comp = constmat[2][0]/radcur3;
              if(shapeclass == 3)
              {
                if(fabs(axis1comp) <= 1 && fabs(axis2comp) <= 1 && fabs(axis3comp) <= 1)
                {
                  inside = 1;
                  axis1comp = axis1comp+1;
                  axis2comp = axis2comp+1;
                  axis3comp = axis3comp+1;
                  if(((-axis1comp)+(-axis2comp)+(axis3comp)-((-0.5*Nvalue)+(-0.5*Nvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(axis3comp)-((2-(0.5*Nvalue))+(-0.5*Nvalue)+2)) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(axis3comp)-((2-(0.5*Nvalue))+(2-(0.5*Nvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(axis3comp)-((-0.5*Nvalue)+(2-(0.5*Nvalue))+2)) > 0) inside = -1;
                  if(((-axis1comp)+(-axis2comp)+(-axis3comp)-((-0.5*Nvalue)+(-0.5*Nvalue))) > 0) inside = -1;
                  if(((axis1comp)+(-axis2comp)+(-axis3comp)-((2-(0.5*Nvalue))+(-0.5*Nvalue))) > 0) inside = -1;
                  if(((axis1comp)+(axis2comp)+(-axis3comp)-((2-(0.5*Nvalue))+(2-(0.5*Nvalue)))) > 0) inside = -1;
                  if(((-axis1comp)+(axis2comp)+(-axis3comp)-((-0.5*Nvalue)+(2-(0.5*Nvalue)))) > 0) inside = -1;
                }
              }
              if(shapeclass == 2)
              {
                axis1comp = pow(axis1comp,Nvalue);
                axis2comp = pow(axis2comp,Nvalue);
                axis3comp = pow(axis3comp,Nvalue);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(shapeclass == 1)
              {
                axis1comp = pow(axis1comp,2);
                axis2comp = pow(axis2comp,2);
                axis3comp = pow(axis3comp,2);
                inside = 1-axis1comp-axis2comp-axis3comp;
              }
              if(inside >= 0)
              {
                int currentpoint = (xpoints*ypoints*plane)+(xpoints*row)+column;
                int resdiff = int(double(resx1)/double(resx)+0.00000000001);
                int columnleft = column%resdiff;
                int rowleft = row%resdiff;
                int planeleft = plane%resdiff;
                if(columnleft == (resdiff/2) && rowleft == (resdiff/2) && planeleft == (resdiff/2))
                {
                  int columnc = column/resdiff;
                  int rowc = row/resdiff;
                  int planec = plane/resdiff;
                  int currentpointcourse = (xpointscourse*ypointscourse*planec)+(xpointscourse*rowc)+columnc;
                  gridcourse[currentpointcourse].inside = in;
                }
                gridfine[currentpoint].inside = in;
                insidecount++;
                int used = gridfine[currentpoint].available90;
                if(used == 0 && overlapassignment == 1) badcount++;
                if(used == 0 && overlapassignment == 2)
                {
                  int usedgrain = gridfine[currentpoint].grainname;
                  if((i%2) == 0)
                  {
                    gremovals[usedgrain]++;
                  }
                  if((i%2) == 1)
                  {
                    badcount++;
                  }
                }
              }
            }
          }
        }
        double uberincrease = 0;
        double nsdistchange = 0;
        for (int n = 0; n < i; n++)
        {
          double xold = packedgrain[n].x;
          double yold = packedgrain[n].y;
          double zold = packedgrain[n].z;
          double radold1 = packedgrain[n].radius1;
          double radold2 = packedgrain[n].radius2;
          double radold3 = packedgrain[n].radius3;
          double xdist = fabs(xc-xold);
          double ydist = fabs(yc-yold);
          double zdist = fabs(zc-zold);
          double totdist = (xdist*xdist)+(ydist*ydist)+(zdist*zdist);
          totdist = pow(totdist,0.5);
          if((totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))) && (totdist < (0.7*(radcur1+radold1))))
          {
            int nnum = packedgrain[n].tempneighnum;
            double lognnum = log10(double(nnum));
            if(nnum == 0) lognnum = 0;
            double nsizevol = 1.3333333333*(3.1415926535897)*radold1*radold2*radold3;
            double nsize3 = ((nsizevol*(0.75))/(3.1415926535897));
            double nsize = 2*(pow(nsize3,0.33333333333));
            double sizevol = 1.3333333333*(3.1415926535897)*radcur1*radcur2*radcur3;
            double size3 = ((sizevol*(0.75))/(3.1415926535897));
            double size = 2*(pow(size3,0.33333333333));
            int nbin = int(nsize);
            int bin = int(size);
            if(nbin >= numsizebins) nbin = numsizebins-1;
            if(bin >= numsizebins) bin = numsizebins-1;
            int was = nnum;
            nnum = nnum + 1;
            int now = nnum;
            double wasprob = svn[nbin][was];
            double nowprob = svn[nbin][now];
            double increase = (nowprob - wasprob);
            if(increase > 0) increase = 1;
            if(increase < 0) increase = -1;
            uberincrease = uberincrease + increase;
            nsdist[n][bin]++;
            double total = 0;
            for(int q = 0; q < numsizebins; q++)
            {
              total = total + nsdist[n][q];
            }
            double error = 0;
            for(int r = 0; r < numsizebins; r++)
            {
              error = error + (svs[nbin][r] - (nsdist[n][r]/total))*(svs[nbin][r] - (nsdist[n][r]/total));
            }
            double nserror = packedgrain[n].nserror;
            nsdistchange = nsdistchange + (error-nserror);
            nsdist[n][bin] = nsdist[n][bin] - 1;
          }
        }
        double randomsize = rg.Random();
        double randomsize1 = randomsize;
        double acceptable = badcount/insidecount;
        if(insidecount == 0)
        {
          int stop = 0;
        }
        int toomuch = 0;
        if(overlapassignment == 2)
        {
          for(int d=0;d<i;d++)
          {
            double ratio = double(packedgrain[d].currentsize-gremovals[d])/double(packedgrain[d].initsize);
            if(ratio < (1-(overlapallowed/2.0))) toomuch = 1;
          }
        }
        if(acceptable > overlapallowed || uberincrease < 0 || nsdistchange > 0 || toomuch == 1) good = 0;
        triedcount++;
      }
      if(good == 0)
      {
        for(int temp1 = xmin; temp1 < xmax+1; temp1++)
        {
          for(int temp2 = ymin; temp2 < ymax+1; temp2++)
          {
            for(int temp3 = zmin; temp3 < zmax+1; temp3++)
            {
              int point = (temp1+(temp2*xpoints)+(temp3*xpoints*ypoints));
              int inside = gridfine[point].inside;
              if(inside == 1)
              {
                gridfine[point].inside = out;
              }
            }
          }
        }
        for(int temp4 = xmin; temp4 < xmax+1; temp4++)
        {
          for(int temp5 = ymin; temp5 < ymax+1; temp5++)
          {
            for(int temp6 = zmin; temp6 < zmax+1; temp6++)
            {
              int column = temp4;
              int row = temp5;
              int plane = temp6;
              int resdiff = int(double(resx1)/double(resx)+0.00000000001);
              column = column/resdiff;
              row = row/resdiff;
              plane = plane/resdiff;
              int point = (column+(row*xpointscourse)+(plane*xpointscourse*ypointscourse));
              int inside = gridcourse[point].inside;
              if(inside == 1)
              {
                gridcourse[point].inside = out;
              }
            }
          }
        }
      }
    }
    ncount = 0;
    for (int p = 0; p < i; p++)
    {
      double xold = packedgrain[p].x;
      double yold = packedgrain[p].y;
      double zold = packedgrain[p].z;
      double radold1 = packedgrain[p].radius1;
      double radold2 = packedgrain[p].radius2;
      double radold3 = packedgrain[p].radius3;
      double xdist = fabs(xc-xold);
      double ydist = fabs(yc-yold);
      double zdist = fabs(zc-zold);
      if((xdist < (0.7*(radcur1+radold1))) && (ydist < (0.7*(radcur1+radold1))) && (zdist < (0.7*(radcur1+radold1))))
      {
        ncount++;
        int nnum = packedgrain[p].tempneighnum;
        nnum = nnum + 1;
        packedgrain[p].tempneighnum = nnum;
        double volume = 1.3333333333*(3.1415926535897)*radcur1*radcur2*radcur3;
        double volbin = volume*0.75/3.1415926535897;
        int vbin = int(2*pow(volbin,0.3333333333));
        if(vbin >= numsizebins) vbin = numsizebins-1;
        nsdist[p][vbin]++;
        double nvolume = 1.3333333333*(3.1415926535897)*radold1*radold2*radold3;
        double nvolbin = nvolume*0.75/3.1415926535897;
        int nvbin = int(2*pow(nvolbin,0.3333333333));
        if(nvbin >= numsizebins) nvbin = numsizebins-1;
        double total = 0;
        for(int q = 0; q < numsizebins; q++)
        {
          total = total + nsdist[p][q];
        }
        double error = 0;
        for(int r = 0; r < numsizebins; r++)
        {
          error = error + (svs[nvbin][r] - (nsdist[p][r]/total))*(svs[nvbin][r] - (nsdist[p][r]/total));
        }
        packedgrain[p].nserror = error;
      }
    }
    tempsurf = 0;
    for(int temp7 = xmin; temp7 < xmax+1; temp7++)
    {
      for(int temp8 = ymin; temp8 < ymax+1; temp8++)
      {
        for(int temp9 = zmin; temp9 < zmax+1; temp9++)
        {
          int point = (temp7+(temp8*xpoints)+(temp9*xpoints*ypoints));
          int inside = gridfine[point].inside;
          if(inside == 1)
          {
            int columncheck = point%xpoints;
            int rowcheck = (point/xpoints)%ypoints;
            int planecheck = point/(xpoints*ypoints);
            if(columncheck == 0 || rowcheck == 0 || planecheck == 0 || columncheck == (xpoints-1) || rowcheck == (ypoints-1) || planecheck == (zpoints-1)) tempsurf = 1;
            int current = gridfine[point].available90;
            if(current == 1)
            {
              uniquecursize++;
              totalcursize++;
              gridfine[point].grainname = i;
              voxelsvector[i].push_back(point);
            }
            if(current == 0 && overlapassignment == 2)
            {
              int checkgrain = gridfine[point].grainname;
              if((gremovals[checkgrain]/packedgrain[checkgrain].initsize) < (overlapallowed/5.0))
              {
                gridfine[point].grainname = i;
                voxelsvector[i].push_back(point);
                totalcursize++;
              }
            }
            gridfine[point].inside = out;
            gridfine[point].available90 = taken90;
          }
        }
      }
    }
    for(int temp10 = xmin; temp10 < xmax+1; temp10++)
    {
      for(int temp11 = ymin; temp11 < ymax+1; temp11++)
      {
        for(int temp12 = zmin; temp12 < zmax+1; temp12++)
        {
          int column = temp10;
          int row = temp11;
          int plane = temp12;
          int resdiff = int(double(resx1)/double(resx)+0.00000000001);
          column = column/resdiff;
          row = row/resdiff;
          plane = plane/resdiff;
          int point = (column+(row*xpointscourse)+(plane*xpointscourse*ypointscourse));
          int inside = gridcourse[point].inside;
          if(inside == 1)
          {
            int currentcourse = gridcourse[point].available90;
            if(currentcourse == 1)
            {
              uniquecurcoursesize++;
            }
            gridcourse[point].available90 = taken90;
          }
        }
      }
    }
    if(overlapassignment == 2)
    {
      for(int e=0;e<i;e++)
      {
        if((gremovals[e]/packedgrain[e].initsize) < (overlapallowed/5.0))
        {
          int newsize = packedgrain[e].currentsize-gremovals[e];
          packedgrain[e].currentsize = newsize;
        }
      }
    }
    nottakencountcourse = nottakencountcourse - uniquecurcoursesize;
    nottakencount = nottakencount - uniquecursize;
    uniquecursize = 0;
    uniquecurcoursesize = 0;
    int pointcolumn = j%xpointscourse;
    int pointrow = (j/xpointscourse)%ypointscourse;
    int pointplane = j/(xpointscourse*ypointscourse);
    double pointx = (pointcolumn*resx1)+(resx1/2);
    double pointy = (pointrow*resy1)+(resy1/2);
    double pointz = (pointplane*resz1)+(resz1/2);
    double ea1 = grains[i].euler1;
    double ea2 = grains[i].euler2;
    double ea3 = grains[i].euler3;
    packedgrain[i].initsize = totalcursize;
    packedgrain[i].currentsize = totalcursize;
    packedgrain[i].grainname = i;
    packedgrain[i].xc = pointx;
    packedgrain[i].yc = pointy;
    packedgrain[i].zc = pointz;
    packedgrain[i].axis1 = radcur1;
    packedgrain[i].axis2 = radcur2;
    packedgrain[i].axis3 = radcur3;
    packedgrain[i].temponsurf = tempsurf;
    packedgrain[i].euler1 = ea1;
    packedgrain[i].euler2 = ea2;
    packedgrain[i].euler3 = ea3;
    packedgrain[i].tempneighnum = ncount;
    totalcursize = 0;
    cout << i << "-" << pointx << "-" << pointy << "-" << pointz << "-" << radcur1 << "-" << radcur2 << "-" << radcur3 << endl;
    count++;
  }
  for(int u=0;u<numgrains;u++)
  {
    gsizes[u] = 0;
  }
  for(int t=0;t<(xpoints*ypoints*zpoints);t++)
  {
    int available = gridfine[t].available90;
    if(available == 0)
    {
      int gname = gridfine[t].grainname;
      gsizes[gname]++;
    }
  }
  for(int v=0;v<numgrains;v++)
  {
    int cursize = gsizes[v];
    packedgrain[v].currentsize = cursize;
  }
}

void  MicroGen3D::fill_gaps(int numgrains)
{
  int count = 1;
  int dup = 0;
  int fixed = 0;
  vector<int> neighs;
  vector<int> remove;
  int column = 0;
  int row = 0;
  int plane = 0;
  int *n;
  n = new int[numgrains];
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int taken = gridfine[i].available90;
      if(taken == 1)
      {
        for(int c = 0; c < numgrains; c++)
        {
          n[c] = 0;
        }
        column = i%xpoints;
        row = (i/xpoints)%ypoints;
        plane = i/(xpoints*ypoints);
        count++;
        if(column > 0)
        {
          int grain1 = gridfine[i-1].grainname;
          int taken1 = gridfine[i-1].available90;
          if(taken1 == 0)
          {
            neighs.push_back(grain1);
          }
        }
        if(column < (sizex/resx)-1)
        {
          int grain2 = gridfine[i+1].grainname;
          int taken2 = gridfine[i+1].available90;
          if(taken2 == 0)
          {
            neighs.push_back(grain2);
          }
        }
        if(row > 0)
        {
          int grain3 = gridfine[i-(xpoints)].grainname;
          int taken3 = gridfine[i-(xpoints)].available90;
          if(taken3 == 0)
          {
            neighs.push_back(grain3);
          }
        }
        if(row < (sizey/resy)-1)
        {
          int grain4 = gridfine[i+(xpoints)].grainname;
          int taken4 = gridfine[i+(xpoints)].available90;
          if(taken4 == 0)
          {
            neighs.push_back(grain4);
          }
        }
        if(plane > 0)
        {
          int grain5 = gridfine[i-(xpoints*ypoints)].grainname;
          int taken5 = gridfine[i-(xpoints*ypoints)].available90;
          if(taken5 == 0)
          {
            neighs.push_back(grain5);
          }
        }
        if(plane < (sizez/resz)-1)
        {
          int grain6 = gridfine[i+(xpoints*ypoints)].grainname;
          int taken6 = gridfine[i+(xpoints*ypoints)].available90;
          if(taken6 == 0)
          {
            neighs.push_back(grain6);
          }
        }
        int now = 0;
        int then = 0;
        double thendiff = 0;
        double nowdiff = 0;
        double nowsizegoal = 0;
        double nowsize = 0;
        int nowtemponsurf = 0;
        int curgrain = 0;
        int size = int(neighs.size());
        for(int k=0;k<size;k++)
        {
          int first = neighs[k];
          n[first]++;
          now = n[first];
          nowsize = packedgrain[first].currentsize;
          nowsizegoal = grains[first].volume;
          nowtemponsurf = packedgrain[first].temponsurf;
          nowdiff = nowsizegoal-nowsize;
          if(now > then)
//          if(now > then || (now == then && nowdiff > thendiff))
          {
            then = now;
            thendiff = nowdiff;
            curgrain = first;
          }
        }
        if(size > 0)
        {
          int bordered = 1;
          gridfine[i].hasneighbor = bordered;
          gridfine[i].neighbor = curgrain;
          neighs.clear();
        }
      }
    }
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int taken = gridfine[j].available90;
      int hasneigh = gridfine[j].hasneighbor;
      if(taken == 1 && hasneigh == 1)
      {
        int neighbor = gridfine[j].neighbor;
        gridfine[j].grainname = neighbor;
        gridfine[j].available90 = fixed;
      }
    }
  }
}


void  MicroGen3D::loadMisoData(string inname10)
{
    ifstream inputFile;
    inputFile.open(inname10.c_str());
  int count = 0;
  double height = 0;
    for (int k = 0; k < nummisobins; k++)
    {
    inputFile >> height;
    actualmisobin[count].height = height;
        count++;
    }
    inputFile.close();
}

void  MicroGen3D::loadMicroData(string inname11)
{
    ifstream inputFile;
    inputFile.open(inname11.c_str());
  int count = 0;
  double height = 0;
    for (int k = 0; k < nummicrobins; k++)
    {
    inputFile >> height;
    actualmicrobin[count].height = height;
        count++;
    }
    inputFile.close();
}

void  MicroGen3D::rank_misobins(int numgrains)
{
  int count = 0;
  int check = 0;
  for(int h = 0; h < nummisobins; h++)
  {
    int tempheight = 0;
    simmisobin[h].height = tempheight;
  }
  for(int i = 0; i < numgrains; i++)
  {
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(misolist.size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist[k];
      int misocur = int(misofirst/(100/nummisobins));
      double height = simmisobin[misocur].height;
      height = height + 1;
      simmisobin[misocur].height = height;
      count++;
    }
  }
  for(int j = 0; j < nummisobins; j ++)
  {
    double actualheight = actualmisobin[j].height;
    actualheight = actualheight*count;
    double simheight = simmisobin[j].height;
    double diff = fabs(simheight-actualheight);
    simmisobin[j].difference = diff;
    double temp = 0;
    simmisobin[j].binrank = temp;
  }
  int difflast = count;
  int curbin = 0;
  while(check < nummisobins)
  {
    int diffcur = 0;
    for(int k = 0; k < nummisobins; k++)
    {
      double diff = simmisobin[k].difference;
      double ranked = simmisobin[k].binrank;
      if(diff > diffcur && ranked == 0)
      {
        diffcur = int(diff);
        curbin = k;
      }
    }
    double rank = nummisobins-check;
    double actual = actualmisobin[curbin].height;
    actual = actual*count;
    double sim = simmisobin[curbin].height;
    if((sim-actual) < 0)
    {
      rank = -rank;
    }
    simmisobin[curbin].binrank = rank;
    check++;
  }
}

void  MicroGen3D::count_misorientations(int numgrains)
{
  int count = 0;
  double lowangle = 0;
  for(int i = 0; i < numgrains; i++)
  {
    lowangle = 0;
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(misolist.size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist[k];
      if(misofirst < 15) lowangle++;
    }
    double fraction = lowangle/size;
    packedgrain[i].lowanglefraction = fraction;
    count++;
  }
}

int  MicroGen3D::rank_microbins(int numgrains)
{
  int count = 0;
  int check = 0;
  for(int h = 0; h < nummicrobins; h++)
  {
    int tempheight = 0;
    simmicrobin[h].height = tempheight;
  }
  for(int i = 0; i < numgrains; i++)
  {
    double microtexture = packedgrain[i].lowanglefraction;
    int microcur = int(microtexture*(nummicrobins));
    double height = simmicrobin[microcur].height;
    height = height + 1;
    simmicrobin[microcur].height = height;
  }
  for(int j = 0; j < nummicrobins; j ++)
  {
    double actualheight = actualmicrobin[j].height;
    actualheight = actualheight*numgrains;
    double simheight = simmicrobin[j].height;
    double diff = fabs(simheight-actualheight);
    simmicrobin[j].difference = diff;
    double temp = 0;
    simmicrobin[j].binrank = temp;
  }
  int curbin = 0;
  while(check < nummicrobins)
  {
    int diffcur = 0;
    for(int k = 0; k < nummicrobins; k++)
    {
      double diff = simmicrobin[k].difference;
      double ranked = simmicrobin[k].binrank;
      if(diff > diffcur && ranked == 0)
      {
        diffcur = int(diff);
        curbin = k;
      }
    }
    double rank = nummicrobins-check;
    double actual = actualmicrobin[curbin].height;
    actual = actual*numgrains;
    double sim = simmicrobin[curbin].height;
    if((sim-actual) < 0)
    {
      rank = -rank;
    }
    simmicrobin[curbin].binrank = rank;
    check++;
  }
  double heightcheck = simmicrobin[nummicrobins-1].height;
  double heightcheck2temp = actualmicrobin[nummicrobins-1].height;
  double heightcheck2 = heightcheck2temp*numgrains;
  double percenterror = (heightcheck2-heightcheck)/heightcheck2;
  if(percenterror < 0.15)
  {
    count = 1;
  }
return count;
}

void  MicroGen3D::rank_grains1(int numgrains)
{
  int count = 0;
  for(int i = 0; i < numgrains; i++)
  {
    double rank = 0;
    double temprank = 0;
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(misolist.size());
    for(int k=0;k<size;k++)
    {
      double misofirst = misolist[k];
      int misocur = int(misofirst/(100/nummisobins));
      temprank = simmisobin[misocur].binrank;
      rank = rank + temprank;
      if(misofirst == 0)
      {
        rank = rank + 100;
      }
    }
    double frozen = packedgrain[i].frozen;
    packedgrain[i].grainrank = rank;
    count++;
  }
}

void  MicroGen3D::identify_grains1(int numgrains,int nummisomoves)
{
  int check = 0;
  int worst = 0;
  int clear = 0;
  double bad = 1;
  int count = 0;
  for(int h = 0; h < numgrains; h++)
  {
    packedgrain[h].picked = clear;
  }
  while(check < nummisomoves)
  {
    double rankcur = -10000000;
    count = 0;
    for(int i = 0; i < numgrains; i++)
    {
      double rank = packedgrain[i].grainrank;
      double picked = packedgrain[i].picked;
      double froze = packedgrain[i].frozen;
      if(rank > rankcur && picked == 0 && froze == 0)
      {
        rankcur = rank;
        worst = i;
        count++;
      }
    }
    packedgrain[worst].picked = bad;
    check++;
  }
}

void  MicroGen3D::move_grains1(int numgrains)
{
  int check = 0;
  vector<int> pickedlist;
  vector<int> temppickedlist;
  for(int i = 0; i < numgrains; i++)
  {
    double ea1temp = packedgrain[i].euler1;
    double ea2temp = packedgrain[i].euler2;
    double ea3temp = packedgrain[i].euler3;
    packedgrain[i].oeuler1 = ea1temp;
    packedgrain[i].oeuler2 = ea2temp;
    packedgrain[i].oeuler3 = ea3temp;
    double picked = packedgrain[i].picked;
    if(picked == 1)
    {
      pickedlist.push_back(i);
      check++;
    }
  }
  for(int j = 0; j < numgrains; j++)
  {
    double picked = packedgrain[j].picked;
    if(picked == 1)
    {
      int size = int(pickedlist.size());
      double random = rg.Random();
      double random1 = random;
      int remainder = int(random1*size);
      if(size <= remainder)
      {
        remainder = remainder - 1;
      }
      int swap = pickedlist[remainder];
      int sizecheck = int(pickedlist.size());
      if(swap == j && sizecheck > 1)
      {
        swap = pickedlist[(remainder+1)];
        pickedlist.erase(pickedlist.begin()+remainder+1);
      }
      if(swap != j) pickedlist.erase(pickedlist.begin()+remainder);
      int grainname = packedgrain[j].grainname;
      double ea1 = packedgrain[swap].oeuler1;
      double ea2 = packedgrain[swap].oeuler2;
      double ea3 = packedgrain[swap].oeuler3;
      int nnum = packedgrain[j].neighnum;
      packedgrain[j].euler1 = ea1;
      packedgrain[j].euler2 = ea2;
      packedgrain[j].euler3 = ea3;
    }
  }
}

void  MicroGen3D::rank_grains2(int numgrains)
{
  int count = 0;
  for(int i = 0; i < numgrains; i++)
  {
    double microrank = 0;
    double temprank = 0;
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(neighborvector[i].size());
    for(int j = 0; j < size; j++)
    {
      int neighfirst = neighborvector[i][j];
      double neighfrac = packedgrain[neighfirst].lowanglefraction;
      if(neighfrac > 0.24) neighfrac = neighfrac*4;
      if(neighfrac > 0.49) neighfrac = neighfrac;
      if(neighfrac > 0.74) neighfrac = neighfrac*2;
      temprank = neighfrac;
      double misofirst = misolist[j];
      if(misofirst < 15)
      {
        temprank = 0;
        if(neighfrac > 4)
        {
          microrank = 0;
          {break;}
        }
      }
      double ownfrac = packedgrain[i].lowanglefraction;
      if(ownfrac < 0.4) temprank = temprank*2;
      microrank = microrank + temprank;
    }
    packedgrain[i].grainrank = microrank;
    count++;
  }
}

void  MicroGen3D::identify_grains2(int numgrains,int nummicromoves)
{
  int check = 0;
  int check2 = 0;
  int worst = 0;
  int clear = 0;
  double bad = 1;
  double none = 2;
  nummicromoves = 150;
  for(int h = 0; h < numgrains; h++)
  {
    packedgrain[h].picked = clear;
  }
  while(check < nummicromoves)
  {
    double rankcur = 0;
    for(int i = 0; i < numgrains; i++)
    {
      double rank = packedgrain[i].grainmicrorank;
      double picked = packedgrain[i].picked;
      if(rank > rankcur && picked == 0)
      {
        rankcur = rank;
        worst = i;
      }
    }
    packedgrain[worst].picked = bad;
    check++;
  }
  while(check2 < nummicromoves)
  {
    double rankcur2 = 0;
    for(int j = 0; j < numgrains; j++)
    {
      double rank2 = packedgrain[j].grainmicrorank;
      double picked2 = packedgrain[j].picked;
      if(rank2 > rankcur2 && picked2 == 0)
      {
        rankcur2 = rank2;
        worst = j;
      }
    }
    packedgrain[worst].picked = none;
    check2++;
  }
}

void  MicroGen3D::move_grains2(int numgrains)
{
  int check = 0;
  vector<int> pickedlist;
  vector<int> nonelist;
  vector<int> tempnonelist;
  for(int i = 0; i < numgrains; i++)
  {
    double ea1temp = packedgrain[i].euler1;
    double ea2temp = packedgrain[i].euler2;
    double ea3temp = packedgrain[i].euler3;
    packedgrain[i].oeuler1 = ea1temp;
    packedgrain[i].oeuler2 = ea2temp;
    packedgrain[i].oeuler3 = ea3temp;
    double picked = packedgrain[i].picked;
    if(picked == 2)
    {
      nonelist.push_back(i);
      check++;
    }
  }
  for(int j = 0; j < numgrains; j++)
  {
    double picked = packedgrain[j].picked;
    if(picked == 1)
    {
      int size = int(nonelist.size());
      double random = rg.Random();
      double random1 = random;
      int remainder = int(random1*size);
      if(size <= remainder)
      {
        remainder = remainder - 1;
      }
      int swap = nonelist[remainder];
      nonelist.erase(nonelist.begin()+remainder);
      int grainname = packedgrain[j].grainname;
      double ea1 = packedgrain[swap].oeuler1;
      double ea2 = packedgrain[swap].oeuler2;
      double ea3 = packedgrain[swap].oeuler3;
      int nnum = packedgrain[j].neighnum;
      packedgrain[j].euler1 = ea1;
      packedgrain[j].euler2 = ea2;
      packedgrain[j].euler3 = ea3;
      int sgrainname = packedgrain[swap].grainname;
      double sea1 = packedgrain[j].oeuler1;
      double sea2 = packedgrain[j].oeuler2;
      double sea3 = packedgrain[j].oeuler3;
      int snnum = packedgrain[swap].neighnum;
      packedgrain[swap].euler1 = sea1;
      packedgrain[swap].euler2 = sea2;
      packedgrain[swap].euler3 = sea3;
    }
  }
}

void  MicroGen3D::freeze_grains(int numgrains)
{
  int count = 0;
  double froze = 0;
  for(int i = 0; i < numgrains; i++)
  {
    froze = 0;
    double fraction = packedgrain[i].lowanglefraction;
    if(fraction > 0.85)
    {
      froze = 1;
    }
    vector<double> misolist = packedgrain[i].misorientationlist;
    int size = int(neighborvector[i].size());
    for(int j = 0; j < size; j++)
    {
      int neighfirst = neighborvector[i][j];
      double neighmiso = misolist[j];
      double neighfrac = packedgrain[neighfirst].lowanglefraction;
      if(neighfrac > 0.85 && neighmiso < 15)
      {
        froze = 1;
      }
    }
    packedgrain[i].frozen = froze;
  }
}


void  MicroGen3D::writeCube(string outname1, int numgrains)
{
    ofstream outFile;
    outFile.open(outname1.c_str());
  int xpoints = int(sizex/resx);
  int ypoints = int(sizey/resy);
  int zpoints = int(sizez/resz);
  outFile << "# vtk DataFile Version 2.0" << endl;
  outFile << "data set from FFT2dx_GB" << endl;
  outFile << "ASCII" << endl;
  outFile << "DATASET STRUCTURED_POINTS" << endl;
  outFile << "DIMENSIONS " << xpoints << " " << ypoints << " " << zpoints << endl;
  outFile << "ORIGIN 0.0 0.0 0.0" << endl;
  outFile << "SPACING " << resx << " " << resy << " " << resz << endl;
  outFile << "POINT_DATA " << xpoints*ypoints*zpoints << endl;
  outFile << endl;
  outFile << endl;
  outFile << "SCALARS GrainID int  1" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    if(i%20 == 0 && i > 0) outFile << endl;
    int name = gridfine[i].grainname;
    int column = (i%xpoints);
    int row = ((i/xpoints)%ypoints);
    int plane = (i/(xpoints*ypoints));
    double xc = (column*resx)+(resx/2);
    double yc = (row*resy)+(resy/2);
    double zc = (plane*resz)+(resz/2);
    int gnum = gridfine[i].grainname;
    double r = packedgrain[gnum].red;
    double g = packedgrain[gnum].green;
    double b = packedgrain[gnum].blue;
    outFile << "   ";
    if(name < 10000) outFile << " ";
    if(name < 1000) outFile << " ";
    if(name < 100) outFile << " ";
    if(name < 10) outFile << " ";
    outFile << name;
  }
  outFile.close();
}


void  MicroGen3D::write_grains(string outname4, int numgrains)
{
    ofstream outFile;
    outFile.open(outname4.c_str());
  outFile << numgrains << endl;
  for(int i=0;i<numgrains;i++)
  {
    int name = packedgrain[i].grainname;
    double xc = packedgrain[i].xc;
    double yc = packedgrain[i].yc;
    double zc = packedgrain[i].zc;
    double ea1 = packedgrain[i].euler1;
    double ea2 = packedgrain[i].euler2;
    double ea3 = packedgrain[i].euler3;
    int nnum = packedgrain[i].neighnum;
    outFile << name << "  " << xc << "  " << yc << "  " << zc << "  " << ea1 << " " << ea2 << " " << ea3 << " " << nnum;
    int size = int(neighborvector[i].size());
    for(int j = 0; j < size; j++)
    {
      int neigh = neighborvector[i][j];
      outFile << "  " << neigh;
    }
    outFile << endl;
  }
}
void  MicroGen3D::write_volume(string writename5)
{
    ofstream outFile;
    outFile.open(writename5.c_str());
  int xpoints = int(sizex/resx);
  int ypoints = int(sizey/resy);
  int zpoints = int(sizez/resz);
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = gridfine[i].grainname;
    double ea1 = packedgrain[grainname].euler1;
    double ea2 = packedgrain[grainname].euler2;
    double ea3 = packedgrain[grainname].euler3;
    int column = (i%xpoints);
    int row = ((i/xpoints)%ypoints);
    int plane = (i/(xpoints*ypoints));
    double xc = (column*resx)+(resx/2);
    double yc = (row*resy)+(resy/2);
    double zc = (plane*resz)+(resz/2);
    double ci = 1;
    outFile << grainname << " " << ea1 << " " << ea2 << " " << ea3 << " " << xc << "  " << yc << "  " << zc << "  " << ci << endl;
  }
  outFile.close();
}

double MicroGen3D::gamma(double x)
{
    int i,k,m;
    double ga,gr,r,z;


    static double g[] = {
        1.0,
        0.5772156649015329,
       -0.6558780715202538,
       -0.420026350340952e-1,
        0.1665386113822915,
       -0.421977345555443e-1,
       -0.9621971527877e-2,
        0.7218943246663e-2,
       -0.11651675918591e-2,
       -0.2152416741149e-3,
        0.1280502823882e-3,
       -0.201348547807e-4,
       -0.12504934821e-5,
        0.1133027232e-5,
       -0.2056338417e-6,
        0.6116095e-8,
        0.50020075e-8,
       -0.11812746e-8,
        0.1043427e-9,
        0.77823e-11,
       -0.36968e-11,
        0.51e-12,
       -0.206e-13,
       -0.54e-14,
        0.14e-14};

    if (x > 171.0) return 1e308;    // This value is an overflow flag.
    if (x == (int)x) {
        if (x > 0.0) {
            ga = 1.0;               // use factorial
            for (i=2;i<x;i++) {
               ga *= i;
            }
         }
         else
            ga = 1e308;
     }
     else {
        if (fabs(x) > 1.0) {
            z = fabs(x);
            m = (int)z;
            r = 1.0;
            for (k=1;k<=m;k++) {
                r *= (z-k);
            }
            z -= m;
        }
        else
            z = x;
        gr = g[24];
        for (k=23;k>=0;k--) {
            gr = gr*z+g[k];
        }
        ga = 1.0/(gr*z);
        if (fabs(x) > 1.0) {
            ga *= r;
            if (x < 0.0) {
                ga = -M_PI/(x*ga*sin(M_PI*x));
            }
        }
    }
    return ga;
}

