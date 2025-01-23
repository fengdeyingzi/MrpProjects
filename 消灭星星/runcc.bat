
mrpbuilder -t build -i  game/ inc/ popstar/ res/popstar/ tools/ main.c -Igame -Iinc -Ipopstar -Ires/popstar/ -Itools/ ^
 -o mrp_core.mrp -bmp565 ^
-appid 30100 -version 1000 -desc "小蟀制作 2024/7/18" -vendor "小蟀" -filename "mrp_core.mrp" -displayname "消灭星星"