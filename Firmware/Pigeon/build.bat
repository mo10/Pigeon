@echo off
cd /d %~dp0
::setting up path
SET PATH=%PATH%;%~dp0\bin

make %*