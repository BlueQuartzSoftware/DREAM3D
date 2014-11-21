Find Feature Saltykov Sizes {#findsaltykovsizes}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the Saltykov sizes of all **Features**.  The filter takes the **EquavalentDiameters** if assumed 2D and estimates the 3D volume.  For more information, see: Joseph C. Tucker, Lisa H. Chan, Gregory S. Rohrer, Michael A. Groeber, and Anthony D. Rollett. Comparison of grain size distributions in a ni-based superalloy in three and two dimensions using the saltykov method. Scripta Materialia, 66(8):554 - 557, 2012.

## Parameters ##

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Feature | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Feature**. | Find Feature Sizes (Statistics) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Feature | SaltykovEquivalentDiameters | Diameter (float) of the Saltykov estimated sphere. |  |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES, Inc.)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.1.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

