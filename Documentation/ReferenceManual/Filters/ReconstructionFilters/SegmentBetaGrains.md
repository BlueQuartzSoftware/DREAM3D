
Segment Grains (Misorientation) {#}
======
<h1 class="pHeading1">Segment Grains (Misorientation) Filter</h1>
<p class="pCellBody">
<a href="../Filters/SegmentBetaGrains.html#wp2">Description</a>
| <a href="../Filters/SegmentBetaGrains.html#wp3">Options</a>
| <a href="../Filters/SegmentBetaGrains.html#wp4">Required Arrays</a>
| <a href="../Filters/SegmentBetaGrains.html#wp5">Created Arrays</a>
| <a href="../Filters/SegmentBetaGrains.html#wp1">Authors</a> 

## Parameters ## 

| Name | Type |
|------|------|
| Misorientation Tolerance | Double |
| Axis Tolerance | Double |
| Angle Tolerance | Double |

## Required Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | CellPhases |  |
| Cell | Quats |  |
| Ensemble | CrystalStructures |  |

## Created Arrays ##

| Type | Name | Comment |
|------|------|---------|
| Cell | GoodVoxels |  |
| Cell | GrainIds |  |
| Field | Active |  |

## Authors ##

**Copyright** 2012 Michael A. Groeber (AFRL),2012 Michael A. Jackson (BlueQuartz Software)

**Contact Info** dream3d@bluequartz.net

**Version** 1.0.0

**License**  See the License.txt file that came with DREAM3D.



