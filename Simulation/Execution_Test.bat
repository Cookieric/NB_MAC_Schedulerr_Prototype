REM relative path is related to the excuted path of excuted file
REM Ex:Put program in the following:
REM /home/user/code/main.cpp --> Inside cpp write a output a log in ./log.txt
REM Excuted Files put in /home/usr/code/bin/main

REM 1.
REM $ cd /home/usr/code/bin
REM $ ./main
REM Ouput a /home/usr/code/bin/log.txt

REM 2.
REM $ cd /home/usr/code
REM $ ./bin/main
REM Ouput a /home/usr/code/log.txt
REM The path is related to excuted index path...

@echo off

REM FOR %%a IN (香蕉,你的,巴拉) DO (
REM   echo %%a
REM )

REM For /L %%a in (0,1,9) DO (
REM start /D"C:\Users\bmw\Desktop\Thesis_Being\NB_MAC_Scheduler_Prototype\Simulation\build\" Simulation.exe %%a)

start /D"C:\Users\bmw\Desktop\Thesis_Being\NB_MAC_Scheduler_Prototype\Simulation\build\" Simulation.exe 0

echo Simulation is executed...

pause
