$zipFileLocalPath = "$PSScriptRoot/antlr4-cpp-runtime-4.9.1-source.zip"

if(-not [System.IO.File]::Exists($zipFileLocalPath)){
    wget https://www.antlr.org/download/antlr4-cpp-runtime-4.9.1-source.zip -outfile $zipFileLocalPath
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


$rootSrcDirectory = "$PSScriptRoot\antlr_src"
Remove-Item $rootSrcDirectory -Recurse -ErrorAction Ignore
Expand-Archive -LiteralPath $zipFileLocalPath -DestinationPath "$rootSrcDirectory"

$rootDeployDirectory = "$PSScriptRoot\antlr"
Remove-Item $rootDeployDirectory -Recurse -ErrorAction Ignore
New-Item $rootDeployDirectory -Type Directory

Write-Host "Copying header files ..."

CopyDirectory -sourceDir "$rootSrcDirectory\runtime\src" -targetDir "$rootDeployDirectory\runtime" -include "*.h"

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

CopyDirectory -sourceDir "$rootSrcDirectory\runtime\bin" -targetDir "$rootDeployDirectory\lib" -include "*.dll"
CopyDirectory -sourceDir "$rootSrcDirectory\runtime\bin" -targetDir "$rootDeployDirectory\lib" -include "*.lib"

Remove-Item $rootSrcDirectory -Recurse -ErrorAction Ignore
Remove-Item $zipFileLocalPath -Recurse -ErrorAction Ignore