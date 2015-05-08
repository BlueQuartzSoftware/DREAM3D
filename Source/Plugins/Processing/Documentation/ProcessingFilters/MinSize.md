Minimum Size Filter (All Phases) {#minsize}
=============

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter removes **Features** that have a total number of **Cells** below the minimum threshold defined by the user. Entering a number larger than the largest **Feature** generates an _error_ (since all **Features** would be removed). Hence, a choice of threshold should be carefully be chosen if it is not known how many **Cells** are in the largest **Features**. After removing all the small **Features**, the remaining **Features** are isotropically coarsened to fill the gaps left by the small **Features**.
The Filter can be run in a mode where the minimum number of neighbors is applied to a single **Ensemble**.  The user can select to apply the minimum to one specific **Ensemble**.

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Minimum Allowed **Feature** Size | Int | Number of **Cells** that must be present in the **Feature** for it to remain in the sample |
| Apply to Single Phase | Boolean | Tells the Filter whether to apply minimum to single ensemble or all ensembles |
| Phase Index | Int | Tells the Filter which **Ensemble** to apply minimum to (if Apply to Single Phase is set to true) |

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |
| Feature | NumCells | Int | (1) | Specifies the number of **Cells** each **Feature** has |
| Feature | Phases | Int | (1) | Specifies the **Ensemble** of the **Feature** - only required if Apply to Single Phase is true  |

## Created Arrays ##
None

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

