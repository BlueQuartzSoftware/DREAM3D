Align Sections (Misorientation) {#alignsectionsmisorientation}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This Filter attempts to align consecutive 'sections' perpendicular to the z-direction of the sample by determining the position that results in the minimum amount of misorientation between **Cells** directly above-below each other. The algorithm of this Filter is as follows:

1. Calculate the misorientation between each **Cell** in a section and the **Cell** directly above it in the next section.  
2. Count the number of **Cell** pairs that have a misorientation above the user defined tolerance and store that as the misalignment value for that position.
3. Repeat steps 1 and 2 for each position when shifting the second slice (relative to the first) from three (3) **Cells** to the left to three (3) **Cells** to the right, as well as from three (3) **Cells** up to three (3) **Cells** down. *Note that this creates a 7x7 grid*
4. Determine the position in the 7x7 grid that has the lowest misalignment value. (It will be the position with the fewest different **Cell** pairs).
5. Repeat steps 1-4 with the center of each (new) 7x7 grid at the best position from the last 7x7 grid until the best position in the current/new 7x7 grid is the same as the last 7x7 grid.
6) Repeat steps 1-5 for each pair of neighboring sections.

*Note that this is similar to a downhill simplex and can get caught in a local minima*

If the user elects to use a mask array, the **Cells** flagged as *false* in the mask array will not be considered during the alignment process.  

The user can choose to write the determined shift to an output file by enabling *Write Alignment Shifts File* and providing a file path.  

The user can also decide to remove a 'background shift' present in the sample. The process for this is to fit a line to the x and y shifts along the z-direction of the sample.  The individual shifts are then modified to make the slope of the fit line be 0.  Effectively, this process is trying to keep the top and bottom section of the sample fixed.  Some combinations of sample geometry and internal features can result in this algorithm introducing a 'shear' in the sample and the *Linear Background Subtraction* will attempt to correct for this.

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Misorientation Tolerance | Double | Tolerance used to decide if **Cells** above/below one another should be considered to be 'the same'. The value selected should be similar to the tolerance one would use to define **Features** (ie 2-10 degrees) |
| Write Alignment Shift File | Boolean | Specifies whether to write the shifts applied to each section to a file |
| Alignment File | File Path | Path where the user would like the shifts applied to the section to be written. (Only required if *Write Alignment Shifts File* is set to *true* |
| Linear Background Subtraction | Boolean | Specifies if the user wants to remove a 'background' shift present in the alignment |
| Use Mask Array | Boolean | Specifies whether the user would like to remove some **Cells** from consideration in the alignment process |

 

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell| Phases | Int | (1) | Specifies the **Ensemble** of the **Cell** |
| Cell | GoodVoxels | Boolean | (1) | Specifies if the **Cell** is to be counted as a phase or not (Only required if *Use Mask Array* is set to *true* |
| Cell | Quats | Float | (4) | Specifies the orientation of the **Element** in quaternion representation |
| Ensemble | CrystalStructures | Int | (1) | Specifies the crystal structure of each Ensemble using an enumeration defined by DREAM3D (Hexagonal_High=0, Cubic_High=1, Hexagonal_Low=2, Cubic_Low=3, Triclinic=4, Monoclinic=5, Orthorhombic=6, Tetragonal_Low=7, Tetragonal_High=8, Trigonal_Low=9, Trigonal_High=10, Unknown=999) |

## Created Arrays ##
None

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


