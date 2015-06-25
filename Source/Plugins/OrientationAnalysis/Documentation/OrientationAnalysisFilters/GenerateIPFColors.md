Generate IPF Colors {#generateipfcolors}
=============

## Group (Subgroup) ##
Processing (Crystallography)

## Description ##
This Filter will generate _inverse pole figure_ (IPF) colors for cubic, hexagonal or trigonal crystal structures. The user can enter the _Reference Direction_, which defaults to [001]. The **Filter** also has the option to apply a black color to all "bad" **Elements**, as defined by a boolean _mask_ array, which can be generated using the [Threshold Objects](@ref multithresholdobjects) **Filter**.

### Originating Data Notes ###

+ TSL (.ang file)
    - If the data originates from a TSL .ang file, then **Elements** that the TSL software could not reliably identify the Euler angles for will have a "Fit of Solution" = 180 and/or an "Image Quality" = 0.0.
    - This means that when the user runs some sort of threshold filter the _mask_ will be those **Elements** that have an Image Quality > 0 and/or Fit < 180.0
+ HKL (.ctf file)
    - If the data originates from an HKL (or Bruker) system (.ctf file) then bad voxels can typically be found by setting "Error" > 0
    - This means that when the user runs some sort of threshold filter the _mask_ will be those **Elements** that have an Error = 0

-----

![IPF Color Triangle](images/IPFFilterLegend.png)
@image latex IPFFilterLegend.png "IPF Color Triangle" width=6in

-----

![Example Data Set](images/IPFColor_1.png)
@image latex IPFColor_1.png "Example Data Set" width=6in

-----

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Reference Direction | Float (3x) | The reference axis with respect to compute the IPF colors |
| Apply to Good Elements Only (Bad Elements Will Be Black) | Bool | Whether to assign a black color to "bad" **Elements** |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | EulerAngles | Float | (3)  | Three angles defining the orientation of the **Element** in Bunge convention (Z-X-Z) |
| Element | Phases | Int | (1) | Phase Id specifying the phase of the **Element** |
| Element | Mask | Bool | (1) | Boolean array that flags **Elements** as "good" (true) or "bad" (false) |
| Ensemble | CrystalStructures | Int | (1) | Enumeration representing the crystal structure for each phase |

## Created Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Element | IPFColor |  UInt8 | (3) | The RGB colors encoded as unsigned chars for each **Element** |

## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


