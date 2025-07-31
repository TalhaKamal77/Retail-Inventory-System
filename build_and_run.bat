@echo off
echo ========================================
echo Inventory Management System
echo ========================================
echo.

echo Checking for g++ compiler...
g++ --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ERROR: g++ compiler not found!
    echo.
    echo Please install MinGW-w64 or Visual Studio Build Tools.
    echo You can download MinGW-w64 from: https://www.mingw-w64.org/
    echo.
    pause
    exit /b 1
)

echo Compiling server...
g++ -std=c++11 -Wall -Wextra -O2 server.cpp -o inventory_server.exe -lws2_32

if %errorlevel% neq 0 (
    echo ERROR: Compilation failed!
    echo.
    echo Please check that:
    echo 1. All source files are present
    echo 2. You have the required libraries installed
    echo 3. Your compiler is properly configured
    echo.
    pause
    exit /b 1
)

echo.
echo Compilation successful!
echo.
echo Starting inventory server...
echo.
echo The server will prompt you for configuration settings.
echo Press Ctrl+C to stop the server.
echo.

inventory_server.exe

echo.
echo Server stopped.
pause 