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

#include "ReconstructionFunc.h"

#if 0
// -i C:\Users\GroebeMA\Desktop\NewFolder --outputDir C:\Users\GroebeMA\Desktop\NewFolder -f Slice_ --angMaxSlice 400 -s 1 -e 30 -z 0.25 -t -g 10 -c 0.1 -o 5.0 -x 2
#endif


const static double m_pi = 3.1415926535897;
const static double m_OnePointThree = 1.33333333333;

#include <sstream>

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------

using namespace std;

ReconstructionFunc::ReconstructionFunc() :
  voxels(NULL),
  grains(NULL),
  eulerbin(NULL)
{

}

ReconstructionFunc::~ReconstructionFunc()
{

  delete [] voxels;
  delete [] grains;
  delete [] eulerbin;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionFunc::initialize(double stepX, double stepY, double stepZ,
                  int32 angNumCols, int32 angNumRows, int32 angSlices,
                  bool v_mergetwinsoption,
                  int32 v_minallowedgrainsize,
                  double v_minseedconfidence,
                  double v_misorientationtolerance,
                  int32 v_crystruct,
                  bool v_alreadyformed)
{

  resx = stepX;
  resy = stepY;
  resz = stepZ;

  mergetwinsoption = (v_mergetwinsoption == true) ? 1 : 0;
  minallowedgrainsize = v_minallowedgrainsize;
  minseedconfidence = v_minseedconfidence;
  misorientationtolerance = v_misorientationtolerance;
  crystruct = v_crystruct;
  alreadyformed = (v_alreadyformed == true) ? 1 : 0;


  xpoints = angNumCols;
  ypoints = angNumRows;
  zpoints = angSlices;

  sizex = (xpoints - 1 ) * resx;
  sizey = (ypoints - 1 ) * resy;
  sizez = (zpoints - 1 ) * resz;

  totalpoints = xpoints * ypoints * zpoints;


#if 0
  std::cout << "X Size: " << sizex << std::endl;
  std::cout << "Y Size: " << sizey << std::endl;
  std::cout << "Z Size: " << sizez << std::endl;
  std::cout << "X Res: " << resx << std::endl;
  std::cout << "Y Res: " << resy << std::endl;
  std::cout << "Z Res: " << resz << std::endl;
  std::cout << "X Points: " << xpoints << std::endl;
  std::cout << "Y Points: " << ypoints << std::endl;
  std::cout << "Z Points: " << zpoints << std::endl;
  std::cout << "Total Points: " << totalpoints << std::endl;
#endif



  voxels = new Voxel[totalpoints];
  grains = new Grain[totalpoints/20];
} 
void ReconstructionFunc::loadSlices()
{
  m_angFileHelper->loadData(voxels, xpoints, ypoints, zpoints, resz);
  //TODO: we should return an error code here.
}

int  ReconstructionFunc::form_grains()
{

  int totalsize = 0;
  int noseeds = 0;
  int checked = 1;
  int graincount = 1;
  double n1;
  double n2;
  double n3;
  size_t size = 0;

  //  int voxelslist[100000];
  size_t initialVoxelsListSize = 1000;
  std::vector<int> voxelslist(initialVoxelsListSize, 0);
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
   // double maxconfidence = 0;
    for(int k = 0; k < zpoints; ++k)
    {
      for(int j = 0; j < ypoints; ++j)
      {
        for(int i = 0; i < xpoints; ++i)
        {
          int point = (k*xpoints*ypoints)+(j*xpoints)+i;
          double confidence = voxels[point].confidence;
          if(confidence > minseedconfidence && voxels[point].alreadychecked == 0)
          {
            seed = point;
          }
          if(seed > -1) {break;}
        }
      }
    }
    if(seed == -1) { noseeds = 1; }
    if(seed >= 0)
    {
      size = 0;
      voxels[seed].alreadychecked = checked;
      voxels[seed].grainname = graincount;
     // cout << "Making Grain - " << graincount << endl;

      voxelslist[size] = seed;
      size++;
      for(size_t j = 0; j < size; ++j)
      {
        int currentpoint = voxelslist[j];
        double v1ea1 = voxels[currentpoint].euler1;
        double v1ea2 = voxels[currentpoint].euler2;
        double v1ea3 = voxels[currentpoint].euler3;
        for(int i = 0; i < 6; i++)
        {
          int neighbor = currentpoint+neighborhood[i];
          if(neighbor >= 0 && neighbor < totalpoints && voxels[neighbor].alreadychecked == 0)
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
              if (size >= voxelslist.size() )
              {
               // std::cout << "Resizing voxelslist to " << size << std::endl;
                voxelslist.resize( size + initialVoxelsListSize);
              }
            }
          }
        }
      }
      grains[graincount].numvoxels = size;
      totalsize = totalsize+size;
      graincount++;
	  voxelslist.clear();
	  voxelslist.resize(initialVoxelsListSize);
    }
  }
  numgrains = graincount;
  return graincount;
}


