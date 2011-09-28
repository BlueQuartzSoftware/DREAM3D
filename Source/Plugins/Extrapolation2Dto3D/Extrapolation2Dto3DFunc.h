
#ifndef _Extrapolation2Dto3DFunc_H
#define _Extrapolation2Dto3DFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <stdlib.h>

#include <string>
#include <iostream>
#include <cmath>
#include <fstream>
#include <list>

#include <boost/shared_array.hpp>

#include "MXA/MXA.h"
#include "MXA/Common/MXASetGetMacros.h"
#include "MXA/Common/LogTime.h"

#include "DREAM3DLib/DREAM3DLib.h"
#include "DREAM3D/Common/DREAM3DRandom.h"
#include "DREAM3D/Common/Observable.h"

/**
 * @class Extrapolation2Dto3DFunc Extrapolation2Dto3DFunc.h AIM/Common/Extrapolation2Dto3DFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class  Extrapolation2Dto3DFunc : public Observable
{
public:
    MXA_SHARED_POINTERS(Extrapolation2Dto3DFunc)
    MXA_STATIC_NEW_MACRO(Extrapolation2Dto3DFunc)

    virtual ~Extrapolation2Dto3DFunc();

    typedef boost::shared_array<float>    SharedFloatArray;
    typedef boost::shared_array<int>      SharedIntArray;

	unsigned long long int GGseed;

	double totalcount;
	double runningcount;
	double numsizebins;
	double numneighbins;
	double numdiambins;
	double numshapebins;
	double nummisobins;
	double nummicrobins;
	double sizeprobbin[501][2];
	double shapeprobbin[136];
	double elliptype[136][4];
	double eulerbin[18][18][18][5];
	double eulerbindata[18][18][18][3][20][10];
	double eulerbinpgo[18][18][18][3][20][10];
	double actxface[20][10];
	double actyface[20][10];
	double actzface[20][10];
	double tempxface[20][10];
	double tempyface[20][10];
	double tempzface[20][10];
	double maxxarea;
	double avgxarea;
	double maxxaxis;
	double maxyarea;
	double avgyarea;
	double maxyaxis;
	double maxzarea;
	double avgzarea;
	double maxzaxis;
	double averagearea5;
	double averagearea6;
	double averagearea7;
	double totaladjust5;
	double totaladjust6;
	double totaladjust7;
	double xpb[20][20];
	double ypb[20][20];
	double zpb[20][20];
	double xshapesbin[136][20][20];
	double xshapesbin2[136][20][20];
	double xshapesbin3[20][20][136];
	double xsizebin[136][20];
	double yshapesbin[136][20][20];
	double yshapesbin2[136][20][20];
	double yshapesbin3[20][20][136];
	double ysizebin[136][20];
	double zshapesbin[136][20][20];
	double zshapesbin2[136][20][20];
	double zshapesbin3[20][20][136];
	double zsizebin[136][20];
	double temppsgox[136][20][20];
	double temppsgoy[136][20][20];
	double temppsgoz[136][20][20];
	double tempgrainscount[136];
	double xgrains[136][5000][2];
	double ygrains[136][5000][2];
	double zgrains[136][5000][2];
	int xgrainscount[136];
	int ygrainscount[136];
	int zgrainscount[136];
	double xshapescount[20][20];
	double xsimshapescount[20][20];
	double yshapescount[20][20];
	double ysimshapescount[20][20];
	double zshapescount[20][20];
	double zsimshapescount[20][20];
	long numsections;
	double res;
	double avgarea;
	double maximumarea;
	double averageselectedrad;
	int zerocount;

	void loadtwodimxgrains(const std::string & filename);
	void loadtwodimygrains(const std::string & filename);
	void loadtwodimzgrains(const std::string & filename);
	void find_bincontributions();
	void find_3Dpoints();
	void find_adjustment();
	void find_lognormal();
	void find_pixels();


protected:
    Extrapolation2Dto3DFunc();

private:
    Extrapolation2Dto3DFunc(const Extrapolation2Dto3DFunc& );
    void operator =(const Extrapolation2Dto3DFunc& );
};


#endif
