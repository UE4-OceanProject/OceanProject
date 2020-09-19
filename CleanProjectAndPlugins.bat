@echo off
setlocal
:PROMPT
set /p var=Are you sure you want to clean the project and all plugins?[Y/N]: 
if /I not %var%== Y exit

del /q/s *".sln"

rmdir /q/s ".vs"
rmdir /q/s "Build"
rmdir /q/s "Binaries"
rmdir /q/s "Intermediate"
rmdir /q/s "Saved"

rmdir /q/s "Plugins/TimePlugin/Binaries"
rmdir /q/s "Plugins/TimePlugin/Intermediate"

rmdir /q/s "Plugins/SkyPlugin/Binaries"
rmdir /q/s "Plugins/SkyPlugin/Intermediate"

rmdir /q/s "Plugins/OceanPlugin/Binaries"
rmdir /q/s "Plugins/OceanPlugin/Intermediate"

rmdir /q/s "Plugins/FishPlugin/Binaries"
rmdir /q/s "Plugins/FishPlugin/Intermediate"

rmdir /q/s "Plugins/BuoyancyPlugin/Binaries"
rmdir /q/s "Plugins/BuoyancyPlugin/Intermediate"

endlocal