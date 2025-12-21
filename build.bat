@echo off
echo ==== DEBUG TETRIS ====

REM Build with debug symbols
g++ -std=c++17 -g -Wall -Isrc src\*.cpp -o tetris.exe

IF %ERRORLEVEL% NEQ 0 (
    echo Build failed
    pause
    exit /b
)

echo.
echo Starting Tetris in separate window...
start tetris.exe