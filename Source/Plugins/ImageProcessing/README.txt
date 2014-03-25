Building ImageProcessing plugin
-Build or install ITK with the following configuration changes:
	-Deselect ITK_BUILD_DEFAULT_MODULES
	-Create/Select required groups
		-ITKGroup_Core
		-ITKGroup_Filtering
		-ITKGroup_Registration (not yet required by seems likely)
		-ITKGroup_Segmentation (not yet required but seems likely)	
-Open cmake
-Add ImageProcessing to DREAM3D_EXTRA_PLUGINS
-Configure
-If cmake doesn't find ITK set ITK_DIR manually to the directory containing ITKConfig.CMAKE (something like .../ITK/lib/cmake/ITK/)
-reconfigure/generate

Writing an ImageProcessing filter
-include ITKUtilities in your filter (dream3d<-->itk wrapping / conversion functions)	
-ImageProcessingConstants is included by ITKUtilities (pixel/image/iterator types etc are already defined here)
-itk code can be written as normal with the following exceptions (look at DiscreteGaussianBlur as an exapmle)
	-'importing' an image to itk should be handled with ITKUtilities::Dream3DtoITK(m, m_DataArray)
	-'exporting' ImageToImageFilter results can be done by
		-Setting the output buffer of the filter to a dream3d array using ITKUtilities::SetITKOutputOutput(filter->GetOutput(), m_OutputArray, numberOfVoxels)
		-Allowing ITK to allocate its own memory and copy to a dream3d array using ITKUtilities::ITKtoDream3D(filter->GetOutput(), m_OutputArray)
-ITK is included in dream3d on a module by modules basis
	-modules can be added in the plugin's CMakeLists.txt
	-modules using hdf5 CANNOT be included to avoid symbol conflicts

questions: willlenthe@gmail.com