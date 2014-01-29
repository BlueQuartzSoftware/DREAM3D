Generate Ensemble Statistics {#generateensemblestatistics}
==========

## Group (Subgroup) ##
Statistics Filters (Ensemble)

## Description ##
This filter will fit "common" statistical distributions (as chosen by the user) to specific **Feature** level attributes.  The user can chose both which attributes to fit and with what distribution to fit them.  The parameters that define the "best-fit" distributions will be stored in an **Ensemble** array.

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
| Feature | FeaturePhases | Phase Id (int) specifying the phase of the **Feature**| | Find Feature Phases (Generic), Read Feature Info File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Feature | AvgQuats | Five (5) values (floats) defining the average orientation of the **Feature** in quaternion representation | Filter will calculate average quaternions for **Features** if not already calculated. | Find Feature Average Orientations (Statistics) |
| Feature | FeatureEulerAngles | Three (3) angles (floats) defining the orientation of each **Feature** in Bunge convention (Z-X-Z) | This array is only required if the ODF option is checked | Find Feature Average Orientations (Statistics) |
| Feature | EquivalentDiameters | Diameter (float) of a sphere with the same volume as the **Feature**. | This array is only required if the Size, Aspect Ratio, Omega3, Neighborhoods or AxisODF option is checked | Find Feature Sizes (Statistics) |
| Feature | Volumes | Volume (float) in um^3 of the **Feature**. | This array is only required if the ODF option is checked | Find Feature Sizes (Statistics) |
| Feature | AspectRatios |  | This array is only required if the Aspect Ratio option is checked | Find Feature Shapes (Statistics) |
| Feature | AxisEulerAngles |  | This array is only required if the AxisODF option is checked | Find Feature Shapes (Statistics) |
| Feature | Omega3s |  | This array is only required if the Omega3 option is checked | Find Feature Shapes (Statistics) |
| Feature | NeighborLists | List of the  contiguous neighboring **Features** for a given **Feature** | This array is only required if the MDF option is checked | Find Feature Neighbors (Statistics) |
| Feature | SharedSurfaceAreaLists | List of the area shared between contiguous neighboring **Features** for a given **Feature** | This array is only required if the MDF option is checked | Find Feature Neighbors (Statistics) |
| Feature | SurfaceFeatures | Boolean flag equal to 1 if the **Feature** touches an outer surface of the sample and equal to 0 if it does not. | This array is only required if the ODF or MDF option is checked | Find Surface Features (Generic)
| Feature | Neighborhoods | Number (int) of **Features** that have their centroid within one equivalent sphere diameter of each **Feature** | This array is only required if the Neighborhoods option is checked | Find Feature Neighborhoods (Statistics)
| Feature | BiasedFeatures | Boolean flag of 1 if **Feature** is biased or of 0 if it is not | This array is only required if the Size, Aspect Ratio, Omega3, Neighborhoods or AxisODF option is checked | Find Biased Features (Bounding Box) (Generic) |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Read Ensemble Info File (IO), Initialize Synthetic Volume (SyntheticBuilding) |

## Created Arrays ##
| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Ensemble | Statistics |  |  |

## Authors ##

**Copyright:** 2014 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 5.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

