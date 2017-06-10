:: Remove everything except for the code. It can be regenerated using GenerateSolutions.bat

cd "./FenixDK"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db
@RD /S /Q "./build"
@RD /S /Q "./obj"

cd "../Samples"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db

cd "./UnitTests"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db
@RD /S /Q "./build"
@RD /S /Q "./obj"

cd "../Basic"
del *.vcxproj *.vcxproj.user *.vcxproj.filters *.sdf *.sln *.suo *.db
@RD /S /Q "./build"
@RD /S /Q "./obj"