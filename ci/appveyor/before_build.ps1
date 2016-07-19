New-Item build -type directory
Set-Location -Path build

$FRED_STATUS="ON"
if ($Env:VS_VERSION -eq "10") {
    $FRED_STATUS="OFF"
}

cmake -DFSO_BUILD_FRED2="${FRED_STATUS}" -DFSO_USE_SPEECH=OFF -DFSO_USE_VOICEREC=OFF -G "$Env:CMAKE_GENERATOR" ..