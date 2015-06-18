Read Feature Info File {#featureinforeader}
=============

## Group (Subgroup) ##
IO (Input)

## Description ##
This Filter reads information about an existing set of **Feature** into the a new **Feature Attribute Matrix**. The basic format of the file is the number of **Features** on the first line of the file followed by columnar formatted data listing the **Feature** Id, **Ensemble**  Id, Euler Angle 1, Euler Angle 2 and Euler Angle 3. Currently, only the **Ensemble** Id and Euler angle information can be read using this Filter.

*If you do not have Euler Angle data for each **Feature** then substituting Zero (0.0) is fine.*

**Feature** and **Ensemble** Ids start from One (1) in DREAM.3D. DREAM.3D uses the Zero (0) index internally which has several ramifications that the user needs to understand. Statistics will _NOT_ be calculated for any **Feature** or **Ensemble** with an Id value of Zero (0). If your segmented data numbering starts with Zero (0), then the user needs to find a way to renumber **Feature**=0 to another value, typically the max Id + 1. For example, if you have a file with 123 **Features** and 2 **Ensembles** then the file would be similar to the following: 

## Example Input ##
For example, if you have a file with 123 **Features** and 2 **Ensembles** then the file would be similar to the following:

    123
    1   2  1.2  0.4  0.8
    2   1  0.8  0.99  1.0
    .....
    121 1 0.3  0.5  0.9
    123 2 1.0  1.1  0.03

## Parameters ##
| Name | Type | Description |
|------|------|-------------|
| Input Feature Info File | File Path | Input file path |
| Create Cell Level Arrays | Boolean | Whether to create **Element** level arrays for the imported data |
| Renumber Features | Boolean | Randomly shuffle the Feature Ids  |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | FeatureIds | Int | (1) | Specifies to which **Feature** each **Element** belongs |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|----------|
| Element | EulerAngles | Float | (3)  | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z). Only created if _Create Cell Level Arrays_ is checked |
| Element | Phases | Int | (1) | Phase Id specifying the phase of the **Element**. Only created if _Create Cell Level Arrays_ is checked |
| Feature | FeatureEulerAngles | Float | (3) | Three angles defining the orientation of each **Feature** in Bunge convention (Z-X-Z) |
| Feature | FeaturePhases | Int | (1) | Phase Id (int) specifying the phase of the **Feature** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


