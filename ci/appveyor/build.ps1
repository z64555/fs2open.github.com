msbuild "FS2_Open.sln" /p:Configuration="$Env:CONFIGURATION" /m /verbosity:minimal

if (! ($?)) {
    exit 1
}