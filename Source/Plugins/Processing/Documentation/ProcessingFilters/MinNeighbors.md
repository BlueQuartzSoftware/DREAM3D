Minimum Number of Neighbors Filter {#minneighbors}
=============

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter sets the minimum number of contiguous neighboring **Features** a **Feature** must have to remain in the structure. Entering zero results in nothing changing.  Entering a number larger than the maximum number of neighbors of any **Feature** generates an _error_ (since all **Features** would be removed). The user needs to proceed conservatively here when choosing the value for the minimum to avoid accidentally exceeding the maximum.
After **Features** are removed for not having enough neighbors, the remaining **Features** are _coarsened_ iteratively, one **Cell** per iteration, until the gaps left by the removed **Features** are filled.  Effectively, this is an isotropic **Feature** growth in the regions around removed **Features**.
The Filter can be run in a mode where the minimum number of neighbors is applied to a single **Ensemble**.  The user can select to apply the minimum to one specific **Ensemble**.


## Parameters ##
| Name | Type | Description |
|------|------|------|
| Minimum Number Neighbors | Int | This is the number of neighbors a **Feature** must have to remain as a **Feature** |
| Apply to Single Phase | Boolean | Tells the Filter whether to apply minimum to single ensemble or all ensembles |
| Phase Index | Int | Tells the Filter which **Ensemble** to apply minimum to (if Apply to Single Phase is set to true) |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | NumNeighbors | Int | (1) | Array holding number of contiguous neighboring **Features** for each **Feature** |
| Feature | Phases | Int | (1) | Specifies the **Ensemble** of the **Feature** - only required if Apply to Single Phase is true  |

## Created Arrays ##
None

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


