Insert Precipitate Phases {#insertprecipitatephases}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
If there are precipitate phases in the Statistics file, then this filter will place precipitate **Features** with the sizes, shapes, physical orientations and locations corresponding to the goal statistics.  The user can specify if they want *periodic boundary conditions* and whether they would like to write out the goal attributes of the generated **Features**. 

Currently, the parameters that are matched to target parameters include: 

<ul>
<li>the fraction of precipitates on a grain boundary</li>
<li>the size distribution of the precipitates</li>
<li>the volume fraction of precipitates</li>
<li>the Omega 3 distribution of precipitates in each size bin</li>
<li>the b/a size distribution of precipitates in each size bin </li>
<li>the c/a size distribution of precipitates in each size bin </li>
<li>the axis orientation distribution </li>
<li>the normalized radial distribution function (if opted for by the user) </li>
</ul>

Note that for an ellipsoid a > b > c. Additionally, there is an exclusion zone around each precipitate such that no precipitate can be within another precipitate’s equivalent sphere diameter. 



## Parameters ##

| Name | Type |
|------|------|
| Periodic Boundary | Boolean (On or Off) |
| Match Radial Distribution Function | Boolean (On or Off) |
| Already Have Precipitates | Boolean (On or Off) |
| Precipitate Input File | Input File if “Already Have Precipitates” is checked|
| Write Goal Attributes | Boolean (On or Off) |
| Goal Attributes CSV File | Output File if “Write Goal Attributes” is checked|

## Required DataContainers ##
Volume

## Required Arrays ##
## Required Arrays ##

| Type | Default Name | Description | Comment | Filters Known to Create Data |
|------|--------------|-------------|---------|-----|
| Ensemble | Statistics | A data array of the goal statistics (size distribution, RDF, shape distribution, volume fraction and more) of the phases to be synthetically generated |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |  
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) |  | Generate Ensemble Statistics (Statistics), StatsGenerator Application |
| Ensemble | ShapeTypes |Enumeration (int) specifying the type of shape of each Ensemble/phase (Ellipsoid=0, Super Ellipsoid=1, Cube Octahedron=2, Cylinder=3, Unknown Shape Type=4)  |  | Establish Shape Types (SyntheticBuilding) |
| Cell | FeatureIds | Ids (ints) that specify to which **Feature** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Segment Features (Misorientation, C-Axis Misorientation, Scalar) (Reconstruction), Read Dx File (IO), Read Ph File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. | Read H5Ebsd File (IO), Pack Primary Phases (SyntheticBuilding), Insert Precipitate Phases (SyntheticBuilding), Establish Matrix Phase (SyntheticBuilding) |
| Cell | BoundaryCells | Boolean specifiying whether the cell is a boundary cell (true) or not (false)  |  | Find Boundary Cells (Generic), Find Feature Neighbors (Statistics) |
| Feature | FeaturePhases |Phase Id (int) specifying the phase of the **Feature**  |  | PackPrimaryPhases (Synthetic Building), Establish Matrix Phase (Synthetic Building)|
| Ensemble | NumFeatures | Number of features in each ensemble/phase (int) |  | PackPrimaryPhases (Synthetic Building), Establish Matrix Phase (Synthetic Building)|

## Created Arrays ##

| Type | Default Name | Comment |
|------|--------------|---------|
|  |  |  |

## Authors ##

**Copyright:** 2013 Joseph C. Tucker (UES), 2012 Michael A. Groeber (AFRL) ,2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.




See a bug? Does this documentation need updated with a citation? Send comments, corrections and additions to [The DREAM3D development team](mailto:dream3d@bluequartz.net?subject=Documentation%20Correction)

