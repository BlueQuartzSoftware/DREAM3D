Merge Colonies {#}
======
<h1 class="pHeading1">Merge Colonies Filter</h1>
<p class="pCellBody">
<a href="../ReconstructionFilters/MergeColonies.html#wp2">Description</a> | <a href="../ReconstructionFilters/MergeColonies.html#wp3">Options</a> | <a href="../ReconstructionFilters/MergeColonies.html#wp4">Required Arrays</a> | <a href="../ReconstructionFilters/MergeColonies.html#wp5">Created Arrays</a> | <a href="../ReconstructionFilters/MergeColonies.html#wp1">Authors</a> 

Reconstruction Filters


In Ti alpha-beta alloys, cooling from the beta (BCC) phase slowly to the alpha phase (HCP) produces collections of alpha phase within a beta **Field** that are referred to as alpha colonies.  
Once the user has segmented the grains, this Filter can be run.  This Filter looks at
neighboring grains (as defined by the _Segment Grains (Misorientation)_ Filter) and asks for their
misorientation (i.e., the misorientation between the average orientation(s) of the two grains).  If the misorientation is found to be "special", i.e., one of the six (6) possible misorientations between colonies transformed from the same beta **Field**,
then the grains are merged together and called one **Field**.  Hence, the outputs of this Filter are the beta grains, because all the colonies transformed from a beta **Field** will all have the same **Field** id.  

## Parameters ##

| Name | Type |
|------|------|
| Axis Tolerance | Double |
| Angle Tolerance | Double |

## Required Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Cell | GrainIds | Ids (ints) that specify to which **Field** each **Cell** belongs. | Values should be present from segmentation of experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the **Field** in quaternion representation | Filter will calculate average quaternions for **Fields** if not already calculated. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the **Field** | Filter will determine **Field** phases if not previously determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |

## Created Arrays ##

| Type | Default Name | Description | Comment |
|------|--------------|-------------|---------|
| Field | Active | Boolean value specifying if the **Field** is still in the sample (1 if the **Field** is in the sample and 0 if it is not) | At the end of the filter, all **Fields** will be "Active" as the "Inactive" **Fields** will have been removed.  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



