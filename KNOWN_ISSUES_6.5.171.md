# Known Issues in DREAM.3D 6.5.171

- **Applies to:** DREAM.3D 6.5.171 (the last 6.x release, built 2023-04-07)
- **Support status:** DREAM.3D 6.5.171 is end of life. BlueQuartz Software does not release fixes for it. DREAM3D-NX replaces it.
- **Updates:** We add entries and change the DREAM3D-NX status of entries when we find new information. Check the
  [Revision history](#revision-history) to see when this page last changed.

## Revision history

| Revision | Date | Changes |
|---|---|---|
| 1 | 2026-11-03 | First published version. 63 entries. |

## How to use this page

- Find the filters in your pipeline in the [Index by filter](#index-by-filter).
- Read the **Trigger** of each entry. The defect affects your results only when your pipeline matches the trigger.
- Use the **Workaround** if there is one. If there is no workaround, use DREAM3D-NX for that step, and check the
  **DREAM3D-NX status** first.

## Summary

| Severity | Count |
|---|---|
| Crash | 13 |
| Wrong result | 34 |
| Precision | 3 |
| Error / usability | 13 |
| **Total entries** | **63** |

Some entries cover more than one filter.

---

## 1. Wrong results

### KI-001 Import Oxford Instr. EBSD Data (.ctf): unindexed points become phase 1

- **Filter:** Import Oxford Instr. EBSD Data (.ctf)
- **Severity:** Wrong result
- **Symptom:** Every point with phase 0 (not indexed) gets phase 1. For a hexagonal phase 1, these points also get the +30 degree Euler correction.
- **Trigger:** Any .ctf file that has unindexed points.
- **Workaround:** Make a mask from the original Error or Bands columns, and use it in place of Phases > 0.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** [DREAM3D#1037](https://github.com/BlueQuartzSoftware/DREAM3D/issues/1037), [simplnx#937](https://github.com/BlueQuartzSoftware/simplnx/issues/937)

### KI-002 Find Neighbor Slip Transmission Metrics: all four metrics are wrong

- **Filter:** Find Neighbor Slip Transmission Metrics
- **Severity:** Wrong result
- **Symptom:** The filter copies both feature orientations into the same variable. The second orientation is never set. All F1, F1spt, F7 and mPrime values are wrong.
- **Trigger:** Every use of the filter.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Not present in DREAM3D-NX. A different defect is open there: the metrics are calculated only for phase index 1, not for every cubic phase ([simplnx#1861](https://github.com/BlueQuartzSoftware/simplnx/issues/1861)).
- **References:** —

### KI-003 Find Feature Shapes: Axis Euler Angles are wrong for some features

- **Filter:** Find Feature Shapes
- **Severity:** Wrong result
- **Symptom:** The code must make the principal-axis frame right-handed. That correction never runs. For features with a left-handed axis frame, the Euler angles describe the wrong a and b axes. The c axis is correct. Aspect ratios and axis lengths are not affected.
- **Trigger:** Any feature whose computed principal axes form a left-handed frame. In a test with 8 rotated ellipsoids, 6 of 8 were wrong.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** [simplnx#1896](https://github.com/BlueQuartzSoftware/simplnx/issues/1896)

### KI-004 Neighbor Orientation Comparison (Bad Data): wrong cells are marked good

- **Filter:** Neighbor Orientation Comparison (Bad Data)
- **Severity:** Wrong result
- **Symptom:** The filter does one level too few. It also uses the misorientation of an earlier neighbor when the current neighbor has a different phase. Some bad cells are marked good, or good cells stay bad.
- **Trigger:** Every use of the filter. The result changes when neighbors have different phases.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** [SIMPL#439](https://github.com/BlueQuartzSoftware/SIMPL/issues/439), [simplnx#1499](https://github.com/BlueQuartzSoftware/simplnx/issues/1499)

### KI-005 Neighbor Orientation Correlation: wrong cleanup result

- **Filter:** Neighbor Orientation Correlation
- **Severity:** Wrong result
- **Symptom:** Three defects. (1) A neighbor pair with a different phase uses the misorientation of the previous pair. (2) The filter does half of the documented passes. (3) The "best neighbor" is not always the most similar neighbor.
- **Trigger:** Every use of the filter.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-006 Find GBCD: results depend on how the work is divided

- **Filter:** Find GBCD
- **Severity:** Wrong result
- **Symptom:** Triangles with a negative face label are skipped in the triangle counter. Later triangles are put in the wrong bins. The result changes with the number of triangles in each chunk.
- **Trigger:** Triangle geometries that have faces on the outer surface (face label -1). This is the normal case for Quick Surface Mesh output.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** [DREAM3D#508](https://github.com/BlueQuartzSoftware/DREAM3D/issues/508), [simplnx#1568](https://github.com/BlueQuartzSoftware/simplnx/pull/1568)

### KI-007 Quick Surface Mesh: transferred face data goes to the wrong side

- **Filter:** Quick Surface Mesh
- **Severity:** Wrong result
- **Symptom:** With **Attribute Arrays to Transfer**, component 0 of a face array must belong to the feature in Face Labels component 0. When the feature id of the cell is smaller than the feature id of its neighbor, the two sides are swapped. On outer-surface faces, the cell value is written to the exterior (-1) side.
- **Trigger:** Any array in **Attribute Arrays to Transfer**.
- **Workaround:** Do not transfer arrays. Make face data from Face Labels and feature-level arrays in later filters.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (side swap). The exterior-side write is only in 6.5.171.
- **References:** —

### KI-008 Rotate Sample Reference Frame: wrong resolution for some rotations

- **Filter:** Rotate Sample Reference Frame
- **Severity:** Wrong result
- **Symptom:** The new resolution and dimensions are assigned to the wrong axes.
- **Trigger:** A rotation that is not about the X, Y or Z axis (for example axis (1,1,1), 120 degrees), on data with different X, Y and Z resolutions.
- **Workaround:** Use rotations about one coordinate axis. Do several rotations if necessary.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-009 ITK::Maximum Projection Image Filter and ITK::Standard Deviation Projection Image Filter: other arrays are deleted

- **Filter:** ITK::Maximum Projection Image Filter, ITK::Standard Deviation Projection Image Filter
- **Severity:** Wrong result
- **Symptom:** All other arrays in the Cell Attribute Matrix are deleted. There is no error or warning.
- **Trigger:** Every use, with **Save as New Array** on or off.
- **Workaround:** Copy the arrays you need to a different Attribute Matrix before this filter.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-010 Align Sections (Feature Centroid): one section is not shifted

- **Filter:** Align Sections (Feature Centroid)
- **Severity:** Wrong result
- **Symptom:** One section always gets a shift of 0.
- **Trigger:** **Use Reference Slice** on, with **Reference Slice** not 0.
- **Workaround:** Use Reference Slice 0.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-011 Import EDAX OIMAnalysis Data (.h5): Stacking Order buttons do nothing

- **Filter:** Import EDAX OIMAnalysis Data (.h5) (also the Esprit .h5 reader)
- **Severity:** Wrong result
- **Symptom:** The Stacking Order buttons have no effect. The Z order is always the order of the scan list.
- **Trigger:** Any multi-scan import where you select a Stacking Order.
- **Workaround:** Select the scans in the Z order that you want.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (similar)
- **References:** —

### KI-012 Export INL File: Symmetry is 0 for most crystal structures

- **Filter:** Export INL File
- **Severity:** Wrong result
- **Symptom:** The Symmetry column is 0 for every phase that is not cubic or hexagonal.
- **Trigger:** Any phase with a crystal structure that is not Cubic or Hexagonal.
- **Workaround:** Edit the Symmetry column in the output file.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-013 Find Feature Face Curvature: garbage values for one-triangle faces

- **Filter:** Find Feature Face Curvature
- **Severity:** Wrong result
- **Symptom:** The filter reads past the end of an array. The curvature values are random.
- **Trigger:** A feature face that has only one triangle.
- **Workaround:** Ignore curvature values for faces with one triangle.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (similar)
- **References:** —

### KI-014 Generate Vector Colors: some vectors are black

- **Filter:** Generate Vector Colors
- **Severity:** Wrong result
- **Symptom:** A vector with a very small negative trend gets no color (black or gray).
- **Trigger:** Rare. Depends on the vector values.
- **Workaround:** None.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-015 Export StatsGenerator ODF Angle File: colon delimiter writes a quote

- **Filter:** Export StatsGenerator ODF Angle File
- **Severity:** Wrong result
- **Symptom:** The file uses `"` between values, not `:`.
- **Trigger:** **Delimiter** = colon.
- **Workaround:** Use a different delimiter.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-016 Vtk Rectilinear Grid Exporter: small coordinates written as 0

- **Filter:** Vtk Rectilinear Grid Exporter
- **Severity:** Wrong result
- **Symptom:** In ASCII mode, coordinates are written with 6 decimal places. Spacing or origin values smaller than about 5e-7 become 0.000000. Cell data is written with 6 significant digits.
- **Trigger:** **Write Binary File** off, with very small spacing or origin.
- **Workaround:** Write a binary file.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (coordinates)
- **References:** —

### KI-017 Find Feature Clustering: RDF bins are about 0.2% high

- **Filter:** Find Feature Clustering (the same function is used by StatsGenerator, Insert Precipitate Phases and Generate Precipitate StatsData)
- **Severity:** Precision
- **Symptom:** The radial distribution function values are about 0.2% too high.
- **Trigger:** Every use.
- **Workaround:** None. The error is small.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-018 Find Feature Neighborhoods: neighborhood is a box, not a sphere

- **Filter:** Find Feature Neighborhoods
- **Severity:** Wrong result
- **Symptom:** A feature counts as a neighbor when it is inside a box around the centroid. The documented test is a sphere (a distance). Neighborhood counts are too high.
- **Trigger:** Every use.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-019 Find Feature Neighbors: shared surface area is wrong for non-cubic cells

- **Filter:** Find Feature Neighbors
- **Severity:** Wrong result
- **Symptom:** Every shared face uses the X-Y face area. Faces normal to X or Y get the wrong area. Surface-feature detection is also wrong on 2D geometry.
- **Trigger:** Cells with different X, Y and Z resolutions, or a geometry with one dimension of 1.
- **Workaround:** Use equal resolutions.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-020 Isolate Largest Feature (Identify Sample): holes are not filled on 2D data

- **Filter:** Isolate Largest Feature (Identify Sample)
- **Severity:** Wrong result
- **Symptom:** With **Fill Holes** on, every cell is treated as on the boundary. No holes are filled.
- **Trigger:** A 2D (or 1D) Image Geometry.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-021 Find Feature Phases: feature 0 gets a phase

- **Filter:** Find Feature Phases
- **Severity:** Wrong result
- **Symptom:** Feature 0 (background) gets the phase of the last background cell. It must be 0.
- **Trigger:** Data with background cells (Feature Id 0).
- **Workaround:** Set Feature Phases[0] to 0 after the filter.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** [simplnx#1672](https://github.com/BlueQuartzSoftware/simplnx/pull/1672)

### KI-022 Find Feature Average Orientations: feature 0 and empty features

- **Filter:** Find Feature Average Orientations
- **Severity:** Wrong result
- **Symptom:** Feature 0 gets an invalid (all-zero) quaternion. A feature with no cells gets NaN values.
- **Trigger:** Feature 0, or a feature id with no cells.
- **Workaround:** Ignore feature 0 and empty features.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-023 Find Feature Neighbor Misorientations and Find Feature Neighbor C-Axis Misalignments: wrong average

- **Filter:** Find Feature Neighbor Misorientations, Find Feature Neighbor C-Axis Misalignments
- **Severity:** Wrong result
- **Symptom:** The per-feature average uses the wrong divisor. Find Feature Neighbor C-Axis Misalignments also does not support Hexagonal-Low.
- **Trigger:** Features with neighbors that have a different phase.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-024 Find Kernel Average Misorientations: wrong kernel in X

- **Filter:** Find Kernel Average Misorientations
- **Severity:** Wrong result
- **Symptom:** The kernel in X uses the Z kernel size.
- **Trigger:** **Kernel Radius** X different from Z.
- **Workaround:** Use the same X and Z kernel radius.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-025 Generate IPF Colors (Face): wrong colors on mixed-phase faces

- **Filter:** Generate IPF Colors (Face)
- **Severity:** Wrong result
- **Symptom:** The color of the second side uses the crystal structure of the first side's phase.
- **Trigger:** Faces between two features with different crystal structures.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-026 Trigonal (-3) misorientation distribution: wrong axis

- **Filter:** Filters that compute the misorientation distribution (MDF) for Trigonal -3 phases (for example StatsGenerator, Generate Ensemble Statistics)
- **Severity:** Wrong result
- **Symptom:** The misorientation axis has a typo (y is overwritten with z). The MDF for Trigonal -3 phases is wrong.
- **Trigger:** A phase with Laue class Trigonal -3.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-027 Generate Primary StatsData: Rolled preset ignores aspect ratios

- **Filter:** Generate Primary StatsData
- **Severity:** Wrong result
- **Symptom:** The Rolled preset does not use the aspect ratios. Features are equiaxed.
- **Trigger:** Microstructure Preset Model = Rolled.
- **Workaround:** Use StatsGenerator and enter the aspect ratios there.
- **DREAM3D-NX status:** Not in DREAM3D-NX (planned port has the same defect)
- **References:** [DREAM3D#873](https://github.com/BlueQuartzSoftware/DREAM3D/issues/873)

### KI-028 StatsGenerator: NaN in neighbor distribution

- **Filter:** StatsGenerator
- **Severity:** Wrong result
- **Symptom:** The neighbor distribution standard deviation is NaN.
- **Trigger:** 14 or more bins (16 or more with the Rolled preset).
- **Workaround:** Use fewer bins.
- **DREAM3D-NX status:** Not in DREAM3D-NX (planned port has the same formula)
- **References:** [DREAM3D#987](https://github.com/BlueQuartzSoftware/DREAM3D/issues/987)

### KI-029 Establish Shape Types: Super Ellipsoid gives NaN radius

- **Filter:** Establish Shape Types (with synthetic microstructure filters)
- **Severity:** Wrong result
- **Symptom:** Synthetic generation fails or makes no features. The radius is NaN.
- **Trigger:** Shape type Super Ellipsoid with 2D statistics or statistics without Omega3.
- **Workaround:** Use Ellipsoid.
- **DREAM3D-NX status:** Not in DREAM3D-NX (planned port has the same code)
- **References:** [DREAM3D#633](https://github.com/BlueQuartzSoftware/DREAM3D/issues/633)

### KI-030 Slip transmission metrics for non-cubic phases are 0

- **Filter:** Find Neighbor Slip Transmission Metrics (and the mPrime, F1, F1spt, F7 functions)
- **Severity:** Wrong result
- **Symptom:** The metrics are 0 for all non-cubic Laue classes. The cubic math was never validated.
- **Trigger:** Non-cubic phases.
- **Workaround:** None.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** [DREAM3D#106](https://github.com/BlueQuartzSoftware/DREAM3D/issues/106)

### KI-031 Fundamental-zone quaternions are not unique on the zone boundary

- **Filter:** Filters that reduce orientations to the fundamental zone (getFZQuat)
- **Severity:** Wrong result
- **Symptom:** Orientations on the fundamental-zone boundary are not moved to one unique form. For Laue classes that are not cubic, hexagonal or orthorhombic, the input is returned unchanged.
- **Trigger:** Orientations on the zone boundary, or non-cubic/hexagonal/orthorhombic phases.
- **Workaround:** None.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** [DREAM3D#458](https://github.com/BlueQuartzSoftware/DREAM3D/issues/458), [EbsdLib#63](https://github.com/BlueQuartzSoftware/EbsdLib/issues/63)

### KI-032 Read H5EBSD File: missing slices are not detected

- **Filter:** Read H5EBSD File
- **Severity:** Wrong result
- **Symptom:** The end slice is not checked against the slices in the file. For H5EBSD files made from .ang files, missing slices are filled with zeros without an error. For files made from .ctf files, the filter stops with an error.
- **Trigger:** An End Slice larger than the last slice in the file (pipelines edited by hand, or files with missing slices).
- **Workaround:** Check the slice range in the file before you run.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** [DREAM3D#952](https://github.com/BlueQuartzSoftware/DREAM3D/issues/952), [simplnx#1867](https://github.com/BlueQuartzSoftware/simplnx/issues/1867)

### KI-033 PipelineRunner misreads numbers in comma-decimal locales

- **Filter:** Some text-file readers, when run with PipelineRunner on macOS or Linux
- **Severity:** Wrong result
- **Symptom:** Numbers like 0.5 are read wrong when the system locale uses a comma for decimals.
- **Trigger:** PipelineRunner on macOS or Linux with a locale such as de_DE.
- **Workaround:** Set `LC_ALL=C` (or `LC_NUMERIC=C`) before you run PipelineRunner.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (not yet verified)
- **References:** [SIMPL#102](https://github.com/BlueQuartzSoftware/SIMPL/issues/102), [simplnx#1877](https://github.com/BlueQuartzSoftware/simplnx/issues/1877)

### KI-034 Table values are rounded when a pipeline is opened

- **Filter:** Filters with a table input (for example Create Data Array, Create Attribute Matrix)
- **Severity:** Wrong result
- **Symptom:** Table values are saved with 6 significant digits. Large values (for example tuple counts above 999999) change when you open the pipeline again.
- **Trigger:** Values with more than 6 significant digits.
- **Workaround:** Check table values after you open a pipeline.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (not yet verified)
- **References:** [DREAM3D#1003](https://github.com/BlueQuartzSoftware/DREAM3D/issues/1003)

### KI-035 Export DAMASK Files: grain-wise geometry file format

- **Filter:** Export DAMASK Files
- **Severity:** Wrong result
- **Symptom:** The grain-wise .geom file does not use the format that DAMASK expects (10 values per line).
- **Trigger:** Grain-wise output.
- **Workaround:** Reformat the .geom file.
- **DREAM3D-NX status:** Not in DREAM3D-NX
- **References:** [DREAM3D#982](https://github.com/BlueQuartzSoftware/DREAM3D/issues/982)

### KI-036 Find Feature Centroids, Find Feature Sizes and other filters: small precision errors

- **Filter:** Find Feature Centroids, Find Feature Sizes (Rectilinear Grid), Find Average C-Axis Orientations, Find Feature Reference C-Axis Misalignments, Rotate Euler Reference Frame, Hexagonal misorientation calculations, Convert Orientation Representation, Neighbor Orientation Comparison tolerance
- **Severity:** Precision
- **Symptom:** These filters use single precision (float) where double precision is necessary. Results differ by a small amount. Convert Orientation Representation can also change valid input Euler angles (Phi = pi).
- **Trigger:** Every use. The effect is largest for large geometries or values near a boundary.
- **Workaround:** None. The errors are small.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-038 Find Feature Shapes: Omega3 is not accurate for long, thin features

- **Filter:** Find Feature Shapes
- **Severity:** Precision
- **Symptom:** The moment sums for each feature are added in single precision (float). For features with many cells, Omega3 can be wrong in the third decimal place.
- **Trigger:** Large features, especially long and thin ones. In a test with a 4096 x 8 x 4 cuboid, Omega3 was 0.80248 instead of 0.80352.
- **Workaround:** None. Axis lengths and aspect ratios are only slightly affected (about 3e-8 in the same test).
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** [DREAM3D#425](https://github.com/BlueQuartzSoftware/DREAM3D/issues/425), [simplnx#1871](https://github.com/BlueQuartzSoftware/simplnx/issues/1871)

---

## 2. Crashes

### KI-050 Import STL File: crash on a corrupt file

- **Filter:** Import STL File
- **Severity:** Crash
- **Symptom:** The application crashes.
- **Trigger:** A binary STL file with a wrong triangle count in the header (for example a truncated file).
- **Workaround:** Repair the STL file in another program.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (6.5.171 is worse: there is no file-size check)
- **References:** —

### KI-051 ITK::Import Images (3D Stack): crash or zero slices

- **Filter:** ITK::Import Images (3D Stack)
- **Severity:** Crash
- **Symptom:** The application crashes when the images have different pixel types. When the images have different sizes, some slices are filled with zeros and there is no error.
- **Trigger:** An image stack with mixed pixel types or mixed sizes.
- **Workaround:** Convert all images to the same size and pixel type first.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (crash, similar). The size check is only broken in 6.5.171.
- **References:** —

### KI-052 Add Bad Data: crash with string arrays

- **Filter:** Add Bad Data
- **Severity:** Crash
- **Symptom:** The application crashes, or 64-bit and neighbor-list arrays get random values.
- **Trigger:** **Add Random Noise** on, and the Attribute Matrix of the Boundary Euclidean Distances also holds string, neighbor-list or 64-bit arrays.
- **Workaround:** Move those arrays to another Attribute Matrix first.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (similar)
- **References:** —

### KI-053 ITK::Image Reader: multi-page TIFF with mixed pages

- **Filter:** ITK::Image Reader
- **Severity:** Crash
- **Symptom:** The application crashes, or the data is wrong.
- **Trigger:** A multi-page TIFF where the pages have different samples per pixel.
- **Workaround:** Split the TIFF into one file per page.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (similar; defect is in ITK)
- **References:** —

### KI-054 Find Feature Volumes from Triangle Geometry: crash with gaps in Face Labels

- **Filter:** Find Feature Volumes from Triangle Geometry
- **Severity:** Crash
- **Symptom:** The filter writes past the end of an array. The application can crash, or other data can change.
- **Trigger:** Face Labels with gaps in the feature ids (for example 1, 2, 5).
- **Workaround:** Renumber the features so that the ids have no gaps.
- **DREAM3D-NX status:** Not present in DREAM3D-NX
- **References:** —

### KI-055 Find Feature Reference Misorientations: crash in mode 1

- **Filter:** Find Feature Reference Misorientations
- **Severity:** Crash
- **Symptom:** The application crashes.
- **Trigger:** **Reference Orientation** = the orientation at the feature centroid (mode 1).
- **Workaround:** Use mode 0 (average orientation).
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-056 Import Oxford Instr. EBSD Data (.ctf): crash on malformed files

- **Filter:** Import Oxford Instr. EBSD Data (.ctf)
- **Severity:** Crash
- **Symptom:** The application crashes.
- **Trigger:** A .ctf file with an empty phase header, missing columns, or a phase value that is out of range.
- **Workaround:** Check the file in a text editor.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-057 Import EDAX EBSD Data (.ang): crash with gaps in phase numbers

- **Filter:** Import EDAX EBSD Data (.ang)
- **Severity:** Crash
- **Symptom:** The filter writes past the end of an array.
- **Trigger:** An .ang file where the phase numbers have gaps (for example phases 1 and 3).
- **Workaround:** Renumber the phases in the header.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** —

### KI-058 Insert Atoms: crash with low memory

- **Filter:** Insert Atoms
- **Severity:** Crash
- **Symptom:** The application crashes when there is not enough memory.
- **Trigger:** Large lattice constants relative to the geometry.
- **Workaround:** Use smaller geometries.
- **DREAM3D-NX status:** Not in DREAM3D-NX
- **References:** [DREAM3D#944](https://github.com/BlueQuartzSoftware/DREAM3D/issues/944)

### KI-059 Setting the Data Directory crashes DREAM.3D

- **Filter:** Preferences (user interface)
- **Severity:** Crash
- **Symptom:** The application crashes.
- **Trigger:** Change the Data Directory.
- **Workaround:** None known.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (not yet verified)
- **References:** [DREAM3D#1000](https://github.com/BlueQuartzSoftware/DREAM3D/issues/1000)

### KI-060 Pipeline view: crash when you delete or drag filters quickly

- **Filter:** Pipeline view (user interface)
- **Severity:** Crash
- **Symptom:** The application crashes.
- **Trigger:** Delete several filters quickly, or drag a filter by its delete button.
- **Workaround:** Delete filters slowly, one at a time. Save the pipeline often.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [SIMPL#315](https://github.com/BlueQuartzSoftware/SIMPL/issues/315)

### KI-061 Pipeline view: duplicate filters

- **Filter:** Pipeline view (user interface)
- **Severity:** Crash
- **Symptom:** Two copies of the same filter in one pipeline cause wrong selections and can crash.
- **Trigger:** The same filter instance shows twice in the pipeline.
- **Workaround:** Add a new filter from the filter list in place of a copy.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [DREAM3D#921](https://github.com/BlueQuartzSoftware/DREAM3D/issues/921)

### KI-062 XDMF output for non-3-component data

- **Filter:** Write DREAM.3D Data File (Write Xdmf File on)
- **Severity:** Crash
- **Symptom:** ParaView can crash or show wrong data when it reads the .xdmf file.
- **Trigger:** Arrays that need a hyperslab with dimensions other than "3 2".
- **Workaround:** Open the .dream3d file directly, or edit the .xdmf file.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** [DREAM3D#305](https://github.com/BlueQuartzSoftware/DREAM3D/issues/305), [simplnx#1853](https://github.com/BlueQuartzSoftware/simplnx/issues/1853)

---

## 3. Errors and usability

### KI-080 Execute Process: arguments are split wrong

- **Filter:** Execute Process
- **Severity:** Error / usability
- **Symptom:** An argument can start with a space.
- **Trigger:** A quoted path followed by more arguments, or two spaces between arguments.
- **Workaround:** Use one space between arguments, and put quoted paths last.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-081 Read DREAM.3D Data File: missing items are dropped without a warning

- **Filter:** Read DREAM.3D Data File
- **Severity:** Error / usability
- **Symptom:** Selected arrays that are not in the file are removed from the selection. There is no warning.
- **Trigger:** A saved pipeline used with a different .dream3d file.
- **Workaround:** Check the selection after you change the file.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (similar)
- **References:** —

### KI-082 Vtk Rectilinear Grid Exporter and VTK Structured Points Exporter: arrays with more than 4 components

- **Filter:** Vtk Rectilinear Grid Exporter, VTK Structured Points Exporter
- **Severity:** Error / usability
- **Symptom:** Arrays with more than 4 components are written as SCALARS. Some VTK readers reject the file.
- **Trigger:** Arrays with more than 4 components.
- **Workaround:** Do not export those arrays.
- **DREAM3D-NX status:** Also present in DREAM3D-NX
- **References:** —

### KI-083 VTK STRUCTURED_POINTS Importer: 2D files fail

- **Filter:** VTK STRUCTURED_POINTS Importer
- **Severity:** Error / usability
- **Symptom:** The filter fails with error -61006.
- **Trigger:** A 2D file (DIMENSIONS with Z = 1) that has CELL_DATA.
- **Workaround:** None in 6.5.171.
- **DREAM3D-NX status:** Also present in DREAM3D-NX (similar)
- **References:** —

### KI-084 Generate Ensemble Statistics fails without a .dream3d input

- **Filter:** Generate Ensemble Statistics
- **Severity:** Error / usability
- **Symptom:** The filter fails.
- **Trigger:** The data was not read from a .dream3d file.
- **Workaround:** Write the data to a .dream3d file and read it again before this filter.
- **DREAM3D-NX status:** Not in DREAM3D-NX
- **References:** [DREAM3D#653](https://github.com/BlueQuartzSoftware/DREAM3D/issues/653)

### KI-085 File readers: file extension check is case sensitive

- **Filter:** File readers (for example Convert Hexagonal Grid Data to Square Grid Data)
- **Severity:** Error / usability
- **Symptom:** Files with an upper-case extension (for example .ANG) are rejected.
- **Trigger:** Upper-case file extensions.
- **Workaround:** Rename the file with a lower-case extension.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (not yet verified) for most readers. Also present in DREAM3D-NX for Convert Hexagonal Grid Data to Square Grid Data and for Import Orientation File(s) to H5EBSD.
- **References:** [DREAM3D#902](https://github.com/BlueQuartzSoftware/DREAM3D/issues/902), [simplnx#1906](https://github.com/BlueQuartzSoftware/simplnx/issues/1906), [simplnx#1867](https://github.com/BlueQuartzSoftware/simplnx/issues/1867)

### KI-086 File readers: unclear error when the file does not exist

- **Filter:** File readers
- **Severity:** Error / usability
- **Symptom:** The error message does not say that the file is missing.
- **Trigger:** An input file path that does not exist.
- **Workaround:** Check the file path.
- **DREAM3D-NX status:** Fixed in DREAM3D-NX (verified)
- **References:** [DREAM3D#904](https://github.com/BlueQuartzSoftware/DREAM3D/issues/904)

### KI-087 Delete Data: selections are not restored

- **Filter:** Delete Data
- **Severity:** Error / usability
- **Symptom:** The checked items are not restored when you open a pipeline.
- **Trigger:** Open a pipeline that contains Delete Data.
- **Workaround:** Check the selections after you open the pipeline.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [SIMPL#438](https://github.com/BlueQuartzSoftware/SIMPL/issues/438)

### KI-088 Table input ignores the system locale

- **Filter:** Filters with a table input
- **Severity:** Error / usability
- **Symptom:** Number input in tables does not use the system decimal separator.
- **Trigger:** A locale that uses a comma for decimals.
- **Workaround:** Use a period for decimals.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [SIMPL#99](https://github.com/BlueQuartzSoftware/SIMPL/issues/99)

### KI-089 Array selection does not check the Attribute Matrix type

- **Filter:** Filters with an array selection (user interface)
- **Severity:** Error / usability
- **Symptom:** You can select an array from an Attribute Matrix of the wrong type.
- **Trigger:** Select an array from a different Attribute Matrix type than the filter expects.
- **Workaround:** Check the Attribute Matrix type yourself.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [DREAM3D#603](https://github.com/BlueQuartzSoftware/DREAM3D/issues/603)

### KI-090 Selections are not reset when an earlier filter changes

- **Filter:** Filters with a Data Container selection (user interface)
- **Severity:** Error / usability
- **Symptom:** The old selection stays after the data type changes in an earlier filter.
- **Trigger:** Change the output type of an earlier filter.
- **Workaround:** Select the Data Container again.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [DREAM3D#619](https://github.com/BlueQuartzSoftware/DREAM3D/issues/619)

### KI-091 Opening a pipeline from a .dream3d file adds filters twice

- **Filter:** Read DREAM.3D Data File (pipeline import)
- **Severity:** Error / usability
- **Symptom:** The imported pipeline contains an extra Read DREAM.3D Data File filter.
- **Trigger:** Open a pipeline from a .dream3d file.
- **Workaround:** Delete the extra filter.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [DREAM3D#798](https://github.com/BlueQuartzSoftware/DREAM3D/issues/798)

### KI-092 Bookmarks sometimes do not open

- **Filter:** Bookmarks (user interface)
- **Severity:** Error / usability
- **Symptom:** A double-click on a bookmark does not open it.
- **Trigger:** Intermittent.
- **Workaround:** Double-click again, or use the context menu.
- **DREAM3D-NX status:** Not in DREAM3D-NX (different user interface)
- **References:** [DREAM3D#801](https://github.com/BlueQuartzSoftware/DREAM3D/issues/801)

---

## Index by filter

| Filter | Entries |
|---|---|
| Add Bad Data | KI-052 |
| Align Sections (Feature Centroid) | KI-010 |
| Convert Hexagonal Grid Data to Square Grid Data | KI-085 |
| Convert Orientation Representation | KI-036 |
| Delete Data | KI-087 |
| Establish Shape Types | KI-029 |
| Execute Process | KI-080 |
| Export DAMASK Files | KI-035 |
| Export INL File | KI-012 |
| Export StatsGenerator ODF Angle File | KI-015 |
| Find Average C-Axis Orientations | KI-036 |
| Find Feature Average Orientations | KI-022 |
| Find Feature Centroids | KI-036 |
| Find Feature Clustering | KI-017 |
| Find Feature Face Curvature | KI-013 |
| Find Feature Neighbor C-Axis Misalignments | KI-023 |
| Find Feature Neighbor Misorientations | KI-023 |
| Find Feature Neighborhoods | KI-018 |
| Find Feature Neighbors | KI-019 |
| Find Feature Phases | KI-021 |
| Find Feature Reference C-Axis Misalignments | KI-036 |
| Find Feature Reference Misorientations | KI-055 |
| Find Feature Shapes | KI-003, KI-038 |
| Find Feature Sizes | KI-036 |
| Find Feature Volumes from Triangle Geometry | KI-054 |
| Find GBCD | KI-006 |
| Find Kernel Average Misorientations | KI-024 |
| Find Neighbor Slip Transmission Metrics | KI-002, KI-030 |
| Generate Ensemble Statistics | KI-084 |
| Generate IPF Colors (Face) | KI-025 |
| Generate Primary StatsData | KI-027 |
| Generate Vector Colors | KI-014 |
| Import EDAX EBSD Data (.ang) | KI-057 |
| Import EDAX OIMAnalysis Data (.h5) | KI-011 |
| Import Orientation File(s) to H5EBSD | KI-085 |
| Import Oxford Instr. EBSD Data (.ctf) | KI-001, KI-056 |
| Import STL File | KI-050 |
| Insert Atoms | KI-058 |
| Isolate Largest Feature (Identify Sample) | KI-020 |
| ITK::Image Reader | KI-053 |
| ITK::Import Images (3D Stack) | KI-051 |
| ITK::Maximum Projection Image Filter | KI-009 |
| ITK::Standard Deviation Projection Image Filter | KI-009 |
| Neighbor Orientation Comparison (Bad Data) | KI-004, KI-036 |
| Neighbor Orientation Correlation | KI-005 |
| Quick Surface Mesh | KI-007 |
| Read DREAM.3D Data File | KI-081, KI-091 |
| Read H5EBSD File | KI-032 |
| Rotate Euler Reference Frame | KI-036 |
| Rotate Sample Reference Frame | KI-008 |
| StatsGenerator | KI-028, KI-026 |
| VTK STRUCTURED_POINTS Importer | KI-083 |
| Vtk Rectilinear Grid Exporter | KI-016, KI-082 |
| Write DREAM.3D Data File (XDMF) | KI-062 |
| All filters (PipelineRunner, locale) | KI-033 |
| Filters with table input | KI-034, KI-088 |
| User interface | KI-059, KI-060, KI-061, KI-089, KI-090, KI-092 |
| Fundamental-zone reduction | KI-031 |
| File readers | KI-085, KI-086 |
