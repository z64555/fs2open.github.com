cmake --build . --config "$Env:CONFIGURATION" -- /verbosity:minimal

if (! ($?)) {
    exit 1
}