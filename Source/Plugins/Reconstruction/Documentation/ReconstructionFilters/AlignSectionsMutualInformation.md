Align Sections (Mutual Information) {#alignsectionsmutualinformation}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This Filter segments each 2D slice, creating *FeatureIds* that are used when determining the _mutual information_ between neighboring slices. The slices are shifted relative to one another until the position of maximum _mutual information_  is determined for each section.  The *FeatureIds* are temporary, they apply to this Filter only and are not related to the *FeatureIds* generated in other Filters.  The algorithm of this Filter is listed below:

1. Segment *Features* on each 'section' of the sample perpendicular to the z-direction.  This is done using the same algorithm in the "Segment Features (Misorientation) Filter" in this same plugin (only in 2D on each section).   
2. Calculate the *mutual information* between neighboring sections and store that as the misalignment value for that position. *Mutual information* is related to the ratio of joint probability to individual probabilities of variables (i.e. p(x,y)/p(x)p(y) ). Details of the actual *mutual information* calculation can be found in the references below, but can be thought of as the inherent dependence between variables (here the *FeatureIds* on neighboring sections).  
3. Repeat step 2 for each position when shifting the second slice (relative to the first) from three (3) **Cells** to the left 
to three (3) **Cells** to the right, as well as from three (3) **Cells** up to three (3) **Cells** down.
*Note that this creates a 7x7 grid*
4. Determine the position in the 7x7 grid that has the highest *mutual information* value.
5. Repeat steps 2-4 with the center of each (new) 7x7 grid at the best position from the last 7x7 grid until the best position in the current/new 7x7 grid is the same as the last 7x7 grid.
6) Repeat steps 2-5 for each pair of neighboring sections.

*Note that this is similar to a downhill simplex and can get caught in a local minima*

The user choses the level of _misorientation tolerance_ by which to align **Cells**, where here the tolerance means the _misorientation_ cannot exceed a given value. If the rotation angle is below the tolerance, then the **Cell** is grouped with other **Cells** that satisfy the criterion.

The approach used in this filter is to group neighboring **Cells** on a slice that have a _misorientation_ below the tolerance the user entered. _Misorientation_ here means the minimum rotation angle of one **Cell**'s crystal axis needed to coincide with another **Cell**'s crystal axis. When the **Features** in the slices are defined, they are moved until _'disks'_ in neighboring slices align with each other.

_Mutual information_ is one of many quantities that measures how much one random variable
tells us about another. It is a dimensionless quantity with (generally) units of bits, and can be
thought of as the reduction in uncertainty about one random variable given knowledge of another.
High _mutual information_ indicates a large reduction in uncertainty; low _mutual information_
indicates a small reduction; and zero _mutual information_ between two random variables
means the variables are independent.

If the user elects to use a mask array, the **Cells** flagged as *false* in the mask array will not be considered during the alignment process.  

The user can choose to write the determined shift to an output file by enabling *Write Alignment Shifts File* and providing a file path.  

The user can also decide to remove a 'background shift' present in the sample. The process for this is to fit a line to the x and y shifts along the z-direction of the sample.  The individual shifts are then modified to make the slope of the fit line be 0.  Effectively, this process is trying to keep the top and bottom section of the sample fixed.  Some combinations of sample geometry and internal features can result in this algorithm introducing a 'shear' in the sample and the *Linear Background Subtraction* will attempt to correct for this.

_Source:www.scholarpedia.org/article/Mutual\_information_. A more detailed discussion is presented on this site. 

__Journal articles on _Mutual Information_ that are useful:
__ Cover, T.M. and Thomas, J.A. (1991). 
_Elements of information theory_. John Wiley & Sons, New York, NY.Gray, R.M. (1990). 
_Entropy and Information Theory_. Springer-Verlag, New York, NY. Nirenberg, S. and Latham, P.E. (2003). 
_Decoding neuronal spike trains: how important are correlations?_ Proc. Natl. Acad. Sci. 100:7348-7353. Shannon, C.E. and Weaver, W. (1949). 
_The mathematical theory of communication_. University of Illinois Press, Urbana, Illinois. Mézard, M. and Monatanari, A. (2009). 
_Information, Physics, and Computation_. Oxford University Press, Oxford.


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


