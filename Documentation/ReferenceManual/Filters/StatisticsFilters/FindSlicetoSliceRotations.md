Find Slice to Slice Crystallographic Misalignments {#findslicetoslicerotations}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##

## Parameters ##

| Name | Type |
|------|------|
| Slice to Slice Rotations File | Output File |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



