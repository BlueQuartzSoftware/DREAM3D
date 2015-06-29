# Using GMT With DREAM3D #


## Tools Needed ##

+ GMT version 5.1
+ ImageMagick installed (which has the "convert" utility)
+ Ghostscript installed (to convert postscript files to image files
+ Shell environment that can run 'bash'. This means "MSys" on windows.	


## Windows Setup ##

### Install ImageMagick ###

Download the zip version of *ImageMagick* from [http://www.imagemagick.org/script/binary-releases.php](http://www.imagemagick.org/script/binary-releases.php). Select the type if release that you are allowed to install. Note for windows that there is a .zip which does not require an installer to be executed. This is probably the best route for those without sys-admin privileges on their machines.


### Install Ghostscript ###

Download Ghostscript from [http://www.ghostscript.com/download/gsdnld.html](http://www.ghostscript.com/download/gsdnld.html). **NOTE:** Download the **32bit** version regardless of what version of Windows you are running. The ImageMagick installation seems to default to using a 32 bit version of Ghostscript. There are ways around this that involve hacking the ImageMagick Installation.

### Install an MSys Environment ###
I just used the MSys that came with "Git-Bash" for windows.

### Install GMT Version 5.x ###

Download and install GMT version 5.x from the GMT website [http://gmt.soest.hawaii.edu/projects/gmt/wiki/Download](http://gmt.soest.hawaii.edu/projects/gmt/wiki/Download). Again select what works best for your system and access privileges.

### Getting started with GMT ###
Launch a "Git Bash" shell which is an MSys shell. Now we need to export the paths to all the packages we just got done installing. For example I have installed all the packages (GMT, ImageMagick, Ghostscript) into C:/Applications so I would need to do the following:

	export PATH=/c/Applications/gmt5/bin:$PATH
	export PATH=/c/Applications/gs/gs9.10/bin:$PATH
	export PATH=/c/Applications/ImageMagick-6.8.8-4:$PATH

After you have executed those commands (Which you could put in your .bashrc file) you can then verify that the executables are in the path:

	$ which gmt
	/c/Applications/gmt5/bin/gmt
	$ which convert
	/c/Applications/gs/gs9.10/bin/convert
	$ which gswin32c
	/c/Applications/ImageMagick-6.8.8-4/gswin32c


## Unix/Linux Setup ##

Use your package manager to install/verify that GMT5, ImageMagick and Ghostscript are all installed.

## OS X Setup ##
Download the GMT standalone installer which is a .dmg file. Double click the .dmg, copy the gmt.app into your applications folder.

Make sure you have the "convert" program installed. If you have a recent LaTeX package installed you probably have this. You can also use MacPorts to install ImageMagick if you do not have 'convert' installed.

## Command Line 5D Parameter GBCD ##

+ Generate the GMT .dat file from DREAM.3D
+ Make sure the GMT environment is setup completely
+ Execute the following:


		./DrawStereograms [number_of_files] [Stereo_file_name] 5d rainbow [Lo] [Hi] [step] stereo  HEX|CUBIC
		

### Example ###

		./Draw_stereograms 1 gbcd_Mg_9bins_gmt_ 5d rainbow  0.79   9.71  0.89
	

Note the actual input file is named *gbcd_Mg_9bins_gmt_1.dat* but we are using a substring of that file name as the argument to the script

