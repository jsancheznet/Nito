@echo off

pushd build

set SDL2Include="..\vendor\SDL2-2.0.20\include"
set SDL2Lib="..\vendor\SDL2-2.0.20\lib\x64"
set GladInclude="..\vendor\glad\include"
set AssimpInclude="..\vendor\assimp-5.2.2\include"
set AssimpLib="..\vendor\assimp-5.2.2\lib\Release"

set IncludeDirectories=-I%SDL2Include% -I%GladInclude% -I%AssimpInclude%
set LibDirectories=-LIBPATH:%SDL2Lib% -LIBPATH:%AssimpLib%

set CompilerFlags= /FS -DDEBUG -nologo -W4 -WX -Od -FS %IncludeDirectories% -Zi -EHsc -MD /D "_WINDOWS"
set LinkerFlags=-nologo -DEBUG %LibDirectories%

REM GLM is C++, need to be C++ in order to run the tests
cl /Tc ..\nito_main.c %CompilerFlags% /link %LinkerFlags% -SUBSYSTEM:CONSOLE SDL2.lib SDL2main.lib assimp-vc142-mt.lib shell32.lib

popd
