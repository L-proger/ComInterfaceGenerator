$zipFileLocalPath = "$PSScriptRoot/antlr-source.zip"
$rootSrcDirectory = "$PSScriptRoot/antlr_src"

if(-not [System.IO.File]::Exists($zipFileLocalPath)){
    wget https://www.antlr.org/download/antlr4-cpp-runtime-4.9.2-source.zip -outfile $zipFileLocalPath
}


function CopyDirectory() {
    param (
        [string]$sourceDir,
        [string]$targetDir,
        [string]$include
    )

    Get-ChildItem $sourceDir -Recurse -Include "$include" | % {
        $dest = $targetDir + $_.FullName.SubString($sourceDir.Length)
        #Write-Host "FROM: $($_.FullName) TO: $dest"
        $destDir = Split-Path $dest
        If(!(Test-Path $destDir)) { New-Item $destDir -Type Directory }
        Copy-Item -Path $_.FullName -Destination $dest -Force -Recurse -Container
    }
}



Remove-Item $rootSrcDirectory -Recurse -ErrorAction Ignore
Expand-Archive -LiteralPath $zipFileLocalPath -DestinationPath "$rootSrcDirectory"

$rootDeployDirectory = "$PSScriptRoot\antlr"
Remove-Item $rootDeployDirectory -Recurse -ErrorAction Ignore
New-Item $rootDeployDirectory -Type Directory

Write-Host "Copying header files ..."

CopyDirectory -sourceDir "$rootSrcDirectory\runtime\src" -targetDir "$rootDeployDirectory\runtime" -include "*.h"


$vsVersion = 2022
$vsEdition = "Preview"

$vsPath = "C:\Program Files\Microsoft Visual Studio\$vsVersion\$vsEdition\"

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

mkdir "$rootSrcDirectory/build"
pushd "$rootSrcDirectory/build"
cmake ../
    pushd "$rootSrcDirectory/build/runtime"
    msbuild antlr4_static.vcxproj  /p:configuration="Release" /p:platform=x64
    msbuild antlr4_static.vcxproj  /p:configuration="Debug" /p:platform=x64
    popd
popd


CopyDirectory -sourceDir "$rootSrcDirectory\dist\Debug" -targetDir "$rootDeployDirectory\lib\Debug" -include "*.lib"
CopyDirectory -sourceDir "$rootSrcDirectory\dist\Debug" -targetDir "$rootDeployDirectory\lib\Debug" -include "*.pdb"
CopyDirectory -sourceDir "$rootSrcDirectory\dist\Release" -targetDir "$rootDeployDirectory\lib\Release" -include "*.lib"

Remove-Item $rootSrcDirectory -Recurse -ErrorAction Ignore
Remove-Item $zipFileLocalPath -Recurse -ErrorAction Ignore