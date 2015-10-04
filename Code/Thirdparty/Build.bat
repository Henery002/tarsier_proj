@echo off

rem 切换到当前文件所在目录
cd /d %~dp0

set Configuration=%1

rem can be[Debug, Release]
if "%Configuration%"=="" (
	set Configuration=Debug
)

set VsDevCmd="C:\Program Files\Microsoft Visual Studio 11.0\Common7\Tools\VsDevCmd.bat"
if exist %VsDevCmd% call %VsDevCmd%

set VsDevCmd="C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\Tools\VsDevCmd.bat"
if exist %VsDevCmd% call %VsDevCmd%

MSBuild.exe Thirdparty.sln /p:Configuration=%Configuration%
