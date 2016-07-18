New-Item build -type directory
Set-Location -Path build

$FRED_STATUS="TRUE"
if ($Env:VS_VERSION -eq "10") {
    $FRED_STATUS="FALSE"
}

cmake -DFSO_BUILD_FRED2=$FRED_STATUS -DFSO_USE_SPEECH=False -G "$Env:CMAKE_GENERATOR" ..