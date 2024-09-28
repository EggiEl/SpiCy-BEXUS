@echo off

:loop
cls

converter.exe rawdata[Sep-4-15-14-38].bin

echo Waiting for 10 seconds...
timeout /t 10 /nobreak

goto loop