@echo off

:loop
cls

converter.exe rawdata[Aug-30-13-14-56].bin

echo Waiting for 10 seconds...
timeout /t 10 /nobreak

goto loop