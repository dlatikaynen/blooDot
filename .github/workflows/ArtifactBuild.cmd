@echo off
cd..
cd..
pushd "%~dp0"
powershell Compress-7Zip "x64\Release" -ArchiveFileName "blooDot.zip" -Format Zip
:exit
popd
@echo on
