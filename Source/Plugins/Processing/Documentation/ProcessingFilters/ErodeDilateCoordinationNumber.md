Smooth Bad Data (coordination Number) {#erodedilatecoordinationnumber}
=============

## Group (Subgroup) ##
Processing Filters (Cleanup)

## Description ##
This Filter smoothes the interface between *good* and *bad* data. The user can specify a *coordination number* that is the number of neighboring **Cells** of opposite type (i.e. *good* or *bad*) compared to a given **Cell** that is acceptable.  For example, a single *bad* **Cell** surrounded by *good* **Cells** would have a *coordination number* of *6*.  The number entered by the user is actually the maximum tolerated *coordination number*.  If the user entered a value of *4*, then all *good* **Cells** with 5 or more *bad* neighbors and *bad* **Cells** with 5 or more *good* neighbors would be removed.  After **Cells** with unacceptable *coordination number* are removed, then the neighboring **Cells** are *coarsened* to fill the removed **Cells**.  

By default, the Filter will only perform a single iteration and will not concern itself with the possibility that after one iteration, **Cells** that were acceptable may become unacceptable by the original *coordination number* criteria due to the small changes to the structure during the *coarsening*.  The user can opt to enable the "Loop Until Gone" parameter, which will continue to run until no **Cells** fail the original criteria.

## Parameters ##
| Name | Type | Description |
|------|------|------|
| Coordination Number | Integer | Number of neighboring **Cells** that can be of opposite classification before a **Cell** will be removed |
| Loop Until Gone | Boolean | Tells the Filter whether to run a single iteration or iterate until no *bad* **Cells** have more than the above number of *good* neighbor **Cells** |

## Required Geometry ##
Image / Rectilinear Grid 

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs. |

## Created Arrays ##
None

## Authors ##
**Copyright:** 2015 BlueQuartz Software, LLC

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

