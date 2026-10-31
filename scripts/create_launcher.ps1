# Create and build native Windows launcher (Launch.exe)
$ErrorActionPreference = "Stop"

$repoRoot = Resolve-Path "$PSScriptRoot\.."
Set-Location $repoRoot

Write-Host "=== Building Launch.exe via CMake Preset ===" -ForegroundColor Cyan
cmake --build --preset windows-msvc-x64-release --target Launch

$launcherPath = "$repoRoot\build\windows-msvc-x64-release\src\launcher\Release\Launch.exe"
if (Test-Path $launcherPath) {
    $info = Get-Item $launcherPath
    $hash = (Get-FileHash -Path $launcherPath -Algorithm SHA256).Hash
    Write-Host "Launch.exe built successfully!" -ForegroundColor Green
    Write-Host "  Path:   $launcherPath"
    Write-Host "  Size:   $($info.Length) bytes"
    Write-Host "  SHA256: $hash"
} else {
    Write-Error "Failed to locate built Launch.exe at $launcherPath"
}

