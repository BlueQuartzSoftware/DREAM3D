
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

void Extrapolation2Dto3DFunc::generate_3dgrains()
{
	for(int ix = 0; ix < numtwodimxgrains; ix++)
	{
		double grainarea = twodimxgrain[ix].grainarea;
		double majoraxis = twodimxgrain[ix].majoraxis;
		double minoraxis = twodimxgrain[ix].minoraxis;
		int nnum = twodimxgrain[ix].neighnum;
		double aspectratio = minoraxis/majoraxis;
		double avgrad = pow((grainarea/3.1415926535897),0.5);
		double averagerad = pow((avgxarea/3.1415926535897),0.5);
		double arearatio = avgrad/5.98;
		int aspectratiobin = 20*double(aspectratio);
		int arearatiobin = double(arearatio/0.125);
		if(aspectratiobin > 19) aspectratiobin = 19;
		if(arearatiobin > 19) arearatiobin = 19;
		xshapescount[aspectratiobin][arearatiobin] = xshapescount[aspectratiobin][arearatiobin]+(1.0/numtwodimxgrains);
	}
	for(int iy = 0; iy < numtwodimygrains; iy++)
	{
		double grainarea = twodimygrain[iy].grainarea;
		double majoraxis = twodimygrain[iy].majoraxis;
		double minoraxis = twodimygrain[iy].minoraxis;
		int nnum = twodimygrain[iy].neighnum;
		double aspectratio = minoraxis/majoraxis;
		double avgrad = pow((grainarea/3.1415926535897),0.5);
		double averagerad = pow((avgyarea/3.1415926535897),0.5);
		double arearatio = avgrad/5.98;
		int aspectratiobin = 20*double(aspectratio);
		int arearatiobin = double(arearatio/0.125);
		if(aspectratiobin > 19) aspectratiobin = 19;
		if(arearatiobin > 19) arearatiobin = 19;
		yshapescount[aspectratiobin][arearatiobin] = yshapescount[aspectratiobin][arearatiobin]+(1.0/numtwodimygrains);
	}
	for(int iz = 0; iz < numtwodimzgrains; iz++)
	{
		double grainarea = twodimzgrain[iz].grainarea;
		double majoraxis = twodimzgrain[iz].majoraxis;
		double minoraxis = twodimzgrain[iz].minoraxis;
		int nnum = twodimzgrain[iz].neighnum;
		double aspectratio = minoraxis/majoraxis;
		double avgrad = pow((grainarea/3.1415926535897),0.5);
		double averagerad = pow((avgzarea/3.1415926535897),0.5);
		double arearatio = avgrad/5.98;
		int aspectratiobin = 20*double(aspectratio);
		int arearatiobin = double(arearatio/0.125);
		if(aspectratiobin > 19) aspectratiobin = 19;
		if(arearatiobin > 19) arearatiobin = 19;
		zshapescount[aspectratiobin][arearatiobin] = zshapescount[aspectratiobin][arearatiobin]+(1.0/numtwodimzgrains);
	}
	double cur1 = 0;
	double cur2 = 0;
	int place = 0;
	double binerror = 0;
	for(int iter=0;iter<136;iter++)
	{
		tempgrainscount[iter] = 1.0/136.0;
	}
	int timesthrough = 0;
	double toterror = 0;
	double avgerror = 1;
	double avgerror1 = 1;
	double avgerror2 = 1;
	double avgerror3 = 1;
	while(((avgerror1/2.0) > 0.1 || (avgerror2/2.0) > 0.1 || (avgerror3/2.0) > 0.1) && timesthrough < 5)
	{
		timesthrough++;
		avgerror = 0;
		avgerror1 = 0;
		avgerror2 = 0;
		avgerror3 = 0;
		for(int b = 0; b < 20; b++)
		{
			for(int c = 0; c < 20; c++)
			{
				xpb[b][c] = 0;
				ypb[b][c] = 0;
				zpb[b][c] = 0;
			}
		}
		for(int d=0;d<136;d++)
		{
			for(int b = 0; b < 20; b++)
			{
				for(int c = 0; c < 20; c++)
				{
					xpb[b][c] = xpb[b][c] + (xshapesbin2[d][b][c]*tempgrainscount[d]);
					ypb[b][c] = ypb[b][c] + (yshapesbin2[d][b][c]*tempgrainscount[d]);
					zpb[b][c] = zpb[b][c] + (zshapesbin2[d][b][c]*tempgrainscount[d]);
				}
			}
		}
		for(int a = 0; a < 136; a++)
		{
			for(int b = 0; b < 20; b++)
			{
				for(int c = 0; c < 20; c++)
				{
					double simcurbinx = xshapesbin2[a][b][c];
					double simcurbiny = yshapesbin2[a][b][c];
					double simcurbinz = zshapesbin2[a][b][c];
					if(a == 0) avgerror1 = avgerror1 + (fabs(xshapescount[b][c]-xpb[b][c]));
					if(a == 0) avgerror2 = avgerror2 + (fabs(yshapescount[b][c]-ypb[b][c]));
					if(a == 0) avgerror3 = avgerror3 + (fabs(zshapescount[b][c]-zpb[b][c]));
					if(xpb[b][c]==0) xpb[b][c]=1;
					if(ypb[b][c]==0) ypb[b][c]=1;
					if(zpb[b][c]==0) zpb[b][c]=1;
					temppsgox[a][b][c] = ((simcurbinx*tempgrainscount[a])/xpb[b][c]);
					temppsgoy[a][b][c] = ((simcurbiny*tempgrainscount[a])/ypb[b][c]);
					temppsgoz[a][b][c] = ((simcurbinz*tempgrainscount[a])/zpb[b][c]);
				}
			}
			tempgrainscount[a] = 0;
		}
		avgerror1 = pow(avgerror1,1);
		avgerror2 = pow(avgerror2,1);
		avgerror3 = pow(avgerror3,1);
		avgerror = (avgerror1+avgerror2+avgerror3)/3.0;
		double normalizer = 0;
		for(int f = 0; f < 136; f++)
		{
			for(int b = 0; b < 20; b++)
			{
				for(int c = 0; c < 20; c++)
				{
					double actcurbinx = xshapescount[b][c];
					double actcurbiny = yshapescount[b][c];
					double actcurbinz = zshapescount[b][c];
					tempgrainscount[f] = tempgrainscount[f] + (actcurbinx*temppsgox[f][b][c]);
					tempgrainscount[f] = tempgrainscount[f] + (actcurbiny*temppsgoy[f][b][c]);
					tempgrainscount[f] = tempgrainscount[f] + (actcurbinz*temppsgoz[f][b][c]);
				}
			}
			tempgrainscount[f] = (tempgrainscount[f])/3.0;
			normalizer = normalizer + tempgrainscount[f];
		}
		for(int f=0;f<136;f++)
		{
			tempgrainscount[f] = tempgrainscount[f]/normalizer;
		}
	}
	int xchecker = 0;
	int ychecker = 0;
	int zchecker = 0;
	for(int g = 0; g < 136; g++)
	{
		xgrainscount[g] = double(tempgrainscount[g]*numtwodimxgrains);
		ygrainscount[g] = double(tempgrainscount[g]*numtwodimygrains);
		zgrainscount[g] = double(tempgrainscount[g]*numtwodimzgrains);
		xchecker = xchecker+xgrainscount[g];
		ychecker = ychecker+ygrainscount[g];
		zchecker = zchecker+zgrainscount[g];
	}
	int alreadyusedx[136];
	int alreadyusedy[136];
	int alreadyusedz[136];
	while(xchecker < numtwodimxgrains)
	{
		int bestremainder=0;
		int bestbin = 0;
		for(int h=0;h<136;h++)
		{
			int remainder = int((tempgrainscount[h]*numtwodimxgrains)*100)%100;
			if(remainder > bestremainder && alreadyusedx[h] != 1) bestbin = h, bestremainder = remainder;
		}
		xgrainscount[bestbin]++;
		alreadyusedx[bestbin]=1;
		xchecker=xchecker+1;
	}
	while(ychecker < numtwodimygrains)
	{
		int bestremainder=0;
		int bestbin = 0;
		for(int h=0;h<136;h++)
		{
			int remainder = int((tempgrainscount[h]*numtwodimygrains)*100)%100;
			if(remainder > bestremainder && alreadyusedy[h] != 1) bestbin = h, bestremainder = remainder;
		}
		ygrainscount[bestbin]++;
		alreadyusedy[bestbin]=1;
		ychecker=ychecker+1;
	}
	while(zchecker < numtwodimzgrains)
	{
		int bestremainder=0;
		int bestbin = 0;
		for(int h=0;h<136;h++)
		{
			int remainder = int((tempgrainscount[h]*numtwodimzgrains)*100)%100;
			if(remainder > bestremainder && alreadyusedz[h] != 1) bestbin = h, bestremainder = remainder;
		}
		zgrainscount[bestbin]++;
		alreadyusedz[bestbin]=1;
		zchecker=zchecker+1;
	}
	int taken = 1;
	int xtotalgraincounter = 0;
	int ytotalgraincounter = 0;
	int ztotalgraincounter = 0;
	int xforcomparing = 0;
	int yforcomparing = 0;
	int zforcomparing = 0;
	for(int temp9 = 0; temp9 < 136; temp9++)
	{
		int xubercounter = 0;
		int yubercounter = 0;
		int zubercounter = 0;
		xforcomparing = xforcomparing + xgrainscount[temp9];
		yforcomparing = yforcomparing + ygrainscount[temp9];
		zforcomparing = zforcomparing + zgrainscount[temp9];
		int xclasscount = xgrainscount[temp9];
		int yclasscount = ygrainscount[temp9];
		int zclasscount = zgrainscount[temp9];
		double denom = 0;
		for(int temp10 = 0; temp10 < 20; temp10++)
		{
			int xnumgrainstopick = 0;
			int ynumgrainstopick = 0;
			int znumgrainstopick = 0;
			double xprob = 0;
			double yprob = 0;
			double zprob = 0;
			xprob = xshapesbin2[temp9][temp10][19];
			yprob = yshapesbin2[temp9][temp10][19];
			zprob = zshapesbin2[temp9][temp10][19];
			xnumgrainstopick = double(xprob)*xclasscount;
			ynumgrainstopick = double(yprob)*yclasscount;
			znumgrainstopick = double(zprob)*zclasscount;
			if(temp10 == 19) xnumgrainstopick = xclasscount-xubercounter;
			if(temp10 == 19) ynumgrainstopick = yclasscount-yubercounter;
			if(temp10 == 19) znumgrainstopick = zclasscount-zubercounter;
			int xcounter = 0;
			int ycounter = 0;
			int zcounter = 0;
			int xthroughonce = 0;
			int ythroughonce = 0;
			int zthroughonce = 0;
			while(xcounter < xnumgrainstopick)
			{
				for(int temp11 = 0; temp11 < numtwodimxgrains; temp11++)
				{
					int used = twodimxgrain[temp11].used;
					double grainarea = twodimxgrain[temp11].grainarea;
					double majoraxis = twodimxgrain[temp11].majoraxis;
					double minoraxis = twodimxgrain[temp11].minoraxis;
					int nnum = twodimxgrain[temp11].neighnum;
					double aspectratio = minoraxis/majoraxis;
					int aspectratiobin = 20*double(aspectratio);
					if((aspectratiobin == temp10 && used != 1) || (aspectratiobin <= temp10 && used != 1 && xthroughonce == 1) || ((aspectratiobin-temp10) == 1 && xthroughonce == 2 && used != 1) || ((aspectratiobin-temp10) == 2 && xthroughonce == 3 && used != 1) || ((aspectratiobin-temp10) == 3 && xthroughonce == 4 && used != 1) || (xthroughonce > 4 && used != 1))
					{
						xgrains[temp9][xubercounter+xcounter][0]=temp11;
						twodimxgrain[temp11].set_taken(taken);
						xcounter++;
						xtotalgraincounter++;
						{break;}
					}
					if(temp11 == (numtwodimxgrains-1))
					{
						xthroughonce++;
					}
				}
			}
			while(ycounter < ynumgrainstopick)
			{
				for(int temp11 = 0; temp11 < numtwodimygrains; temp11++)
				{
					int used = twodimygrain[temp11].used;
					double grainarea = twodimygrain[temp11].grainarea;
					double majoraxis = twodimygrain[temp11].majoraxis;
					double minoraxis = twodimygrain[temp11].minoraxis;
					int nnum = twodimygrain[temp11].neighnum;
					double aspectratio = minoraxis/majoraxis;
					int aspectratiobin = 20*double(aspectratio);
					if((aspectratiobin == temp10 && used != 1) || (aspectratiobin <= temp10 && used != 1 && ythroughonce == 1) || ((aspectratiobin-temp10) == 1 && ythroughonce == 2 && used != 1) || ((aspectratiobin-temp10) == 2 && ythroughonce == 3 && used != 1) || ((aspectratiobin-temp10) == 3 && ythroughonce == 4 && used != 1) || (ythroughonce > 4 && used != 1))
					{
						ygrains[temp9][yubercounter+ycounter][0]=temp11;
						twodimygrain[temp11].set_taken(taken);
						ycounter++;
						ytotalgraincounter++;
						{break;}
					}
					if(temp11 == (numtwodimygrains-1))
					{
						ythroughonce++;
					}
				}
			}
			while(zcounter < znumgrainstopick)
			{
				for(int temp11 = 0; temp11 < numtwodimzgrains; temp11++)
				{
					int used = twodimzgrain[temp11].used;
					double grainarea = twodimzgrain[temp11].grainarea;
					double majoraxis = twodimzgrain[temp11].majoraxis;
					double minoraxis = twodimzgrain[temp11].minoraxis;
					int nnum = twodimzgrain[temp11].neighnum;
					double aspectratio = minoraxis/majoraxis;
					int aspectratiobin = 20*double(aspectratio);
					if((aspectratiobin == temp10 && used != 1) || (aspectratiobin <= temp10 && used != 1 && zthroughonce == 1) || ((aspectratiobin-temp10) == 1 && zthroughonce == 2 && used != 1) || ((aspectratiobin-temp10) == 2 && zthroughonce == 3 && used != 1) || ((aspectratiobin-temp10) == 3 && zthroughonce == 4 && used != 1) || (zthroughonce > 4 && used != 1))
					{
						zgrains[temp9][zubercounter+zcounter][0]=temp11;
						twodimzgrain[temp11].set_taken(taken);
						zcounter++;
						ztotalgraincounter++;
						{break;}
					}
					if(temp11 == (numtwodimzgrains-1))
					{
						zthroughonce++;
					}
				}
			}
			xubercounter = xubercounter+xcounter;
			yubercounter = yubercounter+ycounter;
			zubercounter = zubercounter+zcounter;
		}
	}
	int threedcounter = 0;
	for(int l = 0; l < 136; l++)
	{
		int xclasscount = xgrainscount[l];
		int yclasscount = ygrainscount[l];
		int zclasscount = zgrainscount[l];
		int xubercounter = 0;
		int yubercounter = 0;
		int zubercounter = 0;
		for(int k = 0; k < 20; k++)
		{
			double xprob2 = 0;
			double yprob2 = 0;
			double zprob2 = 0;
			if(k == 0) xprob2 = xsizebin[l][k];
			if(k == 0) yprob2 = ysizebin[l][k];
			if(k == 0) zprob2 = zsizebin[l][k];
			if(k > 0) xprob2 = xsizebin[l][k]-xsizebin[l][k-1];
			if(k > 0) yprob2 = ysizebin[l][k]-ysizebin[l][k-1];
			if(k > 0) zprob2 = zsizebin[l][k]-zsizebin[l][k-1];
			double bovera = 0;
			double coverb = 0;
			double a1 = 0;
			double a2 = 0;
			double a1delta = 0;
			double a2delta = 0;
			a1 = elliptype[l][0];
			a2 = elliptype[l][1];
			a1delta = elliptype[l][2];
			a2delta = elliptype[l][3];
			int xnumgrainstopick = 0;
			int ynumgrainstopick = 0;
			int znumgrainstopick = 0;
			if(k < 19) xnumgrainstopick = ((double(xprob2)*xclasscount)+0.95);
			if(k < 19) ynumgrainstopick = ((double(yprob2)*yclasscount)+0.95);
			if(k < 19) znumgrainstopick = ((double(zprob2)*zclasscount)+0.95);
			if(k == 19) xnumgrainstopick = xclasscount-xubercounter;
			if(k == 19) ynumgrainstopick = yclasscount-yubercounter;
			if(k == 19) znumgrainstopick = zclasscount-zubercounter;
			if(xubercounter == xclasscount) xnumgrainstopick = 0;
			if(yubercounter == yclasscount) ynumgrainstopick = 0;
			if(zubercounter == zclasscount) znumgrainstopick = 0;
			if((xubercounter+xnumgrainstopick) > xclasscount) xnumgrainstopick = xclasscount-xubercounter;
			if((yubercounter+ynumgrainstopick) > yclasscount) ynumgrainstopick = yclasscount-yubercounter;
			if((zubercounter+znumgrainstopick) > zclasscount) znumgrainstopick = zclasscount-zubercounter;
			int xcounter = 0;
			int ycounter = 0;
			int zcounter = 0;
			while(xcounter < xnumgrainstopick)
			{
				double curmaxarea = 0;
				int curgrain;
				int grainpicked;
				for(int iterator = 0; iterator < xclasscount; iterator++)
				{
					int alreadypicked = xgrains[l][iterator][1];
					int gnum = xgrains[l][iterator][0];
					double area = twodimxgrain[gnum].grainarea;
					if(area > curmaxarea && alreadypicked != 1)
					{
						curgrain = gnum;
						grainpicked = iterator;
						curmaxarea = area;
					}
				}
				double randomnum1 = rg.genrand_res53();
				bovera = a1+(randomnum1*a1delta);
				if(bovera > 1) bovera = 1;
				double randomnum2 = rg.genrand_res53();
				coverb = a2+(randomnum2*a2delta);
				if(coverb > 1) coverb = 1;
				double adjust = 0;
				double adjustdelta = 0;
				if(k == 0) adjust = (0.95);
				if(k == 1) adjust = (0.9);
				if(k == 2) adjust = (0.85);
				if(k == 3) adjust = (0.8);
				if(k == 4) adjust = (0.75);
				if(k == 5) adjust = (0.7);
				if(k == 6) adjust = (0.65);
				if(k == 7) adjust = (0.6);
				if(k == 8) adjust = (0.55);
				if(k == 9) adjust = (0.5);
				if(k == 10) adjust = (0.45);
				if(k == 11) adjust = (0.4);
				if(k == 12) adjust = (0.35);
				if(k == 13) adjust = (0.3);
				if(k == 14) adjust = (0.25);
				if(k == 15) adjust = (0.2);
				if(k == 16) adjust = (0.15);
				if(k == 17) adjust = (0.1);
				if(k == 18) adjust = (0.05);
				if(k == 19) adjust = (0);
				if(k == 0) adjustdelta = (0.05);
				if(k == 1) adjustdelta = (0.05);
				if(k == 2) adjustdelta = (0.05);
				if(k == 3) adjustdelta = (0.05);
				if(k == 4) adjustdelta = (0.05);
				if(k == 5) adjustdelta = (0.05);
				if(k == 6) adjustdelta = (0.05);
				if(k == 7) adjustdelta = (0.05);
				if(k == 8) adjustdelta = (0.05);
				if(k == 9) adjustdelta = (0.05);
				if(k == 10) adjustdelta = (0.05);
				if(k == 11) adjustdelta = (0.05);
				if(k == 12) adjustdelta = (0.05);
				if(k == 13) adjustdelta = (0.05);
				if(k == 14) adjustdelta = (0.05);
				if(k == 15) adjustdelta = (0.05);
				if(k == 16) adjustdelta = (0.05);
				if(k == 17) adjustdelta = (0.05);
				if(k == 18) adjustdelta = (0.05);
				if(k == 19) adjustdelta = (0.05);
				double grainarea = twodimxgrain[curgrain].grainarea;
				int nnum = twodimxgrain[curgrain].neighnum;
				double ea1 = twodimxgrain[curgrain].euler1;
				double ea2 = twodimxgrain[curgrain].euler2;
				double ea3 = twodimxgrain[curgrain].euler3;
				double grainrad = pow((grainarea/3.1415926535897),0.5);
				double randomnum3 = rg.genrand_res53();
				double adjustment = adjust+(randomnum3*adjustdelta);
				double maxgrainrad = grainrad/adjustment;
				double maxgrainarea = 3.1415926535897*maxgrainrad*maxgrainrad;
				double a = pow((maxgrainarea/(3.1415926535897*bovera)),0.5);
				double b = bovera*a;
				double c = coverb*b;
				double vol = 1.3333333*3.1415926535897*a*b*c;
				double weighter = double(maxgrainarea/maxxarea);
				if(k < 3) nnum = nnum*(2+(2*weighter));
				if(k >= 3) nnum = nnum*(4-(1*weighter));
				if(nnum < 0)
				{
					int stop = 1;
				}
				threedimgrain[threedcounter].set(vol,a,b,c,nnum,l,ea1,ea2,ea3);
				xgrains[l][grainpicked][1] = 1;
				xcounter++;
				threedcounter++;
			}
			while(ycounter < ynumgrainstopick)
			{
				double curmaxarea = 0;
				int curgrain;
				int grainpicked;
				for(int iterator = 0; iterator < yclasscount; iterator++)
				{
					int alreadypicked = ygrains[l][iterator][1];
					int gnum = ygrains[l][iterator][0];
					double area = twodimygrain[gnum].grainarea;
					if(area > curmaxarea && alreadypicked != 1)
					{
						curgrain = gnum;
						grainpicked = iterator;
						curmaxarea = area;
					}
				}
				double randomnum1 = rg.genrand_res53();
				bovera = a1+(randomnum1*a1delta);
				if(bovera > 1) bovera = 1;
				double randomnum2 = rg.genrand_res53();
				coverb = a2+(randomnum2*a2delta);
				if(coverb > 1) coverb = 1;
				double adjust = 0;
				double adjustdelta = 0;
				if(k == 0) adjust = (0.95);
				if(k == 1) adjust = (0.9);
				if(k == 2) adjust = (0.85);
				if(k == 3) adjust = (0.8);
				if(k == 4) adjust = (0.75);
				if(k == 5) adjust = (0.7);
				if(k == 6) adjust = (0.65);
				if(k == 7) adjust = (0.6);
				if(k == 8) adjust = (0.55);
				if(k == 9) adjust = (0.5);
				if(k == 10) adjust = (0.45);
				if(k == 11) adjust = (0.4);
				if(k == 12) adjust = (0.35);
				if(k == 13) adjust = (0.3);
				if(k == 14) adjust = (0.25);
				if(k == 15) adjust = (0.2);
				if(k == 16) adjust = (0.15);
				if(k == 17) adjust = (0.1);
				if(k == 18) adjust = (0.05);
				if(k == 19) adjust = (0);
				if(k == 0) adjustdelta = (0.05);
				if(k == 1) adjustdelta = (0.05);
				if(k == 2) adjustdelta = (0.05);
				if(k == 3) adjustdelta = (0.05);
				if(k == 4) adjustdelta = (0.05);
				if(k == 5) adjustdelta = (0.05);
				if(k == 6) adjustdelta = (0.05);
				if(k == 7) adjustdelta = (0.05);
				if(k == 8) adjustdelta = (0.05);
				if(k == 9) adjustdelta = (0.05);
				if(k == 10) adjustdelta = (0.05);
				if(k == 11) adjustdelta = (0.05);
				if(k == 12) adjustdelta = (0.05);
				if(k == 13) adjustdelta = (0.05);
				if(k == 14) adjustdelta = (0.05);
				if(k == 15) adjustdelta = (0.05);
				if(k == 16) adjustdelta = (0.05);
				if(k == 17) adjustdelta = (0.05);
				if(k == 18) adjustdelta = (0.05);
				if(k == 19) adjustdelta = (0.05);
				double grainarea = twodimygrain[curgrain].grainarea;
				int nnum = twodimygrain[curgrain].neighnum;
				double ea1 = twodimygrain[curgrain].euler1;
				double ea2 = twodimygrain[curgrain].euler2;
				double ea3 = twodimygrain[curgrain].euler3;
				double grainrad = pow((grainarea/3.1415926535897),0.5);
				double randomnum3 = rg.genrand_res53();
				double adjustment = adjust+(randomnum3*adjustdelta);
				double maxgrainrad = grainrad/adjustment;
				double maxgrainarea = 3.1415926535897*maxgrainrad*maxgrainrad;
				double a = pow((maxgrainarea/(3.1415926535897*bovera)),0.5);
				double b = bovera*a;
				double c = coverb*b;
				double vol = 1.3333333*3.1415926535897*a*b*c;
				double weighter = double(maxgrainarea/maxxarea);
				if(k < 3) nnum = nnum*(2+(2*weighter));
				if(k >= 3) nnum = nnum*(4-(1*weighter));
				if(nnum < 0)
				{
					int stop = 1;
				}
				threedimgrain[threedcounter].set(vol,a,b,c,nnum,l,ea1,ea2,ea3);
				ygrains[l][grainpicked][1] = 1;
				ycounter++;
				threedcounter++;
			}
			while(zcounter < znumgrainstopick)
			{
				double curmaxarea = 0;
				int curgrain;
				int grainpicked;
				for(int iterator = 0; iterator < zclasscount; iterator++)
				{
					int alreadypicked = zgrains[l][iterator][1];
					int gnum = zgrains[l][iterator][0];
					double area = twodimzgrain[gnum].grainarea;
					if(area > curmaxarea && alreadypicked != 1)
					{
						curgrain = gnum;
						grainpicked = iterator;
						curmaxarea = area;
					}
				}
				double randomnum1 = rg.genrand_res53();
				bovera = a1+(randomnum1*a1delta);
				if(bovera > 1) bovera = 1;
				double randomnum2 = rg.genrand_res53();
				coverb = a2+(randomnum2*a2delta);
				if(coverb > 1) coverb = 1;
				double adjust = 0;
				double adjustdelta = 0;
				if(k == 0) adjust = (0.95);
				if(k == 1) adjust = (0.9);
				if(k == 2) adjust = (0.85);
				if(k == 3) adjust = (0.8);
				if(k == 4) adjust = (0.75);
				if(k == 5) adjust = (0.7);
				if(k == 6) adjust = (0.65);
				if(k == 7) adjust = (0.6);
				if(k == 8) adjust = (0.55);
				if(k == 9) adjust = (0.5);
				if(k == 10) adjust = (0.45);
				if(k == 11) adjust = (0.4);
				if(k == 12) adjust = (0.35);
				if(k == 13) adjust = (0.3);
				if(k == 14) adjust = (0.25);
				if(k == 15) adjust = (0.2);
				if(k == 16) adjust = (0.15);
				if(k == 17) adjust = (0.1);
				if(k == 18) adjust = (0.05);
				if(k == 19) adjust = (0);
				if(k == 0) adjustdelta = (0.05);
				if(k == 1) adjustdelta = (0.05);
				if(k == 2) adjustdelta = (0.05);
				if(k == 3) adjustdelta = (0.05);
				if(k == 4) adjustdelta = (0.05);
				if(k == 5) adjustdelta = (0.05);
				if(k == 6) adjustdelta = (0.05);
				if(k == 7) adjustdelta = (0.05);
				if(k == 8) adjustdelta = (0.05);
				if(k == 9) adjustdelta = (0.05);
				if(k == 10) adjustdelta = (0.05);
				if(k == 11) adjustdelta = (0.05);
				if(k == 12) adjustdelta = (0.05);
				if(k == 13) adjustdelta = (0.05);
				if(k == 14) adjustdelta = (0.05);
				if(k == 15) adjustdelta = (0.05);
				if(k == 16) adjustdelta = (0.05);
				if(k == 17) adjustdelta = (0.05);
				if(k == 18) adjustdelta = (0.05);
				if(k == 19) adjustdelta = (0.05);
				double grainarea = twodimzgrain[curgrain].grainarea;
				int nnum = twodimzgrain[curgrain].neighnum;
				double ea1 = twodimzgrain[curgrain].euler1;
				double ea2 = twodimzgrain[curgrain].euler2;
				double ea3 = twodimzgrain[curgrain].euler3;
				double grainrad = pow((grainarea/3.1415926535897),0.5);
				double randomnum3 = rg.genrand_res53();
				double adjustment = adjust+(randomnum3*adjustdelta);
				double maxgrainrad = grainrad/adjustment;
				double maxgrainarea = 3.1415926535897*maxgrainrad*maxgrainrad;
				double a = pow((maxgrainarea/(3.1415926535897*bovera)),0.5);
				double b = bovera*a;
				double c = coverb*b;
				double vol = 1.3333333*3.1415926535897*a*b*c;
				double weighter = double(maxgrainarea/maxxarea);
				if(k < 3) nnum = nnum*(2+(2*weighter));
				if(k >= 3) nnum = nnum*(4-(1*weighter));
				if(nnum < 0)
				{
					int stop = 1;
				}
				threedimgrain[threedcounter].set(vol,a,b,c,nnum,l,ea1,ea2,ea3);
				zgrains[l][grainpicked][1] = 1;
				zcounter++;
				threedcounter++;
			}
			xubercounter = xubercounter + xcounter;
			yubercounter = yubercounter + ycounter;
			zubercounter = zubercounter + zcounter;
		}
	}
}

