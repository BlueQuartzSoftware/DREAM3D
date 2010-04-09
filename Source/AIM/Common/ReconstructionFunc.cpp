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
#include <AIM/ANG/AngFileReader.h>

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
  tempvoxels(NULL),
  grains(NULL),
  eulerodf(NULL),
  axisodf(NULL)
{

}

ReconstructionFunc::~ReconstructionFunc()
{

  delete [] voxels;
  delete [] tempvoxels;
  delete [] grains;
  delete [] graincenters;
  delete [] grainmoments;
  delete [] eulerodf;
  delete [] axisodf;

}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void ReconstructionFunc::initialize(double stepX, double stepY, double stepZ,
                  int angNumCols, int angNumRows, int angSlices,
                  bool v_mergetwinsoption,
                  bool v_mergecoloniesoption,
                  int v_minallowedgrainsize,
                  double v_minseedconfidence,
                  double v_minseedimagequality,
                  double v_misorientationtolerance,
                  int v_crystruct,
                  bool v_alreadyformed)
{

  resx = stepX;
  resy = stepY;
  resz = stepZ;

  mergetwinsoption = (v_mergetwinsoption == true) ? 1 : 0;
  mergecoloniesoption = (v_mergecoloniesoption == true) ? 1 : 0;
  minallowedgrainsize = v_minallowedgrainsize;
  minseedconfidence = v_minseedconfidence;
  minseedimagequality = v_minseedimagequality;
  misorientationtolerance = v_misorientationtolerance;
  crystruct = v_crystruct;
  alreadyformed = (v_alreadyformed == true) ? 1 : 0;

  tempxpoints = angNumCols;
  tempypoints = angNumRows;
  xpoints = cutoutxsize;
  ypoints = cutoutysize;
  zpoints = angSlices;

  sizex = (xpoints - 1 ) * resx;
  sizey = (ypoints - 1 ) * resy;
  sizez = (zpoints - 1 ) * resz;

  totalpoints = xpoints * ypoints * zpoints;
  totaltemppoints = tempxpoints * tempypoints * 2;

  nummisobins = 10;
  nummicrobins = 10;

  tempvoxels = new Voxel[totaltemppoints];
  voxels = new Voxel[totalpoints];
  grains = new Grain[totalpoints/10];
  eulerodf = new Bin[18*18*18];
  axisodf = new Bin[18*18*18];
} 
void ReconstructionFunc::find_cutout(string angFName, int angNumCols, int angNumRows, double v_minseedconfidence, double v_minseedimagequality)
{
	double imagequality;
	double confidence;
	double x;
	double y;
	int index;
	int sum=0;
	int maxx=0;
	int minx=100000;
	int maxy=0;
	int miny=100000;
	arr = new int *[angNumCols];
	for(int i=0;i<angNumCols;i++)
	{
		arr[i] = new int [angNumRows];
	}
    AngFileReader::Pointer reader = AngFileReader::New();
    int err = reader->readFile(angFName);
	float* confPtr = reader->getConfidenceIndexData()->getPointer(0);
	float* imqualPtr = reader->getImageQualityData()->getPointer(0);
	for (int j = 0; j < angNumRows; j++)
	{
		for (int i = 0; i < angNumCols; i++)
		{
			index = (j * angNumCols) + i;
			imagequality = imqualPtr[index];
			confidence = confPtr[index];
			if(confidence >= v_minseedconfidence && imagequality >= v_minseedimagequality) arr[i][j] = 1;
			if(confidence < v_minseedconfidence || imagequality < v_minseedimagequality) arr[i][j] = 0;
		}
	}
	for (int j = 0; j < angNumRows; j++)
	{
		sum=0;
		for (int i = 0; i < angNumCols; i++)
		{
			sum = sum+arr[i][j];
		}
		if(sum > 10 && j<miny) miny=j;
		if(sum > 10 && j>maxy) maxy=j;
	}
	for (int j = 0; j < angNumCols; j++)
	{
		sum=0;
		for (int i = 0; i < angNumRows; i++)
		{
			sum = sum+arr[j][i];
		}
		if(sum > 10 && j<minx) minx=j;
		if(sum > 10 && j>maxx) maxx=j;
	}
	cmaxx = maxx;
	cminx = minx;
	cmaxy = maxy;
	cminy = miny;
	cutoutxsize = (cmaxx-cminx)+1;
	cutoutysize = (cmaxy-cminy)+1;
}
void ReconstructionFunc::loadSlices(double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
    shifts = new int *[1];
    shifts[0] = new int [2];
    shifts[0][0]=0;
    shifts[0][1]=0;
	for(int i=0;i<zpoints;i++)
	{
	  m_angFileHelper->loadData(tempvoxels, tempxpoints, tempypoints, i, resz, minseedconfidence, minseedimagequality);
	  align_sections(i,quat_symmcubic,quat_symmhex);
	}
  //TODO: we should return an error code here.
}

void ReconstructionFunc::align_sections(int slice,double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
  double disorientation = 0;
  double mindisorientation = 100000;
  int xshift = 0;
  int yshift = 0;
  int tempxshift = 0;
  int tempyshift = 0;
  int count = 0;
  int step = 0;
  double w;
  double n1,n2,n3;
  mindisorientation = 100000;
  xshift = 0;
  yshift = 0;
  tempxshift = 0;
  tempyshift = 0;
  if(slice > 0)
  {
	  for(int a=0;a<2;a++)
	  {
		 if(a == 0) step = 3;
		 if(a == 1) step = 2;
		 if(a == 2) step = 1;
		 for(int j=-3;j<4;j++)
		 {
			for(int k=-3;k<4;k++)
			{
				disorientation = 0;
				count = 0;
				for(int l=0;l<tempypoints;l++)
				{
					for(int m=0;m<tempxpoints;m++)
					{
						int refposition = (tempxpoints*tempypoints)+(l*tempxpoints)+m;
						int curposition = ((l+(j*step)+tempyshift)*tempxpoints)+(m+(k*step)+tempxshift);
						if((l+(j*step)+tempyshift) >= 0 && (l+(j*step)+tempyshift) < tempypoints && (m+(k*step)+tempxshift) >= 0 && (m+(k*step)+tempxshift) < tempxpoints)
						{
							double refci = tempvoxels[refposition].confidence;
							double curci = tempvoxels[curposition].confidence;
							double refiq = tempvoxels[refposition].imagequality;
							double curiq = tempvoxels[curposition].imagequality;
							if(a < 2)
							{
								disorientation = disorientation + fabs(refiq-curiq);
								count++;
							}
							if(a == 2)
							{
								if(refci > minseedconfidence && curci > minseedconfidence)
								{
									double g1ea1 = tempvoxels[refposition].euler1;
									double g1ea2 = tempvoxels[refposition].euler2;
									double g1ea3 = tempvoxels[refposition].euler3;
									double g2ea1 = tempvoxels[curposition].euler1;
									double g2ea2 = tempvoxels[curposition].euler2;
									double g2ea3 = tempvoxels[curposition].euler3;
									if(crystruct == 1) w = getmisoquathexagonal(quat_symmhex,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
									if(crystruct == 2) w = getmisoquatcubic(misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
									if(w < 5) w = 0;
									if(w > 5) w = 1;
									disorientation = disorientation + w;
									count++;
								}
//								if(refci < minseedconfidence && curci > minseedconfidence) disorientation = disorientation + 62.54, count++;
//								if(refci > minseedconfidence && curci < minseedconfidence) disorientation = disorientation + 62.54, count++;
//								if(refci < minseedconfidence && curci < minseedconfidence) disorientation = disorientation + 0.0, count++;
							}
						}
					}
				}
				disorientation = disorientation/double(count);
				if(disorientation < mindisorientation)
				{
					xshift = (k*step)+tempxshift;
					yshift = (j*step)+tempyshift;
					mindisorientation = disorientation;
				}
			}
		 }
		 tempxshift = xshift;
		 tempyshift = yshift;
	  }
//	  shifts[0][0] = shifts[0][0]+xshift;
//	  shifts[0][1] = shifts[0][1]+yshift;
	  shifts[0][0] = xshift;
	  shifts[0][1] = yshift;
	  for(int l=0;l<=ypoints-1;l++)
	  {
		for(int m=0;m<=xpoints-1;m++)
		{
			int position = (((zpoints-1)-slice)*xpoints*ypoints)+(l*xpoints)+m;
			int tempposition = ((l+shifts[0][1]+cminy)*tempxpoints)+(m+shifts[0][0]+cminx);
			voxels[position].euler1 = tempvoxels[tempposition].euler1; 
			voxels[position].euler2 = tempvoxels[tempposition].euler2; 
			voxels[position].euler3 = tempvoxels[tempposition].euler3; 
			voxels[position].quat[0] = tempvoxels[tempposition].quat[0];
			voxels[position].quat[1] = tempvoxels[tempposition].quat[1];
			voxels[position].quat[2] = tempvoxels[tempposition].quat[2];
			voxels[position].quat[3] = tempvoxels[tempposition].quat[3];
			voxels[position].quat[4] = tempvoxels[tempposition].quat[4];
			voxels[position].confidence = tempvoxels[tempposition].confidence; 
			voxels[position].imagequality = tempvoxels[tempposition].imagequality; 
			voxels[position].alreadychecked = tempvoxels[tempposition].alreadychecked; 
			voxels[position].grainname = tempvoxels[tempposition].grainname; 
		}
	  }
  }
  if(slice == 0)
  {
		for(int l=0;l<=ypoints-1;l++)
		{
			for(int m=0;m<=xpoints-1;m++)
			{
					int position = (((zpoints-1)-slice)*xpoints*ypoints)+(l*xpoints)+m;
					int tempposition = ((l+cminy)*tempxpoints)+(m+cminx);
					voxels[position].euler1 = tempvoxels[tempposition].euler1; 
					voxels[position].euler2 = tempvoxels[tempposition].euler2; 
					voxels[position].euler3 = tempvoxels[tempposition].euler3; 
					voxels[position].quat[0] = tempvoxels[tempposition].quat[0];
					voxels[position].quat[1] = tempvoxels[tempposition].quat[1];
					voxels[position].quat[2] = tempvoxels[tempposition].quat[2];
					voxels[position].quat[3] = tempvoxels[tempposition].quat[3];
					voxels[position].quat[4] = tempvoxels[tempposition].quat[4];
					voxels[position].confidence = tempvoxels[tempposition].confidence; 
					voxels[position].imagequality = tempvoxels[tempposition].imagequality; 
					voxels[position].alreadychecked = tempvoxels[tempposition].alreadychecked; 
					voxels[position].grainname = tempvoxels[tempposition].grainname; 
			}
		}
  }
}
int  ReconstructionFunc::form_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5])
{

  int totalsize = 0;
  int noseeds = 0;
  int checked = 1;
  int graincount = 1;
  double w;
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
        if(seed > -1) {break;}
      }
      if(seed > -1) {break;}
    }
    if(seed == -1) { noseeds = 1; }
    if(seed >= 0)
    {
      size = 0;
      voxels[seed].alreadychecked = checked;
      voxels[seed].grainname = graincount;
      cout << "Making Grain - " << graincount << endl;

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
		    if(crystruct == 1) w = getmisoquathexagonal(quat_symmhex,misorientationtolerance,v1ea1,v1ea2,v1ea3,v2ea1,v2ea2,v2ea3,n1,n2,n3);
			if(crystruct == 2) w = getmisoquatcubic(misorientationtolerance,v1ea1,v1ea2,v1ea3,v2ea1,v2ea2,v2ea3,n1,n2,n3);
            if(w < misorientationtolerance)
            {
              voxels[neighbor].alreadychecked = checked;
              voxels[neighbor].grainname = graincount;
              voxelslist[size] = neighbor;
              size++;
              if (size >= voxelslist.size() )
              {
               // std::cout << "Resizing voxelslist to " << size << std::endl;
                voxelslist.resize(size+initialVoxelsListSize,-1);
              }
            }
          }
        }
      }
	  grains[graincount].nucleus = seed;
      grains[graincount].numvoxels = size;
      totalsize = totalsize+size;
	  voxelslist.erase(std::remove(voxelslist.begin(),voxelslist.end(),-1),voxelslist.end());
      if (grains[graincount].voxellist == NULL)
      {
        grains[graincount].voxellist = new std::vector<int>(voxelslist.size(),-1);
      }
      grains[graincount].voxellist->swap(voxelslist);
      graincount++;
	  voxelslist.clear();
	  voxelslist.resize(initialVoxelsListSize,-1);
    }
  }
  numgrains = graincount;
  return graincount;
}


