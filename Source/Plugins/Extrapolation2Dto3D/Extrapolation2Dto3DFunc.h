
#ifndef _Extrapolation2Dto3DFunc_H
#define _Extrapolation2Dto3DFunc_H

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <boost/shared_array.hpp>

#include <MXA/MXATypes.h>
#include <MXA/Common/MXASetGetMacros.h>

#include "DREAM3D/DREAM3DConfiguration.h"


/**
 * @class Extrapolation2Dto3DFunc Extrapolation2Dto3DFunc.h AIM/Common/Extrapolation2Dto3DFunc.h
 * @brief
 * @author
 * @date
 * @version 1.0
 */
class DREAM3DLib_EXPORT Extrapolation2Dto3DFunc
{
public:
    MXA_SHARED_POINTERS(Extrapolation2Dto3DFunc)
    MXA_STATIC_NEW_MACRO(Extrapolation2Dto3DFunc)

    virtual ~Extrapolation2Dto3DFunc();

    typedef boost::shared_array<float>    SharedFloatArray;
    typedef boost::shared_array<int>      SharedIntArray;

	twodimgrains *twodimxgrain;
	twodimgrains *twodimygrain;
	twodimgrains *twodimzgrain;
	threedimgrains *threedimgrain;
	int32 seeder = time(0);
	TRandomMotherOfAll rg(seeder);
	double numtwodimxgrains;
	double numtwodimygrains;
	double numtwodimzgrains;
	double numthreedimgrains;
	double totalcount = 0;
	double runningcount = 0;
	double numsizebins = 20;
	double numneighbins = 101;
	double numdiambins = 1001;
	double numshapebins = 1001;
	double nummisobins = 10;
	double nummicrobins = 10;
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
	double maxxarea = 0;
	double avgxarea = 0;
	double maxxaxis = 0;
	double maxyarea = 0;
	double avgyarea = 0;
	double maxyaxis = 0;
	double maxzarea = 0;
	double avgzarea = 0;
	double maxzaxis = 0;
	double averagearea5 = 0;
	double averagearea6 = 0;
	double averagearea7 = 0;
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
	long numsections = 13600000;
	double res;
	double avgarea = 0;
	double maximumarea = 0;
	double averageselectedrad = 0;
	int zerocount = 0;

	void loadtwodimxgrains(string inname1)
	void loadtwodimygrains(string inname2)
	void loadtwodimzgrains(string inname3)
	void load_ellipsoids(string inname5)
	void find_bincontributions()
	void find_3Dpoints()
	void find_adjustment()
	void find_lognormal()
	void find_pixels(long numsections)
	void generate_3dgrains(int numtwodimxgrains,int numtwodimygrains,int numtwodimzgrains)
	void volume_stats(int num3dgrains, string outname3)
	void writeAllData (string writename1)
	void write3dgrains(string writename2, int num3dgrains)
	void write_eulerangles(string writename3, int num3dgrains)
	void write_axisorientations(string writename4)
	void volume_stats(int num3dgrains, string writename5, string writename6, string writename7, string writename8, string writename9, string writename10, string writename11, string writename12, string writename13)


protected:
    Extrapolation2Dto3DFunc();
    
private:
    Extrapolation2Dto3DFunc(const Extrapolation2Dto3DFunc& );
    void operator =(const Extrapolation2Dto3DFunc& );
};


#endif