void Extrapolation2Dto3DFunc::volume_stats()
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
	double neighcount = 0;
	double maxvol = 0;
	double maxneigh = 0;
	double maxdiam = 0;
	double maxlogdiam = 0;
	double maxbovera = 0;
	double maxcovera = 0;
	double maxcoverb = 0;
	double svn[100][5];
	double svs[100][5];
	double svbovera[100][5];
	double svcovera[100][5];
	double svcoverb[100][5];
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
	for(int i = 0; i < num3dgrains; i++)
	{
		double g1ea1 = threedimgrain[i].euler1;
		double g1ea2 = threedimgrain[i].euler2;
		double g1ea3 = threedimgrain[i].euler3;
		actualgrains++;
		double vol = threedimgrain[i].grainvolume;
		double logvol = log(vol);
		double rad3 = 0.75*(1/3.1415926535897)*vol;
		double diam = 2*pow(rad3,0.333333333);
		int diamint = diam;
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
			int ea1bin = 0.5+ea1/(3.1415926535897/18);
			int ea2bin = 0.5+ea2/(3.1415926535897/18);
			int ea3bin = 0.5+ea3/(3.1415926535897/18);
			if(ea1 >= 0 && ea2 >= 0 && ea3 >= 0 && ea1 <= (3.1415926535897/2) && ea2 <= (3.1415926535897/2) && ea3 <= (3.1415926535897/2))
			{
				if(diamint > 11) diamint = 11;
				eulercount[diamint][ea1bin][ea2bin][ea3bin]++;
			}
		}
		double a = threedimgrain[i].majoraxis;
		double b = threedimgrain[i].intermediateaxis;
		double c = threedimgrain[i].minoraxis;
		double bovera = b/a;
		double covera = c/a;
		double coverb = c/b;
		int nnum = threedimgrain[i].neighnum;
		double lognnum = log(double(nnum));
