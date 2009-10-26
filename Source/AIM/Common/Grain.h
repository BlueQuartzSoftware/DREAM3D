

#ifndef _GRAINS_H_
#define _GRAINS_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif


#include <vector>




class Grain
{


  public:
    Grain();
    virtual ~Grain();

    int numvoxels;
    int numneighbors;
    int newgrainname;
    std::vector<int> neighborlist;
    int gotsizemerged;
    int gotcontainedmerged;
    int gottwinmerged;
    double avgeuler1;
    double avgeuler2;
    double avgeuler3;
    double centroidx;
    double centroidy;
    double centroidz;
    int surfacegrain;
    double Ixx;
    double Iyy;
    double Izz;
    double Ixy;
    double Iyz;
    double Ixz;
    double axis1;
    double axis2;
    double axis3;
    double axis1x;
    double axis1y;
    double axis1z;
    double axis2x;
    double axis2y;
    double axis2z;
    double axis3x;
    double axis3y;
    double axis3z;
    std::vector<double> misorientationlist;
    double averagemisorientation;
    int twinnewnumberbeenset;
    int twinnewnumber;
    double red;
    double green;
    double blue;
    double schmidfactor;
    double convexity;
    double euler1;
    double euler2;
    double euler3;
    double oeuler1;
    double oeuler2;
    double oeuler3;
    int grainname;
    int neighnum;
    double volume;
    double Nvalue;
    double nserror;
    double x;
    double y;
    double z;
    double xc;
    double yc;
    double zc;
    int currentsize;
    int initsize;
    int temponsurf;
    int tempneighnum;
    double radius1;
    double radius2;
    double radius3;
    double lowanglefraction;
    double grainrank;
    double grainmicrorank;
    double picked;
    double frozen;

  private:
    Grain(const Grain&);    // Copy Constructor Not Implemented
    void operator=(const Grain&);  // Operator '=' Not Implemented
};

#endif /* GRAINS_H_ */
