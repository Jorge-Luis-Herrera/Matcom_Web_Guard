@echo off
echo Compiling the graphical interface...
gcc -o gui.exe gui.c -lws2_32 -lpsapi -mwindows
if %errorlevel% equ 0 (
    echo.
    echo Compilation successful!
    echo Running the application...
    echo.
    gui.exe
) else (
    echo.
    echo Compilation error
    pause
)
