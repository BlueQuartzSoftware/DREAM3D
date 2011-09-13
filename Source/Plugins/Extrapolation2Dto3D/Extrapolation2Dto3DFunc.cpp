
#include "Extrapolation2Dto3DFunc.h"


using namespace std;

Extrapolation2Dto3DFunc::Extrapolation2Dto3DFunc()
{
  
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
Extrapolation2Dto3DFunc::~Extrapolation2Dto3DFunc()
{
 
}
// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void Extrapolation2Dto3DFunc::loadtwodimxgrains(const std::string & inname1)
{ 
    ifstream inputFile;
    inputFile.open(inname1.c_str());
	int gnum;
	double area;
	double a;
	double b;
	int nnum;
	double x1;
	double y1;
    double x2;
	double y2;
	double ea1;
	double ea2;
	double ea3;
	int count = 0;
	int surfacegrain;
	int size;
	inputFile >> size;
	for (long k = 0; k < size; k++)
    {
		inputFile >> gnum >> area >> a >> b >> nnum >> x1 >> y1 >> x2 >> y2 >> ea1 >> ea2 >> ea3 >> surfacegrain;
		if(x1 != 1 && y1 != 1 && surfacegrain != 1)
		{
			int arbin = (b/a)/0.1;
			if(arbin > 9) arbin = 9;
			int orbin = 0;
			int fix = 1;
			if(y1 < 0) fix = -1;
			if(y1 > 0) fix = 1;
			orbin = 20-acos(x1*fix)/double(0.15707);
			if(y1 == 0) orbin = 19;
			if(orbin > 19) orbin = 19;
			actxface[orbin][arbin] = actxface[orbin][arbin]+(1.0);
			count++;
			averagearea5 = averagearea5+area;
		}
    }
	averagearea5 = averagearea5/double(count);
    inputFile.close();
}

void Extrapolation2Dto3DFunc::loadtwodimygrains(const std::string & inname2)
{ 
    ifstream inputFile;
    inputFile.open(inname2.c_str());
	int gnum;
	double area;
	double a;
	double b;
	int nnum;
    double x1;
	double y1;
    double x2;
	double y2;
	double ea1;
	double ea2;
	double ea3;
	int count = 0;
	int surfacegrain;
	int size;
	inputFile >> size;
	for (long k = 0; k < size; k++)
    {
		inputFile >> gnum >> area >> a >> b >> nnum >> x1 >> y1 >> x2 >> y2 >> ea1 >> ea2 >> ea3 >> surfacegrain;
		if(x1 != 1 && y1 != 1 && surfacegrain != 1)
		{
			int arbin = (b/a)/0.1;
			if(arbin > 9) arbin = 9;
			int orbin = 0;
			int fix = 1;
			if(y1 < 0) fix = -1;
			if(y1 > 0) fix = 1;
			orbin = 20-acos(x1*fix)/double(0.15707);
			if(y1 == 0) orbin = 19;
			if(orbin > 19) orbin = 19;
			actyface[orbin][arbin] = actyface[orbin][arbin]+(1.0);
			count++;
			averagearea6 = averagearea6+area;
		}
    }
	averagearea6 = averagearea6/double(count);
    inputFile.close();
}

void Extrapolation2Dto3DFunc::loadtwodimzgrains(const std::string & inname3)
{ 
    ifstream inputFile;
    inputFile.open(inname3.c_str());
	int gnum;
	double area;
	double a;
	double b;
	int nnum;
    double x1;
	double y1;
    double x2;
	double y2;
	double ea1;
	double ea2;
	double ea3;
	int count = 0;
	int surfacegrain;
	int size;
	inputFile >> size;
	for (long k = 0; k < size; k++)
    {
		inputFile >> gnum >> area >> a >> b >> nnum >> x1 >> y1 >> x2 >> y2 >> ea1 >> ea2 >> ea3 >> surfacegrain;
		if(x1 != 1 && y1 != 1 && surfacegrain != 1)
		{
			int arbin = (b/a)/0.1;
			if(arbin > 9) arbin = 9;
			int orbin = 0;
			int fix = 1;
			if(y1 < 0) fix = -1;
			if(y1 > 0) fix = 1;
			orbin = 20-acos(x1*fix)/double(0.15707);
			if(y1 == 0) orbin = 19;
			if(orbin > 19) orbin = 19;
			actzface[orbin][arbin] = actzface[orbin][arbin]+(1.0);
			count++;
			averagearea7 = averagearea7+area;
		}
    }
	averagearea7 = averagearea7/double(count);
    inputFile.close();
}
void Extrapolation2Dto3DFunc::find_bincontributions()
{
    DREAM3D_RANDOMNG_NEW_SEEDED(GGseed)

	int count = 0;
	double go[3][3];
	double ea1 = 0;
	double ea2 = 0;
	double ea3 = 0;
	double totalcheck = 0;
	for(int i=0;i<18;i++)
	{
		for(int j=0;j<18;j++)
		{
			for(int k=0;k<18;k++)
			{
				for(int q=0;q<20;q++)
				{
					for(int r=0;r<10;r++)
					{
						tempxface[q][r] = 0;
						tempyface[q][r] = 0;
						tempzface[q][r] = 0;
					}
				}
				double adjust5 = 0;
				double adjust6 = 0;
				double adjust7 = 0;
				double area5 = 0;
				double area6 = 0;
				double area7 = 0;
				double totalarea5 = 0;
				double totalarea6 = 0;
				double totalarea7 = 0;
				double areaadjustment = 1;
				for(int l=0;l<1088;l++)
				{
					int sbin = l%136;
					ea1 = (10*i*0.0174532)+(5*0.0174532);
					ea2 = (10*j*0.0174532)+(5*0.0174532);
					ea3 = (10*k*0.0174532)+(5*0.0174532);
					if((l/136) == 0) ea1 = ea1-(2.5*0.0174532), ea2 = ea2-(2.5*0.0174532), ea3 = ea3-(2.5*0.0174532);
					if((l/136) == 1) ea1 = ea1-(2.5*0.0174532), ea2 = ea2-(2.5*0.0174532), ea3 = ea3+(2.5*0.0174532);
					if((l/136) == 2) ea1 = ea1-(2.5*0.0174532), ea2 = ea2+(2.5*0.0174532), ea3 = ea3-(2.5*0.0174532);
					if((l/136) == 3) ea1 = ea1-(2.5*0.0174532), ea2 = ea2+(2.5*0.0174532), ea3 = ea3+(2.5*0.0174532);
					if((l/136) == 4) ea1 = ea1+(2.5*0.0174532), ea2 = ea2-(2.5*0.0174532), ea3 = ea3-(2.5*0.0174532);
					if((l/136) == 5) ea1 = ea1+(2.5*0.0174532), ea2 = ea2-(2.5*0.0174532), ea3 = ea3+(2.5*0.0174532);
					if((l/136) == 6) ea1 = ea1+(2.5*0.0174532), ea2 = ea2+(2.5*0.0174532), ea3 = ea3-(2.5*0.0174532);
					if((l/136) == 7) ea1 = ea1+(2.5*0.0174532), ea2 = ea2+(2.5*0.0174532), ea3 = ea3+(2.5*0.0174532);
					go[0][0] = cos(ea1)*cos(ea3)-sin(ea1)*sin(ea3)*cos(ea2);
					go[0][1] = sin(ea1)*cos(ea3)+cos(ea1)*sin(ea3)*cos(ea2);
					go[0][2] = sin(ea3)*sin(ea2);		
					go[1][0] = -cos(ea1)*sin(ea3)-sin(ea1)*cos(ea3)*cos(ea2);	
					go[1][1] = -sin(ea1)*sin(ea3)+cos(ea1)*cos(ea3)*cos(ea2);
					go[1][2] =  cos(ea3)*sin(ea2);	
					go[2][0] =  sin(ea1)*sin(ea2);
					go[2][1] = -cos(ea1)*sin(ea2);
					go[2][2] =  cos(ea2);
					double a1 = 1;
					double a2 = 0;
					double a3 = 0;
					double a2delta = 0;
					double a3delta = 0;
					int bin = l%136;
					a2 = elliptype[bin][0];
					a3 = elliptype[bin][1];
					a2delta = elliptype[bin][2];
					a3delta = elliptype[bin][3];
					a2 = a2 + (0.5*a2delta);
					a3 = a3 + (0.5*a3delta);
					a3 = a3*a2;
					double random3 = rg.genrand_res53();
					double selectedrad = sizeprobbin[0][1];
					for(int tempiter = 0; tempiter < 501; tempiter++)
					{
						double prob = sizeprobbin[tempiter][0];
						if(random3 > prob)
						{
							selectedrad = sizeprobbin[tempiter][1];
						}
					}
					double averatio = double(a1+a2+a3)/double(3);
					double axis1 = selectedrad*(double(a1/averatio));
					double axis2 = selectedrad*(double(a2/averatio));
					double axis3 = selectedrad*(double(a3/averatio));
					axis1 = selectedrad/pow((a1*a2*a3),0.333333333);
					axis2 = axis1*a2;
					axis3 = axis1*a3;
					a1 = 1/pow(axis1,2);
					a2 = 1/pow(axis2,2);
					a3 = 1/pow(axis3,2);
					for(int face=5;face<8;face++)
					{
						double alpha[3][1];
						double beta[3][1];
						double omega[3][1];
						double majoraxis[3][1];
						if(face == 5)
						{
							alpha[0][0] = 1;
							alpha[1][0] = 0;
							alpha[2][0] = 0;
							beta[0][0] = (alpha[0][0]*go[0][0])+(alpha[1][0]*go[1][0])+(alpha[2][0]*go[2][0]);
							beta[1][0] = (alpha[0][0]*go[0][1])+(alpha[1][0]*go[1][1])+(alpha[2][0]*go[2][1]);
							beta[2][0] = (alpha[0][0]*go[0][2])+(alpha[1][0]*go[1][2])+(alpha[2][0]*go[2][2]);
							double coefficient1 = (a2*a3*pow(beta[0][0],2))+(a1*a3*pow(beta[1][0],2))+(a1*a2*pow(beta[2][0],2));
							double coefficient2 = -(((a2+a3)*pow(beta[0][0],2))+((a1+a3)*pow(beta[1][0],2))+((a1+a2)*pow(beta[2][0],2)));
							double r1squared = (-coefficient2 + pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
							double r2squared = (-coefficient2 - pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
							omega[0][0] = beta[0][0]/(a1*r1squared-1);
							omega[1][0] = beta[1][0]/(a2*r1squared-1);
							omega[2][0] = beta[2][0]/(a3*r1squared-1);
							if((a1*r1squared-1) == 0) omega[0][0] = 0;
							if((a2*r1squared-1) == 0) omega[1][0] = 0;
							if((a3*r1squared-1) == 0) omega[2][0] = 0;
							double normalizer = pow(((omega[0][0]*omega[0][0])+(omega[1][0]*omega[1][0])+(omega[2][0]*omega[2][0])),0.5);
							omega[0][0] = omega[0][0]/normalizer;
							omega[1][0] = omega[1][0]/normalizer;
							omega[2][0] = omega[2][0]/normalizer;
							majoraxis[0][0] = (omega[0][0]*go[0][0])+(omega[1][0]*go[0][1])+(omega[2][0]*go[0][2]);
							majoraxis[1][0] = (omega[0][0]*go[1][0])+(omega[1][0]*go[1][1])+(omega[2][0]*go[1][2]);
							majoraxis[2][0] = (omega[0][0]*go[2][0])+(omega[1][0]*go[2][1])+(omega[2][0]*go[2][2]);
							double x = majoraxis[0][0];
							double y = majoraxis[1][0];
							double z = majoraxis[2][0];
							double xcomp = (y)/(pow(1,0.5)*pow((x*x+y*y+z*z),0.5));
							double ycomp = (z)/(pow(1,0.5)*pow((x*x+y*y+z*z),0.5));
							int arbin = 0;
							if(r1squared >= r2squared)
							{
								arbin = double(pow((r2squared/r1squared),0.5))/double(0.1);
							}
							if(r2squared > r1squared)
							{
								arbin = double(pow((r1squared/r2squared),0.5))/double(0.1);
							}
							if(arbin > 9) arbin = 9;
							area5 = (3.1415926535897*(pow(r1squared,0.5))*(pow(r2squared,0.5)));
							int orbin = 0;
							int fix = 1;
							if(ycomp < 0) fix = -1;
							if(ycomp > 0) fix = 1;
							orbin = 20-acos(xcomp*fix)/double(0.15707);
							if(ycomp == 0) orbin = 19;
							if(orbin > 19) orbin = 19;
							tempxface[orbin][arbin] = tempxface[orbin][arbin]+(shapeprobbin[sbin]/8.0);
							for(int iter5=0;iter5<10;iter5++)
							{
								double pt = (3.1415926535897*axis1*axis2*axis3)/area5;
								double random = rg.genrand_res53();
								double ps = pt*random;
								double sectionarea = area5*(1-((ps*ps)/(pt*pt)));
								totalarea5 = totalarea5+sectionarea;
								double adjustedsectionarea = (3.1415926535897*axis2*axis3)*(1-((ps*ps)/(pt*pt)));
								adjust5 = adjust5 + (sectionarea/adjustedsectionarea);
							}
						}
						if(face == 6)
						{
							alpha[0][0] = 0;
							alpha[1][0] = 1;
							alpha[2][0] = 0;
							beta[0][0] = (alpha[0][0]*go[0][0])+(alpha[1][0]*go[1][0])+(alpha[2][0]*go[2][0]);
							beta[1][0] = (alpha[0][0]*go[0][1])+(alpha[1][0]*go[1][1])+(alpha[2][0]*go[2][1]);
							beta[2][0] = (alpha[0][0]*go[0][2])+(alpha[1][0]*go[1][2])+(alpha[2][0]*go[2][2]);
							double coefficient1 = (a2*a3*pow(beta[0][0],2))+(a1*a3*pow(beta[1][0],2))+(a1*a2*pow(beta[2][0],2));
							double coefficient2 = -(((a2+a3)*pow(beta[0][0],2))+((a1+a3)*pow(beta[1][0],2))+((a1+a2)*pow(beta[2][0],2)));
							double r1squared = (-coefficient2 + pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
							double r2squared = (-coefficient2 - pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
							omega[0][0] = beta[0][0]/(a1*r1squared-1);
							omega[1][0] = beta[1][0]/(a2*r1squared-1);
							omega[2][0] = beta[2][0]/(a3*r1squared-1);
							if((a1*r1squared-1) == 0) omega[0][0] = 0;
							if((a2*r1squared-1) == 0) omega[1][0] = 0;
							if((a3*r1squared-1) == 0) omega[2][0] = 0;
							double normalizer = pow(((omega[0][0]*omega[0][0])+(omega[1][0]*omega[1][0])+(omega[2][0]*omega[2][0])),0.5);
							omega[0][0] = omega[0][0]/normalizer;
							omega[1][0] = omega[1][0]/normalizer;
							omega[2][0] = omega[2][0]/normalizer;
							majoraxis[0][0] = (omega[0][0]*go[0][0])+(omega[1][0]*go[0][1])+(omega[2][0]*go[0][2]);
							majoraxis[1][0] = (omega[0][0]*go[1][0])+(omega[1][0]*go[1][1])+(omega[2][0]*go[1][2]);
							majoraxis[2][0] = (omega[0][0]*go[2][0])+(omega[1][0]*go[2][1])+(omega[2][0]*go[2][2]);
							double x = majoraxis[0][0];
							double y = majoraxis[1][0];
							double z = majoraxis[2][0];
							double xcomp = (x)/(pow(1,0.5)*pow((x*x+y*y+z*z),0.5));
							double ycomp = (z)/(pow(1,0.5)*pow((x*x+y*y+z*z),0.5));
							int arbin = 0;
							if(r1squared >= r2squared)
							{
								arbin = double(pow((r2squared/r1squared),0.5))/double(0.1);
							}
							if(r2squared > r1squared)
							{
								arbin = double(pow((r1squared/r2squared),0.5))/double(0.1);
							}
							if(arbin > 9) arbin = 9;
							area6 = (3.1415926535897*(pow(r1squared,0.5))*(pow(r2squared,0.5)));
							int orbin = 0;
							int fix = 1;
							if(ycomp < 0) fix = -1;
							if(ycomp > 0) fix = 1;
							orbin = 20-acos(xcomp*fix)/double(0.15707);
							if(ycomp == 0) orbin = 19;
							if(orbin > 19) orbin = 19;
							tempyface[orbin][arbin] = tempyface[orbin][arbin]+(shapeprobbin[sbin]/8.0);
							for(int iter6=0;iter6<10;iter6++)
							{
								double pt = (3.1415926535897*axis1*axis2*axis3)/area6;
								double random = rg.genrand_res53();
								double ps = pt*random;
								double sectionarea = area6*(1-((ps*ps)/(pt*pt)));
								totalarea6 = totalarea6+sectionarea;
								double adjustedsectionarea = (3.1415926535897*axis1*axis3)*(1-((ps*ps)/(pt*pt)));
								adjust6 = adjust6 + (sectionarea/adjustedsectionarea);
							}
						}
						if(face == 7)
						{
							alpha[0][0] = 0;
							alpha[1][0] = 0;
							alpha[2][0] = 1;
							beta[0][0] = (alpha[0][0]*go[0][0])+(alpha[1][0]*go[1][0])+(alpha[2][0]*go[2][0]);
							beta[1][0] = (alpha[0][0]*go[0][1])+(alpha[1][0]*go[1][1])+(alpha[2][0]*go[2][1]);
							beta[2][0] = (alpha[0][0]*go[0][2])+(alpha[1][0]*go[1][2])+(alpha[2][0]*go[2][2]);
							double coefficient1 = (a2*a3*pow(beta[0][0],2))+(a1*a3*pow(beta[1][0],2))+(a1*a2*pow(beta[2][0],2));
							double coefficient2 = -(((a2+a3)*pow(beta[0][0],2))+((a1+a3)*pow(beta[1][0],2))+((a1+a2)*pow(beta[2][0],2)));
							double r1squared = (-coefficient2 + pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
							double r2squared = (-coefficient2 - pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
							omega[0][0] = beta[0][0]/(a1*r1squared-1);
							omega[1][0] = beta[1][0]/(a2*r1squared-1);
							omega[2][0] = beta[2][0]/(a3*r1squared-1);
							if((a1*r1squared-1) == 0) omega[0][0] = 0;
							if((a2*r1squared-1) == 0) omega[1][0] = 0;
							if((a3*r1squared-1) == 0) omega[2][0] = 0;
							double normalizer = pow(((omega[0][0]*omega[0][0])+(omega[1][0]*omega[1][0])+(omega[2][0]*omega[2][0])),0.5);
							omega[0][0] = omega[0][0]/normalizer;
							omega[1][0] = omega[1][0]/normalizer;
							omega[2][0] = omega[2][0]/normalizer;
							majoraxis[0][0] = (omega[0][0]*go[0][0])+(omega[1][0]*go[0][1])+(omega[2][0]*go[0][2]);
							majoraxis[1][0] = (omega[0][0]*go[1][0])+(omega[1][0]*go[1][1])+(omega[2][0]*go[1][2]);
							majoraxis[2][0] = (omega[0][0]*go[2][0])+(omega[1][0]*go[2][1])+(omega[2][0]*go[2][2]);
							double x = majoraxis[0][0];
							double y = majoraxis[1][0];
							double z = majoraxis[2][0];
							double xcomp = (x)/(pow(1,0.5)*pow((x*x+y*y+z*z),0.5));
							double ycomp = (y)/(pow(1,0.5)*pow((x*x+y*y+z*z),0.5));
							int arbin = 0;
							if(r1squared >= r2squared)
							{
								arbin = double(pow((r2squared/r1squared),0.5))/double(0.1);
							}
							if(r2squared > r1squared)
							{
								arbin = double(pow((r1squared/r2squared),0.5))/double(0.1);
							}
							if(arbin > 9) arbin = 9;
							area7 = (3.1415926535897*(pow(r1squared,0.5))*(pow(r2squared,0.5)));
							int orbin = 0;
							int fix = 1;
							if(ycomp < 0) fix = -1;
							if(ycomp > 0) fix = 1;
							orbin = 20-acos(xcomp*fix)/double(0.15707);
							if(ycomp == 0) orbin = 19;
							if(orbin > 19) orbin = 19;
							tempzface[orbin][arbin] = tempzface[orbin][arbin]+(shapeprobbin[sbin]/8.0);
							for(int iter7=0;iter7<10;iter7++)
							{
								double pt = (3.1415926535897*axis1*axis2*axis3)/area7;
								double random = rg.genrand_res53();
								double ps = pt*random;
								double sectionarea = area7*(1-((ps*ps)/(pt*pt)));
								totalarea7 = totalarea7+sectionarea;
								double adjustedsectionarea = (3.1415926535897*axis1*axis2)*(1-((ps*ps)/(pt*pt)));
								adjust7 = adjust7 + (sectionarea/adjustedsectionarea);
							}
						}
					}
				}
				for(int iter1=0;iter1<20;iter1++)
				{
					for(int iter2=0;iter2<10;iter2++)
					{
						eulerbindata[i][j][k][0][iter1][iter2] = tempxface[iter1][iter2];
						eulerbindata[i][j][k][1][iter1][iter2] = tempyface[iter1][iter2];
						eulerbindata[i][j][k][2][iter1][iter2] = tempzface[iter1][iter2];
					}
				}
				double simxzradratio = totalarea5/totalarea7;
				double simyzradratio = totalarea6/totalarea7;
				double simxyradratio = totalarea5/totalarea6;
				double actxzradratio = averagearea5/averagearea7;
				double actyzradratio = averagearea6/averagearea7;
				double actxyradratio = averagearea5/averagearea6;
				double adjustment1 = fabs(actxyradratio-simxyradratio)/actxyradratio;
				double adjustment2 = fabs(actxzradratio-simxzradratio)/actxzradratio;
				double adjustment3 = fabs(actyzradratio-simyzradratio)/actyzradratio;
				if(adjustment1 > 0.34) areaadjustment = areaadjustment + 1.0;
				if(adjustment1 > 0.475) areaadjustment = areaadjustment + 1.0;
				if(adjustment1 > 0.4975) areaadjustment = areaadjustment + 1.0;
				if(adjustment2 > 0.34) areaadjustment = areaadjustment + 1.0;
				if(adjustment2 > 0.475) areaadjustment = areaadjustment + 1.0;
				if(adjustment2 > 0.4975) areaadjustment = areaadjustment + 1.0;
				if(adjustment3 > 0.34) areaadjustment = areaadjustment + 1.0;
				if(adjustment3 > 0.475) areaadjustment = areaadjustment + 1.0;
				if(adjustment3 > 0.4975) areaadjustment = areaadjustment + 1.0;
				double euler2 = (10*j*0.0174532)+(5*0.0174532);
				double check = (0.174532*0.174532)*(cos((euler2-0.087266))-cos((euler2+0.087266)));
				eulerbin[i][j][k][0]=(check/(2.0*3.14159265357*3.14159265357));
				totalcheck = totalcheck + (check/(2.0*3.14159265357*3.14159265357));
				eulerbin[i][j][k][1]=areaadjustment;
				eulerbin[i][j][k][2]=adjust5/double(1088*10);
				eulerbin[i][j][k][3]=adjust6/double(1088*10);
				eulerbin[i][j][k][4]=adjust7/double(1088*10);
				eulerbin[i][j][k][2]=pow(eulerbin[i][j][k][2],0.5);
				eulerbin[i][j][k][3]=pow(eulerbin[i][j][k][3],0.5);
				eulerbin[i][j][k][4]=pow(eulerbin[i][j][k][4],0.5);
				count++;
			}
		}
	}
	double pb5m[20][10];
	double pb6m[20][10];
	double pb7m[20][10];
	int timesthrough = 0;
	double avgerror = 1;
	double avgerror1 = 1;
	double avgerror2 = 1;
	double avgerror3 = 1;
	while(((avgerror1/2.0) > 0.125 || (avgerror2/2.0) > 0.125 || (avgerror3/2.0) > 0.125) && timesthrough < 100)
	{
		timesthrough++;
		avgerror = 0;
		avgerror1 = 0;
		avgerror2 = 0;
		avgerror3 = 0;
		for(int iter1=0;iter1<20;iter1++)
		{
			for(int iter2=0;iter2<10;iter2++)
			{
				pb5m[iter1][iter2] = 0;
				pb6m[iter1][iter2] = 0;
				pb7m[iter1][iter2] = 0;
			}
		}
		double checker = 0;
		for(int n=0;n<18;n++)
		{
			for(int o=0;o<18;o++)
			{
				for(int p=0;p<18;p++)
				{
					for(int q=0;q<20;q++)
					{
						for(int r=0;r<10;r++)
						{
							pb5m[q][r] = pb5m[q][r] + (eulerbindata[n][o][p][0][q][r]*eulerbin[n][o][p][0]);
							pb6m[q][r] = pb6m[q][r] + (eulerbindata[n][o][p][1][q][r]*eulerbin[n][o][p][0]);
							pb7m[q][r] = pb7m[q][r] + (eulerbindata[n][o][p][2][q][r]*eulerbin[n][o][p][0]);
							checker = checker + (eulerbindata[n][o][p][0][q][r]*eulerbin[n][o][p][0]);
						}
					}
				}
			}
		}
		for(int i=0;i<18;i++)
		{
			for(int j=0;j<18;j++)
			{
				for(int k=0;k<18;k++)
				{	
					for(int l=0;l<20;l++)
					{
						for(int m =0;m<10;m++)
						{
							double simcurbin5 = eulerbindata[i][j][k][0][l][m];
							double simcurbin6 = eulerbindata[i][j][k][1][l][m];
							double simcurbin7 = eulerbindata[i][j][k][2][l][m];
							if(i == 0 && j == 0 && k == 0) avgerror1 = avgerror1 + (fabs(actxface[l][m]-pb5m[l][m])); 
							if(i == 0 && j == 0 && k == 0) avgerror2 = avgerror2 + (fabs(actyface[l][m]-pb6m[l][m])); 
							if(i == 0 && j == 0 && k == 0) avgerror3 = avgerror3 + (fabs(actzface[l][m]-pb7m[l][m]));
							double add1 = ((simcurbin5*eulerbin[i][j][k][0])/pb5m[l][m]);
							double add2 = ((simcurbin6*eulerbin[i][j][k][0])/pb6m[l][m]);
							double add3 = ((simcurbin7*eulerbin[i][j][k][0])/pb7m[l][m]);
							if(pb5m[l][m] == 0) add1 = 0;
							if(pb6m[l][m] == 0) add2 = 0;
							if(pb7m[l][m] == 0) add3 = 0;
							eulerbinpgo[i][j][k][0][l][m] = add1;
							eulerbinpgo[i][j][k][1][l][m] = add2;
							eulerbinpgo[i][j][k][2][l][m] = add3;
						}
					}
					eulerbin[i][j][k][0] = 0;
				}
			}
		}
		avgerror1 = pow(avgerror1,1);
		avgerror2 = pow(avgerror2,1);
		avgerror3 = pow(avgerror3,1);
		avgerror = (avgerror1+avgerror2+avgerror3)/3.0;
		avgerror = 0;
		double normalizer = 0;
		for(int i=0;i<18;i++)
		{
			for(int j=0;j<18;j++)
			{
				for(int k=0;k<18;k++)
				{	
					for(int l=0;l<20;l++)
					{
						for(int m =0;m<10;m++)
						{
							double actcurbin5 = actxface[l][m];
							double actcurbin6 = actyface[l][m];
							double actcurbin7 = actzface[l][m];
							eulerbin[i][j][k][0] = eulerbin[i][j][k][0] + (actcurbin5*eulerbinpgo[i][j][k][0][l][m]);
							eulerbin[i][j][k][0] = eulerbin[i][j][k][0] + (actcurbin6*eulerbinpgo[i][j][k][1][l][m]);
							eulerbin[i][j][k][0] = eulerbin[i][j][k][0] + (actcurbin7*eulerbinpgo[i][j][k][2][l][m]);
						}
					}
					double euler2 = (10*j*0.0174532)+(5*0.0174532);
					double check = (0.174532*0.174532)*(cos((euler2-0.087266))-cos((euler2+0.087266)));
					eulerbin[i][j][k][0] = ((eulerbin[i][j][k][0])/(3.0));
					normalizer = normalizer + eulerbin[i][j][k][0];
				}
			}
		}
		for(int i=0;i<18;i++)
		{
			for(int j=0;j<18;j++)
			{
				for(int k=0;k<18;k++)
				{	
					eulerbin[i][j][k][0] = eulerbin[i][j][k][0]/normalizer;
					if(eulerbin[i][j][k][0] > avgerror) 
					{
						avgerror = eulerbin[i][j][k][0];
					}
				}
			}
		}
	}
	totalcount = 0;
	for(int i=0;i<18;i++)
	{
		for(int j=0;j<18;j++)
		{
			for(int k=0;k<18;k++)
			{	
				double euler2 = (10*j*0.0174532)+(5*0.0174532);
				double check = (0.174532*0.174532)*(cos((euler2-0.087266))-cos((euler2+0.087266)));
				totalcount = totalcount + (eulerbin[i][j][k][0]);
				eulerbin[i][j][k][0] = totalcount;
			}
		}
	}
}

void Extrapolation2Dto3DFunc::find_pixels()
{
    DREAM3D_RANDOMNG_NEW_SEEDED(GGseed)

	for(long temp1 = 0; temp1 < 136; temp1++)
	{
		for(long temp3 = 0; temp3 < 10; temp3++)
		{
			for(int temp6 = 0; temp6 < 20; temp6++)
			{
				xshapesbin[temp1][temp3][temp6] = 0;
				yshapesbin[temp1][temp3][temp6] = 0;
				zshapesbin[temp1][temp3][temp6] = 0;
			}
		}
	}
	for(long temp4 = 0; temp4 < 136; temp4++)
	{
		for(long temp5 = 0; temp5 < 20; temp5++)
		{
			xsizebin[temp4][temp5] = 0;
			ysizebin[temp4][temp5] = 0;
			zsizebin[temp4][temp5] = 0;
		}
	}
	double go[3][3];
	for(long i = 0; i < numsections; i++)
	{
		int obin1 = 0;
		int obin2 = 0;
		int obin3 = 0;
		double orientrandom = rg.genrand_res53();
		for(int d = 0; d < 18; d++)
		{
			if((eulerbin[d+1][0][0][0]/totalcount)>orientrandom || d == 17)
			{
				for(int e = 0; e < 18; e++)
				{
					if((eulerbin[d][e+1][0][0]/totalcount)>orientrandom || e == 17)
					{
						for(int f = 0; f < 18; f++)
						{
							double density = (eulerbin[d][e][f][0]/totalcount);
							if(density < orientrandom)
							{
								obin1 = d;
								obin2 = e;
								obin3 = f;
							}
						}
					}
				}
			}
		}
		double rn1 = rg.genrand_res53();
		rn1 = rn1-0.5;
		rn1 = rn1*2.0;
		double rn2 = rg.genrand_res53();
		rn2 = rn2-0.5;
		rn2 = rn2*2.0;
		double rn3 = rg.genrand_res53();
		rn3 = rn3-0.5;
		rn3 = rn3*2.0;
		double phi = (10*0.0174532*obin1)+(5*0.0174532)+(5*rn1*0.0174532);
		double lambda = (10*0.0174532*obin2)+(5*0.0174532)+(5*rn2*0.0174532);
		double theta = (10*0.0174532*obin3)+(5*0.0174532)+(5*rn3*0.0174532);
		go[0][0] = cos(phi)*cos(theta)-sin(phi)*sin(theta)*cos(lambda);
		go[0][1] = sin(phi)*cos(theta)+cos(phi)*sin(theta)*cos(lambda);
		go[0][2] = sin(theta)*sin(lambda);		
		go[1][0] = -cos(phi)*sin(theta)-sin(phi)*cos(theta)*cos(lambda);	
		go[1][1] = -sin(phi)*sin(theta)+cos(phi)*cos(theta)*cos(lambda);
		go[1][2] =  cos(theta)*sin(lambda);	
		go[2][0] =  sin(phi)*sin(lambda);
		go[2][1] = -cos(phi)*sin(lambda);
		go[2][2] =  cos(lambda);
		double axis1 = 0;
		double axis2 = 0;
		double axis3 = 0;
		double bovera = 0;
		double coverb = 0;
		double random1 = rg.genrand_res53();
		double random2 = rg.genrand_res53();
		double random3 = rg.genrand_res53();
		double ar1 = 0;
		double ar2 = 0;
		double ar1delta = 0;
		double ar2delta = 0;
		int axisratiobin = i/(numsections/136);
		ar1 = elliptype[axisratiobin][0];
		ar2 = elliptype[axisratiobin][1];
		ar1delta = elliptype[axisratiobin][2];
		ar2delta = elliptype[axisratiobin][3];
		ar1 = ar1 + (random1*ar1delta);
		ar2 = ar2 + (random2*ar2delta);
		ar2 = ar1*ar2;
		double selectedrad = 0;
		for(int tempiter = 0; tempiter < 501; tempiter++)
		{
			double prob = sizeprobbin[tempiter][0];
			if(random3 > prob)
			{
				selectedrad = sizeprobbin[tempiter][1];
			}
		}
		double averatio = double(1+ar1+ar2)/double(3);
		axis1 = selectedrad*(double(1/averatio));
		axis2 = selectedrad*(double(ar1/averatio));
		axis3 = selectedrad*(double(ar2/averatio));
		axis1 = selectedrad/pow((1*ar1*ar2),0.333333333);
		axis2 = axis1*ar1;
		axis3 = axis1*ar2;
		bovera = axis2/axis1;
		coverb = axis3/axis2;
		double a1 = 1/pow(axis1,2);
		double a2 = 1/pow(axis2,2);
		double a3 = 1/pow(axis3,2);
		averageselectedrad = averageselectedrad + selectedrad;
		for(int slice = 0; slice < 3; slice++)
		{
			int aspectratiobin = 0;
			double area= 0;
			double alpha[3][1];
			double beta[3][1];
			if(slice == 0)
			{
				alpha[0][0] = 1;
				alpha[1][0] = 0;
				alpha[2][0] = 0;
				beta[0][0] = (alpha[0][0]*go[0][0])+(alpha[1][0]*go[1][0])+(alpha[2][0]*go[2][0]);
				beta[1][0] = (alpha[0][0]*go[0][1])+(alpha[1][0]*go[1][1])+(alpha[2][0]*go[2][1]);
				beta[2][0] = (alpha[0][0]*go[0][2])+(alpha[1][0]*go[1][2])+(alpha[2][0]*go[2][2]);
				double coefficient1 = (a2*a3*pow(beta[0][0],2))+(a1*a3*pow(beta[1][0],2))+(a1*a2*pow(beta[2][0],2));
				double coefficient2 = -(((a2+a3)*pow(beta[0][0],2))+((a1+a3)*pow(beta[1][0],2))+((a1+a2)*pow(beta[2][0],2)));
				double r1squared = (-coefficient2 + pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
				double r2squared = (-coefficient2 - pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
				if(r1squared >= r2squared)
				{
					aspectratiobin = 19*double(pow((r2squared/r1squared),0.5));
					if(aspectratiobin > 19)aspectratiobin = 19;
				}
				if(r2squared > r1squared)
				{
					aspectratiobin = 19*double(pow((r1squared/r2squared),0.5));
					if(aspectratiobin > 19)aspectratiobin = 19;
				}
				double centerarea = (3.1415926535897*(pow(r1squared,0.5))*(pow(r2squared,0.5)));
				double pt = (3.1415926535897*axis1*axis2*axis3)/centerarea;
				double random = rg.genrand_res53();
				double ps = pt*random;
				area = centerarea*(1-((ps*ps)/(pt*pt)));
			}
			if(slice == 1)
			{
				alpha[0][0] = 0;
				alpha[1][0] = 1;
				alpha[2][0] = 0;
				beta[0][0] = (alpha[0][0]*go[0][0])+(alpha[1][0]*go[1][0])+(alpha[2][0]*go[2][0]);
				beta[1][0] = (alpha[0][0]*go[0][1])+(alpha[1][0]*go[1][1])+(alpha[2][0]*go[2][1]);
				beta[2][0] = (alpha[0][0]*go[0][2])+(alpha[1][0]*go[1][2])+(alpha[2][0]*go[2][2]);
				double coefficient1 = (a2*a3*pow(beta[0][0],2))+(a1*a3*pow(beta[1][0],2))+(a1*a2*pow(beta[2][0],2));
				double coefficient2 = -(((a2+a3)*pow(beta[0][0],2))+((a1+a3)*pow(beta[1][0],2))+((a1+a2)*pow(beta[2][0],2)));
				double r1squared = (-coefficient2 + pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
				double r2squared = (-coefficient2 - pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
				if(r1squared >= r2squared)
				{
					aspectratiobin = 19*double(pow((r2squared/r1squared),0.5));
					if(aspectratiobin > 19)aspectratiobin = 19;
				}
				if(r2squared > r1squared)
				{
					aspectratiobin = 19*double(pow((r1squared/r2squared),0.5));
					if(aspectratiobin > 19)aspectratiobin = 19;
				}
				double centerarea = (3.1415926535897*(pow(r1squared,0.5))*(pow(r2squared,0.5)));
				double pt = (3.1415926535897*axis1*axis2*axis3)/centerarea;
				double random = rg.genrand_res53();
				double ps = pt*random;
				area = centerarea*(1-((ps*ps)/(pt*pt)));
			}
			if(slice == 2)
			{
				alpha[0][0] = 0;
				alpha[1][0] = 0;
				alpha[2][0] = 1;
				beta[0][0] = (alpha[0][0]*go[0][0])+(alpha[1][0]*go[1][0])+(alpha[2][0]*go[2][0]);
				beta[1][0] = (alpha[0][0]*go[0][1])+(alpha[1][0]*go[1][1])+(alpha[2][0]*go[2][1]);
				beta[2][0] = (alpha[0][0]*go[0][2])+(alpha[1][0]*go[1][2])+(alpha[2][0]*go[2][2]);
				double coefficient1 = (a2*a3*pow(beta[0][0],2))+(a1*a3*pow(beta[1][0],2))+(a1*a2*pow(beta[2][0],2));
				double coefficient2 = -(((a2+a3)*pow(beta[0][0],2))+((a1+a3)*pow(beta[1][0],2))+((a1+a2)*pow(beta[2][0],2)));
				double r1squared = (-coefficient2 + pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
				double r2squared = (-coefficient2 - pow(((coefficient2*coefficient2)-(4*coefficient1*1)),0.5))/(2*coefficient1);
				if(r1squared >= r2squared)
				{
					aspectratiobin = 19*double(pow((r2squared/r1squared),0.5));
					if(aspectratiobin > 19)aspectratiobin = 19;
				}
				if(r2squared > r1squared)
				{
					aspectratiobin = 19*double(pow((r1squared/r2squared),0.5));
					if(aspectratiobin > 19)aspectratiobin = 19;
				}
				double centerarea = (3.1415926535897*(pow(r1squared,0.5))*(pow(r2squared,0.5)));
				double pt = (3.1415926535897*axis1*axis2*axis3)/centerarea;
				double random = rg.genrand_res53();
				double ps = pt*random;
				area = centerarea*(1-((ps*ps)/(pt*pt)));
			}
			avgarea = avgarea + area;
			double avgrad = pow((area/3.1415926535897),0.5);
			double radratio = avgrad/6.11;
			double maxrad = pow((axis1*axis2),0.5);
			double arearatio2 = avgrad/maxrad;
			long radbin2 = 0;
			if(arearatio2 >= 0.95) radbin2 = 0;
			if(arearatio2 >= 0.9 && arearatio2 < 0.95) radbin2 = 1;
			if(arearatio2 >= 0.85 && arearatio2 < 0.9) radbin2 = 2;
			if(arearatio2 >= 0.8 && arearatio2 < 0.85) radbin2 = 3;
			if(arearatio2 >= 0.75 && arearatio2 < 0.8) radbin2 = 4;
			if(arearatio2 >= 0.7 && arearatio2 < 0.75) radbin2 = 5;
			if(arearatio2 >= 0.65 && arearatio2 < 0.7) radbin2 = 6;
			if(arearatio2 >= 0.6 && arearatio2 < 0.65) radbin2 = 7;
			if(arearatio2 >= 0.55 && arearatio2 < 0.6) radbin2 = 8;
			if(arearatio2 >= 0.5 && arearatio2 < 0.55) radbin2 = 9;
			if(arearatio2 >= 0.45 && arearatio2 < 0.5) radbin2 = 10;
			if(arearatio2 >= 0.4 && arearatio2 < 0.45) radbin2 = 11;
			if(arearatio2 >= 0.35 && arearatio2 < 0.4) radbin2 = 12;
			if(arearatio2 >= 0.3 && arearatio2 < 0.35) radbin2 = 13;
			if(arearatio2 >= 0.25 && arearatio2 < 0.3) radbin2 = 14;
			if(arearatio2 >= 0.2 && arearatio2 < 0.25) radbin2 = 15;
			if(arearatio2 >= 0.15 && arearatio2 < 0.2) radbin2 = 16;
			if(arearatio2 >= 0.1 && arearatio2 < 0.15) radbin2 = 17;
			if(arearatio2 >= 0.5 && arearatio2 < 0.1) radbin2 = 18;
			if(arearatio2 >= 0 && arearatio2 < 0.05) radbin2 = 19;
			if(slice == 0)
			{
				int radbin = double(radratio/0.125);
				if(radbin > 19) radbin = 19;
				xshapesbin[axisratiobin][aspectratiobin][radbin]++;
				xsizebin[axisratiobin][radbin2]++;
			}
			if(slice == 1)
			{
				int radbin = double(radratio/0.125);
				if(radbin > 19) radbin = 19;
				yshapesbin[axisratiobin][aspectratiobin][radbin]++;
				ysizebin[axisratiobin][radbin2]++;
			}
			if(slice == 2)
			{
				int radbin = double(radratio/0.125);
				if(radbin > 19) radbin = 19;
				zshapesbin[axisratiobin][aspectratiobin][radbin]++;
				zsizebin[axisratiobin][radbin2]++;
			}
		}
	}
	for(long temp7 = 0; temp7 < 136; temp7++)
	{
		for(long temp11 = 0; temp11 < 20; temp11++)
		{
			for(int temp12 = 0; temp12 < 20; temp12++)
			{
				int axisratiobin = temp7;
				int aspectratiobin = temp11;
				int arearatiobin = temp12;
				if(arearatiobin == 0)
				{
					xshapesbin[axisratiobin][aspectratiobin][arearatiobin] = xshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					yshapesbin[axisratiobin][aspectratiobin][arearatiobin] = yshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					zshapesbin[axisratiobin][aspectratiobin][arearatiobin] = zshapesbin[axisratiobin][aspectratiobin][arearatiobin];
				}
				if(arearatiobin > 0)
				{
					xshapesbin[axisratiobin][aspectratiobin][arearatiobin] = xshapesbin[axisratiobin][aspectratiobin][arearatiobin-1] + xshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					yshapesbin[axisratiobin][aspectratiobin][arearatiobin] = yshapesbin[axisratiobin][aspectratiobin][arearatiobin-1] + yshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					zshapesbin[axisratiobin][aspectratiobin][arearatiobin] = zshapesbin[axisratiobin][aspectratiobin][arearatiobin-1] + zshapesbin[axisratiobin][aspectratiobin][arearatiobin];
				}
				if(axisratiobin == 0)
				{
					xshapesbin3[aspectratiobin][arearatiobin][axisratiobin] = xshapesbin[axisratiobin][aspectratiobin][arearatiobin];	
					yshapesbin3[aspectratiobin][arearatiobin][axisratiobin] = yshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					zshapesbin3[aspectratiobin][arearatiobin][axisratiobin] = zshapesbin[axisratiobin][aspectratiobin][arearatiobin];
				}
				if(axisratiobin > 0)
				{
					xshapesbin3[aspectratiobin][arearatiobin][axisratiobin] = xshapesbin3[aspectratiobin][arearatiobin][axisratiobin-1] + xshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					yshapesbin3[aspectratiobin][arearatiobin][axisratiobin] = yshapesbin3[aspectratiobin][arearatiobin][axisratiobin-1] + yshapesbin[axisratiobin][aspectratiobin][arearatiobin];
					zshapesbin3[aspectratiobin][arearatiobin][axisratiobin] = zshapesbin3[aspectratiobin][arearatiobin][axisratiobin-1] + zshapesbin[axisratiobin][aspectratiobin][arearatiobin];
				}
				xshapesbin2[axisratiobin][aspectratiobin][arearatiobin] = xshapesbin[axisratiobin][aspectratiobin][arearatiobin];
				yshapesbin2[axisratiobin][aspectratiobin][arearatiobin] = yshapesbin[axisratiobin][aspectratiobin][arearatiobin];
				zshapesbin2[axisratiobin][aspectratiobin][arearatiobin] = zshapesbin[axisratiobin][aspectratiobin][arearatiobin];
			}
		}
	}
	for(long temp9 = 0; temp9 < 136; temp9++)
	{
		for(long temp10 = 0; temp10 < 20; temp10++)
		{
			int axisratiobin = temp9;
			int geoclassdiv = temp10;
			if(geoclassdiv == 0)
			{
				xsizebin[axisratiobin][geoclassdiv] = xsizebin[axisratiobin][geoclassdiv];
				ysizebin[axisratiobin][geoclassdiv] = ysizebin[axisratiobin][geoclassdiv];
				zsizebin[axisratiobin][geoclassdiv] = zsizebin[axisratiobin][geoclassdiv];
			}
			if(geoclassdiv > 0)
			{
				xsizebin[axisratiobin][geoclassdiv] = xsizebin[axisratiobin][geoclassdiv-1] + xsizebin[axisratiobin][geoclassdiv];
				ysizebin[axisratiobin][geoclassdiv] = ysizebin[axisratiobin][geoclassdiv-1] + ysizebin[axisratiobin][geoclassdiv];
				zsizebin[axisratiobin][geoclassdiv] = zsizebin[axisratiobin][geoclassdiv-1] + zsizebin[axisratiobin][geoclassdiv];
			}
		}
	}
	for(int k = 0; k < 136; k++)
	{
		double xnormalizer = 0;
		double ynormalizer = 0;
		double znormalizer = 0;
		for(int r = 0; r < 20; r++)
		{
			xnormalizer = xnormalizer + xshapesbin[k][r][19];
			ynormalizer = ynormalizer + yshapesbin[k][r][19];
			znormalizer = znormalizer + zshapesbin[k][r][19];
		}
		for(int l = 0; l < 20; l++)
		{
			for(int m = 0; m < 20; m++)
			{
				xshapesbin2[k][l][m] = xshapesbin2[k][l][m]/xnormalizer;
				yshapesbin2[k][l][m] = yshapesbin2[k][l][m]/ynormalizer;
				zshapesbin2[k][l][m] = zshapesbin2[k][l][m]/znormalizer;
				if(xnormalizer == 0) xshapesbin2[k][l][m] = 0;
				if(ynormalizer == 0) yshapesbin2[k][l][m] = 0;
				if(znormalizer == 0) zshapesbin2[k][l][m] = 0;
			}
		}
	}
	for(int n = 0; n < 136; n++)
	{
		double xnormalizer2 = xsizebin[n][19];
		double ynormalizer2 = ysizebin[n][19];
		double znormalizer2 = zsizebin[n][19];
		for(int p = 0; p < 20; p++)
		{
			xsizebin[n][p] = xsizebin[n][p]/xnormalizer2;
			ysizebin[n][p] = ysizebin[n][p]/ynormalizer2;
			zsizebin[n][p] = zsizebin[n][p]/znormalizer2;
			if(xnormalizer2 == 0) xsizebin[n][p] = 0;
			if(ynormalizer2 == 0) ysizebin[n][p] = 0;
			if(znormalizer2 == 0) zsizebin[n][p] = 0;
		}
	}
	for(int u = 0; u < 20; u++)
	{
		for(int s = 0; s < 20; s++)
		{
			double xnormalizer3 = xshapesbin3[u][s][135];
			double ynormalizer3 = yshapesbin3[u][s][135];
			double znormalizer3 = zshapesbin3[u][s][135];
			for(int t = 0; t < 136; t++)
			{
				xshapesbin3[u][s][t] = xshapesbin3[u][s][t]/xnormalizer3;
				yshapesbin3[u][s][t] = yshapesbin3[u][s][t]/ynormalizer3;
				zshapesbin3[u][s][t] = zshapesbin3[u][s][t]/znormalizer3;
				if(xnormalizer3 == 0) xshapesbin3[u][s][t] = 0;
				if(ynormalizer3 == 0) yshapesbin3[u][s][t] = 0;
				if(znormalizer3 == 0) zshapesbin3[u][s][t] = 0;
			}
		}
	}
}

