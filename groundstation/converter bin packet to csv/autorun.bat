@echo off

:loop
cls

converter.exe rawdata[Sep-5-13-23-31].bin

echo Waiting for 1 seconds...
timeout /t 1 /nobreak

goto loop