void  ReconstructionFunc::remove_smallgrains()
{
  int zero = 0;
  int badgrain = -1;
  int sizemerged = 1;
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    if(grainname >= 1)
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

int  ReconstructionFunc::renumber_grains1()
{
  int graincount = 1;
  for(int i=1;i<numgrains;i++)
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
    if(grainname >= 1)
    {
      int newgrainname = grains[grainname].newgrainname;
      voxels[j].grainname = newgrainname;
    }
  }
  return graincount;
}

void ReconstructionFunc::write_volume(string writename10)
{
  //std::cout << "ReconstructionFunc::write_volume1: '" << writename10 << "'" << std::endl;
  ofstream outFile;
  outFile.open(writename10.c_str());
  for (int i = 0; i < (xpoints * ypoints * zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    double ea1 = voxels[i].euler1;
    double ea2 = voxels[i].euler2;
    double ea3 = voxels[i].euler3;
    double ci = voxels[i].confidence;
    outFile << grainname << " " << ea1 << " " << ea2 << " " << ea3 << " " << ci << endl;
  }
  outFile.close();
}

void  ReconstructionFunc::assign_badpoints()
{
  vector<int> neighs;
  vector<int> remove;
  int checked = 1;
  int count = 1;
  double x = 0;
  double y = 0;
  double z = 0;

  std::vector<int> n(numgrains);
  while(count != 0)
  {
    count = 0;
    for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
    {
      int grainname = voxels[i].grainname;
      if(grainname == -1)
      {
        for(int c = 1; c < numgrains; c++)
        {
          n[c] = 0;
        }
	    x = i%xpoints;
		y = (i/xpoints)%ypoints;
	    z = i/(xpoints*ypoints);
        count++;
        if(x > 0)
        {
          int grain1 = voxels[i-1].grainname;
          if(grain1 > -1)
          {
            neighs.push_back(grain1);
          }
        }
        if(x < xpoints-1)
        {
          int grain2 = voxels[i+1].grainname;
          if(grain2 > -1)
          {
            neighs.push_back(grain2);
          }
        }
        if(y > 0)
        {
          int grain3 = voxels[i-(xpoints)].grainname;
          if(grain3 > -1)
          {
            neighs.push_back(grain3);
          }
        }
        if(y < ypoints-1)
        {
          int grain4 = voxels[i+(xpoints)].grainname;
          if(grain4 > -1)
          {
            neighs.push_back(grain4);
          }
        }
        if(z > 0)
        {
          int grain5 = voxels[i-(xpoints*ypoints)].grainname;
          if(grain5 > -1)
          {
            neighs.push_back(grain5);
          }
        }
        if(z < zpoints-1)
        {
          int grain6 = voxels[i+(xpoints*ypoints)].grainname;
          if(grain6 > -1)
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
void  ReconstructionFunc::find_neighbors()
{
  double x = 0;
  double y = 0;
  double z = 0;
  size_t nListSize = 1000;
  vector<int> nlist(nListSize, -1);


  for(int i = 0; i < numgrains; i++)
  {
    //std::cout << "find_neighbors: " << i << " of " << numgrains << " grains." << std::endl;
    if(grains[i].gotsizemerged != 1)
    {
      size_t nListIndex = 0;
      for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
      {
//        int onsurf = 0;
        int gnum = voxels[j].grainname;
        if(gnum == i)
        {
		    x = j%xpoints;
		    y = (j/xpoints)%ypoints;
		    z = j/(xpoints*ypoints);
//          int first = voxels[j].grainname;
          if(x > 0)
          {
              int grain1 = voxels[j-1].grainname;
              if(grain1 != i)
            {
              nlist[nListIndex] = (grain1);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(x < xpoints-1)
          {
            int grain2 = voxels[j+1].grainname;
            if(grain2 != i)
            {
              nlist[nListIndex] = (grain2);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(y > 0)
          {
            int grain3 = voxels[j-(xpoints)].grainname;
            if(grain3 != i)
            {
              nlist[nListIndex] = (grain3);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(y < ypoints-1)
          {
            int grain4 = voxels[j+(xpoints)].grainname;
            if(grain4 != i)
            {
              nlist[nListIndex] = (grain4);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(z > 0)
          {
            int grain5 = voxels[j-(xpoints*ypoints)].grainname;
            if(grain5 != i)
            {
              nlist[nListIndex] = (grain5);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
          if(z < zpoints-1)
          {
            int grain6 = voxels[j+(xpoints*ypoints)].grainname;
            if(grain6 != i)
            {
              nlist[nListIndex] = (grain6);
              ++nListIndex;
              if (nListIndex == nlist.size())
              {
                nlist.resize(nListIndex + nListSize);
              }
            }
          }
        }
      }
      if (nListIndex > 0)
      {
        vector<int>::iterator newend;
        sort(nlist.begin(),nlist.end());
        newend = unique(nlist.begin(),nlist.end());
        nlist.erase(newend,nlist.end());
        int numneighs = int(nlist.size());
        grains[i].numneighbors = numneighs;
        grains[i].neighborlist = new std::vector<int>(numneighs);
        grains[i].neighborlist->swap(nlist);
//        grains[i].neighborlist = nlist;
      }
      nlist.clear();
      nlist.resize(nListSize);
    }
  }
}


void  ReconstructionFunc::merge_containedgrains()
{
  int containedmerged = 1;
  vector<int> nlistgood;
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    int grainname = voxels[i].grainname;
    int numneighbors = grains[grainname].numneighbors;
    if(numneighbors == 1)
    {
      vector<int>* neighborlist = grains[grainname].neighborlist;
      int neighbor = neighborlist->at(0);
      voxels[i].grainname = neighbor;
      grains[grainname].gotcontainedmerged = containedmerged;
    }
  }
  for(int j = 0; j < numgrains; j++)
  {
    if(grains[j].gotsizemerged != 1 && grains[j].gotcontainedmerged != 1)
    {
      vector<int>* neighborlist = grains[j].neighborlist;
      int size = 0;
      if (NULL != neighborlist ) { size = neighborlist->size(); }
      for(int k=0;k<size;k++)
      {
        int firstneighbor = neighborlist->at(k);
        int gotcontainedmerged = grains[firstneighbor].gotcontainedmerged;
        int gotsizemerged = grains[firstneighbor].gotsizemerged;
        if(gotcontainedmerged != 1 && gotsizemerged != 1) nlistgood.push_back(firstneighbor);
      }
      int neighcount = int(nlistgood.size());
      grains[j].numneighbors = neighcount;
      if (grains[j].neighborlist == NULL) { grains[j].neighborlist = new std::vector<int>(nlistgood.size() ); }
      grains[j].neighborlist->swap(nlistgood);
      nlistgood.clear();
    }
  }
}
int  ReconstructionFunc::renumber_grains2()
{
  int graincount = 1;
  for(int i=1;i<numgrains;i++)
  {
    int gotcontainedmerged = grains[i].gotcontainedmerged;
    if(gotcontainedmerged != 1)
    {
      grains[i].newgrainname = graincount;
      int size = grains[i].numvoxels;
      int numneighbors = grains[i].numneighbors;
      vector<int>* nlist = grains[i].neighborlist;
      grains[graincount].numvoxels = size;
      grains[graincount].numneighbors = numneighbors;
      if (nlist != NULL) {
        grains[graincount].neighborlist->swap(*nlist);
      }
      graincount++;
    }
  }
  for(int j=0;j<(xpoints*ypoints*zpoints);j++)
  {
    int grainname = voxels[j].grainname;
    if(grainname >= 1)
    {
      int newgrainname = grains[grainname].newgrainname;
      voxels[j].grainname = newgrainname;
    }
  }
  return graincount;
}
void  ReconstructionFunc::homogenize_grains()
{
    grains[0].avgeuler1 = 12.566;
    grains[0].avgeuler2 = 12.566;
    grains[0].avgeuler3 = 12.566;
    grains[0].averagemisorientation = 0.0;
  for(int i = 1; i < numgrains; i++)
  {
    //std::cout << "homogenize_grains: " << i << std::endl;
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
          if(sine3 < 0) ea3 = (2*m_pi)-ea3;
          if(sine1 < 0) ea1 = (2*m_pi)-ea1;
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
//      double ci = voxels[iter].confidence;
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
void  ReconstructionFunc::load_data(string readname)
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
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    inputFile >> gnum >> ea1 >> ea2 >> ea3 >> ci;
    x = i%xpoints;
    y = (i/xpoints)%ypoints;
    z = i/(xpoints*ypoints);
    voxels[i].grainname = gnum;
    voxels[i].euler1 = ea1;
    voxels[i].euler2 = ea2;
    voxels[i].euler3 = ea3;
    voxels[i].xc = x;
    voxels[i].yc = y;
    voxels[i].zc = z;
    voxels[i].confidence = ci;
    if(gnum > numgrains) numgrains = gnum;
  }
  inputFile.close();
}
void  ReconstructionFunc::merge_twins ()
{
  int twinmerged = 1;
  int beenset = 1;
//  int mergedcount = 0;
  double angcur = 180;
  vector<int> twinlist;
  double n1,n2,n3;
  for(int i = 1; i < numgrains; i++)
  {
    if(grains[i].gotsizemerged != 1 && grains[i].gotcontainedmerged != 1 && grains[i].twinnewnumberbeenset != 1)
    {
      twinlist.push_back(i);
      int tsize = int(twinlist.size());
      for(int m=0;m<tsize;m++)
      {
        tsize = int(twinlist.size());
        int firstgrain = twinlist[m];
        vector<int>* nlist = grains[firstgrain].neighborlist;
        int size = int(nlist->size());
        for(int l=0;l<size;l++)
        {
          angcur = 180;
          int twin = 0;
          int neigh = nlist->at(l);
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

void  ReconstructionFunc::characterize_twins()
{
  for(int i=0;i<numgrains;i++)
  {

  }
}

int  ReconstructionFunc::renumber_grains3()
{
  int graincount = 1;
  std::vector<int> newnames(numgrains);
  for(int i = 1; i < numgrains; i++)
  {
    int gottwinmerged = grains[i].gottwinmerged;
    if(gottwinmerged != 1)
    {
	  newnames[i] = graincount;
      double ea1good = grains[i].avgeuler1;
      double ea2good = grains[i].avgeuler2;
      double ea3good = grains[i].avgeuler3;
      int size = grains[i].numvoxels;
      int numneighbors = grains[i].numneighbors;
      vector<int>* nlist = grains[i].neighborlist;
      grains[graincount].numvoxels = size;
      grains[graincount].numneighbors = numneighbors;
    if (grains[graincount].neighborlist == NULL)
    {
      grains[graincount].neighborlist = new std::vector<int>(numneighbors);
    }
      if (NULL != nlist) {
          grains[graincount].neighborlist->swap(*nlist);
    }
      grains[graincount].avgeuler1 = ea1good;
      grains[graincount].avgeuler2 = ea2good;
      grains[graincount].avgeuler3 = ea3good;
      graincount++;
    }
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int grainname = voxels[j].grainname;
    if(grainname >= 1)
	{
		int newgrainname = newnames[grainname];
		voxels[j].grainname = newgrainname;
	}
  }
  return graincount;
}
void  ReconstructionFunc::find_goodneighbors()
{
  double x = 0;
  double y = 0;
  double z = 0;
    size_t nListSize = 1000;
  vector<int> nlist(nListSize, -1);
    size_t nListIndex = 0;
  int neighcount = 0;
  for(int i = 0; i < numgrains; i++)
  {
    for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
    {
      int onsurf = 0;
      int gnum = voxels[j].grainname;
      if(gnum == i)
      {
	    x = j%xpoints;
	    y = (j/xpoints)%ypoints;
	    z = j/(xpoints*ypoints);
//        int first = voxels[j].grainname;
        if(x > 0)
        {
          int grain1 = voxels[j-1].grainname;
          if(grain1 != i)
          {
            onsurf = 1;
            nlist[nListIndex] = grain1;
            ++nListIndex;
            if (nListIndex == nlist.size() )
            {
              nlist.resize(nlist.size() + nListSize);
            }
          }
        }
        if(x < xpoints-1)
        {
          int grain2 = voxels[j+1].grainname;
          if(grain2 != i)
          {
            onsurf = 1;
            nlist[nListIndex] = grain2;
            ++nListIndex;
            if (nListIndex == nlist.size() )
            {
              nlist.resize(nlist.size() + nListSize);
            }
          }
        }
        if(y > 0)
        {
          int grain3 = voxels[j-(xpoints)].grainname;
          if(grain3 != i)
          {
            onsurf = 1;
            nlist[nListIndex] = grain3;
            ++nListIndex;
            if (nListIndex == nlist.size() )
            {
              nlist.resize(nlist.size() + nListSize);
            }
          }
        }
        if(y < ypoints-1)
        {
          int grain4 = voxels[j+(xpoints)].grainname;
          if(grain4 != i)
          {
            onsurf = 1;
            nlist[nListIndex] = grain4;
            ++nListIndex;
            if (nListIndex == nlist.size() )
            {
              nlist.resize(nlist.size() + nListSize);
            }
          }
        }
        if(z > 0)
        {
          int grain5 = voxels[j-(xpoints*ypoints)].grainname;
          if(grain5 != i)
          {
            onsurf = 1;
            nlist[nListIndex] = grain5;
            ++nListIndex;
            if (nListIndex == nlist.size() )
            {
              nlist.resize(nlist.size() + nListSize);
            }
          }
        }
        if(z < zpoints-1)
        {
          int grain6 = voxels[j+(xpoints*ypoints)].grainname;
          if(grain6 != i)
          {
            onsurf = 1;
            nlist[nListIndex] = grain6;
            ++nListIndex;
            if (nListIndex == nlist.size() )
            {
              nlist.resize(nlist.size() + nListSize);
            }
          }
        }
      }
      voxels[j].surfacevoxel = onsurf;
    }
    if ( nListIndex > 0) {
      vector<int>::iterator newend;
      sort(nlist.begin(),nlist.end());
      newend = unique(nlist.begin(),nlist.end());
      nlist.erase(newend,nlist.end());
      neighcount = int(nlist.size());
      grains[i].numneighbors = neighcount;
      if (grains[i].neighborlist == NULL)
      {
        grains[i].neighborlist = new std::vector<int>(nlist.size() );
      }
      grains[i].neighborlist->swap(nlist);
    }
    nlist.clear();
    nlist.resize(nListSize);
    nListIndex = 0;
  }
}
void  ReconstructionFunc::find_centroids()
{
//  int count = 0;
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
	    int col = j%xpoints;
	    int row = (j/xpoints)%ypoints;
	    int plane = j/(xpoints*ypoints);
        if(col <= 0) onedge = 1;
        if(col >= xpoints-1) onedge = 1;
        if(row <= 0) onedge = 1;
        if(row >= ypoints-1) onedge = 1;
        if(plane <= 0) onedge = 1;
        if(plane >= zpoints-1) onedge = 1;
        sumx = sumx + x;
        sumy = sumy + y;
        sumz = sumz + z;
      }
    }
    centerx = sumx/size;
    centery = sumy/size;
    centerz = sumz/size;
    grains[i].centroidx = centerx;
    grains[i].centroidy = centery;
    grains[i].centroidz = centerz;
    grains[i].numvoxels = size;
	double diametercubed = (0.75*size)/m_pi;
	int diameter = int(pow(diametercubed,0.3333333333));
	if(diameter > maxdiameter) maxdiameter = diameter;
	if(diameter < mindiameter) mindiameter = diameter;
    grains[i].surfacegrain = onedge;
  }
}


void  ReconstructionFunc::find_moments ()
{
//  int count = 0;
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
    double xc = grains[i].centroidx;
    double yc = grains[i].centroidy;
    double zc = grains[i].centroidz;
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
    grains[i].Ixx = sumxx;
    grains[i].Iyy = sumyy;
    grains[i].Izz = sumzz;
    grains[i].Ixy = -sumxy;
    grains[i].Ixz = -sumxz;
    grains[i].Iyz = -sumyz;
  }
}
void  ReconstructionFunc::find_axes ()
{
  for (int i = 0; i < numgrains; i++)
  {
    double Ixx = grains[i].Ixx;
    double Iyy = grains[i].Iyy;
    double Izz = grains[i].Izz;
    double Ixy = grains[i].Ixy;
    double Iyz = grains[i].Iyz;
    double Ixz = grains[i].Ixz;
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
    grains[i].axis1 = r1;
    grains[i].axis2 = r2;
    grains[i].axis3 = r3;
  }
}
void  ReconstructionFunc::find_vectors ()
{
  for(int i = 0; i < numgrains; i++)
  {
    int size = grains[i].numvoxels;
    if(size > 1)
    {
      double Ixx = grains[i].Ixx;
      double Iyy = grains[i].Iyy;
      double Izz = grains[i].Izz;
      double Ixy = grains[i].Ixy;
      double Iyz = grains[i].Iyz;
      double Ixz = grains[i].Ixz;
      double axis1 = grains[i].axis1;
      double axis2 = grains[i].axis2;
      double axis3 = grains[i].axis3;
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
      grains[i].axis1x = n1x;
      grains[i].axis1y = n1y;
      grains[i].axis1z = n1z;
      grains[i].axis2x = n2x;
      grains[i].axis2y = n2y;
      grains[i].axis2z = n2z;
      grains[i].axis3x = n3x;
      grains[i].axis3y = n3y;
      grains[i].axis3z = n3z;
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
      grains[i].axis1x = n1x;
      grains[i].axis1y = n1y;
      grains[i].axis1z = n1z;
      grains[i].axis2x = n2x;
      grains[i].axis2y = n2y;
      grains[i].axis2z = n2z;
      grains[i].axis3x = n3x;
      grains[i].axis3y = n3y;
      grains[i].axis3z = n3z;
    }
  }
}
void  ReconstructionFunc::measure_misorientations ()
{
  vector<double> misolist;
  double n1;
  double n2;
  double n3;
  for (int i = 0; i < numgrains; i++)
  {
    vector<int>* nlist = grains[i].neighborlist;
    double g1ea1 = grains[i].avgeuler1;
    double g1ea2 = grains[i].avgeuler2;
    double g1ea3 = grains[i].avgeuler3;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
    for(int j=0;j<size;j++)
    {
      int nname = nlist->at(j);
      double g2ea1 = grains[nname].avgeuler1;
      double g2ea2 = grains[nname].avgeuler2;
      double g2ea3 = grains[nname].avgeuler3;
      double w = getmisoquat(crystruct,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      misolist.push_back(w);
    }
    grains[i].misorientationlist = misolist;
    misolist.clear();
  }
}
double ReconstructionFunc::getmisoquat(double crystruct,double misorientationtolerance,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wmin=9999999; //,na,nb,nc;
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
  wmin = (360.0/m_pi)*wmin;
  return wmin;
}

void  ReconstructionFunc::find_colors()
{
  for(int i = 0; i < numgrains; i++)
  {
    double g1ea1 = grains[i].avgeuler1;
    double g1ea2 = grains[i].avgeuler2;
    double g1ea3 = grains[i].avgeuler3;
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
    grains[i].red = r;
    grains[i].green = g;
    grains[i].blue = b;
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
    grains[i].schmidfactor = schmid;
  }
}
void  ReconstructionFunc::find_convexities()
{
  for(int i = 0; i < numgrains; i++)
  {
    int size = grains[i].numvoxels;
    if(size > 1)
    {
      double insidecount = 0;
      int vol = grains[i].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double xc = grains[i].centroidx;
      double yc = grains[i].centroidy;
      double zc = grains[i].centroidz;
      double rad1x = grains[i].axis1x;
      double rad1y = grains[i].axis1y;
      double rad1z = grains[i].axis1z;
      double rad2x = grains[i].axis2x;
      double rad2y = grains[i].axis2y;
      double rad2z = grains[i].axis2z;
      double rad3x = grains[i].axis3x;
      double rad3y = grains[i].axis3y;
      double rad3z = grains[i].axis3z;
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
      double I1 = grains[i].axis1;
      double I2 = grains[i].axis2;
      double I3 = grains[i].axis3;
      I1 = (15*I1)/(4*m_pi);
      I2 = (15*I2)/(4*m_pi);
      I3 = (15*I3)/(4*m_pi);
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
      double rad_1 = voxvol/(m_OnePointThree * m_pi * bovera * covera );
      //double rad_1 = voxvol/(m_OnePointThree*m_pi*bovera*covera);
      rad_1 = pow(rad_1,0.3333333);
      double rad_2 = rad_1*bovera;
      double rad_3 = rad_1*covera;
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
          double inside = 1-(((constmat[0][0])/rad_3)*((constmat[0][0])/rad_3))-(((constmat[1][0])/rad_2)*((constmat[1][0])/rad_2))-(((constmat[2][0])/rad_1)*((constmat[2][0])/rad_1));
          if(inside >= 0)
          {
            insidecount++;
          }
        }
      }
      double convex = insidecount/size;
      grains[i].convexity = convex;
    }
    if(size == 1)
    {
      double convex = 1;
      grains[i].convexity = convex;
    }
  }
}
void ReconstructionFunc::volume_stats(string writename1,string writename2,string writename3)
{
  //TODO: There is a problem calculating some of the array indices which will cause an array out of
  //TODO: bounds exception during execution on Windows. Not sure how GCC is getting past this.
  if (true) return;

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
  double svshape[100][5];
  double svbovera[100][5];
  double svcovera[100][5];
  double svcoverb[100][5];
  double svschmid[100][5];
  double svem[100][5];
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
  for(int iter1 = 0; iter1 < 10; iter1++)
  {
    for(int iter2 = 0; iter2 < 10; iter2++)
    {
      for(int iter3 = 0; iter3 < 10; iter3++)
      {
        eulercount[iter1][iter2][iter3] = 0;
      }
    }
  }
  for(int i = 0; i < numgrains; i++)
  {
    int onedge = grains[i].surfacegrain;
    if(onedge == 0)
    {
      double g1ea1 = grains[i].avgeuler1;
      double g1ea2 = grains[i].avgeuler2;
      double g1ea3 = grains[i].avgeuler3;
      actualgrains++;
      int vol = grains[i].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double logvol = log(voxvol);
      double rad_3 = 0.75*(1/m_pi)*voxvol;
      double diam = 2*pow(rad_3,0.333333333);
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
        if(sine3 < 0) ea3 = (2*m_pi)-ea3;
        if(sine1 < 0) ea1 = (2*m_pi)-ea1;
        int ea1bin = int(0.5+ea1/(m_pi/36));
        int ea2bin = int(0.5+ea2/(m_pi/36));
        int ea3bin = int(0.5+ea3/(m_pi/36));
        if(ea1 >= 0 && ea2 >= 0 && ea3 >= 0 && ea1 <= (m_pi/2) && ea2 <= (m_pi/2) && ea3 <= (m_pi/2))
        {
          eulercount[ea1bin][ea2bin][ea3bin]++;
        }
      }
      double I1 = grains[i].axis1;
      double I2 = grains[i].axis2;
      double I3 = grains[i].axis3;
      I1 = (15*I1)/(4*m_pi);
      I2 = (15*I2)/(4*m_pi);
      I3 = (15*I3)/(4*m_pi);
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
      int nnum = grains[i].numneighbors;
      double lognnum = log(double(nnum));
      double schmid = grains[i].schmidfactor;
      double em = grains[i].convexity;
      vector<int>* nlist = grains[i].neighborlist;
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
      int size = 0;
      if (NULL != nlist) { size = nlist->size(); }
      for(int k=0;k<size;k++)
      {
        int neigh = nlist->at(k);
        int neighvol = grains[neigh].numvoxels;
        double neighvoxvol = neighvol*resx*resy*resz;
    //    double neighlogvol = log(neighvoxvol);
        double neighrad3 = 0.75*(1/m_pi)*neighvoxvol;
        double neighdiam = 2*pow(neighrad3,0.333333333);
        double logneighdiam = log(neighdiam);
//        int neighdiamint = int(neighdiam);
        svs[diamint][0]++;
        svs[diamint][1] = svs[diamint][1] + neighdiam;
        svs[diamint][3] = svs[diamint][3] + logneighdiam;
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
      svs[temp3][3] = svs[temp3][3]/svs[temp3][0];
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
  double misobin[10];
  double microbin[10];
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
    int onedge = grains[j].surfacegrain;
    if(onedge == 0)
    {
      int vol = grains[j].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double logvol = log(voxvol);
      double rad_3 = 0.75*(1/m_pi)*voxvol;
      double diam = 2*pow(rad_3,0.333333333);
      int diamint = int(diam);
      double logdiam = log(diam);
      double I1 = grains[j].axis1;
      double I2 = grains[j].axis2;
      double I3 = grains[j].axis3;
      I1 = (15*I1)/(4*m_pi);
      I2 = (15*I2)/(4*m_pi);
      I3 = (15*I3)/(4*m_pi);
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
      int nnum = grains[j].numneighbors;
      double lognnum = log(double(nnum));
      double schmid = grains[j].schmidfactor;
      double em = grains[j].convexity;
      vector<int>* nlist = grains[j].neighborlist;
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
      int size = 0;
      if (NULL != nlist) { size = nlist->size(); }
      for(int k=0;k<size;k++)
      {
        int neigh = (*nlist)[k];
        int neighvol = grains[neigh].numvoxels;
        double neighvoxvol = neighvol*resx*resy*resz;
//        double neighlogvol = log(neighvoxvol);
        double neighrad3 = 0.75*(1/m_pi)*neighvoxvol;
        double neighdiam = 2*pow(neighrad3,0.333333333);
        double logneighdiam = log(neighdiam);
        int neighdiamint = int(neighdiam);
        svs[diamint][2] = svs[diamint][2] + ((neighdiam-svs[diamint][1])*(neighdiam-svs[diamint][1]));
        svs[diamint][4] = svs[diamint][4] + ((logneighdiam-svs[diamint][3])*(logneighdiam-svs[diamint][3]));
        sddiam2 = sddiam2 + ((neighdiam-avgdiam2)*(neighdiam-avgdiam2));
      }
    }
  }
  for(int temp4 = 0; temp4 < (maxdiamint+1); temp4++)
  {
    if(svn[temp4][0] != 0)
    {
      svn[temp4][2] = svn[temp4][2]/svn[temp4][0];
      svn[temp4][4] = svn[temp4][4]/svn[temp4][0];
      svs[temp4][2] = svs[temp4][2]/svs[temp4][0];
      svs[temp4][4] = svs[temp4][4]/svs[temp4][0];
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
//  double volvar = sdvol;
//  double vollnvar = sdlnvol;
  double neighvar = sdneigh;
//  double neighlnvar = sdlnneigh;
  double boveravar = sdbovera;
  double coveravar = sdcovera;
  double coverbvar = sdcoverb;
//  double diamvar = sddiam;
//  double logdiamvar = sdlogdiam;
  double diamvar2 = sddiam2;
  double schmidvar = sdschmid;
  double emvar = sdem;
//  double pbovera = avgbovera*(((avgbovera*(1-avgbovera))/boveravar)-1);
//  double qbovera = (1-avgbovera)*(((avgbovera*(1-avgbovera))/boveravar)-1);
//  double pcovera = avgcovera*(((avgcovera*(1-avgcovera))/coveravar)-1);
//  double qcovera = (1-avgcovera)*(((avgcovera*(1-avgcovera))/coveravar)-1);
//  double pcoverb = avgcoverb*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
//  double qcoverb = (1-avgcoverb)*(((avgcoverb*(1-avgcoverb))/coverbvar)-1);
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
    int onedge = grains[l].surfacegrain;
    if(onedge == 0)
    {
      double microcount = 0;
      vector<double> misolist = grains[l].misorientationlist;
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
  double delta = m_pi/36;
  double texindex = 0;
  double texstrength = 0;
  for(int iter51 = 0; iter51 < 36; iter51++)
  {
    for(int iter52 = 0; iter52 < 36; iter52++)
    {
      for(int iter53 = 0; iter53 < 36; iter53++)
      {
        double f = 0;
        if(iter52 == 0)
        {
          f = (m_pi*m_pi/4)*(double(eulercount[iter51][iter52][iter53]/numgrains)/((delta)*(delta)*cos(double((iter52*delta)+(delta/2)))));
        }
        if(iter52 == 36)
        {
          f = (m_pi*m_pi/4)*(double(eulercount[iter51][iter52][iter53]/numgrains)/((delta)*(delta)*cos(double((iter52*delta)-(delta/2)))));
        }
        if(iter52 != 0 && iter52 != 36)
        {
          f = (m_pi*m_pi/4)*(double(eulercount[iter51][iter52][iter53]/numgrains)/((delta)*(delta)*(cos(double((iter52*delta)-(delta/2)))-cos(double((iter52*delta)+(delta/2))))));
        }
        texindex = texindex + (f*f);
      }
    }
    texindex = texindex/(36*36*36);
    texstrength = pow(texindex,0.5);
  }
  ofstream outFile;
  outFile.open(writename1.c_str());
  outFile << (maxdiameter-mindiameter)+1 << " " << maxdiameter << " " << mindiameter << endl;
  outFile << endl;
  outFile << avglogdiam << " " << sdlogdiam << endl;
  outFile << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
	outFile << temp7 <<	" " << svbovera[temp7][3] << " " << svbovera[temp7][4] << endl;
  }
  outFile << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svcovera[temp7][3] << "  " << svcovera[temp7][4] << endl;
  }
  outFile << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svcoverb[temp7][3] << "  " << svcoverb[temp7][4] << endl;
  }
  outFile << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svn[temp7][3] << " " << svn[temp7][4] << endl;
  }
  outFile << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svs[temp7][3] << " " << svs[temp7][4] << endl;
  }
  outFile << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svshape[temp7][3] << " " << svshape[temp7][4] << endl;
  }
  outFile.close();
  ofstream outFile7;
  outFile7.open(writename2.c_str());
  for(int i = 0; i < nummisobins; i++)
  {
    outFile7 << (misobin[i]/misocount) << endl;
  }
  outFile7.close();
  ofstream outFile8;
  outFile8.open(writename3.c_str());
  for(int i = 0; i < nummicrobins; i++)
  {
    outFile8 << (microbin[i]/actualgrains) << endl;
  }
  outFile8.close();
}
void  ReconstructionFunc::create_visualization(string writename11)
{
    ofstream outFile;
    outFile.open(writename11.c_str());
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


void ReconstructionFunc::create_dxfile(string dxfile)
{
  ofstream outFile;
  outFile.open(dxfile.c_str());
  outFile << "object 1 class gridpositions counts " << xpoints << " " << ypoints << " " << zpoints << endl;
  //TODO: Finish the implementation
  outFile.close();
}

void  ReconstructionFunc::write_grains(string writename12)
{
    ofstream outFile;
    outFile.open(writename12.c_str());
  outFile << numgrains<< endl;
  for(int i=0;i<numgrains;i++)
  {
    double xc = grains[i].centroidx;
    double yc = grains[i].centroidy;
    double zc = grains[i].centroidz;
    double ea1 = grains[i].avgeuler1;
    double ea2 = grains[i].avgeuler2;
    double ea3 = grains[i].avgeuler3;
    int nnum = grains[i].numneighbors;
    vector<int>* nlist = grains[i].neighborlist;
    outFile << i << " " << xc << "  " << yc << "  " << zc << "  " << ea1 << " " << ea2 << " " << ea3 << " " << nnum;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
    for(int k=0;k<size;k++)
    {
      int neigh = (*nlist)[k];
      outFile << "  " << neigh;
    }
    outFile << endl;
  }
}

void  ReconstructionFunc::write_axisorientations(string writename14)
{
    ofstream outFile;
    outFile.open(writename14.c_str());
  outFile << numgrains << endl;
  for(int i = 0; i < numgrains; i++)
  {
    double r1x = grains[i].axis1x;
    double r1y = grains[i].axis1y;
    double r1z = grains[i].axis1z;
    double r2x = grains[i].axis2x;
    double r2y = grains[i].axis2y;
    double r2z = grains[i].axis2z;
    double r3x = grains[i].axis3x;
    double r3y = grains[i].axis3y;
    double r3z = grains[i].axis3z;
    outFile << r1x << " " << r1y << " " << r1z << " " << r2x << " " << r2y << " " << r2z << " " << r3x << " " << r3y << " " << r3z << endl;
  }
  outFile.close();
}
void  ReconstructionFunc::write_eulerangles(string writename15)
{
    ofstream outFile;
    outFile.open(writename15.c_str());
  outFile << numgrains <<endl;
  for(int i = 0; i < numgrains; i++)
  {
    double ea1 = grains[i].avgeuler1;
    double ea2 = grains[i].avgeuler2;
    double ea3 = grains[i].avgeuler3;
    outFile << ea1 << " " << ea2 << " " << ea3 << endl;
  }
  outFile.close();
}

double ReconstructionFunc::gamma(double x)
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
                ga = -1 * m_pi/(x*ga*sin(m_pi*x));
            }
        }
    }
    return ga;
}

