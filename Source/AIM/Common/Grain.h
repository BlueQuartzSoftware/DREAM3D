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

#ifndef _GRAINS_H_
#define _GRAINS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>

/**
* @class Grain Grain.h AIM/Common/Grain.h
* @brief Support class for the MicroGen3D class
* @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
* @date Nov 4, 2009
* @version 1.0
*/
class Grain
{


  public:
    Grain();
    virtual ~Grain();

	int nucleus;
    int numvoxels;
    int numneighbors;
    int newgrainname;
    int gotsizemerged;
    int gotcontainedmerged;
    int gottwinmerged;
    int gotcolonymerged;
    int surfacegrain;
    int twinnewnumberbeenset;
    int twinnewnumber;
    int colonynewnumberbeenset;
    int colonynewnumber;
    int currentsize;
    int initsize;
    int temponsurf;
    int tempneighnum;
    double centroidx;
    double centroidy;
    double centroidz;
    double Ixx;
    double Iyy;
    double Izz;
    double Ixy;
    double Iyz;
    double Ixz;
	double omega3;
    double averagemisorientation;
    double red;
    double green;
    double blue;
    double schmidfactor;
    double convexity;
    double euler1;
    double euler2;
    double euler3;
    double axiseuler1;
    double axiseuler2;
    double axiseuler3;
    double volume;
    double nserror;
    double radius1;
    double radius2;
    double radius3;
    double lowanglefraction;
    double grainrank;
    double grainmicrorank;
    double picked;
    double frozen;
	double avg_quat[5];
    std::vector<int>* neighborlist;
    std::vector<int>* voxellist;
    std::vector<double>* misorientationlist;
    std::vector<double>* neighborsurfarealist;

  private:
    Grain(const Grain&);    // Copy Constructor Not Implemented
    void operator=(const Grain&);  // Operator '=' Not Implemented
};

#endif /* GRAINS_H_ */
