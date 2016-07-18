cmake --build --config "$Env:CONFIGURATION" .

if (! ($?)) {
    exit 1
}