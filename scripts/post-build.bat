@echo off

REM taken from https://serverfault.com/questions/227345/locale-unaware-date-and-time-in-batch-files
REM ---- Start Extract Date/Time ----
REM Get the time from WMI - at least that's a format we can work with
SET X=
for /f "skip=1 delims=" %%x in ('wmic os get localdatetime') do if not defined X SET X=%%x
ECHO.%X%

REM dissect into parts
SET DATE.YEAR=%X:~0,4%
SET DATE.MONTH=%X:~4,2%
SET DATE.DAY=%X:~6,2%
SET DATE.HOUR=%X:~8,2%
SET DATE.MINUTE=%X:~10,2%
SET DATE.SECOND=%X:~12,2%
SET DATE.FRACTIONS=%X:~15,6%
SET DATE.OFFSET=%X:~21,4%

SET fullDate=%DATE.YEAR%_%DATE.MONTH%_%DATE.DAY%_%DATE.HOUR%_%DATE.MINUTE%_%DATE.SECOND%
REM ---- End Extract Date/Time ----

SET sourceFilePath=%1
SET targetDirectoryPath=%2

FOR %%i IN ("%sourceFilePath%") DO (
  SET fileName=%%~ni
  SET fileExtension=%%~xi
)

SET appendedFileName=%fileName%_%fullDate%%fileExtension%
SET targetFilePath=%targetDirectoryPath%%appendedFileName%

REM ECHO %sourceFilePath%
REM ECHO %targetDirectoryPath%
REM ECHO %fileName%
REM ECHO %fileExtension%
REM ECHO %appendedFileName%
REM ECHO %targetFilePath%

ECHO xcopy %sourceFilePath% %targetFilePath%
xcopy %sourceFilePath% %targetFilePath%*