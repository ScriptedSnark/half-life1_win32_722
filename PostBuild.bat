@echo off
@setlocal enableextensions enabledelayedexpansion

::
:: Post-build auto-deploy script
:: Create and fill DeployPath.ini file with path to deployment folder for specified {ProjectName}
:: Call it so:
:: IF EXIST "$(ProjectDir)..\..\PostBuild.bat" (CALL "$(ProjectDir)..\..\PostBuild.bat" {ProjectName} "$(OutDir)$(TargetName)$(TargetExt)")
::

chcp 65001 > nul

set targetProject=%~1
set targetSrcPath=%~2
set targetName=%~3
set targetDstPathDefined=0

set configFile=DeployPath.user.ini
if not exist "%~dp0\%configFile%" (
	set configFile=DeployPath.ini
)

if not exist "%~dp0\%configFile%" (
	@echo No deployment path specified. Create %configFile% near PostBuild.bat with keypair {ProjectName}.path=path on separate lines for auto deployment.
	exit /b 0
)

for /f "eol=; tokens=1,* delims==" %%A in (%~dp0\%configFile%) do (
	if /i "%%A"=="Root.path" (
		set "rootPath=%%~B"
	) else if /i "%%A"=="%targetProject%.path" (
		set "targetDstPath=%%~B"
		set "targetDstPathDefined=1"
	)
)

set targetFullPath="%targetSrcPath%\%targetName%"

@call :normalizePath targetFullPath
@call :NormalizePath rootPath
@call :NormalizePath targetDstPath

if not exist "%targetFullPath%" (
	@echo Invalid source path: %targetFullPath%
	exit /b 1
)

if "%targetDstPathDefined%" == "0" (
	@echo No deployment path specified for %targetProject%.path
	exit /b 0
)

if defined rootPath (
	if not "!rootPath:~1,2!"==":\" (
		@echo ERROR: %configFile% only an absolute path is allowed for Root.path
		exit /b 1
	)

	if "!targetDstPath:~1,1!"==":" (
		@echo ERROR: %configFile% absolute path is not allowed for %targetProject%.path
		exit /b 1
	)

	if defined targetDstPath (
		set "deployPath=!rootPath!\!targetDstPath!"
	) else (
		set "deployPath=!rootPath!"
	)

) else (
	if not "!targetDstPath:~1,1!"==":" (
		@echo ERROR: %configFile% only an absolute path is allowed for %targetProject%.path
		exit /b 1
	)

	set "deployPath=!targetDstPath!"
)

@call :normalizePath deployPath

if not exist "%deployPath%" (
	@echo Invalid destination path: "%deployPath%"
	exit /b 1
) else (
	@echo Copying built "%targetName%" to "%deployPath%\"
	@xcopy /S /Y "%targetFullPath%" "%deployPath%\"
	if errorlevel 1 (
		@echo PostBuild.bat ^(88^) : warning : Can't copy "%targetFullPath%" to deploy path "%deployPath%"
	)
)

exit /b

:normalizePath
set "str=!%1!"
if not defined str exit /b
set "str=!str:/=\!"
:removeDuplicateBackslashes
set "newStr=!str:\\=\!"
if not "!newStr!"=="!str!" (
	set "str=!newStr!"
	goto :removeDuplicateBackslashes
)
set "%1=!str!"
exit /b
