New-Item build -type directory
Set-Location -Path build

$AdditionalFeatures="ON"
if ($Env:VS_VERSION -eq "10") {
    $AdditionalFeatures="OFF"
}

cmake -DFSO_BUILD_FRED2="${AdditionalFeatures}" -DFSO_USE_SPEECH="${AdditionalFeatures}" -DFSO_USE_VOICEREC="${AdditionalFeatures}" -G "$Env:CMAKE_GENERATOR" ..