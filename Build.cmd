@echo off
pushd "%~dp0"
if exist Release rd /s /q Release
if exist x64 rd /s /q x64
"%programfiles(x86)%\Microsoft Visual Studio\2017\Enterprise\MSBuild\Current\Bin\msbuild.exe" /p:Configuration=Release /p:Platform=x64
:exit
popd
@echo on
