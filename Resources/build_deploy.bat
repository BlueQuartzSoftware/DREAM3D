cd %1
cmake.exe ../
msbuild /p:Configuration=Release DREAM3DProj.sln
msbuild /p:Configuration=Release PACKAGE.vcxproj
cd ..\
