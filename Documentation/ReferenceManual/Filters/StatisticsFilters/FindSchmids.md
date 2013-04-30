Find Schmid Factors {#findschmids}
======

## Group (Subgroup) ##
Statistics Filters (Crystallographic)

## Description ##
This filter calculates the Schmid Factor of each **Field** given its average orientation and a user defined loading axis.
The Schmid Factor is the combination of the component of the axial force F that lies parallel to the slip direction and the component 
that lies perpendicular to the slip plane.  The equation for the Schmid Factor is given as:

Schmid Factor = (cos φ cos λ)
*The angle φ is the angle between the tensile axis and the slip plane normal, and λ is the angle between the tensile axis and the slip direction in the slip plane.*
 
The filter determines the Schmid Factor for each **Field** by using the above equation for all possible slip systems (given the **Field**'s crystal structure).  The largest 
Schmid Factor from all of the slip systems is stored for the **Field**.


## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Loading X: | Double | X component of the loading axis in the sample reference frame |
| Loading Y: | Double | Y component of the loading axis in the sample reference frame |
| Loading Z: | Double | Z component of the loading axis in the sample reference frame |

## Required DataContainers ##
Voxel

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data
|------|--------------|-------------|---------|-----|
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. | Find Field Average Orientations (Statistics) |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Poles | Three (3) values (ints) giving the crystallographic pole that points along the user defined loading direction. |  |
| Field | Schmids | Value of the Schmid Factor (float) for the most favorably oriented slip system (ie the one with the highest Schmid Factor). | Only the Schmid Factor was used in determining which slip system's Schmid Factor to report.  The critical resolved shear stress for the different slip systems is not considered. |
| Field | SlipSystems | Enumeration (int) of the slip system that has the highest Schmid Factor | Numbers correspond to specific slip systems for the different crystal structures as follows: |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



