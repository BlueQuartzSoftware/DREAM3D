
#include "Grain.h"
#include <iostream>


Grain::Grain() :
numvoxels(0),
numneighbors(0),
newgrainname(0),
gotsizemerged(0),
gotcontainedmerged(0),
gottwinmerged(0),
avgeuler1(0.0),
avgeuler2(0.0),
avgeuler3(0.0),
centroidx(0.0),
centroidy(0.0),
centroidz(0.0),
surfacegrain(0),
Ixx(0.0),
Iyy(0.0),
Izz(0.0),
Ixy(0.0),
Iyz(0.0),
Ixz(0.0),
axis1(0.0),
axis2(0.0),
axis3(0.0),
axis1x(0.0),
axis1y(0.0),
axis1z(0.0),
axis2x(0.0),
axis2y(0.0),
axis2z(0.0),
axis3x(0.0),
axis3y(0.0),
axis3z(0.0),
averagemisorientation(0.0),
twinnewnumberbeenset(0),
twinnewnumber(0),
red(0.0),
green(0.0),
blue(0.0),
schmidfactor(0.0),
convexity(0.0),
euler1(0.0),
euler2(0.0),
euler3(0.0),
oeuler1(0.0),
oeuler2(0.0),
oeuler3(0.0),
grainname(0),
neighnum(0),
volume(0.0),
Nvalue(0.0),
nserror(0.0),
x(0.0),
y(0.0),
z(0.0),
xc(0.0),
yc(0.0),
zc(0.0),
currentsize(0),
initsize(0),
temponsurf(0),
tempneighnum(0),
radius1(0.0),
radius2(0.0),
radius3(0.0),
lowanglefraction(0.0),
grainrank(0.0),
grainmicrorank(0.0),
picked(0.0),
frozen(0.0)
{
  neighborlist = NULL;
}

Grain::~Grain()
{
  if (neighborlist != NULL) {
    delete neighborlist;
  }
}
