@echo off
echo Setting VCPKG_ROOT environment variable...
setx VCPKG_ROOT "%CD%\vcpkg"
echo.
echo VCPKG_ROOT has been set to: %CD%\vcpkg
echo.
echo Please restart Visual Studio and rebuild the project.
pause