@echo off

REM Run Case 0~9
For /L %%a in (0,1,9) DO (
start /D"C:\Users\bmw\Desktop\Thesis_Being\NB_MAC_Scheduler_Prototype\Simulation\build\" Simulation.exe %%a)

REM start /D"C:\Users\bmw\Desktop\Thesis_Being\NB_MAC_Scheduler_Prototype\Simulation\build\" Simulation.exe 1

echo Simulation is executed...

pause
