set curpath=%~dp0
cd ..
cd .\complier
call path.bat
cd ..
cd %curpath%
cls

%MRPBUILDER%\mrpbuilder.NET.exe makefile.mpr

