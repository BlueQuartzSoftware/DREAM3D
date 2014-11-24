Find Feature Saltykov Sizes {#findsaltykovsizes}
======

## Group (Subgroup) ##
Statistics Filters (Morphological)

## Description ##
This filter calculates the Saltykov sizes of all **Features**.  The filter takes the **EquavalentDiameters**, of assumed 2D data, and estimates the 3D equivalent diameters.
The filter will continue iteratively performing the Saltykov Method of spheres, starting with an initial guess of 10 size bins, until the number of features equals the 
number of Saltykov equivalent diameters to be sampled.  If the number of **Features** to be sampled from the Saltykov bins is not equal to the number of **Features**
then the difference between the two is calculated.  Then, the number of attempts is incremented and the number of bins is increased/decreased based on the 
"difference" and the bin arrays are resized accordingly.  This approach allows for the correct number of **Features** to result because increasing the number of bins
increases the number of **SaltykovEquavalentDiameters** to be sampled and decreasing the number of bins does the opposite.  However, if the number of maximum attempts,
hardcoded at 10, is reached, random Saltykov bins are selected to add samples to until the the number of **SaltykovEquavalentDiameters** to be sampled equals the 
number of **Features**.  This is done because it is assumed that after 10 attempts the solution is ocsillating between the minima.
The filter is applied in such a way so that **SaltykovEquivalentDiameters** has the correct feature-level data
length.  This way, a direct one-to-one comparies between **EquivalentDiameters** and **SaltykovEquivalentDiameters** is possible.  However, it is important to note that
the **SaltykovEquivalentDiameters** are not a direct transformation of **EquivalentDiameters** that they are matched up with.
For more information, see: Joseph C. Tucker, Lisa H. Chan, Gregory S. Rohrer, Michael A. Groeber, and Anthony D. Rollett. Comparison of grain size 
distributions in a ni-based superalloy in three and two dimensions using the saltykov method. Scripta Materialia, 66(8):554 - 557, 2012.

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

