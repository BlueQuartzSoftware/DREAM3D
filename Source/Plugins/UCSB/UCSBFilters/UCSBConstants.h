/*
 *
 */

#ifndef _UCSB_CONSTANTS_H_
#define _UCSB_CONSTANTS_H_

#include "DREAM3DLib/Common/Constants.h"

namespace DREAM3D
{
  namespace CellData
  {
    const DREAM3D_STRING RodriguesVectors("RodriguesVectors");
    const DREAM3D_STRING AxisAngles("AxisAngles");
    const DREAM3D_STRING Stress11("Stress_11");
    const DREAM3D_STRING Stress22("Stress_22");
    const DREAM3D_STRING Stress33("Stress_33");
    const DREAM3D_STRING Stress23("Stress_23");
    const DREAM3D_STRING Stress31("Stress_31");
    const DREAM3D_STRING Stress12("Stress_12");
    const DREAM3D_STRING Strain11("Strain_11");
    const DREAM3D_STRING Strain22("Strain_22");
    const DREAM3D_STRING Strain33("Strain_33");
    const DREAM3D_STRING Strain23("Strain_23");
    const DREAM3D_STRING Strain31("Strain_31");
    const DREAM3D_STRING Strain12("Strain_12");
    const DREAM3D_STRING Strain("Strain");
    const DREAM3D_STRING Stress("Stress");
    const DREAM3D_STRING EED("EED");
    const DREAM3D_STRING MaxPrincipalStress("MaxPrincipalStress");
    const DREAM3D_STRING MinPrincipalStress("MinPrincipalStress");
  }
  namespace FieldData
  {
    const DREAM3D_STRING MajorAxisAngle("MajorAxisAngle");
    const DREAM3D_STRING RodriguesVectors("RodriguesVectors");
    const DREAM3D_STRING AxisAngles("AxisAngles");
  }
}

#endif
