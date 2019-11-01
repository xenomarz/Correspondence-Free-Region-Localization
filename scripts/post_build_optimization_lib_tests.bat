@echo off

SET sourceDirectory=%1
SET targetDirectory=%2

xcopy %sourceDirectory% %targetDirectory% /s /e /y