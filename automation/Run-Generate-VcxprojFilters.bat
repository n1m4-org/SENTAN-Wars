@echo off
setlocal

set "ScriptDir=%~dp0"
pushd "%ScriptDir%\.."

powershell -NoProfile -ExecutionPolicy Bypass -File "%ScriptDir%Generate-VcxprojFilters.ps1" -VcxprojPath "app\RhythmActionGame.vcxproj"

set "ExitCode=%ERRORLEVEL%"
popd
exit /b %ExitCode%
