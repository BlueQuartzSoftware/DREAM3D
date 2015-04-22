Extract Flagged Features (Rogues Gallery) {#extractflaggedfeatures}
=============

## Group (Subgroup) ##
Sampling Filters (Cropping/Cutting)

## Description ##
This Filter, using a boolean array identifying **Features** to be *extracted*, crops out the smallest bounding box around each **Feature** of interest.  First, the Filter determines the bounding box ((xMin-xMax), (yMin-yMax), (zMin-zMax)) for each **Feature**.  Then, the Filter checks to see if the **Feature** has been "flagged" for extraction.  If the **Feature** is to be extracted, the bounding box is used to define a volume for cropping.  The cropped volume for each extracted **Feature** is stored as a new **Data Container**.  The cropped volumes will have their origins "updated" to ensure that the extracted **Features** remain in the same absolute position relative to each other.

The utility of this Filter is that complex thresholding based on **Feature** attributes can be difficult in *ParaView* (due to memory) and this can greatly reduce the amount of information that is loaded if only the extracted **Feature** **Data Containers** are loaded for viewing. 

*Note:* Storing each extracted volume as a new **Data Container** can result in a large number of **Data Containers** depending on how many **Features** are "flagged". 

## Parameters ##
None

## Required Geometry ##
Image / Rectilinear Grid

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Cell | FeatureIds | Int | (1) | Specifies to which **Feature** each **Cell** belongs |
| Feature | None | Boolean | (1) | Specifies whether each **Feature** should be *extracted* or not |

## Created Arrays ##
None

## Authors ##

**Copyright** 2014 Michael A. Groeber (AFRL), 2014 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)
