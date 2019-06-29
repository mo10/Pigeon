@echo off
cd /d %~dp0

set BASE_PATH=%~dp0

.\bin\make.exe
.\bin\make.exe install