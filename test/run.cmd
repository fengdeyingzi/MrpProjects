cd run/mythroad
del dsm_gm.mrp
cd ../..
cd src
mrpbuilder -t build^
 -i main.c^
 -Lsky_font16_mtk.lib^
 -displayname 应用1^
 -vendor "未知作者"^
 -desc "应用描述"^
 -appid 30089^
 -version 1000^
 -o ../run/mythroad/dsm_gm.mrp
cd ../run
@REM main