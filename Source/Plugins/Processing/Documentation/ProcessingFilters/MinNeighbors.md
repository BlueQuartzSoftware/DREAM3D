Minimum Number of Neighbors Filter {#minneighbors}
======

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter sets the minimum number of contiguous neighboring **Features** a **Feature** must have to remain in the structure. Entering zero results in nothing changing.  Entering a number larger than the maximum number of neighbors of any **Feature** generates an _error_ (since all **Features** would be removed). The user needs to proceed conservatively here when choosing the value for the minimum to avoid accidentally exceeding the maximum.
After **Features** are removed for not having enough neighbors, the remaining **Features** are _coarsened_ iteratively, one **Cell** per iteration, until the gaps left by the removed **Features** are filled.  Effectively, this is an isotropic **Feature** growth in the regions around removed **Features**.
The filter can be run in a mode where the minimum number of neighbors is applied to a single ensemble.  The user can select to apply the minimum to one ensemble and specify which ensemble that is.


## Parameters ##
| Name | Type | Description |
|------|------|------|
| Minimum Number Neighbors | Int | This is the number of neighbors a **Feature** must have to remain as a **Feature** |
| Apply to Single Phase | Boolean | Tells the filter whether to apply minimum to single ensemble or all ensembles |
| Phase Number to Run Min Size Filter on | Int | Tells filter which ensemble to apply minimum to (if apply to single phase is set to true) |

## Required Geometry ##
Image/Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1,1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | NumNeighbors | Int | (1,1) | Array holding number of contiguous neighboring **Features** for each **Feature** |
| Feature | FeaturePhases | Int | (1,1) | Specifies the **Ensemble** of the **Feature** - only required if Apply to Single Phase is true  |

## Created Arrays ##
None

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

