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

#include "MXA/Common/MXASetGetMacros.h"

#include "AIM/Common/AIMCommonConfiguration.h"
#include "AIM/Common/Constants.h"



/**
 * @class Grain Grain.h AIM/Common/Grain.h
 * @brief Support class for the MicroGen3D class
 * @author Michael A. Jackson for BlueQuartz Software, Dr. Michael Groeber for USAFRL
 * @date Nov 4, 2009
 * @version 1.0
 */
class AIMCOMMON_EXPORT Grain
{

  public:
    MXA_SHARED_POINTERS(Grain);
    MXA_STATIC_NEW_MACRO(Grain);
    MXA_TYPE_MACRO(Grain);
    virtual ~Grain();

    /**
     * @brief Performs a Depp copy of the Grain object by making copies of the contained
     * data and not just copying the pointers to the data
     * @param grain Another Grain object to copy from.
     */
    void deepCopy(Grain::Pointer grain);

 //   Grain& operator=(const Grain&);

    IntVectorType     neighborlist;
    DoubleVectorType  neighborsurfarealist;

    // std::vector<int>* neighborlist;
    //  std::vector<double>* neighborsurfarealist;

    std::vector<int>* voxellist;
    std::vector<double>* ellipfunclist;
    std::vector<double>* misorientationlist;
    std::vector<std::vector<int> > neighbordistfunclist;

    int nucleus;
    int active;
    int numvoxels;
    int numneighbors;
    int newgrainname;
    int gotcontainedmerged;
    int gottwinmerged;
    int gotcolonymerged;
    int surfacegrain;
    int twinnewnumber;
    int colonynewnumber;
	  double packquality;
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
  	double averageimagequality;
    double averagemisorientation;
    double kernelmisorientation;
    double red;
    double green;
    double blue;
    double schmidfactor;
    double euler1;
    double euler2;
    double euler3;
    double axiseuler1;
    double axiseuler2;
    double axiseuler3;
    double volume;
    double equivdiameter;
    double neigherror;
    double radius1;
    double radius2;
    double radius3;
    double lowanglefraction;
    double avg_quat[5];
    int neighbordistfunc[3];

  protected:
    Grain();

 //   Grain(const Grain&);
 //   Grain(Grain& grain);

  private:
    Grain(const Grain&); // Copy Constructor Not Implemented
    void operator=(const Grain&); // Operator '=' Not Implemented
};

#endif /* GRAINS_H_ */
