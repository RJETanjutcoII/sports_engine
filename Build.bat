@echo off
cd /d "%~dp0"

echo Configuring project...
"C:\Program Files\CMake\bin\cmake.exe" -B build -S .

echo.
echo Building...
"C:\Program Files\CMake\bin\cmake.exe" --build build --config Release --target SportsEngine

echo.
if exist "build\Release\SportsEngine.exe" (
    echo Build successful! Run Play.bat to start the game.
) else (
    echo Build failed.
)
pause
