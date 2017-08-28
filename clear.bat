:: Remove everything except for the code. It can be regenerated using GenerateSolutions.bat

cd "./FenixDK"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db *.opendb
@RD /S /Q "./build"
@RD /S /Q "./obj"
@RD /S /Q "./.vs"

cd "../Samples"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db *.opendb

cd "./UnitTests"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db *.opendb
@RD /S /Q "./build"
@RD /S /Q "./obj"
@RD /S /Q "./.vs"

cd "../Basic"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db *.opendb
@RD /S /Q "./build"
@RD /S /Q "./obj"
@RD /S /Q "./.vs"