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

Create a pipeline in DREAM3D like normal and save the pipeline to a text file with a _.txt_ extension into one of the directories above or create a new subdirectory under the *PrebuiltPipelines* folder. If you simply need to edit a pipeline edit the file in **this** location. Then compile the project. The prebuilt pipeline files will be copied into the correct location so they are found by DREAM3D when starting up.

In addition to the pipeline that you save you need to open the file in a text editor and add some *ini* text at the top that tell DREAM3D what text to display for the prebuilt pipeline.

	[prebuilt_config]
	Name=[INSERT TEXT HERE]

Save the pipeline file, compile and relaunch DREAM3D.

### CAUTION ###

If you edit the files in the ${BUILD_DIR}/Bin/PrebuiltPipelines folder the next time you compile these changes will be over written.

## Post Editing ##

After you edit your file, compile the project and relaunch DREAM3D to have the new prebuilt pipeline files loaded into DREAM3D.