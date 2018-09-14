set LOCAL_PATH="%cd%"

call clear.bat

cd "%LOCAL_PATH%"
call genie.exe --file=build_engine_project.lua vs2015
call genie.exe --file=build_samples_project.lua vs2015
pause