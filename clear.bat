:: Remove everything except for the code. It can be regenerated using GenerateSolutions.bat

cd "./FenixDK"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db
@RD /S /Q "./build"
@RD /S /Q "./obj"

cd "../Tests"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db
@RD /S /Q "./build"
@RD /S /Q "./obj"