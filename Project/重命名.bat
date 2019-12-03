echo off
set oldName=TestApp
echo 请输入新的名称:
set/p newName= >nul
echo %oldName% %newName%
ren %oldName%.* %newName%.*
ren "%oldName%_Target 1.dep" "%newName%_Target 1.dep"
ren %oldName%_uvopt.bak %newName%_uvopt.bak
ren %oldName%_uvproj.bak %newName%_uvproj.bak