//		list<int> nlist = threedimgrain[i];
		avgvol = avgvol+vol;
		avglnvol = avglnvol+logvol;
		avgneigh = avgneigh+nnum;
		avglnneigh = avglnneigh+lognnum;
		avgbovera = avgbovera+bovera;
		avgcovera = avgcovera+covera;
		avgcoverb = avgcoverb+coverb;
		avgdiam = avgdiam + diam;
		avglogdiam = avglogdiam+logdiam;
		svn[diamint][0]++;
		svbovera[diamint][0]++;
		svcovera[diamint][0]++;
		svcoverb[diamint][0]++;
		svn[diamint][1] = svn[diamint][1] + nnum;
		svn[diamint][3] = svn[diamint][3] + lognnum;
		svbovera[diamint][1] = svbovera[diamint][1] + bovera;
		svcovera[diamint][1] = svcovera[diamint][1] + covera;
		svcoverb[diamint][1] = svcoverb[diamint][1] + coverb;
/*		while(!nlist.empty())
		{
			int neigh = nlist.front();
			double neighvol = threedimgrain[neigh].grainvolume;
			double neighlogvol = log(neighvol);
			double neighrad3 = 0.75*(1/3.1415926535897)*neighvol;
			double neighdiam = 2*pow(neighrad3,0.333333333);
			int neighdiamint = neighdiam;
			svs[diamint][0]++;
			svs[diamint][1] = svs[diamint][1] + neighdiam;
			avgdiam2 = avgdiam2 + neighdiam;
			neighcount++;
			nlist.pop_front();
		}*/
		if(vol > maxvol) maxvol = vol;
		if(nnum > maxneigh) maxneigh = nnum;
		if(bovera > maxbovera) maxbovera = bovera;
		if(covera > maxcovera) maxcovera = covera;
		if(coverb > maxcoverb) maxcoverb = coverb;
		if(diam > maxdiam) maxdiam = diam;
		if(logdiam > maxlogdiam) maxlogdiam = logdiam;
	}
	int maxdiamint = maxdiam;
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
	maxvol = maxvol/avgvol;
	int vbw = (maxvol/25)+1;
	int nbw = (maxneigh/25)+1;
	double sbw1 = 1.0/25.0;
	double sbw2 = 1.0/25.0;
	double sbw3 = 1.0/25.0;
	double vbin[25];
	double nbin[25];
	double sbin1[25];
	double sbin2[25];
	double sbin3[25];
	double svnbin[100][25];
	double svsbin[100][100];
	double svboverabin[100][25];
	double svcoverabin[100][25];
	double svcoverbbin[100][25];
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
	for(int j = 0; j < num3dgrains; j++)
	{
		double vol = threedimgrain[j].grainvolume;
		double logvol = log(vol);
		double rad3 = 0.75*(1/3.1415926535897)*vol;
		double diam = 2*pow(rad3,0.333333333);
		int diamint = diam;
		double logdiam = log(diam);
		double a = threedimgrain[j].majoraxis;
		double b = threedimgrain[j].intermediateaxis;
		double c = threedimgrain[j].minoraxis;
		double bovera = b/a;
		double covera = c/a;
		double coverb = c/b;
		int nnum = threedimgrain[j].neighnum;
		double lognnum = log(double(nnum));
//		list<int> nlist = threedimgrain[j];
		sdvol = sdvol + ((vol-avgvol)*(vol-avgvol));		
		sdlnvol = sdlnvol + ((logvol-avglnvol)*(logvol-avglnvol));		
		sdneigh = sdneigh + ((nnum-avgneigh)*(nnum-avgneigh));		
		sdlnneigh = sdlnneigh + ((lognnum-avglnneigh)*(lognnum-avglnneigh));		
		sdbovera = sdbovera + ((bovera-avgbovera)*(bovera-avgbovera));		
		sdcovera = sdcovera + ((covera-avgcovera)*(covera-avgcovera));		
		sdcoverb = sdcoverb + ((coverb-avgcoverb)*(coverb-avgcoverb));		
		sddiam = sddiam + ((diam-avgdiam)*(diam-avgdiam));
		sdlogdiam = sdlogdiam + ((logdiam-avglogdiam)*(logdiam-avglogdiam));
		svn[diamint][2] = svn[diamint][2] + ((nnum-svn[diamint][1])*(nnum-svn[diamint][1]));		
		svn[diamint][4] = svn[diamint][4] + ((lognnum-svn[diamint][3])*(lognnum-svn[diamint][3]));		
		svbovera[diamint][2] = svbovera[diamint][2] + ((bovera-svbovera[diamint][1])*(bovera-svbovera[diamint][1]));		
		svcovera[diamint][2] = svcovera[diamint][2] + ((covera-svcovera[diamint][1])*(covera-svcovera[diamint][1]));		
		svcoverb[diamint][2] = svcoverb[diamint][2] + ((coverb-svcoverb[diamint][1])*(coverb-svcoverb[diamint][1]));		
/*		while(!nlist.empty())
		{
			int neigh = nlist.front();
			double neighvol = threedimgrain[neigh].grainvolume;
			double neighlogvol = log(neighvol);
			double neighrad3 = 0.75*(1/3.1415926535897)*neighvol;
			double neighdiam = 2*pow(neighrad3,0.333333333);
			int neighdiamint = neighdiam;
			svs[diamint][2] = svs[diamint][2] + ((neighdiam-svs[diamint][1])*(neighdiam-svs[diamint][1]));
			svsbin[diamint][neighdiamint]++;
			sddiam2 = sddiam2 + ((neighdiam-avgdiam2)*(neighdiam-avgdiam2));
			nlist.pop_front();
		}*/
		vol = vol/avgvol;
		int vcur = vol/vbw;
		if(vcur == 25) vcur = 24;
		int ncur = nnum/nbw;
		if(ncur == 25) ncur = 24;
		int s1cur = bovera/sbw1;
		if(s1cur == 25) s1cur = 24;
		int s2cur = covera/sbw2;
		if(s2cur == 25) s2cur = 24;
		int s3cur = coverb/sbw3;
		if(s3cur == 25) s3cur = 24;
		vbin[vcur]++;
		nbin[ncur]++;
		sbin1[s1cur]++;
		sbin2[s2cur]++;
		sbin3[s3cur]++;
		svnbin[diamint][ncur]++;
		svboverabin[diamint][s1cur]++;
		svcoverabin[diamint][s2cur]++;
		svcoverbbin[diamint][s3cur]++;
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
	double svncr = 0;
	double svscr = 0;
	double svboveracr = 0;
	double svcoveracr = 0;
	double svcoverbcr = 0;
	for(int temp5 = 0; temp5 < (maxdiamint+1); temp5++)
	{
		svncr = svncr + (svn[temp5][0]*((svn[temp5][1]-avgneigh)*(svn[temp5][1]-avgneigh)));
		svscr = svscr + (svs[temp5][0]*((svs[temp5][1]-avgdiam2)*(svs[temp5][1]-avgdiam2)));
		svboveracr = svboveracr + (svbovera[temp5][0]*((svbovera[temp5][1]-avgbovera)*(svbovera[temp5][1]-avgbovera)));
		svcoveracr = svcoveracr + (svcovera[temp5][0]*((svcovera[temp5][1]-avgcovera)*(svcovera[temp5][1]-avgcovera)));
		svcoverbcr = svcoverbcr + (svcoverb[temp5][0]*((svcoverb[temp5][1]-avgcoverb)*(svcoverb[temp5][1]-avgcoverb)));
	}
	svncr = svncr/(actualgrains*neighvar);
	svscr = svscr/(neighcount*diamvar2);
	svboveracr = svboveracr/(actualgrains*boveravar);
	svcoveracr = svcoveracr/(actualgrains*coveravar);
	svcoverbcr = svcoverbcr/(actualgrains*coverbvar);
	for(int l = 0; l < num3dgrains; l++)
	{
		double microcount = 0;
/*		list<double> misolist = threedimgrain[l].misorientationlist;
		int nummisos = misolist.size();
		while(!misolist.empty())
		{
			misocount++;
			double firstmiso = misolist.front();
			int misocur = firstmiso/6.5;
			if(misocur == 10) misocur = 9;
			misobin[misocur]++;
			if(firstmiso < 15) microcount++;
			misolist.pop_front();
		}
		microcount = microcount/nummisos;
		int microcur = microcount/0.25;
		if(microcur == 4) microcur = 3;
		microbin[microcur]++;*/
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
/*    ofstream outFile; 
    outFile.open(writename5.c_str());
	outFile << "INDIVIDUAL DISTRIBUTIONS" << endl;
	outFile << endl;
	outFile << "Parameter" << "	" << "Avg." << "	" << "Std. Dev." << endl;
	outFile << "Volume" << "	" << avgvol << "	" << sdvol << endl;
	outFile << "Volume" << "	" << avglnvol << "	" << sdlnvol << endl;
	outFile << "Neighbors" << "	" << avgneigh << "	" << sdneigh << endl;
	outFile << "Neighbors" << "	" << avglnneigh << "	" << sdlnneigh << endl;
	outFile << "b/a" << "	" << avgbovera << "	" << sdbovera << endl;
	outFile << "c/a" << "	" << avgcovera << "	" << sdcovera << endl;
	outFile << "c/b" << "	" << avgcoverb << "	" << sdcoverb <<  endl;
	outFile << "Diameter" << "	" << avgdiam << "	" << sddiam << endl;
	outFile << endl;
	outFile << "Volume Bin" << "	" << "Count" << "	" << "Neighbor Bin" << "	" << "Count" << "	" << "b/a Bin" << "	" << "Count" << "	" << "c/a Bin" << "	" << "Count" << "	" << "c/b Bin" << "	" << "Count" << endl;
	for(int temp6 = 0; temp6 < 25; temp6++)
	{
		outFile << (vbw*temp6) << "	" << vbin[temp6]/actualgrains << "	" << (nbw*temp6) << "	" << nbin[temp6]/actualgrains << "	" << (sbw1*temp6) << "	" << sbin1[temp6]/actualgrains << "	" << (sbw2*temp6) << "	" << sbin2[temp6]/actualgrains << "	" << (sbw3*temp6) << "	" << sbin3[temp6]/actualgrains << endl; 
	}
	outFile << endl;
	outFile << "CORRELATIONS" << endl;
	outFile << endl;
	outFile << "Size V Neighbors" << "	" << svncr << endl;
	outFile << "Size V Neighbor Size" << "	" << svscr << endl;
	outFile << "Size V b/a" << "	" << svboveracr << endl;
	outFile << "Size V c/a" << "	" << svcoveracr << endl;
	outFile << "Size V c/b" << "	" << svcoverbcr << endl;
	outFile << endl;
	outFile << "Diameter" << "	" << "Count" << "	" << "Avg. Neighbors" << "	" << "Std. Dev. Neighbors" << "	" << "Avg. Ln Neighbors" << "	" << "Std. Dev.  Ln Neighbors" << "	" << "Avg. Neighbor Diameter" << "	" << "Std. Dev. Neighbor Diameter" << "	" <<"Avg. b/a" << "	" << "Std. Dev. b/a" << "	" << "Avg. c/a" << "	" << "Std. Dev. c/a" << "	" << "Avg. c/b" << "	" << "Std. Dev. c/b" << "	" <<"P b/a" << "	" << "Q Dev. b/a" << "	" << "P c/a" << "	" << "Q Dev. c/a" << "	" << "P c/b" << "	" << "Q Dev. c/b" << endl; 
	for(int temp7 = 0; temp7 < (maxdiamint+1); temp7++)
	{
		outFile << temp7 << "	" << svn[temp7][0] << "	" << svn[temp7][1] << "	" << svn[temp7][2] << "	" << svn[temp7][3] << "	" << svn[temp7][4] << "	" << svs[temp7][1] << "	" << svs[temp7][2] << "	" << svbovera[temp7][1] << "	" << svbovera[temp7][2] << "	" << svcovera[temp7][1] << "	" << svcovera[temp7][2] << "	" << svcoverb[temp7][1] << "	" << svcoverb[temp7][2] << "	" << svbovera[temp7][3] << "	" << svbovera[temp7][4] << "	" << svcovera[temp7][3] << "	" << svcovera[temp7][4] << "	" << svcoverb[temp7][3] << "	" << svcoverb[temp7][4] << endl; 
	}
	outFile << endl;
	for(int temp8 = 0; temp8 < 25; temp8++)
	{
		for(int temp9 = 0; temp9 < (maxdiamint+1); temp9++)
		{
			outFile << (svnbin[temp9][temp8]/svn[temp9][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp10 = 0; temp10 < (maxdiamint+1); temp10++)
	{
		for(int temp11 = 0; temp11 < (maxdiamint+1); temp11++)
		{
			outFile << (svsbin[temp11][temp10]/svs[temp11][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp12 = 0; temp12 < 25; temp12++)
	{
		for(int temp13 = 0; temp13 < (maxdiamint+1); temp13++)
		{
			outFile << (svboverabin[temp13][temp12]/svn[temp13][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp14 = 0; temp14 < 25; temp14++)
	{
		for(int temp15 = 0; temp15 < (maxdiamint+1); temp15++)
		{
			outFile << (svcoverabin[temp15][temp14]/svn[temp15][0]) << "	";
		}
		outFile << endl;
	}
	outFile << endl;
	for(int temp16 = 0; temp16 < 25; temp16++)
	{
		for(int temp17 = 0; temp17 < (maxdiamint+1); temp17++)
		{
			outFile << (svcoverbbin[temp17][temp16]/svn[temp17][0]) << "	";
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
		outFile << orand[temp26][0] << "	" << orand[temp26][1] << endl;
	}
	outFile.close();
    ofstream outFile1; 
    outFile1.open(writename6.c_str());
	double probability = 0;
	double binwidth = (maxlogdiam*1.5)/double(numdiambins);
	double cummprobability = 0;
	for(int i = 0; i < numdiambins; i++)
	{
		double binval = (binwidth*double(i));
		double diameter = exp(binval);
		probability = (1/(sdlogdiam*(pow((2.0*3.1415926535897),0.5))*binval))*exp((-pow((log(binval)-avglogdiam),2.0))/(2.0*pow(sdlogdiam,2.0)));
		cummprobability = cummprobability+probability;
		outFile1 << cummprobability << "	" << diameter << endl;
	}
	outFile1.close();
    ofstream outFile2; 
    outFile2.open(writename7.c_str());
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
		double gammapq = pow((6.28318/pq),0.5)*pow(((pq/exp(1.0))*inradicalpq),pq);
		double gammap = pow((6.28318/p),0.5)*pow(((p/exp(1.0))*inradicalp),p);
		double gammaq = pow((6.28318/q),0.5)*pow(((q/exp(1.0))*inradicalq),q);
		for(int j = 0; j < numshapebins; j++)
		{
			double bovera = double(i)/double(numshapebins);
			probability = (gammapq/(gammap*gammaq))*pow(bovera,(p-1.0))*pow((1.0-bovera),(q-1.0));
			cummprobability = cummprobability+probability;
			outFile2 << bovera << "	" << cummprobability << endl;
		}
	}
	outFile2.close();
    ofstream outFile3; 
    outFile3.open(writename8.c_str());
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
		double gammapq = pow((6.28318/pq),0.5)*pow(((pq/exp(1.0))*inradicalpq),pq);
		double gammap = pow((6.28318/p),0.5)*pow(((p/exp(1.0))*inradicalp),p);
		double gammaq = pow((6.28318/q),0.5)*pow(((q/exp(1.0))*inradicalq),q);
		for(int j = 0; j < numshapebins; j++)
		{
			double covera = double(i)/double(numshapebins);
			probability = (gammapq/(gammap*gammaq))*pow(covera,(p-1.0))*pow((1.0-covera),(q-1.0));
			cummprobability = cummprobability+probability;
			outFile3 << covera << "	" << cummprobability << endl;
		}
	}
	outFile3.close();
    ofstream outFile4; 
    outFile4.open(writename9.c_str());
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
		for(int j = 0; j < numshapebins; j++)
		{
			double coverb = double(i)/double(numshapebins);
			probability = (gammapq/(gammap*gammaq))*pow(coverb,(p-1.0))*pow((1.0-coverb),(q-1.0));
			cummprobability = cummprobability+probability;
			outFile2 << coverb << "	" << cummprobability << endl;
		}
	}
	outFile4.close();
    ofstream outFile5; 
    outFile5.open(writename10.c_str());
	probability = 0;
	cummprobability = 0;
	for(int i = 0; i < numsizebins; i++)
	{
		for(int j = 0; j < numneighbins; j++)
		{
			double binval = log(double(j));
			probability = (1/(svn[i][4]*(pow((2.0*3.1415926535897),0.5))*binval))*exp((-pow((log(binval)-svn[i][3]),2.0))/(2.0*pow(svn[i][4],2.0)));
			cummprobability = cummprobability+probability;
			outFile5 << j << "	" << probability << endl;
		}
	}
	outFile5.close();
    ofstream outFile6; 
    outFile6.open(writename11.c_str());
	probability = 0;
	for(int i = 0; i < numsizebins; i++)
	{ 
		outFile6 << i;
		for(int j = 0; j < numsizebins; j++)
		{
			outFile6 << "	" << (svsbin[i][j]/svs[i][0]);
		}
	}
	outFile6.close();
    ofstream outFile7; 
    outFile7.open(writename12.c_str());
	probability = 0;
	for(int i = 0; i < nummisobins; i++)
	{
		outFile7 << (misobin[i]/misocount) << endl;
	}
	outFile7.close();
    ofstream outFile8; 
    outFile8.open(writename13.c_str());
	probability = 0;
	for(int i = 0; i < nummicrobins; i++)
	{
		outFile8 << (microbin[i]/actualgrains) << endl;
	}
	outFile8.close();*/
}
