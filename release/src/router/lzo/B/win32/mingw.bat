@echo // Copyright (C) 1996-2015 Markus F.X.J. Oberhumer
@echo //
@echo //   Windows 32-bit
@echo //   MinGW + gcc
@echo //
@call b\prepare.bat
@if "%BECHO%"=="n" echo off


set BLIB=lib%BNAME%.a
set CC=gcc
set CF=-O2 -fomit-frame-pointer -Wall %CFI% %CFASM%
set LF=%BLIB% -lwinmm -s

%CC% %CF% -c src/*.c
@if errorlevel 1 goto error
%CC% -x assembler-with-cpp -c asm/i386/src_gas/*.S
@if errorlevel 1 goto error
ar rcs %BLIB% *.o
@if errorlevel 1 goto error

%CC% %CF% -o dict.exe examples/dict.c %LF%
@if errorlevel 1 goto error
%CC% %CF% -o lzopack.exe examples/lzopack.c %LF%
@if errorlevel 1 goto error
%CC% %CF% -o precomp.exe examples/precomp.c %LF%
@if errorlevel 1 goto error
%CC% %CF% -o precomp2.exe examples/precomp2.c %LF%
@if errorlevel 1 goto error
%CC% %CF% -o simple.exe examples/simple.c %LF%
@if errorlevel 1 goto error

%CC% %CF% -o lzotest.exe lzotest/lzotest.c %LF%
@if errorlevel 1 goto error

%CC% %CF% -Iinclude/lzo -o testmini.exe minilzo/testmini.c minilzo/minilzo.c
@if errorlevel 1 goto error


@call b\done.bat
@goto end
:error
@echo ERROR during build!
:end
@call b\unset.bat
