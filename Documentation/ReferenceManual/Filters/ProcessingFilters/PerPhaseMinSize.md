Minimum Size Filter (Per Phase) {#perphaseminsize}
======

## Group (Subgroup) ##
Processing Filters

## Description ##
This Filter removes grains that have a total of _voxels_ below the _minimum number_ input by the user. The _minimum_ is 1. The _maximum_ is the number of _voxels_ in the largest grain. __Entering 0 is not permitted__ because all grains would be eliminated. Entering the number of _voxels_ associated with the largest grain, all grains will be removed. 
__NOTE: If the minimum number entered is larger than the largest grain,
THE PIPELINE WILL EXIT DURING THE FILTER!
The number of _voxels_ in the largest grain is determined during the running of this filter. Hence, a conservative
approach to selecting the minimum number of _voxels_ is strongly recommended.__
After removing all the small grains, the remaining grains are _coarsened_ to fill the gaps left by the small grains.
The iteration _coarsens_ the grains by one _voxel_ each iteration. This filter has the additional ability to
only apply the filter to a single phase of the users choosing.


## Parameters ##

| Name | Type | Description |
|------|------| ------- |
| Minimum Allowed Grain Size | Integer | Number of Voxels |
| Phase Number | Integer | The phase number to apply this filter to. |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | CellPhases |  | |
| Cell | GrainIds |  | |
| Field | FieldPhases |  |  |

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
| Field | Active |      |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



