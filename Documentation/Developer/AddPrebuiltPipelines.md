Adding Prebuilt Pipelines {#addprebuiltpipelines}
============

## Location of Files ##

In the DREAM3D project folder there is subfolder called *Support* and under that is *PrebuiltPipelines*.

+ DREAM3D
	+ Support
		+ PrebuiltPipelines _[Create new groups in this folder]_
			- Reconstruction
			- Synthetic

## Procedure ##

Create a pipeline in DREAM3D like normal and save the pipeline to a text file with a _.txt_ extension into one of the directories above or create a new subdirectory under the *PrebuiltPipelines* folder. If you simply need to edit a pipeline then edit the file in **this** location. Then compile the project. The prebuilt pipeline files will be copied into the correct location so they are found by DREAM3D when starting up.

When saving the pipeline to a file a default value for the _name_ key will be inserted which has a value equal to the base name of file being saved. DREAM3D uses this value for display in the GUI. If you want to have a different name then edit the file and change the following block:

	[PipelineBuilder]
	Name=CHANGE_THIS_VALUE_TO_WHAT_YOU_WANT_IN_THE_GUI

Save the pipeline file, compile and relaunch DREAM3D.

For example if I wanted to have a prebuilt pipeline that had text in DREAM3D that showed _My Awesome Pipeline_ the the top of my pipeline file would be:

	[PipelineBuilder]
	Name=My Awesome Pipeline

## Name Restrictions ##
 In order to be the most compatible with all the major Operating Systems the name of the file should only contain letters, numbers, underscore and dash characters. There are generally no restrictions on the "name=value" inside the file.


## CAUTION ##

If you edit the files in the ${BUILD_DIR}/Bin/PrebuiltPipelines folder the next time you compile these changes will be over written.

## Post Editing ##

After you edit your file, compile the project and relaunch DREAM3D to have the new prebuilt pipeline files loaded into DREAM3D.