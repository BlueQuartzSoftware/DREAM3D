Input Crystal Compliance Values {#inputcrystalcompliances}
=======

## Group (Subgroup) ##

Generic Filters (Crystallography)

## Description ##

This filter stores the _Crystal Compliance Matrix_ for each **Ensemble**, given the single crystal properties for the **Ensemble**. Currently the specified values will be used for every **Ensemble** present.

The symmetry drop down menu is for convince only (the full 6x6 matrix will be saved for each phase regardless of crystal structure).

## Parameters ##

21 single crystal compliance values (s11, s12, s13, ... , s55, s56, s66) .

## Required Geometry ##

None

## Required Arrays ##

None

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions (dimension, size) | Description |
|------|--------------|-------------|---------|-----|
| Ensemble | CrystalCompliances | Float | (2,6) | 6x6 matrix that specify compliance of the crystal (in the crystal reference frame) |

## Authors ##

**Copyright:** 2014 Will Lenthe (UCSB)

**Contact Info:** willlenthe@gmail.com

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.

See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

