Generate Ensemble Statistics {#generateensemblestatistics}
==========

## Group (Subgroup) ##
Statistics Filters (Ensemble)

## Description ##
This filter will fit "common" statistical distributions (as chosen by the user) to specific **Field** level attributes.  The user can chose both which attributes to fit and with what distribution to fit them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.

## Parameters ##

| Name | Type | Description |
|------|------| ----------- |
| Size Distribution | Boolean | Whether to fit the size distibution |
| Aspect Ratio Distribution | Boolean | Whether to fit the aspect ratio distibution |
| Omega3 Distribution | Boolean | Whether to fit the omega3 distibution |
| Neighborhoods Distribution | Boolean | Whether to fit the neighborhood distibution |
| Orientation Distribution| Boolean | Whether to fit the orientation distibution |
| Misorientation Distribution | Boolean | Whether to fit the misorientation distibution |
| Axis Orientation Distribution | Boolean | Whether to fit the axis orientation distibution |
| Size Distribution Fit Type | Choice | Which distribution to fit the size distibution with |
| Aspect Ratio Distribution Fit Type | Choice | Which distribution to fit the aspect ratio distibution with |
| Omega3 Distribution Fit Type | Choice | Which distribution to fit the omega3 distibution with |
| Neighborhoods Distribution Fit Type | Choice | Which distribution to fit the neighborhood distibution with |

## Required DataContainers ##
Volume

## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field**| | Find Field Phases (Generic), Read Field Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. | Find Field Average Orientations (Statistics) |
| Field | FieldEulerAngles | Three (3) angles (floats) defining the orientation of each **Field** in Bunge convention (Z-X-Z) | This array is only required if the ODF option is checked | Find Field Average Orientations (Statistics) |
| Field | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Field**. | This array is only required if the Size, Aspect Ratio, Omega3, Neighborhoods or AxisODF option is checked | Find Field Sizes (Statistics) |
| Field | Volumes | Volume (float) in um^3 of the **Field**. | This array is only required if the ODF option is checked | Find Field Sizes (Statistics) |
| Field | AspectRatios |  | This array is only required if the Aspect Ratio option is checked | Find Field Shapes (Statistics) |
| Field | AxisEulerAngles |  | This array is only required if the AxisODF option is checked | Find Field Shapes (Statistics) |
| Field | Omega3s |  | This array is only required if the Omega3 option is checked | Find Field Shapes (Statistics) |
| Field | NeighborLists | List of the  contiguous neighboring **Fields** for a given **Field** | This array is only required if the MDF option is checked | Find Field Neighbors (Statistics) |
| Field | SharedSurfaceAreaLists | List of the area shared between contiguous neighboring **Fields** for a given **Field** | This array is only required if the MDF option is checked | Find Field Neighbors (Statistics) |
| Field | SurfaceFields | Boolean flag equal to 1 if the **Field** touches an outer surface of the sample and equal to 0 if it does not. | This array is only required if the ODF or MDF option is checked | Find Surface Fields (Generic)
| Field | Neighborhoods | Number (int) of **Fields** that have their centroid within one equivalent sphere diameter of each **Field** | This array is only required if the Neighborhoods option is checked | Find Field Neighborhoods (Statistics)
| Field | BiasedFields | Boolean flag of 1 if **Field** is biased or of 0 if it is not | This array is only required if the Size, Aspect Ratio, Omega3, Neighborhoods or AxisODF option is checked | Find Biased Fields (Bounding Box) (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | Statistics |  |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

