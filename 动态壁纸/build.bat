mrpbuilder -v

mrpbuilder -t build -gzip -bma -Isrc -i assets/ src/ main.c wallpaper.c game_menu.c -o dtbz.mrp ^
-appid 90005 ^
-version 1000 ^
-scrw 240 ^
-scrh 320 ^
-desc "动态壁纸程序！ 240*320" ^
-vendor "风的影子" ^
-displayname "动态壁纸2025" ^
-filename dtbz.mrp

@REM ghcdjy.mrp