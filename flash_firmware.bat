@echo off
setlocal
chcp 65001 >nul

echo ========================================================
echo   ESP32-P4 Waveshare 10.1" Quick Flash Tool
echo ========================================================
echo.

set /p COM_PORT="Nhap cong COM (vi du: COM27): "

if "%COM_PORT%"=="" (
    echo [ERROR] Ban chua nhap cong COM!
    pause
    exit /b 1
)

echo.
echo Dang nap firmware vao %COM_PORT%...
echo.

python -m esptool --chip esp32p4 -p %COM_PORT% -b 460800 --before default_reset --after hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 16MB 0x2000 binaries\bootloader.bin 0x8000 binaries\partition-table.bin 0x10000 binaries\usb_touch_screen.bin

if %ERRORLEVEL% equ 0 (
    echo.
    echo ========================================================
    echo   NAP THANH CONG! Vui long reset lai board.
    echo ========================================================
) else (
    echo.
    echo [ERROR] Nap that bai. Vui long kiem tra lai ket noi COM hoac giu BOOT + bam RESET!
)

pause
