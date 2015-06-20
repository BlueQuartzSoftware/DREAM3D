Find Schmid Factors {#findschmids}
=============

## Group (Subgroup) ##
Statistics (Crystallographic)

## Description ##
This Filter calculates the Schmid factor of each **Feature** given its average orientation and a user defined loading axis.
The Schmid Factor is the combination of the component of the axial force _F_ that lies parallel to the slip direction and the component that lies perpendicular to the slip plane.  The equation for the Schmid Factor is given as:

Schmid Factor = (cos &phi; cos &lambda;)
*The angle &phi; is the angle between the tensile axis and the slip plane normal, and &lambda; is the angle between the tensile axis and the slip direction in the slip plane.*
 
The Filter determines the Schmid factor for each **Feature** by using the above equation for all possible slip systems (given the **Feature**'s crystal structure).  The largest Schmid Factor from all of the slip systems is stored for the **Feature**. Only the Schmid factor is used in determining which slip system's Schmid factor to report.  The critical resolved shear stress for the different slip systems is not considered. 

## Parameters ##
| Name | Type | Description |
|------|------| ----------- |
| Loading Direction | Float (x3) | Vector defining the loading axis |
| Store Angle Components of Schmid Factor | Boolean | Whether to store the &phi; and &lambda; for each **Feature** |
| Override Default Slip System | Boolean | Allows the user to manually input the slip plane and slip direction |
| Slip Plane | Float (x3) | Vector defining the slip plane normal. Only needed if _Override Default Slip System_ is checked |
| Slip Direction | Float (x3) | Vector defining the slip direction. Only needed if _Override Default Slip System_ is checked |

## Required Geometry ##
Not Applicable

## Required Arrays ##
| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Phases | Int | (1) | Specifies to which **Ensemble** each **Feature** belongs |
| Feature | AvgQuats | Float | (4) | Specifies the average orienation of each **Feature** in quaternion representation |
| Ensemble | CrystalStructures | Enumeration representing the crystal structure for each **Ensemble** |

## Created Arrays ##

| Type | Default Name | Type | Component Dimensions | Description |
|------|--------------|-------------|---------|-----|
| Feature | Schmids | Float | (1) | Value of the Schmid factor for the most favorably oriented slip system (i.e., the one with the highest Schmid factor) |
| Feature | SlipSystems | Int | (1) | Enumeration of the slip system that has the highest Schmid factor |
| Feature | Poles | Ints | (3) | Specifies the crystallographic pole that points along the user defined loading direction |
| Feature | SchmidPhis | Float | (1) | Angle between tensile axis and slip plane normal. Only created if _Store Angle Components of Schmid Factor_ is checked |
| Feature | SchmidLambdas | Float | (1) | Angle between tensile axis and splid drirection. Only created if _Store Angle Components of Schmid Factor_ is checked |


## License & Copyright ##

Please see the description file distributed with this plugin.

## DREAM3D Mailing Lists ##

If you need more help with a filter, please consider asking your question on the DREAM3D Users mailing list:
https://groups.google.com/forum/?hl=en#!forum/dream3d-users