void  ReconstructionFunc::remove_smallgrains()
{
  int zero = 0;
  int badgrain = 0;
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
	  vector<int>* voxellist = grains[i].voxellist;
	  int vsize = voxellist->size();
	  int nucleus = grains[i].nucleus;
      grains[graincount].numvoxels = size;
	  grains[graincount].nucleus = nucleus;
      if (voxellist != NULL)
	  {
		  grains[graincount].voxellist = new std::vector<int>(vsize);
		  grains[graincount].voxellist->swap(*voxellist);
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
      if(grainname == 0)
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
          if(grain1 > 0)
          {
            neighs.push_back(grain1);
          }
        }
        if(x < xpoints-1)
        {
          int grain2 = voxels[i+1].grainname;
          if(grain2 > 0)
          {
            neighs.push_back(grain2);
          }
        }
        if(y > 0)
        {
          int grain3 = voxels[i-(xpoints)].grainname;
          if(grain3 > 0)
          {
            neighs.push_back(grain3);
          }
        }
        if(y < ypoints-1)
        {
          int grain4 = voxels[i+(xpoints)].grainname;
          if(grain4 > 0)
          {
            neighs.push_back(grain4);
          }
        }
        if(z > 0)
        {
          int grain5 = voxels[i-(xpoints*ypoints)].grainname;
          if(grain5 > 0)
          {
            neighs.push_back(grain5);
          }
        }
        if(z < zpoints-1)
        {
          int grain6 = voxels[i+(xpoints*ypoints)].grainname;
          if(grain6 > 0)
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
      if(grainname == 0 && hasneighbor == 1)
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
  int x = 0;
  int y = 0;
  int z = 0;
  int grain;
  int grain1;
  int grain2;
  int grain3;
  int grain4;
  int grain5;
  int grain6;
  int nnum;
  int nListSize = 1000;
  std::vector<int> nlist(nListSize, -1);
  for(int i=0;i<numgrains;i++)
  {
    int numneighs = int(nlist.size());
	grains[i].numneighbors = 0;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist->swap(nlist);
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int grain = voxels[j].grainname;
	x = j%xpoints;
	y = (j/xpoints)%ypoints;
	z = j/(xpoints*ypoints);
	if(x > 0)
	{
	  grain1 = voxels[j-1].grainname;
	  if(grain1 != grain && grain1!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain1;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(x < xpoints-1)
	{
	  grain2 = voxels[j+1].grainname;
	  if(grain2 != grain && grain2!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain2;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(y > 0)
	{
	  grain3 = voxels[j-(xpoints)].grainname;
	  if(grain3 != grain && grain3!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain3;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(y < ypoints-1)
	{
	  grain4 = voxels[j+(xpoints)].grainname;
	  if(grain4 != grain && grain4!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain4;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(z > 0)
	{
	  grain5 = voxels[j-(xpoints*ypoints)].grainname;
	  if(grain5 != grain && grain5!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain5;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(z < zpoints-1)
	{
	  grain6 = voxels[j+(xpoints*ypoints)].grainname;
	  if(grain6 != grain && grain6!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain6;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
  }
  for(int i=1;i<numgrains;i++)
  {
	vector<int>* nlist = grains[i].neighborlist;
	vector<int>::iterator newend;
	sort(nlist->begin(),nlist->end());
    newend = unique(nlist->begin(),nlist->end());
    nlist->erase(newend,nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),-1),nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),0),nlist->end());
    int numneighs = int(nlist->size());
    grains[i].numneighbors = numneighs;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist = nlist;
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
  for(int j = 1; j < numgrains; j++)
  {
//    if(grains[j].gotsizemerged != 1 && grains[j].gotcontainedmerged != 1)
    if(grains[j].gotcontainedmerged != 1)
    {
      vector<int>* neighborlist = grains[j].neighborlist;
      int size = 0;
      if (NULL != neighborlist ) { size = neighborlist->size(); }
      for(int k=0;k<size;k++)
      {
        int firstneighbor = neighborlist->at(k);
        int gotcontainedmerged = grains[firstneighbor].gotcontainedmerged;
//        int gotsizemerged = grains[firstneighbor].gotsizemerged;
//        if(gotcontainedmerged != 1 && gotsizemerged != 1) nlistgood.push_back(firstneighbor);
        if(gotcontainedmerged != 1) nlistgood.push_back(firstneighbor);
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
	  vector<int>* voxellist = grains[i].voxellist;
	  int vsize = voxellist->size();
	  int nucleus = grains[i].nucleus;
      grains[graincount].numvoxels = size;
      grains[graincount].numneighbors = numneighbors;
	  grains[graincount].nucleus = nucleus;
      if (nlist != NULL)
	  {
		grains[graincount].neighborlist = new std::vector<int>(numneighbors);
        grains[graincount].neighborlist->swap(*nlist);
      }
      if (voxellist != NULL)
	  {
		  grains[graincount].voxellist = new std::vector<int>(vsize);
		  grains[graincount].voxellist->swap(*voxellist);
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
void  ReconstructionFunc::homogenize_grains(double quat_symmcubic[24][5],double quat_symmhex[12][5])
{  
  int i, j, k, ii, jj, kk, p, pp, s;
  int sign1, sign2, sign3, sign4; // If the value is 1, it's positive; -1, negative; 0, 0.000...
  float qT[5]; // Temporary quaternions...
  float qN[5]; // Member quaternions inside a grain...
  float qC[5]; // Center quaternions inside that grain...
  float qSum[5]; // Sum of quaternions inside a cloud...
  int numsymm=0;
  int numVoxel;                 // number of voxels in the grain...
  int nucleus, nucl_site, nucl_spin;
  int index;
  int nid, nspin;
  int atSurf;
  int coin;
  int size_threshold;
  size_threshold = 100;          // Gonna pick inside pixel as a nucleus...
  float q4Temp;
  float max, min;
  int maxid, minid, sign;
  float dist2; // square distance between quaternions...
  double diff, sum, tmp, wmin;
  double g1ea1good, g1ea2good, g1ea3good, g2ea1good, g2ea2good, g2ea3good;
  double q1tot=0, q2tot=0, q3tot=0, q4tot=0;
  double n1,n2,n3;
  double averagemiso;
  grains[0].avgeuler1 = 12.566;
  grains[0].avgeuler2 = 12.566;
  grains[0].avgeuler3 = 12.566;
  grains[0].averagemisorientation = 0.0;
  if(crystruct == 1)
  {
	numsymm = 12;
	quat_symm = new double *[numsymm];
	for(i=0;i<numsymm;i++)
	{
		quat_symm[i] = new double [5];
		for(j=0;j<5;j++)
		{
			quat_symm[i][j] = quat_symmhex[i][j];
		}
	}
  }
  if(crystruct == 2)
  {
	numsymm = 24;
	quat_symm = new double *[numsymm];
	for(i=0;i<numsymm;i++)
	{
		quat_symm[i] = new double [5];
		for(j=0;j<5;j++)
		{
			quat_symm[i][j] = quat_symmcubic[i][j];
		}
	}
  }
/*  nucleus = -1;   
  for(int i = 0; i < numgrains; i++)
  {
	grains[i].nucleus = nucleus;
  }
  int gnum = 0;
  for(jj=0; jj<(xpoints*ypoints*zpoints); jj++)
  {
	gnum = voxels[jj].grainname;
	if(grains[gnum].nucleus != -1)
	{
		nucleus = grains[gnum].nucleus;
		if(voxels[jj].confidence > voxels[nucleus].confidence)
		{
			grains[gnum].nucleus = jj;
		}
	}
	if(grains[gnum].nucleus == -1)
	{
		grains[gnum].nucleus = jj;
	}
  }
*/  for(int i = 1; i < numgrains; i++)
  {
	averagemiso=0;
	numVoxel = 0;
    qSum[1] = 0.0;
    qSum[2] = 0.0;
    qSum[3] = 0.0;
    qSum[4] = 0.0;
	nucleus = grains[i].nucleus;
    qC[1] = voxels[nucleus].quat[1];
    qC[2] = voxels[nucleus].quat[2];
    qC[3] = voxels[nucleus].quat[3];
    qC[4] = voxels[nucleus].quat[4];
    // Selecting quaternions with largest absolute valued (4th component)...
    max = 0.0;
    maxid = -1;
    for(ii=0; ii<numsymm; ii++)
	{
      qT[4] = quat_symm[ii][4]*qC[4] - quat_symm[ii][1]*qC[1] - quat_symm[ii][2]*qC[2] - quat_symm[ii][3]*qC[3];
      q4Temp = (float)fabs((double)qT[4]);
      if(q4Temp>max)
	  {
		  max = q4Temp;
		  maxid = ii;
      }
	  else
	  {
		max = max;
		maxid = maxid;
      }
    }
    qT[1] = quat_symm[maxid][1]*qC[4] + quat_symm[maxid][4]*qC[1] - quat_symm[maxid][2]*qC[3] + quat_symm[maxid][3]*qC[2]; 
    qT[2] = quat_symm[maxid][2]*qC[4] + quat_symm[maxid][4]*qC[2] - quat_symm[maxid][3]*qC[1] + quat_symm[maxid][1]*qC[3]; 
    qT[3] = quat_symm[maxid][3]*qC[4] + quat_symm[maxid][4]*qC[3] - quat_symm[maxid][1]*qC[2] + quat_symm[maxid][2]*qC[1]; 
    qT[4] = quat_symm[maxid][4]*qC[4] - quat_symm[maxid][1]*qC[1] - quat_symm[maxid][2]*qC[2] - quat_symm[maxid][3]*qC[3];
    if(qT[4]<0.0)
	{         // Change the sign so that 4th component is positive...
      qT[1] = (-1.0)*qT[1];
      qT[2] = (-1.0)*qT[2];
      qT[3] = (-1.0)*qT[3];
      qT[4] = (-1.0)*qT[4];
    }
    // Now begins to calculate the avg. orientation...
	vector<int>* voxellist = grains[i].voxellist;
	int size = voxellist->size();
	for(jj=0; jj<size; jj++)
	{
		int index = voxellist->at(jj);
		if(voxels[index].confidence > minseedconfidence)
		{
		  numVoxel++;
		  for(pp=1; pp<5; pp++)
		  {
			qC[pp] = voxels[index].quat[pp];
		  }
		  min = 10.0;
		  minid = -1;
		  sign = 1;
		  for(kk=0; kk<numsymm; kk++)
		  {
			 qN[1] = quat_symm[kk][1]*qC[4] + quat_symm[kk][4]*qC[1] - quat_symm[kk][2]*qC[3] + quat_symm[kk][3]*qC[2]; 
			 qN[2] = quat_symm[kk][2]*qC[4] + quat_symm[kk][4]*qC[2] - quat_symm[kk][3]*qC[1] + quat_symm[kk][1]*qC[3]; 
			 qN[3] = quat_symm[kk][3]*qC[4] + quat_symm[kk][4]*qC[3] - quat_symm[kk][1]*qC[2] + quat_symm[kk][2]*qC[1]; 
			 qN[4] = quat_symm[kk][4]*qC[4] - quat_symm[kk][1]*qC[1] - quat_symm[kk][2]*qC[2] - quat_symm[kk][3]*qC[3];
			 dist2 = 2.0 * ( 1 -( qT[4]*qN[4] + qT[1]*qN[1] + qT[2]*qN[2] + qT[3]*qN[3] ) );
			 if(dist2<0.0)
			 {
			   dist2 = 0.0;
			 }
			 if(dist2<min)
			 {
			   min = dist2;
			   minid = kk;
			   sign = 1;
			 }
			 else
			 {
			   min = min;
			   minid = minid;
			   sign = sign;
			 }
			 dist2 = 2.0 * ( 1 + ( qT[4]*qN[4] + qT[1]*qN[1] + qT[2]*qN[2] + qT[3]*qN[3] ) );	
			 if(dist2<0.0)
			 {
			   dist2 = 0.0;
			 }
			 if(dist2<min)
			 {
			   min = dist2;
			   minid = kk;
			   sign = -1;
			 }
			 else
			 {
			   min = min;
			   minid = minid;
			   sign = sign;
			 }
		   }
		   qN[1] = quat_symm[minid][1]*qC[4] + quat_symm[minid][4]*qC[1] - quat_symm[minid][2]*qC[3] + quat_symm[minid][3]*qC[2]; 
		   qN[2] = quat_symm[minid][2]*qC[4] + quat_symm[minid][4]*qC[2] - quat_symm[minid][3]*qC[1] + quat_symm[minid][1]*qC[3]; 
		   qN[3] = quat_symm[minid][3]*qC[4] + quat_symm[minid][4]*qC[3] - quat_symm[minid][1]*qC[2] + quat_symm[minid][2]*qC[1]; 
		   qN[4] = quat_symm[minid][4]*qC[4] - quat_symm[minid][1]*qC[1] - quat_symm[minid][2]*qC[2] - quat_symm[minid][3]*qC[3];
		   if(sign==-1)
		   {
			 qN[1] = (-1.0)*qN[1];
			 qN[2] = (-1.0)*qN[2];
			 qN[3] = (-1.0)*qN[3];
			 qN[4] = (-1.0)*qN[4];
		   }
		   for(p=1; p<5; p++)
		   {
			 qSum[p] = qSum[p] + qN[p];
		   }
		}
    }
	for(int s=1; s<5; s++)
	{
		grains[i].avg_quat[s] = qSum[s]/(float)numVoxel;
    }
	double q1tot = grains[i].avg_quat[1];
	double q2tot = grains[i].avg_quat[2];
	double q3tot = grains[i].avg_quat[3];
	double q4tot = grains[i].avg_quat[4];
	double normalizer = (q1tot*q1tot)+(q2tot*q2tot)+(q3tot*q3tot)+(q4tot*q4tot);
	normalizer = pow(normalizer,0.5);
	q1tot = q1tot/normalizer;
	q2tot = q2tot/normalizer;
	q3tot = q3tot/normalizer;
	q4tot = q4tot/normalizer;
	diff=atan2(q2tot,q1tot);
	sum=atan2(q3tot,q4tot);
	double ea1good=(diff+sum);
	double ea3good=(sum-diff);
	tmp=(q3tot*q3tot)+(q4tot*q4tot);
	tmp = pow(tmp,0.5);
	if(tmp > 1.0) tmp=1.0;
	double ea2good=2*acos(tmp);
    grains[i].avgeuler1 = ea1good;
    grains[i].avgeuler2 = ea2good;
    grains[i].avgeuler3 = ea3good;
    double avgmiso = 0;
    double totalcount = 0;
	for(jj=0; jj<size; jj++)
	{
		int index = voxellist->at(jj);
		if(voxels[index].confidence > minseedconfidence)
		{
	       double angcur = 1000000;
	       double g1ea1 = grains[i].avgeuler1;
	       double g1ea2 = grains[i].avgeuler2;
	       double g1ea3 = grains[i].avgeuler3;
	       double g2ea1 = voxels[index].euler1;
	       double g2ea2 = voxels[index].euler2;
	       double g2ea3 = voxels[index].euler3;
	       if(crystruct == 1) wmin = getmisoquathexagonal(quat_symmhex,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
		   if(crystruct == 2) wmin = getmisoquatcubic(misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
	       voxels[index].misorientation = wmin;
		   avgmiso = avgmiso + wmin;
		   totalcount++;
      }
    }
    avgmiso = avgmiso/totalcount;
	grains[i].averagemisorientation = avgmiso;
  }
}
int  ReconstructionFunc::load_data(string readname)
{
  ifstream inputFile;
  inputFile.open(readname.c_str());
  int gnum;
  string dummy;
  numgrains = 0;
  int vnum;
  int vListSize = 1000;
  for(int i = 0; i < 12; i++)
  {
	getline(inputFile,dummy,'\n');
  }
  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    inputFile >> gnum;
	if(gnum == 7520)
	{
		int stop = 0;
	}
    voxels[i].grainname = gnum;
	vnum = grains[gnum].numvoxels;
    if (grains[gnum].voxellist == NULL)
    {
      grains[gnum].voxellist = new std::vector<int>(vListSize,-1);
    }
	vector<int>* vlist = grains[gnum].voxellist;
	if (vnum >= (vlist->size()/2))
	{
	   vlist->resize(vlist->size()+vListSize,-1);
	}
	vlist->at(vnum) = i;
	vnum++;
	grains[gnum].numvoxels = vnum;
	if(gnum > numgrains) numgrains = gnum;
  }
  numgrains = numgrains+1;
  for(int i=0;i<numgrains;i++)
  {
	if(grains[i].numvoxels > 0)
	{
		vector<int>* vlist = grains[i].voxellist;
		vlist->erase(std::remove(vlist->begin(),vlist->end(),-1),vlist->end());
		int numvoxels = int(vlist->size());
		grains[i].numvoxels = numvoxels;
		grains[i].voxellist = new std::vector<int>(numvoxels);
		grains[i].voxellist = vlist;
	}
  }
  inputFile.close();
return numgrains;
}
void  ReconstructionFunc::merge_twins (double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
  int twinmerged = 1;
  int beenset = 1;
//  int mergedcount = 0;
  double angcur = 180;
  vector<int> twinlist;
  double w;
  double n1,n2,n3;
  for(int i = 1; i < numgrains; i++)
  {
//    if(grains[i].gotsizemerged != 1 && grains[i].gotcontainedmerged != 1 && grains[i].twinnewnumberbeenset != 1)
    if(grains[i].twinnewnumberbeenset != 1)
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
		    if(crystruct == 1) w = getmisoquathexagonal(quat_symmhex,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
			if(crystruct == 2) w = getmisoquatcubic(misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
            double tanhalfang = tan(w/2.0);
            double rodvect1 = tanhalfang*n1;
            double rodvect2 = tanhalfang*n2;
            double rodvect3 = tanhalfang*n3;
            double vecttol = 0.03;
            double rodvectdiff11 = fabs(fabs(rodvect1)-(1.0/3.0));
            double rodvectdiff12 = fabs(fabs(rodvect2)-(1.0/3.0));
            double rodvectdiff13 = fabs(fabs(rodvect3)-(1.0/3.0));
            double rodvectdiff21 = fabs(fabs(rodvect1)-0.2);
            double rodvectdiff22 = fabs(fabs(rodvect2)-0.2);
            double rodvectdiff23 = fabs(fabs(rodvect3)-0.2);
            double rodvectdiff31 = fabs(fabs(rodvect1)-0.25);
            double rodvectdiff32 = fabs(fabs(rodvect2)-0.25);
            double rodvectdiff33 = fabs(fabs(rodvect3)-0.25);
            if(rodvectdiff11 < vecttol && rodvectdiff12 < vecttol && rodvectdiff13 < vecttol) twin = 1;
            if(rodvectdiff11 < vecttol && fabs(rodvect2) < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if(rodvectdiff12 < vecttol && fabs(rodvect1) < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if(rodvectdiff13 < vecttol && fabs(rodvect1) < vecttol && fabs(rodvect2) < vecttol) twin = 1;
            if(rodvectdiff11 < vecttol && rodvectdiff12 < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if(rodvectdiff11 < vecttol && rodvectdiff13 < vecttol && fabs(rodvect2) < vecttol) twin = 1;
            if(rodvectdiff12 < vecttol && rodvectdiff13 < vecttol && fabs(rodvect1) < vecttol) twin = 1;
            if(rodvectdiff21 < vecttol && rodvectdiff22 < vecttol && rodvectdiff23 < vecttol) twin = 1;
            if(rodvectdiff31 < vecttol && rodvectdiff32 < vecttol && fabs(rodvect3) < vecttol) twin = 1;
            if(rodvectdiff31 < vecttol && rodvectdiff33 < vecttol && fabs(rodvect2) < vecttol) twin = 1;
            if(rodvectdiff32 < vecttol && rodvectdiff33 < vecttol && fabs(rodvect1) < vecttol) twin = 1;
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

void  ReconstructionFunc::merge_colonies (double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
  int colonymerged = 1;
  int beenset = 1;
//  int mergedcount = 0;
  double angcur = 180;
  vector<int> colonylist;
  double w;
  double n1,n2,n3;
  for(int i = 1; i < numgrains; i++)
  {
    if(grains[i].colonynewnumberbeenset != 1)
    {
      colonylist.push_back(i);
      int csize = int(colonylist.size());
      for(int m=0;m<csize;m++)
      {
        csize = int(colonylist.size());
        int firstgrain = colonylist[m];
        vector<int>* nlist = grains[firstgrain].neighborlist;
        int size = int(nlist->size());
        for(int l=0;l<size;l++)
        {
          angcur = 180;
          int colony = 0;
          int neigh = nlist->at(l);
          if(neigh != i && grains[neigh].colonynewnumberbeenset != 1)
          {
            double g1ea1 = grains[firstgrain].avgeuler1;
            double g1ea2 = grains[firstgrain].avgeuler2;
            double g1ea3 = grains[firstgrain].avgeuler3;
            double g2ea1 = grains[neigh].avgeuler1;
            double g2ea2 = grains[neigh].avgeuler2;
            double g2ea3 = grains[neigh].avgeuler3;
		    if(crystruct == 1) w = getmisoquathexagonal(quat_symmhex,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
			if(crystruct == 2) w = getmisoquatcubic(misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
            double tanhalfang = tan(w/2.0);
            double rodvect1 = tanhalfang*n1;
            double rodvect2 = tanhalfang*n2;
            double rodvect3 = tanhalfang*n3;
            double vecttol = 0.03;
            double rodvectdiff11 = fabs(fabs(rodvect1)-(1.0/3.0));
            double rodvectdiff12 = fabs(fabs(rodvect2)-(1.0/3.0));
            double rodvectdiff13 = fabs(fabs(rodvect3)-(1.0/3.0));
            double rodvectdiff21 = fabs(fabs(rodvect1)-0.2);
            double rodvectdiff22 = fabs(fabs(rodvect2)-0.2);
            double rodvectdiff23 = fabs(fabs(rodvect3)-0.2);
            double rodvectdiff31 = fabs(fabs(rodvect1)-0.25);
            double rodvectdiff32 = fabs(fabs(rodvect2)-0.25);
            double rodvectdiff33 = fabs(fabs(rodvect3)-0.25);
            double rodvectdiff41 = fabs(fabs(rodvect1)-(1.0/3.0));
            double rodvectdiff42 = fabs(fabs(rodvect2)-(1.0/3.0));
            double rodvectdiff43 = fabs(fabs(rodvect3)-(1.0/3.0));
            double rodvectdiff51 = fabs(fabs(rodvect1)-0.2);
            double rodvectdiff52 = fabs(fabs(rodvect2)-0.2);
            double rodvectdiff53 = fabs(fabs(rodvect3)-0.2);
            double rodvectdiff61 = fabs(fabs(rodvect1)-0.25);
            double rodvectdiff62 = fabs(fabs(rodvect2)-0.25);
            double rodvectdiff63 = fabs(fabs(rodvect3)-0.25);
            if(rodvectdiff23 < vecttol && rodvectdiff33 < vecttol && fabs(rodvect1) < vecttol) colony = 1;
            if(w < angcur)
            {
              angcur = w;
            }
            if(colony == 1)
            {
              grains[neigh].gotcolonymerged = colonymerged;
              grains[neigh].colonynewnumberbeenset = beenset;
              grains[neigh].colonynewnumber = i;
              colonylist.push_back(neigh);
            }
          }
        }
      }
    }
    colonylist.clear();
  }
  for(int k = 0; k < (xpoints*ypoints*zpoints); k++)
  {
    int grainname = voxels[k].grainname;
    int gotcolonymerged = grains[grainname].gotcolonymerged;
    if(gotcolonymerged == 1)
    {
      int colonynewnumber = grains[grainname].colonynewnumber;
      voxels[k].grainname = colonynewnumber;
    }
  }
}

void  ReconstructionFunc::characterize_twins()
{
  for(int i=0;i<numgrains;i++)
  {

  }
}

void  ReconstructionFunc::characterize_colonies()
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
      if (NULL != nlist) 
	  {
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
  int grain;
  int grain1;
  int grain2;
  int grain3;
  int grain4;
  int grain5;
  int grain6;
  int nnum;
  int nListSize = 1000;
  std::vector<int> nlist(nListSize, -1);
  for(int i=0;i<numgrains;i++)
  {
    int numneighs = int(nlist.size());
	grains[i].numneighbors = 0;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist->swap(nlist);
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
    int onsurf = 0;
    int grain = voxels[j].grainname;
	x = j%xpoints;
	y = (j/xpoints)%ypoints;
	z = j/(xpoints*ypoints);
	if(x > 0)
	{
	  grain1 = voxels[j-1].grainname;
	  if(grain1 != grain && grain1!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain1;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(x < xpoints-1)
	{
	  grain2 = voxels[j+1].grainname;
	  if(grain2 != grain && grain2!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain2;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(y > 0)
	{
	  grain3 = voxels[j-(xpoints)].grainname;
	  if(grain3 != grain && grain3!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain3;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(y < ypoints-1)
	{
	  grain4 = voxels[j+(xpoints)].grainname;
	  if(grain4 != grain && grain4!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain4;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(z > 0)
	{
	  grain5 = voxels[j-(xpoints*ypoints)].grainname;
	  if(grain5 != grain && grain5!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain5;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
	if(z < zpoints-1)
	{
	  grain6 = voxels[j+(xpoints*ypoints)].grainname;
	  if(grain6 != grain && grain6!= 0)
	  {
		  nnum = grains[grain].numneighbors;
		  vector<int>* nlist = grains[grain].neighborlist;
          if (nnum >= (0.9*nlist->size()))
          {
             nlist->resize(nnum + nListSize);
          }
		  nlist->at(nnum) = grain6;
		  nnum++;
		  grains[grain].numneighbors = nnum;
	  }
	}
    voxels[j].surfacevoxel = onsurf;
  }
  for(int i=1;i<numgrains;i++)
  {
	vector<int>* nlist = grains[i].neighborlist;
	vector<int>::iterator newend;
	sort(nlist->begin(),nlist->end());
    newend = unique(nlist->begin(),nlist->end());
    nlist->erase(newend,nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),-1),nlist->end());
	nlist->erase(std::remove(nlist->begin(),nlist->end(),0),nlist->end());
    int numneighs = int(nlist->size());
    grains[i].numneighbors = numneighs;
    grains[i].neighborlist = new std::vector<int>(numneighs);
    grains[i].neighborlist = nlist;
  }
}
void  ReconstructionFunc::find_centroids()
{
//  int count = 0;
  int onedge = 0;
  maxdiameter=0;
  mindiameter=100000;
  double x;
  double y;
  double z;
  graincenters = new double *[numgrains];
  for(int i = 0; i < numgrains; i++)
  {
    graincenters[i] = new double [5];
	for(int j=0;j<5;j++)
	{
		graincenters[i][j]=0;
	}
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	    onedge = 0;
        int gnum = voxels[j].grainname;
        graincenters[gnum][0]++;
        x = find_xcoord(j);
        y = find_ycoord(j);
        z = find_zcoord(j);
	    int col = j%xpoints;
	    int row = (j/xpoints)%ypoints;
	    int plane = j/(xpoints*ypoints);
        if(col <= 0) onedge = 1;
        if(col >= xpoints-1) onedge = 1;
        if(row <= 0) onedge = 1;
        if(row >= ypoints-1) onedge = 1;
//        if(plane <= 0) onedge = 1;
//        if(plane >= zpoints-1) onedge = 1;
        graincenters[gnum][1] = graincenters[gnum][1] + x;
        graincenters[gnum][2] = graincenters[gnum][2] + y;
        graincenters[gnum][3] = graincenters[gnum][3] + z;
		if(onedge == 1) graincenters[gnum][4] = 1;
  }
  for(int i=1;i<numgrains;i++)
  {
    graincenters[i][1] = graincenters[i][1]/graincenters[i][0];
    graincenters[i][2] = graincenters[i][2]/graincenters[i][0];
    graincenters[i][3] = graincenters[i][3]/graincenters[i][0];
    grains[i].centroidx = graincenters[i][1];
    grains[i].centroidy = graincenters[i][2];
    grains[i].centroidz = graincenters[i][3];
    grains[i].numvoxels = graincenters[i][0];
	grains[i].volume = (graincenters[i][0]*resx*resy*resz);
    grains[i].surfacegrain = graincenters[i][4];
	double diametercubed = (0.75*grains[i].volume)/m_pi;
	int diameter = int(pow(diametercubed,0.3333333333));
	int col = grains[i].centroidx/resx;
	int row = grains[i].centroidy/resy;
	int plane = grains[i].centroidz/resz;
	int nucleus = (plane*xpoints*ypoints)+(row*xpoints)+col;
	if(voxels[nucleus].grainname == i) grains[i].nucleus = nucleus;
	if(diameter > maxdiameter)
	{
		maxdiameter = diameter;
	}
	if(diameter < mindiameter) mindiameter = diameter;
  }
}
void  ReconstructionFunc::find_moments ()
{
//  int count = 0;
  double u200=0;
  double u020=0;
  double u002=0;
  double u110=0;
  double u011=0;
  double u101=0;
  grainmoments = new double *[numgrains];
  for(int i = 0; i < numgrains; i++)
  {
	grainmoments[i] = new double [6];
	for(int j=0;j<6;j++)
	{
		grainmoments[i][j] = 0;
	}
  }
  for(int j = 0; j < (xpoints*ypoints*zpoints); j++)
  {
	  u200=0;
	  u020=0;
	  u002=0;
	  u110=0;	
	  u011=0;
	  u101=0;
      int gnum = voxels[j].grainname;
      double x = find_xcoord(j);
      double y = find_ycoord(j);
      double z = find_zcoord(j);
      double x1 = x+(resx/2);
      double x2 = x-(resx/2);
      double y1 = y+(resy/2);
      double y2 = y-(resy/2);
      double z1 = z+(resz/2);
      double z2 = z-(resz/2);
      double xdist1 = (x1-graincenters[gnum][1]);
      double ydist1 = (y1-graincenters[gnum][2]);
      double zdist1 = (z1-graincenters[gnum][3]);
      double xdist2 = (x1-graincenters[gnum][1]);
      double ydist2 = (y1-graincenters[gnum][2]);
      double zdist2 = (z2-graincenters[gnum][3]);
      double xdist3 = (x1-graincenters[gnum][1]);
      double ydist3 = (y2-graincenters[gnum][2]);
      double zdist3 = (z1-graincenters[gnum][3]);
      double xdist4 = (x1-graincenters[gnum][1]);
      double ydist4 = (y2-graincenters[gnum][2]);
      double zdist4 = (z2-graincenters[gnum][3]);
      double xdist5 = (x2-graincenters[gnum][1]);
      double ydist5 = (y1-graincenters[gnum][2]);
      double zdist5 = (z1-graincenters[gnum][3]);
      double xdist6 = (x2-graincenters[gnum][1]);
      double ydist6 = (y1-graincenters[gnum][2]);
      double zdist6 = (z2-graincenters[gnum][3]);
      double xdist7 = (x2-graincenters[gnum][1]);
      double ydist7 = (y2-graincenters[gnum][2]);
      double zdist7 = (z1-graincenters[gnum][3]);
      double xdist8 = (x2-graincenters[gnum][1]);
      double ydist8 = (y2-graincenters[gnum][2]);
      double zdist8 = (z2-graincenters[gnum][3]);
      u200 = u200 + ((ydist1)*(ydist1))+((zdist1)*(zdist1)) + ((ydist2)*(ydist2))+((zdist2)*(zdist2)) + ((ydist3)*(ydist3))+((zdist3)*(zdist3)) + ((ydist4)*(ydist4))+((zdist4)*(zdist4)) + ((ydist5)*(ydist5))+((zdist5)*(zdist5)) + ((ydist6)*(ydist6))+((zdist6)*(zdist6)) + ((ydist7)*(ydist7))+((zdist7)*(zdist7)) + ((ydist8)*(ydist8))+((zdist8)*(zdist8));
      u020 = u020 + ((xdist1)*(xdist1))+((zdist1)*(zdist1)) + ((xdist2)*(xdist2))+((zdist2)*(zdist2)) + ((xdist3)*(xdist3))+((zdist3)*(zdist3)) + ((xdist4)*(xdist4))+((zdist4)*(zdist4)) + ((xdist5)*(xdist5))+((zdist5)*(zdist5)) + ((xdist6)*(xdist6))+((zdist6)*(zdist6)) + ((xdist7)*(xdist7))+((zdist7)*(zdist7)) + ((xdist8)*(xdist8))+((zdist8)*(zdist8));
      u002 = u002 + ((xdist1)*(xdist1))+((ydist1)*(ydist1)) + ((xdist2)*(xdist2))+((ydist2)*(ydist2)) + ((xdist3)*(xdist3))+((ydist3)*(ydist3)) + ((xdist4)*(xdist4))+((ydist4)*(ydist4)) + ((xdist5)*(xdist5))+((ydist5)*(ydist5)) + ((xdist6)*(xdist6))+((ydist6)*(ydist6)) + ((xdist7)*(xdist7))+((ydist7)*(ydist7)) + ((xdist8)*(xdist8))+((ydist8)*(ydist8));
      u110 = u110 + ((xdist1)*(ydist1)) + ((xdist2)*(ydist2)) + ((xdist3)*(ydist3)) + ((xdist4)*(ydist4)) + ((xdist5)*(ydist5)) + ((xdist6)*(ydist6)) + ((xdist7)*(ydist7)) + ((xdist8)*(ydist8));
      u011 = u011 + ((ydist1)*(zdist1)) + ((ydist2)*(zdist2)) + ((ydist3)*(zdist3)) + ((ydist4)*(zdist4)) + ((ydist5)*(zdist5)) + ((ydist6)*(zdist6)) + ((ydist7)*(zdist7)) + ((ydist8)*(zdist8));
      u101 = u101 + ((xdist1)*(zdist1)) + ((xdist2)*(zdist2)) + ((xdist3)*(zdist3)) + ((xdist4)*(zdist4)) + ((xdist5)*(zdist5)) + ((xdist6)*(zdist6)) + ((xdist7)*(zdist7)) + ((xdist8)*(zdist8));
	  grainmoments[gnum][0] = grainmoments[gnum][0] + u200;
	  grainmoments[gnum][1] = grainmoments[gnum][1] + u020;
	  grainmoments[gnum][2] = grainmoments[gnum][2] + u002;
	  grainmoments[gnum][3] = grainmoments[gnum][3] + u110;
	  grainmoments[gnum][4] = grainmoments[gnum][4] + u011;
	  grainmoments[gnum][5] = grainmoments[gnum][5] + u101;
  }
  for(int i=1;i<numgrains;i++)
  {
	grainmoments[i][0] = grainmoments[i][0]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][1] = grainmoments[i][1]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][2] = grainmoments[i][2]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][3] = grainmoments[i][3]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][4] = grainmoments[i][4]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	grainmoments[i][5] = grainmoments[i][5]*(resx/2.0)*(resy/2.0)*(resz/2.0);
	double o3 = (grainmoments[i][0]*grainmoments[i][1]*grainmoments[i][2])+(2.0*grainmoments[i][3]*grainmoments[i][5]*grainmoments[i][4])-(grainmoments[i][0]*grainmoments[i][4]*grainmoments[i][4])-(grainmoments[i][1]*grainmoments[i][5]*grainmoments[i][5])-(grainmoments[i][2]*grainmoments[i][3]*grainmoments[i][3]);
	double vol5 = grains[i].volume;
	vol5 = pow(vol5,5);
	double omega3 = vol5/o3;
	grains[i].Ixx = grainmoments[i][0];
	grains[i].Iyy = grainmoments[i][1];
	grains[i].Izz = grainmoments[i][2];
	grains[i].Ixy = -grainmoments[i][3];
	grains[i].Iyz = -grainmoments[i][4];
	grains[i].Ixz = -grainmoments[i][5];
	grains[i].omega3 = omega3;
  }
}
void  ReconstructionFunc::find_axes ()
{
  for (int i = 1; i < numgrains; i++)
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
	grains[i].radius1 = r1;
	grains[i].radius2 = r2;
	grains[i].radius3 = r3;
  }
}
void  ReconstructionFunc::find_vectors ()
{
  for(int i = 1; i < numgrains; i++)
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
	  double radius1 = grains[i].radius1;
	  double radius2 = grains[i].radius2;
	  double radius3 = grains[i].radius3;
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
      e[0][0] = radius1;
      e[1][0] = radius2;
      e[2][0] = radius3;
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
	  double norm1 = pow(((n1x*n1x)+(n1y*n1y)+(n1z*n1z)),0.5);
	  double norm2 = pow(((n2x*n2x)+(n2y*n2y)+(n2z*n2z)),0.5);
	  double norm3 = pow(((n3x*n3x)+(n3y*n3y)+(n3z*n3z)),0.5);
      grains[i].axis1x = n1x/norm1;
      grains[i].axis1y = n1y/norm1;
      grains[i].axis1z = n1z/norm1;
      grains[i].axis2x = n2x/norm2;
      grains[i].axis2y = n2y/norm2;
      grains[i].axis2z = n2z/norm2;
      grains[i].axis3x = n3x/norm3;
      grains[i].axis3y = n3y/norm3;
      grains[i].axis3z = n3z/norm3;
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
	  double norm1 = pow(((n1x*n1x)+(n1y*n1y)+(n1z*n1z)),0.5);
	  double norm2 = pow(((n2x*n2x)+(n2y*n2y)+(n2z*n2z)),0.5);
	  double norm3 = pow(((n3x*n3x)+(n3y*n3y)+(n3z*n3z)),0.5);
      grains[i].axis1x = n1x/norm1;
      grains[i].axis1y = n1y/norm1;
      grains[i].axis1z = n1z/norm1;
      grains[i].axis2x = n2x/norm2;
      grains[i].axis2y = n2y/norm2;
      grains[i].axis2z = n2z/norm2;
      grains[i].axis3x = n3x/norm3;
      grains[i].axis3y = n3y/norm3;
      grains[i].axis3z = n3z/norm3;
    }
  }
}
void  ReconstructionFunc::find_eulerodf ()
{
	totalvol = 0;
	double Oc[3][3];
	double Os[3][3];
	double ga[3][3];
	double m1[3][3];
	double m2[3][3];
	for(int i=1;i<numgrains;i++)
	{
		if(grains[i].surfacegrain == 0)
		{
			double vol = grains[i].volume;
			double ea1good = grains[i].avgeuler1;
			double ea2good = grains[i].avgeuler2;
			double ea3good = grains[i].avgeuler3;
			for(int k = 0; k < 24; k++)
			{
				if (k == 0)
				{
				  Oc[0][0] = 1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
				  Oc[1][0] = 0.0; Oc[1][1] = 1.0; Oc[1][2] = 0.0;
				  Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
				}
				else if (k == 1)
				{
				  Oc[0][0] = 1.0; Oc[0][1] = 0.0; Oc[0][2] =  0.0;
				  Oc[1][0] = 0.0; Oc[1][1] = 0.0; Oc[1][2] = -1.0;
				  Oc[2][0] = 0.0; Oc[2][1] = 1.0; Oc[2][2] =  0.0;
				}
				else if (k == 2)
				{
				  Oc[0][0] = 1.0; Oc[0][1] =  0.0; Oc[0][2] =  0.0;
				  Oc[1][0] = 0.0; Oc[1][1] = -1.0; Oc[1][2] =  0.0;
				  Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
				}
				else if (k == 3)
				{
				  Oc[0][0] = 1.0; Oc[0][1] =  0.0; Oc[0][2] = 0.0;
				  Oc[1][0] = 0.0; Oc[1][1] =  0.0; Oc[1][2] = 1.0;
				  Oc[2][0] = 0.0; Oc[2][1] = -1.0; Oc[2][2] = 0.0;
				}
				else if (k == 4)
				{
				  Oc[0][0] = 0.0; Oc[0][1] = 0.0; Oc[0][2] = -1.0;
				  Oc[1][0] = 0.0; Oc[1][1] = 1.0; Oc[1][2] =  0.0;
				  Oc[2][0] = 1.0; Oc[2][1] = 0.0; Oc[2][2] =  0.0;
				}
				else if (k == 5)
				{
				  Oc[0][0] =  0.0; Oc[0][1] = 0.0; Oc[0][2] = 1.0;
				  Oc[1][0] =  0.0; Oc[1][1] = 1.0; Oc[1][2] = 0.0;
				  Oc[2][0] = -1.0; Oc[2][1] = 0.0; Oc[2][2] = 0.0;
				}
				else if (k == 6)
				{
				  Oc[0][0] = -1.0; Oc[0][1] = 0.0; Oc[0][2] =  0.0;
				  Oc[1][0] =  0.0; Oc[1][1] = 1.0; Oc[1][2] =  0.0;
				  Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
				}
				else if (k == 7)
				{
				  Oc[0][0] = -1.0; Oc[0][1] =  0.0; Oc[0][2] = 0.0;
				  Oc[1][0] =  0.0; Oc[1][1] = -1.0; Oc[1][2] = 0.0;
				  Oc[2][0] =  0.0; Oc[2][1] =  0.0; Oc[2][2] = 1.0;
				}
				else if (k == 8)
				{
				  Oc[0][0] =  0.0; Oc[0][1] = 1.0; Oc[0][2] = 0.0;
				  Oc[1][0] = -1.0; Oc[1][1] = 0.0; Oc[1][2] = 0.0;
				  Oc[2][0] =  0.0; Oc[2][1] = 0.0; Oc[2][2] = 1.0;
				}
				else if (k == 9)
				{
				  Oc[0][0] = 0.0; Oc[0][1] = -1.0; Oc[0][2] = 0.0;
				  Oc[1][0] = 1.0; Oc[1][1] =  0.0; Oc[1][2] = 0.0;
				  Oc[2][0] = 0.0; Oc[2][1] =  0.0; Oc[2][2] = 1.0;
				}
				else if (k == 10)
				{
				  Oc[0][0] =  0.0; Oc[0][1] = -1.0; Oc[0][2] = 0.0;
				  Oc[1][0] =  0.0; Oc[1][1] =  0.0; Oc[1][2] = 1.0;
				  Oc[2][0] = -1.0; Oc[2][1] =  0.0; Oc[2][2] = 0.0;
				}
				else if (k == 11)
				{
				  Oc[0][0] =  0.0; Oc[0][1] =  0.0; Oc[0][2] = 1.0;
				  Oc[1][0] = -1.0; Oc[1][1] =  0.0; Oc[1][2] = 0.0;
				  Oc[2][0] =  0.0; Oc[2][1] = -1.0; Oc[2][2] = 0.0;
				}
				else if (k == 12)
				{
				  Oc[0][0] = 0.0; Oc[0][1] = -1.0; Oc[0][2] =  0.0;
				  Oc[1][0] = 0.0; Oc[1][1] =  0.0; Oc[1][2] = -1.0;
				  Oc[2][0] = 1.0; Oc[2][1] =  0.0; Oc[2][2] =  0.0;
				}
				else if (k == 13)
				{
				  Oc[0][0] = 0.0; Oc[0][1] =  0.0; Oc[0][2] = -1.0;
				  Oc[1][0] = 1.0; Oc[1][1] =  0.0; Oc[1][2] =  0.0;
				  Oc[2][0] = 0.0; Oc[2][1] = -1.0; Oc[2][2] =  0.0;
				}
				else if (k == 14)
				{
				  Oc[0][0] =  0.0; Oc[0][1] = 1.0; Oc[0][2] =  0.0;
				  Oc[1][0] =  0.0; Oc[1][1] = 0.0; Oc[1][2] = -1.0;
				  Oc[2][0] = -1.0; Oc[2][1] = 0.0; Oc[2][2] =  0.0;
				}
				else if (k == 15)
				{
				  Oc[0][0] =  0.0; Oc[0][1] = 0.0; Oc[0][2] = -1.0;
				  Oc[1][0] = -1.0; Oc[1][1] = 0.0; Oc[1][2] =  0.0;
				  Oc[2][0] =  0.0; Oc[2][1] = 1.0; Oc[2][2] =  0.0;
				}
				else if (k == 16)
				{
				  Oc[0][0] = 0.0; Oc[0][1] = 1.0; Oc[0][2] = 0.0;
				  Oc[1][0] = 0.0; Oc[1][1] = 0.0; Oc[1][2] = 1.0;
				  Oc[2][0] = 1.0; Oc[2][1] = 0.0; Oc[2][2] = 0.0;
				}
				else if (k == 17)
				{
				  Oc[0][0] = 0.0; Oc[0][1] = 0.0; Oc[0][2] = 1.0;
				  Oc[1][0] = 1.0; Oc[1][1] = 0.0; Oc[1][2] = 0.0;
				  Oc[2][0] = 0.0; Oc[2][1] = 1.0; Oc[2][2] = 0.0;
				}
				else if (k == 18)
				{
				  Oc[0][0] = 0.0; Oc[0][1] = 1.0; Oc[0][2] =  0.0;
				  Oc[1][0] = 1.0; Oc[1][1] = 0.0; Oc[1][2] =  0.0;
				  Oc[2][0] = 0.0; Oc[2][1] = 0.0; Oc[2][2] = -1.0;
				}
				else if (k == 19)
				{
				  Oc[0][0] = -1.0; Oc[0][1] = 0.0; Oc[0][2] = 0.0;
				  Oc[1][0] =  0.0; Oc[1][1] = 0.0; Oc[1][2] = 1.0;
				  Oc[2][0] =  0.0; Oc[2][1] = 1.0; Oc[2][2] = 0.0;
				}
				else if (k == 20)
				{
				  Oc[0][0] = 0.0; Oc[0][1] =  0.0; Oc[0][2] = 1.0;
				  Oc[1][0] = 0.0; Oc[1][1] = -1.0; Oc[1][2] = 0.0;
				  Oc[2][0] = 1.0; Oc[2][1] =  0.0; Oc[2][2] = 0.0;
				}
				else if (k == 21)
				{
				  Oc[0][0] = -1.0; Oc[0][1] =  0.0; Oc[0][2] =  0.0;
				  Oc[1][0] =  0.0; Oc[1][1] =  0.0; Oc[1][2] = -1.0;
				  Oc[2][0] =  0.0; Oc[2][1] = -1.0; Oc[2][2] =  0.0;
				}
				else if (k == 22)
				{
				  Oc[0][0] =  0.0; Oc[0][1] =  0.0; Oc[0][2] = -1.0;
				  Oc[1][0] =  0.0; Oc[1][1] = -1.0; Oc[1][2] =  0.0;
				  Oc[2][0] = -1.0; Oc[2][1] =  0.0; Oc[2][2] =  0.0;
				}
				else if (k == 23)
				{
				  Oc[0][0] =  0.0; Oc[0][1] = -1.0; Oc[0][2] =  0.0;
				  Oc[1][0] = -1.0; Oc[1][1] =  0.0; Oc[1][2] =  0.0;
				  Oc[2][0] =  0.0; Oc[2][1] =  0.0; Oc[2][2] = -1.0;
				}
				ga[0][0] = cos(ea1good)*cos(ea3good)-sin(ea1good)*sin(ea3good)*cos(ea2good);
				ga[0][1] = sin(ea1good)*cos(ea3good)+cos(ea1good)*sin(ea3good)*cos(ea2good);
				ga[0][2] = sin(ea3good)*sin(ea2good);
				ga[1][0] = -cos(ea1good)*sin(ea3good)-sin(ea1good)*cos(ea3good)*cos(ea2good);
				ga[1][1] = -sin(ea1good)*sin(ea3good)+cos(ea1good)*cos(ea3good)*cos(ea2good);
				ga[1][2] =  cos(ea3good)*sin(ea2good);
				ga[2][0] =  sin(ea1good)*sin(ea2good);
				ga[2][1] = -cos(ea1good)*sin(ea2good);
				ga[2][2] =  cos(ea2good);
				m1[0][0] = Oc[0][0]*ga[0][0] + Oc[0][1]*ga[1][0] + Oc[0][2]*ga[2][0];
				m1[0][1] = Oc[0][0]*ga[0][1] + Oc[0][1]*ga[1][1] + Oc[0][2]*ga[2][1];
				m1[0][2] = Oc[0][0]*ga[0][2] + Oc[0][1]*ga[1][2] + Oc[0][2]*ga[2][2];
				m1[1][0] = Oc[1][0]*ga[0][0] + Oc[1][1]*ga[1][0] + Oc[1][2]*ga[2][0];
				m1[1][1] = Oc[1][0]*ga[0][1] + Oc[1][1]*ga[1][1] + Oc[1][2]*ga[2][1];
				m1[1][2] = Oc[1][0]*ga[0][2] + Oc[1][1]*ga[1][2] + Oc[1][2]*ga[2][2];
				m1[2][0] = Oc[2][0]*ga[0][0] + Oc[2][1]*ga[1][0] + Oc[2][2]*ga[2][0];
				m1[2][1] = Oc[2][0]*ga[0][1] + Oc[2][1]*ga[1][1] + Oc[2][2]*ga[2][1];
				m1[2][2] = Oc[2][0]*ga[0][2] + Oc[2][1]*ga[1][2] + Oc[2][2]*ga[2][2];
				for(int l=0;l<4;l++)
				{
					if (l == 0)
					{
					  Os[0][0] = 1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
					  Os[1][0] = 0.0; Os[1][1] = 1.0; Os[1][2] = 0.0;
					  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = 1.0;
					}
					else if (l == 1)
					{
					  Os[0][0] = -1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
					  Os[1][0] = 0.0; Os[1][1] = 1.0; Os[1][2] = 0.0;
					  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = -1.0;
					}
					else if (l == 2)
					{
					  Os[0][0] = 1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
					  Os[1][0] = 0.0; Os[1][1] = -1.0; Os[1][2] = 0.0;
					  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = -1.0;
					}
					else if (l == 3)
					{
					  Os[0][0] = -1.0; Os[0][1] = 0.0; Os[0][2] = 0.0;
					  Os[1][0] = 0.0; Os[1][1] = -1.0; Os[1][2] = 0.0;
					  Os[2][0] = 0.0; Os[2][1] = 0.0; Os[2][2] = 1.0;
					}
					m2[0][0] = Os[0][0]*m1[0][0] + Os[1][0]*m1[0][1] + Os[2][0]*m1[0][2];
					m2[0][1] = Os[0][1]*m1[0][0] + Os[1][1]*m1[0][1] + Os[2][1]*m1[0][2];
					m2[0][2] = Os[0][2]*m1[0][0] + Os[1][2]*m1[0][1] + Os[2][2]*m1[0][2];
					m2[1][0] = Os[0][0]*m1[1][0] + Os[1][0]*m1[1][1] + Os[2][0]*m1[1][2];
					m2[1][1] = Os[0][1]*m1[1][0] + Os[1][1]*m1[1][1] + Os[2][1]*m1[1][2];
					m2[1][2] = Os[0][2]*m1[1][0] + Os[1][2]*m1[1][1] + Os[2][2]*m1[1][2];
					m2[2][0] = Os[0][0]*m1[2][0] + Os[1][0]*m1[2][1] + Os[2][0]*m1[2][2];
					m2[2][1] = Os[0][1]*m1[2][0] + Os[1][1]*m1[2][1] + Os[2][1]*m1[2][2];
					m2[2][2] = Os[0][2]*m1[2][0] + Os[1][2]*m1[2][1] + Os[2][2]*m1[2][2];
					double ea2 = acos(m2[2][2]);
					double cosine3 = (m2[1][2]/sin(ea2));
					double sine3 = (m2[0][2]/sin(ea2));
					double cosine1 = (-m2[2][1]/sin(ea2));
					double sine1 = (m2[2][0]/sin(ea2));
					double ea3 = acos(cosine3);	
					double ea1 = acos(cosine1);
					if(sine3 < 0) ea3 = (2*m_pi)-ea3;
					if(sine1 < 0) ea1 = (2*m_pi)-ea1;
					int ea1bin = int(ea1/(m_pi/36));
					int ea2bin = int(ea2/(m_pi/36));
					int ea3bin = int(ea3/(m_pi/36));
					int bin=0;
					if(ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 <= (m_pi/2.0) && ea2 <= (m_pi/2.0) && ea3 <= (m_pi/2.0))
					{
					  bin = (ea3bin*18*18)+(ea2bin*18)+(ea1bin);
					  eulerodf[bin].density = eulerodf[bin].density+vol;
					  totalvol = totalvol + vol;
					}
				}
			}
		}
	}
}
void  ReconstructionFunc::find_axisodf ()
{
	totalaxes = 0;
	for(int i=0;i<numgrains;i++)
	{
		if(grains[i].surfacegrain == 0)
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
			for(int k = 0; k < 4; k++)
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
				  o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
				  o[1][0] = 0.0; o[1][1] = 1.0; o[1][2] = 0.0;
				  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
				}
				else if (k == 2)
				{
				  o[0][0] = 1.0; o[0][1] = 0.0; o[0][2] = 0.0;
				  o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
				  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = -1.0;
				}
				else if (k == 3)
				{
				  o[0][0] = -1.0; o[0][1] = 0.0; o[0][2] = 0.0;
				  o[1][0] = 0.0; o[1][1] = -1.0; o[1][2] = 0.0;
				  o[2][0] = 0.0; o[2][1] = 0.0; o[2][2] = 1.0;
				}
				ga[0][0] = r1x;
				ga[0][1] = r2x;
				ga[0][2] = r3x;
				ga[1][0] = r1y;
				ga[1][1] = r2y;
				ga[1][2] = r3y;
				ga[2][0] = r1z;
				ga[2][1] = r2z;
				ga[2][2] = r3z;
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
				int ea1bin = int(ea1/(m_pi/18));
				int ea2bin = int(ea2/(m_pi/18));
				int ea3bin = int(ea3/(m_pi/18));
				int bin=0;
				if(ea1 >= 0.0 && ea2 >= 0.0 && ea3 >= 0.0 && ea1 <= (m_pi) && ea2 <= (m_pi) && ea3 <= (m_pi))
				{
				  bin = (ea3bin*18*18)+(ea2bin*18)+(ea1bin);
				  axisodf[bin].density = axisodf[bin].density+1;
				  totalaxes = totalaxes+1;
				}
			}
		}
	}
}
void  ReconstructionFunc::measure_misorientations (double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
  size_t initialsize = 10;
  vector<double> misolist(initialsize,-1);
  double n1;
  double n2;
  double n3;
  double w;
  for (int i = 1; i < numgrains; i++)
  {
    vector<int>* nlist = grains[i].neighborlist;
    double g1ea1 = grains[i].avgeuler1;
    double g1ea2 = grains[i].avgeuler2;
    double g1ea3 = grains[i].avgeuler3;
    int size = 0;
    if (NULL != nlist) { size = nlist->size(); }
	misolist.resize(size,-1);
    for(int j=0;j<size;j++)
    {
      int nname = nlist->at(j);
      double g2ea1 = grains[nname].avgeuler1;
      double g2ea2 = grains[nname].avgeuler2;
      double g2ea3 = grains[nname].avgeuler3;
      if(crystruct == 1) w = getmisoquathexagonal(quat_symmhex,misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      if(crystruct == 2) w = getmisoquatcubic(misorientationtolerance,g1ea1,g1ea2,g1ea3,g2ea1,g2ea2,g2ea3,n1,n2,n3);
      misolist[j] = w;
    }
	grains[i].misorientationlist = new std::vector<double>(misolist.size() );
	grains[i].misorientationlist->swap(misolist);
    misolist.clear();
  }
}
double ReconstructionFunc::getmisoquatcubic(double misorientationtolerance,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
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

double ReconstructionFunc::getmisoquathexagonal(double quat_symmhex[12][5],double misorientationtolerance,double g1ea1,double g1ea2,double g1ea3,double g2ea1,double g2ea2,double g2ea3,double &n1,double &n2,double &n3)
{
  double wmin=9999999; //,na,nb,nc;
  double w=0;
  double q1[4];
  double q2[4];
  double qr[4];
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
  q2[0]=-q2[0];
  q2[1]=-q2[1];
  q2[2]=-q2[2];
  q2[3]=q2[3];
  qr[0]=q1[0]*q2[3]+q1[3]*q2[0]-q1[1]*q2[2]+q1[2]*q2[1];
  qr[1]=q1[1]*q2[3]+q1[3]*q2[1]-q1[2]*q2[0]+q1[0]*q2[2];
  qr[2]=q1[2]*q2[3]+q1[3]*q2[2]-q1[0]*q2[1]+q1[1]*q2[0];
  qr[3]=q1[3]*q2[3]-q1[0]*q2[0]-q1[1]*q2[1]-q1[2]*q2[2];
  for(int i=0;i<12;i++)
  {
	  qc[0]=quat_symmhex[i][1]*qr[3]+quat_symmhex[i][4]*qr[0]-quat_symmhex[i][2]*qr[2]+quat_symmhex[i][3]*qr[1];
	  qc[1]=quat_symmhex[i][2]*qr[3]+quat_symmhex[i][4]*qr[1]-quat_symmhex[i][3]*qr[0]+quat_symmhex[i][1]*qr[2];
	  qc[2]=quat_symmhex[i][3]*qr[3]+quat_symmhex[i][4]*qr[2]-quat_symmhex[i][1]*qr[1]+quat_symmhex[i][2]*qr[0];
	  qc[3]=quat_symmhex[i][4]*qr[3]-quat_symmhex[i][1]*qr[0]-quat_symmhex[i][2]*qr[1]-quat_symmhex[i][3]*qr[2];
	  if(qc[3] < -1) qc[3] = -1;
	  if(qc[3] > 1) qc[3] = 1;
	  w = acos(qc[3]);
	  w = 2*w;
	  if(w > m_pi) w = (2*m_pi)-w;
	  if(w < wmin) wmin = w;
  }
  wmin = (180.0/m_pi)*wmin;
  return wmin;
}

void  ReconstructionFunc::find_colors(double quat_symmcubic[24][5],double quat_symmhex[12][5])
{
/* This is to calculate colors and schmid for each voxel - no longer stored per voxel

  for(int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    double g1ea1 = voxels[i].euler1;
    double g1ea2 = voxels[i].euler2;
	double g1ea3 = voxels[i].euler3;
    double g[3][3];
    double p[3];
    double d[3];
    g[0][0] = cos(g1ea1)*cos(g1ea3)-sin(g1ea1)*sin(g1ea3)*cos(g1ea2);
    g[0][1] = sin(g1ea1)*cos(g1ea3)+cos(g1ea1)*sin(g1ea3)*cos(g1ea2);
    g[0][2] = sin(g1ea3)*sin(g1ea2);
    g[1][0] = -cos(g1ea1)*sin(g1ea3)-sin(g1ea1)*cos(g1ea3)*cos(g1ea2);
    g[1][1] = -sin(g1ea1)*sin(g1ea3)+cos(g1ea1)*cos(g1ea3)*cos(g1ea2);
    g[1][2] =  cos(g1ea3)*sin(g1ea2);
    g[2][0] =  sin(g1ea1)*sin(g1ea2);
    g[2][1] = -cos(g1ea1)*sin(g1ea2);
    g[2][2] =  cos(g1ea2);
    p[0] = g[0][0]*0+g[0][1]*0+g[0][2]*1;
    p[1] = g[1][0]*0+g[1][1]*0+g[1][2]*1;
    p[2] = g[2][0]*0+g[2][1]*0+g[2][2]*1;
    p[0] = fabs(p[0]);
    p[1] = fabs(p[1]);
    p[2] = fabs(p[2]);
    int j, k, flag = 1;
    double temp;          
    for(j = 0; (j<3)&&flag==1; j++)
    {
      flag = 0;
      for (k=0; k<2; k++)
      {
        if (p[k+1] < p[k])      
        { 
           temp = p[k];           
           p[k] = p[k+1];
           p[k+1] = temp;
           flag = 1;               
        }
      }
    }
	double theta = (p[0]*0)+(p[1]*-sqrt(2.0)/2.0)+(p[2]*sqrt(2.0)/2.0);
	theta = (180.0/m_pi)*acos(theta);
	double red = (90.0-theta)/45.0;
	d[0] = (p[1]*1)-(p[2]*0);
    d[1] = (p[2]*0)-(p[0]*1);
    d[2] = (p[0]*0)-(p[1]*0);
	d[0] = -(d[1]+d[2])/d[0];
    d[1] = 1;
    d[2] = 1;
	double norm = pow(((d[0]*d[0])+(d[1]*d[1])+(d[2]*d[2])),0.5);
	d[0] = d[0]/norm;
	d[1] = d[1]/norm;
	d[2] = d[2]/norm;
	double phi = (d[0]*0)+(d[1]*sqrt(2.0)/2.0)+(d[2]*sqrt(2.0)/2.0);
	phi = (180.0/m_pi)*acos(phi);
	double green = (1-red)*((35.26-phi)/35.26);
	double blue = (1-red)-green;
	double max = red;
	if(green > max) max = green;
	if(blue > max) max = blue;
	red = red/max;
	green = green/max;
	blue = blue/max;
	red = (0.75*red)+0.25;
	green = (0.75*green)+0.25;
	blue = (0.75*blue)+0.25;
	voxels[i].red = red;
	voxels[i].green = green;
	voxels[i].blue = blue;
    double loadx = p[0];
    double loady = p[1];
    double loadz = p[2];
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
	voxels[i].schmid = schmid;
  }
*/
  double red, green, blue;
  double theta, phi;
  for(int i = 1; i < numgrains; i++)
  {
    double g1ea1 = grains[i].avgeuler1;
    double g1ea2 = grains[i].avgeuler2;
    double g1ea3 = grains[i].avgeuler3;
    double q1[4];
    double qc[4];
    double p[3];
    double d[3];
	q1[0]=sin((g1ea2/2.0))*cos(((g1ea1-g1ea3)/2.0));
	q1[1]=sin((g1ea2/2.0))*sin(((g1ea1-g1ea3)/2.0));
	q1[2]=cos((g1ea2/2.0))*sin(((g1ea1+g1ea3)/2.0));
	q1[3]=cos((g1ea2/2.0))*cos(((g1ea1+g1ea3)/2.0));
	if(crystruct == 2)
	{
	    p[0] = (2*q1[0]*q1[2]+2*q1[1]*q1[3])*1;
		p[1] = (2*q1[1]*q1[2]+2*q1[0]*q1[3])*1;
		p[2] = (1-2*q1[0]*q1[0]-2*q1[1]*q1[1])*1;
		double denom = p[0]*p[0]+p[1]*p[1]+p[2]*p[2];
		denom = pow(denom,0.5);
		p[0] = fabs(p[0]/denom);
		p[1] = fabs(p[1]/denom);
		p[2] = fabs(p[2]/denom);
		int j, k, flag = 1;
		double temp;          
		for(j = 0; (j<3)&&flag==1; j++)
		{
		  flag = 0;
		  for (k=0; k<2; k++)
		  {
			if (p[k+1] < p[k])      
			{ 
			   temp = p[k];           
			   p[k] = p[k+1];
			   p[k+1] = temp;
			   flag = 1;               
			}
		  }
		}
		theta = (p[0]*0)+(p[1]*-sqrt(2.0)/2.0)+(p[2]*sqrt(2.0)/2.0);
		theta = (180.0/m_pi)*acos(theta);
		red = (90.0-theta)/45.0;
		d[0] = (p[1]*1)-(p[2]*0);
		d[1] = (p[2]*0)-(p[0]*1);
		d[2] = (p[0]*0)-(p[1]*0);
		d[0] = -(d[1]+d[2])/d[0];
		d[1] = 1;
		d[2] = 1;
		double norm = pow(((d[0]*d[0])+(d[1]*d[1])+(d[2]*d[2])),0.5);
		d[0] = d[0]/norm;
		d[1] = d[1]/norm;
		d[2] = d[2]/norm;
		phi = (d[0]*0)+(d[1]*sqrt(2.0)/2.0)+(d[2]*sqrt(2.0)/2.0);
		phi = (180.0/m_pi)*acos(phi);
		green = (1-red)*((35.26-phi)/35.26);
		blue = (1-red)-green;
		double max = red;
		if(green > max) max = green;
		if(blue > max) max = blue;
		red = red/max;
		green = green/max;
		blue = blue/max;
		red = (0.75*red)+0.25;
		green = (0.75*green)+0.25;
		blue = (0.75*blue)+0.25;
		grains[i].red = red;
		grains[i].green = green;
		grains[i].blue = blue;
	}
	if(crystruct == 1)
	{
		red=1.0/3.0;
		green=1.0/3.0;
		blue=1.0/3.0;
		for(int j=0;j<12;j++)
		{
			qc[0]=quat_symmhex[j][1]*q1[3]+quat_symmhex[j][4]*q1[0]-quat_symmhex[j][2]*q1[2]+quat_symmhex[j][3]*q1[1];
			qc[1]=quat_symmhex[j][2]*q1[3]+quat_symmhex[j][4]*q1[1]-quat_symmhex[j][3]*q1[0]+quat_symmhex[j][1]*q1[2];
			qc[2]=quat_symmhex[j][3]*q1[3]+quat_symmhex[j][4]*q1[2]-quat_symmhex[j][1]*q1[1]+quat_symmhex[j][2]*q1[0];
			qc[3]=quat_symmhex[j][4]*q1[3]-quat_symmhex[j][1]*q1[0]-quat_symmhex[j][2]*q1[1]-quat_symmhex[j][3]*q1[2];
		    p[0] = ((2*qc[0]*qc[2])-(2*qc[1]*qc[3]))*1;
			p[1] = ((2*qc[1]*qc[2])+(2*qc[0]*qc[3]))*1;
			p[2] = (1-(2*qc[0]*qc[0])-(2*qc[1]*qc[1]))*1;
			double denom = p[0]*p[0]+p[1]*p[1]+p[2]*p[2];
			denom = pow(denom,0.5);
			p[0] = p[0]/denom;
			p[1] = p[1]/denom;
			p[2] = p[2]/denom;
			if(p[2] < 0)
			{
				p[0] = -p[0];
				p[1] = -p[1];
				p[2] = -p[2];
			}
			d[0] = (p[1]*1)-(p[2]*0);
			d[1] = (p[2]*0)-(p[0]*1);
			d[2] = (p[0]*0)-(p[1]*0);
			d[0] = -d[1]/d[0];
			d[1] = 1;
			d[2] = 0;
			double norm = pow(((d[0]*d[0])+(d[1]*d[1])+(d[2]*d[2])),0.5);
			d[0] = d[0]/norm;
			d[1] = d[1]/norm;
			d[2] = d[2]/norm;
			if(atan(d[1]/d[0]) >= 0 && atan(d[1]/d[0]) <= (30.0*m_pi/180.0))
			{
				theta = (p[0]*0)+(p[1]*0)+(p[2]*1);
				theta = (180.0/m_pi)*acos(theta);
				red = (90.0-theta)/90.0;
				phi = (d[0]*1)+(d[1]*0)+(d[2]*0);
				phi = (180.0/m_pi)*acos(phi);
				green = (1-red)*((30.0-phi)/30.0);
				blue = (1-red)-green;
			}
		}
		double max = red;
		if(green > max) max = green;
		if(blue > max) max = blue;
		red = red/max;
		green = green/max;
		blue = blue/max;
		red = (0.75*red)+0.25;
		green = (0.75*green)+0.25;
		blue = (0.75*blue)+0.25;
		grains[i].red = red;
		grains[i].green = green;
		grains[i].blue = blue;
	}
    double loadx = p[0];
    double loady = p[1];
    double loadz = p[2];
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
  double convexity = 1;
  for(int i = 1; i < numgrains; i++)
  {
/*    int size = grains[i].numvoxels;
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
      double I1 = grains[i].radius1;
      double I2 = grains[i].radius2;
      double I3 = grains[i].radius3;
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
          double x = find_xcoord(j);
	      double y = find_ycoord(j);
	      double z = find_zcoord(j);
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
*/
	grains[i].convexity = convexity;
  }
}
void ReconstructionFunc::volume_stats(string writename1,string writename2,string writename3)
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
  double avgomega3 = 0;
  double neighcount = 0;
  double maxvol = 0;
  double maxneigh = 0;
  double maxdiam = 0;
  double maxlogdiam = 0;
  double maxbovera = 0;
  double maxcovera = 0;
  double maxcoverb = 0;
  double maxschmid = 0;
  double maxomega3 = 0;
  double svn[100][5];
  double svs[100][5];
  double svbovera[100][5];
  double svcovera[100][5];
  double svcoverb[100][5];
  double svschmid[100][5];
  double svomega3[100][5];
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
      svomega3[temp][temp2] = 0;
    }
  }
  for(int i = 1; i < numgrains; i++)
  {
    int onedge = grains[i].surfacegrain;
    if(onedge == 0)
    {
      actualgrains++;
      int vol = grains[i].numvoxels;
      double voxvol = vol*resx*resy*resz;
      double logvol = log(voxvol);
      double rad_3 = 0.75*(1/m_pi)*voxvol;
      double diam = 2*pow(rad_3,0.333333333);
      int diamint = int(diam);
      double logdiam = log(diam);
      double I1 = grains[i].radius1;
      double I2 = grains[i].radius2;
      double I3 = grains[i].radius3;
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
	  double omega3 = grains[i].omega3;
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
      avgomega3 = avgomega3+omega3;
      svn[diamint][0]++;
      svbovera[diamint][0]++;
      svcovera[diamint][0]++;
      svcoverb[diamint][0]++;
      svschmid[diamint][0]++;
      svomega3[diamint][0]++;
      svn[diamint][1] = svn[diamint][1] + nnum;
      svn[diamint][3] = svn[diamint][3] + lognnum;
      svbovera[diamint][1] = svbovera[diamint][1] + bovera;
      svcovera[diamint][1] = svcovera[diamint][1] + covera;
      svcoverb[diamint][1] = svcoverb[diamint][1] + coverb;
      svschmid[diamint][1] = svschmid[diamint][1] + schmid;
      svomega3[diamint][1] = svomega3[diamint][1] + omega3;
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
      if(omega3 > maxomega3) maxomega3 = omega3;
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
      svomega3[temp3][1] = svomega3[temp3][1]/svomega3[temp3][0];
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
  avgomega3 = avgomega3/actualgrains;
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
  double sdomega3 = 0;
  for(int j = 1; j < numgrains; j++)
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
      double I1 = grains[j].radius1;
      double I2 = grains[j].radius2;
      double I3 = grains[j].radius3;
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
	  double omega3 = grains[j].omega3;
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
      sdomega3 = sdomega3 + ((omega3-avgomega3)*(omega3-avgomega3));
      svn[diamint][2] = svn[diamint][2] + ((nnum-svn[diamint][1])*(nnum-svn[diamint][1]));
      svn[diamint][4] = svn[diamint][4] + ((lognnum-svn[diamint][3])*(lognnum-svn[diamint][3]));
      svbovera[diamint][2] = svbovera[diamint][2] + ((bovera-svbovera[diamint][1])*(bovera-svbovera[diamint][1]));
      svcovera[diamint][2] = svcovera[diamint][2] + ((covera-svcovera[diamint][1])*(covera-svcovera[diamint][1]));
      svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb-svcoverb[diamint][1])*(coverb-svcoverb[diamint][1]));
      svschmid[diamint][2] = svschmid[diamint][2] + ((schmid-svschmid[diamint][1])*(schmid-svschmid[diamint][1]));
      svomega3[diamint][2] = svomega3[diamint][2] + ((omega3-svomega3[diamint][1])*(omega3-svomega3[diamint][1]));
      int size = 0;
      if (NULL != nlist) { size = nlist->size(); }
      for(int k=0;k<size;k++)
      {
		int neigh = nlist->at(k);
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
      svomega3[temp4][2] = svomega3[temp4][2]/svomega3[temp4][0];
      svbovera[temp4][3] = svbovera[temp4][1]*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
      svbovera[temp4][4] = (1-svbovera[temp4][1])*(((svbovera[temp4][1]*(1-svbovera[temp4][1]))/svbovera[temp4][2])-1);
      svcovera[temp4][3] = svcovera[temp4][1]*(((svcovera[temp4][1]*(1-svcovera[temp4][1]))/svcovera[temp4][2])-1);
      svcovera[temp4][4] = (1-svcovera[temp4][1])*(((svcovera[temp4][1]*(1-svcovera[temp4][1]))/svcovera[temp4][2])-1);
      svcoverb[temp4][3] = svcoverb[temp4][1]*(((svcoverb[temp4][1]*(1-svcoverb[temp4][1]))/svcoverb[temp4][2])-1);
      svcoverb[temp4][4] = (1-svcoverb[temp4][1])*(((svcoverb[temp4][1]*(1-svcoverb[temp4][1]))/svcoverb[temp4][2])-1);
      svomega3[temp4][3] = svomega3[temp4][1]*(((svomega3[temp4][1]*(1-svomega3[temp4][1]))/svomega3[temp4][2])-1);
      svomega3[temp4][4] = (1-svomega3[temp4][1])*(((svomega3[temp4][1]*(1-svomega3[temp4][1]))/svomega3[temp4][2])-1);
      svn[temp4][2] = pow(svn[temp4][2],0.5);
      svn[temp4][4] = pow(svn[temp4][4],0.5);
      svs[temp4][2] = pow(svs[temp4][2],0.5);
      svs[temp4][4] = pow(svs[temp4][4],0.5);
      svbovera[temp4][2] = pow(svbovera[temp4][2],0.5);
      svcovera[temp4][2] = pow(svcovera[temp4][2],0.5);
      svcoverb[temp4][2] = pow(svcoverb[temp4][2],0.5);
      svschmid[temp4][2] = pow(svschmid[temp4][2],0.5);
      svomega3[temp4][2] = pow(svomega3[temp4][2],0.5);
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
  sdomega3 = sdomega3/actualgrains;
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
  double omega3var = sdomega3;
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
  sdomega3 = pow(sdomega3,0.5);
  double svncr = 0;
  double svscr = 0;
  double svboveracr = 0;
  double svcoveracr = 0;
  double svcoverbcr = 0;
  double svschmidcr = 0;
  double svomega3cr = 0;
  for(int temp5 = 0; temp5 < (maxdiamint+1); temp5++)
  {
    svncr = svncr + (svn[temp5][0]*((svn[temp5][1]-avgneigh)*(svn[temp5][1]-avgneigh)));
    svscr = svscr + (svs[temp5][0]*((svs[temp5][1]-avgdiam2)*(svs[temp5][1]-avgdiam2)));
    svboveracr = svboveracr + (svbovera[temp5][0]*((svbovera[temp5][1]-avgbovera)*(svbovera[temp5][1]-avgbovera)));
    svcoveracr = svcoveracr + (svcovera[temp5][0]*((svcovera[temp5][1]-avgcovera)*(svcovera[temp5][1]-avgcovera)));
    svcoverbcr = svcoverbcr + (svcoverb[temp5][0]*((svcoverb[temp5][1]-avgcoverb)*(svcoverb[temp5][1]-avgcoverb)));
    svschmidcr = svschmidcr + (svschmid[temp5][0]*((svschmid[temp5][1]-avgschmid)*(svschmid[temp5][1]-avgschmid)));
    svomega3cr = svomega3cr + (svomega3[temp5][0]*((svomega3[temp5][1]-avgomega3)*(svomega3[temp5][1]-avgomega3)));
  }
  svncr = svncr/(actualgrains*neighvar);
  svscr = svscr/(neighcount*diamvar2);
  svboveracr = svboveracr/(actualgrains*boveravar);
  svcoveracr = svcoveracr/(actualgrains*coveravar);
  svcoverbcr = svcoverbcr/(actualgrains*coverbvar);
  svschmidcr = svschmidcr/(actualgrains*schmidvar);
  svomega3cr = svomega3cr/(actualgrains*omega3var);
  for(int l = 1; l < numgrains; l++)
  {
    int onedge = grains[l].surfacegrain;
    if(onedge == 0)
    {
      double microcount = 0;
      vector<double>* misolist = grains[l].misorientationlist;
      int nummisos = int(misolist->size());
      int size = int(misolist->size());
      for(int k=0;k<size;k++)
      {
        misocount++;
        double firstmiso = misolist->at(k);
        int misocur = int(firstmiso/(65.0/nummisobins));
        if(misocur == nummisobins) misocur = nummisobins-1;
        misobin[misocur]++;
        if(firstmiso < 15) microcount++;
      }
      if (nummisos != 0 ) {
      microcount = microcount/nummisos;
      }
      else 
      {
        microcount = 0;
      }
      int microcur = int(microcount/(1.0/nummicrobins));
      if(microcur == nummicrobins) microcur = nummicrobins-1;
      microbin[microcur]++;
    }
  }
  double orand[15][2];
  double delta = m_pi/18;
  double texindex = 0;
  double texstrength = 0;
  int bin = 0;
  for(int iter51 = 0; iter51 < 18; iter51++)
  {
    for(int iter52 = 0; iter52 < 18; iter52++)
    {
      for(int iter53 = 0; iter53 < 18; iter53++)
      {
        double f = 0;
		bin = (iter51*18*18)+(iter52*18)+(iter53);
        if(iter52 == 0)
        {
			f = (m_pi*m_pi/4)*(double(eulerodf[bin].density/totalvol)/((delta)*(delta)*cos(double((iter52*delta)+(delta/2)))));
        }
        if(iter52 == 18)
        {
			f = (m_pi*m_pi/4)*(double(eulerodf[bin].density/totalvol)/((delta)*(delta)*cos(double((iter52*delta)-(delta/2)))));
        }
        if(iter52 != 0 && iter52 != 18)
        {
			f = (m_pi*m_pi/4)*(double(eulerodf[bin].density/totalvol)/((delta)*(delta)*(cos(double((iter52*delta)-(delta/2)))-cos(double((iter52*delta)+(delta/2))))));
        }
        texindex = texindex + (f*f);
      }
    }
    texindex = texindex/(18*18*18);
    texstrength = pow(texindex,0.5);
  }
  ofstream outFile;
  outFile.open(writename1.c_str());
  outFile << "STATS" << endl;
  outFile << "Grain_Diameter_Info" << endl;
  outFile << (maxdiameter-mindiameter)+1 << " " << maxdiameter << " " << mindiameter << endl;
  outFile << "Grain_Size_Distribution" << endl;
  outFile << avglogdiam << " " << sdlogdiam << endl;
  outFile << "Grain_SizeVBoverA_Distributions" << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
	outFile << temp7 <<	" " << svbovera[temp7][3] << " " << svbovera[temp7][4] << endl;
  }
  outFile << "Grain_SizeVCoverA_Distributions" << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svcovera[temp7][3] << "  " << svcovera[temp7][4] << endl;
  }
  outFile << "Grain_SizeVCoverB_Distributions" << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svcoverb[temp7][3] << "  " << svcoverb[temp7][4] << endl;
  }
  outFile << "Grain_SizeVNeighbors_Distributions" << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svn[temp7][3] << " " << svn[temp7][4] << endl;
  }
  outFile << "Grain_SizeVNeighborSize_Distributions" << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svs[temp7][3] << " " << svs[temp7][4] << endl;
  }
  outFile << "Grain_SizeVOmega3_Distributions" << endl;
  for(int temp7 = mindiameter; temp7 < (maxdiameter)+1; temp7++)
  {
    outFile << temp7 << " " << svomega3[temp7][3] << " " << svomega3[temp7][4] << endl;
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
void  ReconstructionFunc::write_volume(string writename11)
{
    ofstream outFile;
    outFile.open(writename11.c_str());
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
    int grainname = voxels[i].grainname;
    outFile << "   ";
    if(grainname < 10000) outFile << " ";
    if(grainname < 1000) outFile << " ";
    if(grainname < 100) outFile << " ";
    if(grainname < 10) outFile << " ";
    outFile << grainname;
  }
  outFile << endl;
  outFile << "SCALARS Misorientation float" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    if(i%20 == 0 && i > 0) outFile << endl;
	outFile << voxels[i].misorientation << " ";
  }
  outFile << endl;
  outFile << "SCALARS ImageQuality float" << endl;
  outFile << "LOOKUP_TABLE default" << endl;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {
    if(i%20 == 0 && i > 0) outFile << endl;
	outFile << voxels[i].imagequality << " ";
  }
  outFile << endl;
  outFile << "COLOR_SCALARS colors 3" << endl;
  double red,green,blue;
  int gnum;
  for (int i = 0; i < (xpoints*ypoints*zpoints); i++)
  {	
	gnum = voxels[i].grainname;
	red = grains[gnum].red;
	green = grains[gnum].green;
	blue = grains[gnum].blue;
/*  This is to color each voxel by its own euler angles - these colors are no longer stored per voxel
	red = voxels[i].red;
	green = voxels[i].green;
	blue = voxels[i].blue;
*/	
	outFile << red << " " << green << " " << blue << endl;
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


void ReconstructionFunc::write_graindata(string gdata)
{
  ofstream outFile;
  outFile.open(gdata.c_str());
  outFile << numgrains << endl;
  for(int i = 1; i < numgrains; i++)
  {
	double volume = grains[i].volume;
	double radius = pow(((0.75)*(1.0/m_pi)*volume),0.3333333333);
	int onsurface = grains[i].surfacegrain;
	double ea1 = grains[i].avgeuler1;
	double ea2 = grains[i].avgeuler2;
	double ea3 = grains[i].avgeuler3;
	outFile << i << "	" << radius << "	" << onsurface << "	" << ea1 << "	" << ea2 << "	" << ea3 << endl;
  }
  outFile.close();

}
void  ReconstructionFunc::write_axisodf(string writename14)
{
  ofstream outFile;
  outFile.open(writename14.c_str());
  outFile << numgrains << endl;
  double density;
  for(int i = 0; i < (18*18*18); i++)
  {
	density = axisodf[i].density;
    outFile << double(density/totalaxes) << endl;
  }
  outFile.close();
}
void  ReconstructionFunc::write_eulerodf(string writename15)
{
  ofstream outFile;
  outFile.open(writename15.c_str());
  outFile << numgrains <<endl;
  double density;
  for(int i = 0; i < (18*18*18); i++)
  {
	density = eulerodf[i].density;
    outFile << double(density/totalvol) << endl;
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


double ReconstructionFunc::find_xcoord(long index)
{
	double x = resx*double(index%xpoints);
	return x;
}
double ReconstructionFunc::find_ycoord(long index)
{
	double y = resy*double((index/xpoints)%ypoints);
	return y;
}
double ReconstructionFunc::find_zcoord(long index)
{
	double z = resz*double(index/(xpoints*ypoints));
	return z;
}