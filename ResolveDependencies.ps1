

$zipFileLocalPath = "$PSScriptRoot/antlr4-cpp-runtime-4.9.1-source.zip"

if(-not [System.IO.File]::Exists($zipFileLocalPath)){
    wget https://www.antlr.org/download/antlr4-cpp-runtime-4.9.1-source.zip -outfile $zipFileLocalPath
}


$rootSrcDirectory = "$PSScriptRoot/antlr_src"
Remove-Item $rootSrcDirectory -Recurse -ErrorAction Ignore
Expand-Archive -LiteralPath $zipFileLocalPath -DestinationPath "$rootSrcDirectory"

$rootDeployDirectory = "$PSScriptRoot/antlr"
Remove-Item $rootDeployDirectory -Recurse -ErrorAction Ignore
New-Item $rootDeployDirectory -Type Directory

Write-Host "Copying header files ..."

Get-ChildItem "$rootSrcDirectory/runtime/src/" | Copy-Item -Destination "$rootDeployDirectory/runtime/" -Recurse -Force -filter *.h


$vsEdition = "Community"

$vsPath = "C:\Program Files (x86)\Microsoft Visual Studio\2019\$vsEdition\"

if ($vsPath -and (Test-Path "$vsPath\Common7\Tools\vsdevcmd.bat")) {
    $json = $(& "${env:COMSPEC}" /s /c "`"$vsPath\Common7\Tools\vsdevcmd.bat`" -no_logo -arch=x64 && powershell -Command `"Get-ChildItem env: | Select-Object Key,Value | ConvertTo-Json`"")
    if ($LASTEXITCODE -ne 0) {
        Write-Error "($LASTEXITCODE) $vsPath\Common7\Tools\vsdevcmd.bat: $json"
    } else {
        # Write-Host $json
        $($json | ConvertFrom-Json) | ForEach-Object {
        $k, $v = $_.Key, $_.Value
        Set-Content env:\"$k" "$v"
        }
    }
}


pushd "$rootSrcDirectory/runtime"
msbuild antlr4cpp-vs2019.vcxproj /p:configuration="Release Static" /p:platform=x64
msbuild antlr4cpp-vs2019.vcxproj /p:configuration="Debug Static" /p:platform=x64
popd

Get-ChildItem "$rootSrcDirectory/runtime/bin/" | Copy-Item -Destination "$rootDeployDirectory/lib/" -Recurse -Force -filter *.dll
Get-ChildItem "$rootSrcDirectory/runtime/bin/" | Copy-Item -Destination "$rootDeployDirectory/lib/" -Recurse -Force -filter *.lib

Remove-Item $rootSrcDirectory -Recurse -ErrorAction Ignore
Remove-Item $zipFileLocalPath -Recurse -ErrorAction Ignore