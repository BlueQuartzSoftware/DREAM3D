Find MDF {#}
======
<h1 class="pHeading1">Find MDF Filter</h1>
<p class="pCellBody">
<a href="../StatisticsFilters/FindMDF.html#wp2">Description</a> | <a href="../StatisticsFilters/FindMDF.html#wp3">Options</a> | <a href="../StatisticsFilters/FindMDF.html#wp4">Required Arrays</a> | <a href="../StatisticsFilters/FindMDF.html#wp5">Created Arrays</a> | <a href="../StatisticsFilters/FindMDF.html#wp1">Authors</a> 

Statistics Filters


To generate a Misorientation Distribution Function (MDF), this filter uses __Field AvgQuats, FieldPhases__, and __SurfaceFields__ and __Ensemble CrystalStructures__, and __TotalSurfaceAreas__ values.


## Parameters ## 

## Required Arrays ##

| Type | Name | Description | Comment |
|------|------|-------------|---------|
| Field | AvgQuats | Five (5) values (floats) defining the average orientation of the feature/grain in quaternion representation | Filter will calculate average quaternions for features/grains if not already calculated. |
| Field | FieldPhases | Phase Id (int) specifying the phase of the feature/grain | Filter will determine feature/grain phases if not previously determined |
| Field | SurfaceFields | Boolean flag of 1 if feature/grain touches an outer surface or of 0 if it does not | Filter will determine if features/grains intersect an outer surface if not already determined |
| Ensemble | CrystalStructures | Enumeration (int) specifying the crystal structure of each Ensemble/phase (Hexagonal=0, Cubic=1, Orthorhombic=2) | Values should be present from experimental data or synthetic generation and cannot be determined by this filter. Not having these values will result in the filter to fail/not execute. |
| Ensemble | TotalSurfaceAreas |  |

## Created Arrays ##
None

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



