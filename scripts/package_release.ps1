# Package Standalone Offline Windows Release
param (
    [string]$OutputDir = "$PSScriptRoot\..\dist\Workbench",
    [string]$QtDir = "C:\Qt\6.8.2\msvc2022_64"
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path "$PSScriptRoot\..").Path
Set-Location $repoRoot

Write-Host "====================================================" -ForegroundColor Cyan
Write-Host "  PASS Science Works - Unified Engineering Workbench" -ForegroundColor Cyan
Write-Host "  Standalone Windows Release Packager" -ForegroundColor Cyan
Write-Host "====================================================" -ForegroundColor Cyan

# 1. Ensure Release Binaries are Built
Write-Host "[1/6] Building Release binaries (workbench_app and Launch)..." -ForegroundColor Yellow
cmake --build --preset windows-msvc-x64-release --target workbench_app Launch --config Release

$appExe = "$repoRoot\build\windows-msvc-x64-release\src\app\Release\workbench_app.exe"
$launcherExe = "$repoRoot\build\windows-msvc-x64-release\src\launcher\Release\Launch.exe"

if (-not (Test-Path $appExe) -or -not (Test-Path $launcherExe)) {
    Write-Error "Release executables not found. Aborting packaging."
}

# 2. Prepare Clean Target Directory
Write-Host "[2/6] Preparing clean distribution directory: $OutputDir" -ForegroundColor Yellow
if (Test-Path $OutputDir) {
    Remove-Item -Recurse -Force $OutputDir
}
New-Item -ItemType Directory -Force -Path $OutputDir | Out-Null

# Copy application and launcher binaries
Copy-Item $appExe -Destination "$OutputDir\workbench_app.exe"
Copy-Item $launcherExe -Destination "$OutputDir\Launch.exe"

# 3. Deploy Qt Runtime via windeployqt
Write-Host "[3/6] Deploying Qt 6 runtime via windeployqt..." -ForegroundColor Yellow
$windeployqt = "$QtDir\bin\windeployqt.exe"
if (-not (Test-Path $windeployqt)) {
    Write-Error "windeployqt.exe not found at $windeployqt. Check Qt installation."
}

& $windeployqt --dir "$OutputDir" --release --compiler-runtime --no-translations --no-quick-import --no-system-d3d-compiler --no-opengl-sw "$OutputDir\workbench_app.exe"

if ($LASTEXITCODE -ne 0) {
    Write-Error "windeployqt failed with exit code $LASTEXITCODE"
}

# Copy additional required Qt DLLs if not pulled automatically by windeployqt
if (Test-Path "$QtDir\bin\Qt6OpenGLWidgets.dll") {
    Copy-Item "$QtDir\bin\Qt6OpenGLWidgets.dll" -Destination "$OutputDir\" -Force
}
if (Test-Path "$QtDir\bin\Qt6OpenGL.dll") {
    Copy-Item "$QtDir\bin\Qt6OpenGL.dll" -Destination "$OutputDir\" -Force
}

# Deploy Microsoft Visual C++ 2022 x64 Runtime DLLs
$crtDir = Get-ChildItem "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Redist\MSVC\*\x64\Microsoft.VC143.CRT" | Select-Object -First 1 -ExpandProperty FullName
if ($crtDir -and (Test-Path $crtDir)) {
    Write-Host "Deploying MSVC x64 runtime from $crtDir..." -ForegroundColor Yellow
    Copy-Item "$crtDir\msvcp140.dll" -Destination "$OutputDir\" -Force -ErrorAction SilentlyContinue
    Copy-Item "$crtDir\vcruntime140.dll" -Destination "$OutputDir\" -Force -ErrorAction SilentlyContinue
    Copy-Item "$crtDir\vcruntime140_1.dll" -Destination "$OutputDir\" -Force -ErrorAction SilentlyContinue
}

# 4. Bundle Required Data & Documentation Assets
Write-Host "[4/6] Bundling data examples and documentation..." -ForegroundColor Yellow
$targetDataDir = "$OutputDir\data\examples"
New-Item -ItemType Directory -Force -Path $targetDataDir | Out-Null

Copy-Item "$repoRoot\data\examples\sample_tetrahedron_ascii.stl" -Destination "$targetDataDir\"
Copy-Item "$repoRoot\data\examples\synthetic_temperature_nodes.csv" -Destination "$targetDataDir\"
Copy-Item "$repoRoot\data\examples\manifest.json" -Destination "$targetDataDir\"

if (Test-Path "$repoRoot\THIRD_PARTY_NOTICES.md") {
    Copy-Item "$repoRoot\THIRD_PARTY_NOTICES.md" -Destination "$OutputDir\"
}
if (Test-Path "$repoRoot\README.md") {
    Copy-Item "$repoRoot\README.md" -Destination "$OutputDir\"
}

# Create safe writable results output folder inside package
New-Item -ItemType Directory -Force -Path "$OutputDir\results" | Out-Null

# 5. Verify Critical DLLs
Write-Host "[5/6] Verifying deployed runtime dependencies..." -ForegroundColor Yellow
$criticalFiles = @(
    "Launch.exe",
    "workbench_app.exe",
    "Qt6Core.dll",
    "Qt6Gui.dll",
    "Qt6Widgets.dll",
    "Qt6OpenGLWidgets.dll",
    "platforms\qwindows.dll",
    "data\examples\sample_tetrahedron_ascii.stl",
    "data\examples\synthetic_temperature_nodes.csv",
    "data\examples\manifest.json"
)

$allFound = $true
foreach ($file in $criticalFiles) {
    $filePath = Join-Path $OutputDir $file
    if (Test-Path $filePath) {
        Write-Host "  [OK] $file" -ForegroundColor Green
    } else {
        Write-Host "  [MISSING] $file" -ForegroundColor Red
        $allFound = $false
    }
}

if (-not $allFound) {
    Write-Error "Critical files missing from package!"
}

# 6. Generate Packaging Manifest
Write-Host "[6/6] Generating manifest and inventory..." -ForegroundColor Yellow
$deployedFiles = Get-ChildItem -Recurse $OutputDir | Where-Object { -not $_.PSIsContainer }
$totalBytes = ($deployedFiles | Measure-Object -Property Length -Sum).Sum

$manifest = @{
    PackageName     = "PASS Science Works - Unified Engineering Workbench";
    Version         = "0.1.0-prototype";
    Architecture    = "x64";
    Platform        = "Windows 11";
    Timestamp       = (Get-Date).ToString("yyyy-MM-dd HH:mm:ss");
    TotalFiles      = $deployedFiles.Count;
    TotalSizeBytes  = $totalBytes;
    TotalSizeMB     = [Math]::Round($totalBytes / 1MB, 2);
    LauncherHash    = (Get-FileHash -Path "$OutputDir\Launch.exe" -Algorithm SHA256).Hash;
    AppHash         = (Get-FileHash -Path "$OutputDir\workbench_app.exe" -Algorithm SHA256).Hash
}

$manifest | Out-String | Write-Host
$manifest | ConvertTo-Json -Depth 4 | Out-File "$OutputDir\package_manifest.json" -Encoding utf8

Write-Host "`nStandalone package successfully staged at: $OutputDir" -ForegroundColor Green
Write-Host "Launch the application by running: $OutputDir\Launch.exe" -ForegroundColor Green

