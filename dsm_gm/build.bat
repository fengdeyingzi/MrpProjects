mrpbuilder -t build -gcc -gzip -lgcc -bma -Isrc/ -Iinfo -i main.c info/ src/ lib/start.mr lib/cfunction.ext lib/logo.ext assets/ -o dsm.mrp ^
-appid 90015 ^
-version 1000 ^
-scrw 240 ^
-scrh 320 ^
-desc "这是一个mrp应用列表" ^
-vendor "风的影子" ^
-displayname "mrp应用列表" ^
-flag 2 ^
-filename dsm.mrp