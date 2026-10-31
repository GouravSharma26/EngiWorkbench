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
if (-not (Test-Path "$DistDir\Launch.exe")) {
    Write-Host "Running release packaging first..." -ForegroundColor Yellow
    & "$PSScriptRoot\package_release.ps1" -OutputDir $DistDir
}

# 2. Ensure raw results are exported and up to date
Write-Host "Exporting verified raw results into results/..." -ForegroundColor Yellow
python "$repoRoot\scripts\export_raw_results.py"

# 3. Ensure Git history and bundle are updated
Write-Host "Generating readable Git commit log and Git bundle..." -ForegroundColor Yellow
git log --graph --oneline --decorate --all > "$repoRoot\COMMIT_HISTORY.txt"
git bundle create "$repoRoot\EngiWorkbench.bundle" --all

# 4. Stage files into a clean temporary staging directory
$stagingDir = "$repoRoot\dist\staging_${CandidateId}"
if (Test-Path $stagingDir) {
    Remove-Item -Recurse -Force $stagingDir
}
New-Item -ItemType Directory -Force -Path $stagingDir | Out-Null

Write-Host "Staging standalone runtime and Launch.exe..." -ForegroundColor Yellow
Copy-Item "$DistDir\*" -Destination $stagingDir -Recurse

Write-Host "Staging source, test, data, and result directories..." -ForegroundColor Yellow
$copyDirs = @("src", "tests", "data", "results", "docs", "packaging", "scripts", ".vscode")
foreach ($d in $copyDirs) {
    if (Test-Path "$repoRoot\$d") {
        if (-not (Test-Path "$stagingDir\$d")) {
            New-Item -ItemType Directory -Force -Path "$stagingDir\$d" | Out-Null
        }
        Copy-Item "$repoRoot\$d\*" -Destination "$stagingDir\$d\" -Recurse -Force
    }
}

$copyFiles = @(
    "CMakeLists.txt",
    "CMakePresets.json",
    "README.md",
    "AGENTS.md",
    "THIRD_PARTY_NOTICES.md",
    "OWNERSHIP.md",
    "COMMIT_HISTORY.txt",
    "EngiWorkbench.bundle",
    ".gitattributes"
)
foreach ($f in $copyFiles) {
    if (Test-Path "$repoRoot\$f") {
        Copy-Item "$repoRoot\$f" -Destination "$stagingDir\$f"
    }
}

# 5. Create ZIP archive
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

