## DREAM.3D Contributors ##
This file contains quick details of contributions to the DREAM.3D project made by developers external to BlueQuartz Software. Developers are presented in alphabetical order. For affiliations, please reference the top level ReadMe. If any information in this file is missing or incorrect, please submit a pull request with the corrections.

+ Dr. Patrick Callahan 
	+ Computation of shape descriptors in "Find Feature Shapes" **Filter**
+ Dr. Lisa Chan
	+ Twin insertion code used in the "Insert Transformation Phase" **Filter**
+ Dr. Krzysztof Glowinski
	+ Implementation of the "Find GBCD (Metric-based Approach)" **Filter**
+ Dr. Marc De Graef 
	+ Underlying framework for "Convert Orientation Representation" **Filter**, which allows for self-consistent transformations between 7 orientation representations
	+ Computation of shape descriptors in "Find Feature Shapes" **Filter**
+ Dr. Michael A. Groeber 
	+ Significant contributions to most core DREAM.3D libraries, particularly those involving orientation, texture, and statistical calculations
	+ Algorithm development and contribution in most **Plugins**, especially the SyntheticBuilding, Statistics, SurfaceMeshing, Processing, and OrientationAnalysis core **Plugins**
	+ Development of the HEDMAnalysis, TransformationPhase, and DDDAnalysisToolbox external **Plugins**
+ Dr. Sukbin Lee 
	+ Multi-material marching cubes (M3C) surface meshing code
+ Mr. William Lenthe 
	+ Several contributions to core DREAM.3D libraries, including the following:
		+ DREAM3DWidgetsLib
			+ Implementation of the Symmetric6x6Widget class
		+ OrientationLib
			+ Implementation of misorientation color generation algorithm by Schuh & Patala in SpaceGroupOps classes
		+ SIMPLib
			+ Implementation of the reorderCopy() function in IDataArray classes
			+ Several additions to the SIMPLib Constants file
			+ Fix for possible bad key values in the FilterManager class
			+ Several additions in the TemplateHelpers class
			+ Implementations of the following Core **Filters**:
				+ "Copy Attribute Array"
				+ "Copy Attribute Matrix"
				+ "Copy Data Container"
			+ Implmentation of functions to compare units of a DataArrayPath object
			+ Add FloatVec4 and FloatVec21 FilterParameters
			+ Add linear regression function to SIMPLibMath class
			+ Several fixes for the convertHSVtoRgb() function in the ColorUtilities class
	+ Several contributions to core DREAM.3D **Plugins**, including the following:
		+ Generic
			+ Implementation of the "Input Crystal Compliances" **Filter**
		+ OrientationAnalysis
			+ Original implementation of the "Convert Orientations" **Filter**, now called "Convert Orientation Representation"
			+ Implementation of the "Find Directional Moduli" **Filter**
			+ Implementation of the "Find Elastic Modulus Mismatch" **Filter**
			+ Implementation of the "Generate Misorientation Colors" **Filter**
			+ Added a safety check into the "Read H5EBSD File" **Filter**
		+ Processing
			+ Implementation of the "Clear Data (Mask)" **Filter**
			+ Add option for only Feature Ids to be replaced in "Erode/Dilate Bad Data" and "Fill Bad Data" **Filters**
			+ Fix potential array bounds crash in "Isolate Largest Feature (Identify Sample)" **Filter**
		+ Reconstruction
			+ Add option for to apply a linear background subtraction in align sections **Filters**
		+ SurfaceMeshing
			+ Implementation of the "Find Face Feature Boundary Areas" **Filter**
	+ Implementation and support for the external ImageProcessing **Plugin**
+ Dr. Adam Pilchak 
	+ Algorithm contributions for the identification and grouping of microtexture regions
+ Mr. Matthew Priddy 
	+ Code for exporting voxelized microstructures as Abaqus inputs, exposed in the "Abaqus Hexahedron Writer" **Filter**
+ Dr. Greg Rohrer 
	+ Computation of grain boundary character distribution (GBCD), exposed in the "Find GBCD" **Filter**
+ Dr. Tony Rollett 
	+ Several code contributions within the GBCD calculations and the Ph and Dx readers and writers
	+ Contributions to the orientation transformation codes
+ Dr. Dave Rowenhorst 
	+ Contributions to the orientation transformation codes
+ Dr. Stephen Sintay 
	+ Contributions to the moving finite element surface mesh smoothing codes
+ Dr. Joseph C. Tucker 
	+ Several contributions to core DREAM.3D libraries
	+ Implementation of various **Filters** for grouping microtexture regions
	+ Original implementation of the external TransformationPhase **Plugin**
+ Mr. Jarrell Waggoner 
	+ Various fixes for Linux operations