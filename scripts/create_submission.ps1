# Create Final Submission ZIP
param (
    [string]$CandidateId = "CandidateID",
    [string]$DistDir = "$PSScriptRoot\..\dist\Workbench",
    [string]$ZipOutput = "$PSScriptRoot\..\${CandidateId}_Workbench.zip"
)

$ErrorActionPreference = "Stop"
$repoRoot = (Resolve-Path "$PSScriptRoot\..").Path
Set-Location $repoRoot

Write-Host "=== Creating Submission Package: $ZipOutput ===" -ForegroundColor Cyan

# 1. Package standalone release if not done
if (!(Test-Path "$DistDir\Launch.exe")) {
    Write-Host "Running release packaging first..." -ForegroundColor Yellow
    & "$PSScriptRoot\package_release.ps1" -OutputDir $DistDir
}

# 2. Stage files into a clean temporary staging directory
$stagingDir = "$repoRoot\dist\staging_${CandidateId}"
if (Test-Path $stagingDir) {
    Remove-Item -Recurse -Force $stagingDir
}
New-Item -ItemType Directory -Force -Path $stagingDir | Out-Null

Write-Host "Staging standalone runtime and Launch.exe..." -ForegroundColor Yellow
Copy-Item "$DistDir\*" -Destination $stagingDir -Recurse

Write-Host "Staging source and project files..." -ForegroundColor Yellow
$copyDirs = @("src", "tests", "data", "docs", "packaging", "scripts", ".vscode")
foreach ($d in $copyDirs) {
    if (Test-Path "$repoRoot\$d") {
        Copy-Item "$repoRoot\$d" -Destination "$stagingDir\$d" -Recurse
    }
}

$copyFiles = @("CMakeLists.txt", "CMakePresets.json", "README.md", "THIRD_PARTY_NOTICES.md", ".gitattributes")
foreach ($f in $copyFiles) {
    if (Test-Path "$repoRoot\$f") {
        Copy-Item "$repoRoot\$f" -Destination "$stagingDir\$f"
    }
}

# Generate git commit log into staging
Write-Host "Generating readable Git commit log..." -ForegroundColor Yellow
git log --graph --oneline --decorate --all > "$stagingDir\COMMIT_HISTORY.txt"

# 3. Create ZIP archive
Write-Host "Compressing archive to $ZipOutput..." -ForegroundColor Yellow
if (Test-Path $ZipOutput) {
    Remove-Item -Force $ZipOutput
}

Compress-Archive -Path "$stagingDir\*" -DestinationPath $ZipOutput -CompressionLevel Optimal

# Cleanup staging
Remove-Item -Recurse -Force $stagingDir

$zipItem = Get-Item $ZipOutput
$zipHash = (Get-FileHash -Path $ZipOutput -Algorithm SHA256).Hash

Write-Host "`nSubmission package successfully created!" -ForegroundColor Green
Write-Host "  Archive:  $ZipOutput"
Write-Host "  Size:     $($zipItem.Length) bytes ($([Math]::Round($zipItem.Length / 1MB, 2)) MB)"
Write-Host "  SHA256:   $zipHash"
