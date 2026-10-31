# Verify Standalone Package Integrity
param (
    [string]$PackageDir = "$PSScriptRoot\..\dist\Workbench"
)

$ErrorActionPreference = "Stop"
Write-Host "=== Verifying Package: $PackageDir ===" -ForegroundColor Cyan

if (!(Test-Path $PackageDir)) {
    Write-Error "Package directory does not exist: $PackageDir"
}

$requiredFiles = @(
    "Launch.exe",
    "workbench_app.exe",
    "Qt6Core.dll",
    "Qt6Gui.dll",
    "Qt6Widgets.dll",
    "Qt6OpenGLWidgets.dll",
    "platforms\qwindows.dll",
    "data\examples\sample_tetrahedron_ascii.stl",
    "data\examples\synthetic_temperature_nodes.csv",
    "data\examples\manifest.json",
    "THIRD_PARTY_NOTICES.md",
    "README.md"
)

$pass = $true
foreach ($req in $requiredFiles) {
    $full = Join-Path $PackageDir $req
    if (Test-Path $full) {
        $size = (Get-Item $full).Length
        Write-Host "  [OK] $req ($size bytes)" -ForegroundColor Green
    } else {
        Write-Host "  [FAIL] Missing: $req" -ForegroundColor Red
        $pass = $false
    }
}

if ($pass) {
    Write-Host "`nAll required standalone assets are verified!" -ForegroundColor Green
} else {
    Write-Error "Verification failed! Package is incomplete."
}
