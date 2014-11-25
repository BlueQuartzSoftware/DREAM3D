RecrystalizeVolume {#recrystalizevolume}
=====

## Group (Subgroup) ##
CellularAutomata (CellularAutomata)


## Description ##
This filter simulates recrystallization using cellular automata. Initially all voxels are unrecrystallized. Cells are evolved with the following rules until all cells are recrystallized:
	
1. If at least one neighbor is recrystallized, pick a random recrystallized neighbor to join
2. If no neighbors are recrystallized a new grain may spontaneously nucleate (according to the nucleation rate).


## Parameters ##
| Name             | Type |
|------------------|------|
| Nucleation Rate | Float |

## Required Arrays ##
None


## Created Arrays ##

| Type | Default Array Name | Description | Comment |
|------|--------------------|-------------|---------|
| Int  | FeatureIds           | Unique ID's for recrystallized grains | number in order of generation   |
| Int  | Recrystallization Time           | Time step of assignment to current feature |  |



## Authors: ##

**Copyright:** Will Lenthe (UCSB)

**Contact Info:** willlenthe@gmail.com

**Version:** 0.1.0

**License:**  see the license that came with DREAM3D


