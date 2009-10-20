
using namespace std;

#include <Representation/Common/MicroGen3D.h>

int main()
{
	uint32 seeder = time(0);
	MicroGen3D MicroGen3DVol;
	MicroGen3DVol.rg.RandomInit(seeder);
	MicroGen3DVol.numgrains = MicroGen3DVol.form_grains();

 return 0;
}

