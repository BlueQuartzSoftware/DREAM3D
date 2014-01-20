Align Sections (Mutual Information) {#alignsectionsmutualinformation}
======

## Group (Subgroup) ##
Reconstruction Filters (Alignment)

## Description ##
This Filter segments 2D slices, creates *GrainIds* that are used to determine the _mutual information_ and then aligns the slices. These *GrainIds* are temporary, and they apply to this filter only. So they are not related to the *GrainIds* generated in other Filters.

The user choses the level of _misorientation tolerance_ by which to align **Cells**, where here the tolerance means the _misorientation_ cannot exceed a given value. If the rotation angle is below the tolerance, then the **Cell** is grouped with other **Cells** that satisfy the criterion.

The approach used in this filter is to group neighboring **Cells** on a slice that have a _misorientation_ below the tolerance the user entered. _Misorientation_ here means the minimum rotation angle of one **Cell**'s crystal axis needed to coincide with another **Cell**'s crystal axis. When the **Features** in the slices are defined, they are moved until _'disks'_ in neighboring slices align with each other.

_Mutual information_ is one of many quantities that measures how much one random variable
tells us about another. It is a dimensionless quantity with (generally) units of bits, and can be
thought of as the reduction in uncertainty about one random variable given knowledge of another.
High _mutual information_ indicates a large reduction in uncertainty; low _mutual information_
indicates a small reduction; and zero _mutual information_ between two random variables
means the variables are independent.

_Source:www.scholarpedia.org/article/Mutual\_information_. A more detailed discussion is presented on this site. 
__Journal articles on _Mutual Information_ that are useful:__ Cover, T.M. and Thomas, J.A. (1991). 

_Elements of information theory_. John Wiley & Sons, New York, NY.Gray, R.M. (1990). 

_Entropy and Information Theory_. Springer-Verlag, New York, NY. Nirenberg, S. and Latham, P.E. (2003). 

_Decoding neuronal spike trains: how important are correlations?_ Proc. Natl. Acad. Sci. 100:7348-7353. Shannon, C.E. and Weaver, W. (1949). 

_The mathematical theory of communication_. University of Illinois Press, Urbana, Illinois. Mézard, M. and Monatanari, A. (2009). 

_Information, Physics, and Computation_. Oxford University Press, Oxford.

## Parameters ##

| Name | Type |
|------|------|
| Misorientation Tolerance | Double |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | GoodVoxels | Boolean values used to define "regions" to be aligned | Values are not required to be based on "good" or "bad" data, rather must only correspond to some identified "regions"  | Single Threshold (Cell Data) (Processing), Multi Threshold (Cell Data) (Processing) |
| Cell | Quats | Five (5) values (floats) that specify the orientation of the **Cell** in quaternion representation | Filter will calculate the quaternion for each **Cell** if it is not already calculated. | Find Cell Quaternions (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

