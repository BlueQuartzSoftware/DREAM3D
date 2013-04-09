Establish Matrix Phase {#establishmatrixphase}
======

## Group (Subgroup) ##
Synthetic Builder Filters (Packing)

## Description ##
To run this Filter properly, the _Initialize Synthetic Volume_ Filter must have been run already.
Based on the phase fraction in the statistics file used in the _Initialize Synthetic Volume_ Filter, this Filter assigns _**Cells**_ to a matrix phase. 

## Parameters ##

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Cell | CellPhases | Phase Id (int) specifying the phase of the **Cell** | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | PhaseTypes | Enumeration (int) specifying the type of phase of each Ensemble/phase (Primary=0, Precipitate=1, Transformation=2, Matrix=3, Boundary=4) | Values should be obtained from the input statistics file read in by IntializeSyntheticVolume |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** |  |

## Authors ##

**Copyright:** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info:** dream3d@bluequartz.net

**Version:** 1.0.0

**License:**  See the License.txt file that came with DREAM3D.



