REM /////////////////////////////////////////////////////////////
REM
REM 
REM
REM /////////////////////////////////////////////////////////////

Set DEVINSTALL=
for /F "tokens=1,2*" %%i in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Microsoft\VisualStudio\SxS\VS7" /v "15.0"') DO (
	if "%%i"=="15.0" (
		SET DEVINSTALL=%%k
	)
)

Set DevenvPath=%DEVINSTALL%Common7\IDE\devenv.exe
if not exist "%DevenvPath%" set DevenvPath=C:\Program Files\Microsoft Visual Studio\2022\Community\Common7\IDE\devenv.exe
set JsoncppPath=%~dp0jsoncpp\jsoncpp.sln
set LibcurlPath=%~dp0libcurl\libcurl.sln
set TinyxmlPath=%~dp0tinyxml\tinyxml.sln
set Tinyxml2Path=%~dp0tinyxml2\vs\tinyxml2.sln
if not exist "BuildLog\" mkdir "BuildLog\"

REM /////////////////////////////////////////////////////////////
echo "build jsoncpp" 
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Debug|x86" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Debug_MT|x86" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Release|x86" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Release_MT|x86" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)

"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Debug|x64" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Debug_MT|x64" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Release|x64" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)
"%DevenvPath%" "%JsoncppPath%" /Project "jsoncpp" /Build "Release_MT|x64" /out BuildLog\jsoncpp.log
if %ERRORLEVEL% NEQ 0 (
	echo Build jsoncpp Failed!
	type %~dp0BuildLog\jsoncpp.log
	pause & exit
)

REM /////////////////////////////////////////////////////////////
echo "build libcurl" 
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Debug|Win32" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Debug_MT|Win32" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Release|Win32" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Release_MT|Win32" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)

"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Debug|x64" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Debug_MT|x64" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Release|x64" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)
"%DevenvPath%" "%LibcurlPath%" /Project "libcurl" /Build "LIB_Release_MT|x64" /out BuildLog\libcurl.log
if %ERRORLEVEL% NEQ 0 (
	echo Build libcurl Failed!
	type %~dp0BuildLog\libcurl.log
	pause & exit
)

REM /////////////////////////////////////////////////////////////
echo "build tinyxml" 
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Debug_Unicode|Win32" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Debug_Unicode_MT|Win32" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Release_Unicode|Win32" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Release_Unicode_MT|Win32" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)

"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Debug_Unicode|x64" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Debug_Unicode_MT|x64" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Release_Unicode|x64" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)
"%DevenvPath%" "%TinyxmlPath%" /Project "tinyxml" /Build "Release_Unicode_MT|x64" /out BuildLog\tinyxml.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml Failed!
	type %~dp0BuildLog\tinyxml.log
	pause & exit
)

REM /////////////////////////////////////////////////////////////
echo "build tinyxml2" 
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Debug|x86" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Debug_MT|x86" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Release|x86" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Release_MT|x86" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)

"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Debug|x64" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Debug_MT|x64" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Release|x64" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)
"%DevenvPath%" "%Tinyxml2Path%" /Project "tinyxml2" /Build "Release_MT|x64" /out BuildLog\tinyxml2.log
if %ERRORLEVEL% NEQ 0 (
	echo Build tinyxml2 Failed!
	type %~dp0BuildLog\tinyxml2.log
	pause & exit
)