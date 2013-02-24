#! /bin/bash

# To be executed in a mingw32 shell (the one shipped with Git works as well)

WIX="C:\Programmi\Windows Installer XML v3.6\bin"
MINGW_BASE="F:\mingw"
QT_BASE="F:\Qt\4.8.4"
DBG_OR_REL="release"
PROGRAM_BASE="C:\Documents and Settings\mardy\git\abc-build-Mingw32-${DBG_OR_REL}"

PROJECT=AstroBinUploader

PROGRAM_LIB="${PROGRAM_BASE}\libabc\src\\${DBG_OR_REL}"
PROGRAM_BIN="${PROGRAM_BASE}\uploader\\${DBG_OR_REL}"

MINGW_BIN="${MINGW_BASE}\bin"
MINGW_LOCAL_BIN="${MINGW_BASE}\msys\1.0\local\bin"

QT_BIN="${QT_BASE}\bin"

echo "Building..."
"${WIX}\\candle" -ext WixUIExtension -ext WixUtilExtension "${PROJECT}.wxs"
"${WIX}\\light" \
        -ext WixUIExtension -ext WixUtilExtension \
	-b abuPath="$PROGRAM_BIN" \
	-b libabcBin="$PROGRAM_LIB" \
	-b mingwBin="$MINGW_BIN" \
	-b mingwLocalBin="$MINGW_LOCAL_BIN" \
	-b qtBin="$QT_BIN" \
	-b qtBase="$QT_BASE" \
	-o "${PROJECT}.msi" \
	"${PROJECT}.wixobj"

#scp "${PROJECT}.msi" mardy.it:incoming/
