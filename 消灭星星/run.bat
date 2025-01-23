call build.bat
cls
del ..\complier\vmrp\mythroad\dsm_gm.mrp
copy .\mrp_core.mrp ..\complier\vmrp\mythroad\dsm_gm.mrp
cd ..
cd .\complier
cd .\vmrp
call main.exe
