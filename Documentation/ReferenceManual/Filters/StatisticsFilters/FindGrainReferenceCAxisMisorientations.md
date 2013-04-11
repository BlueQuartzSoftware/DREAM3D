Find Field Reference C-Axis Misorientations {#findgrainreferencecaxismisorientations}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter is essentially the same as the [Find Field Reference Misorientations](FindGrainReferenceMisorientations.html "") filter, but with the misalignment between c-axes of interest, rather than full misorientations.  The average c-axis orientation for each **Field** is compared with the c-axis orientation of each of the **Cells** within the **Field**. 

## Parameters ##
None

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | CellPhases |  |
| Cell | GrainIds |  |
| Cell | Quats |  |
| Field | AvgCAxes |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Cell | GrainReferenceCAxisMisorientations |  |
| Field | GrainAvgCAxisMisorientations |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



