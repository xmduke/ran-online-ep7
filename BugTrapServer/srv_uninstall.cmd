@echo off
set NET_Version=v2.0.50727
set NET_Path=%WinDir%\Microsoft.NET\Framework\%NET_Version%

cd bin\Debug
%NET_Path%\InstallUtil /u BugTrapServer.exe
cd ..\..
