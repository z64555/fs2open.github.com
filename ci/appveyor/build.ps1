if ([System.Convert]::ToBoolean($env:DeployBuild) -And [System.Convert]::ToBoolean($env:DeployConfig)) {
    
} else {
    cmake --build . --config "$Env:CONFIGURATION" -- /verbosity:minimal

    if (! ($?)) {
        exit 1
    }
}
