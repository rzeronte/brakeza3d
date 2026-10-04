<#
.SYNOPSIS
    Builds the Linux .deb package of Brakeza3D from the master branch, using WSL (Ubuntu).

.DESCRIPTION
    Same approach as package_windows.ps1. Steps:
      1. Reads the engine version from <Branch>:include/Config.h and checks that debian/changelog in
         <Branch> has an entry for that version (the .deb version comes from the changelog).
      2. Checks the Linux binary cmake-build-release-wsl/Brakeza3D (built from CLion with the WSL toolchain,
         Release, <Branch> checked out -- this script never compiles): it must contain v<version> and be
         newer than the last core commit of <Branch>.
      3. Checks that WSL starts and that the packaging tools and build dependencies are installed.
      4. Stages a clean source tree inside WSL (~/brakeza3d-deb): assets, GLSL, config and debian exported
         with "git archive <Branch>" (tracked files only, LF line endings) + the binary.
      5. Runs dpkg-buildpackage -us -uc -b there (debian/rules installs to /opt/brakeza3d + /usr/bin wrapper).
      6. Copies the result to ..\Brakeza3D-x64_86-Linux-installer.deb (the name the website links to) and
         prints its metadata and a content check.

.PARAMETER Branch      Branch to package. Default: master.
.PARAMETER Distro      WSL distribution. Default: Ubuntu.
.PARAMETER OutDir      Where the .deb is copied. Default: the folder that contains the repo (..).
.PARAMETER Force       Package even if the binary version/date checks fail.

.EXAMPLE
    .\tools\package_linux.ps1
#>
param(
    [string]$Branch = "master",
    [string]$Distro = "Ubuntu",
    [string]$OutDir,
    [switch]$Force
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Parent   = Split-Path -Parent $RepoRoot
if (-not $OutDir) { $OutDir = $Parent }
$BinPath  = Join-Path $RepoRoot "cmake-build-release-wsl\Brakeza3D"
$DebName  = "Brakeza3D-x64_86-Linux-installer.deb"

function Step($msg) { Write-Host "[package_linux] $msg" -ForegroundColor Cyan }
function Fail($msg) { Write-Host "[package_linux] ERROR: $msg" -ForegroundColor Red; exit 1 }

# Runs a bash command inside WSL; returns its output lines. $LASTEXITCODE keeps bash's exit code.
function Wsl([string]$cmd) {
    # Continue (not Stop): in PowerShell 5.1 native stderr redirected with 2>&1 becomes error records
    $ErrorActionPreference = "Continue"
    $out = & wsl.exe -d $Distro -- bash -lc $cmd 2>&1
    return ($out | ForEach-Object { "$_".TrimEnd("`r") })
}

Push-Location $RepoRoot
try {
    # ── 1. Version and changelog from <Branch> ────────────────────────────────
    git rev-parse --verify --quiet "$Branch" | Out-Null
    if ($LASTEXITCODE -ne 0) { Fail "branch '$Branch' not found" }
    $configH = git show "${Branch}:include/Config.h" | Out-String
    if ($configH -notmatch 'ENGINE_VERSION\s*=\s*"v?([0-9][0-9.]*)"') { Fail "ENGINE_VERSION not found in ${Branch}:include/Config.h" }
    $Version = $Matches[1]
    $changelog = git show "${Branch}:debian/changelog" | Select-Object -First 1
    if ($changelog -notmatch "^brakeza3d \(([^)]+)\)") { Fail "cannot read the version from ${Branch}:debian/changelog" }
    if ($Matches[1] -ne $Version) {
        Fail "debian/changelog starts with version $($Matches[1]) but ENGINE_VERSION is $Version -- add a changelog entry for $Version"
    }
    Step "Branch '$Branch', engine version $Version (debian/changelog OK)"

    # ── 2. Linux binary checks ────────────────────────────────────────────────
    if (-not (Test-Path $BinPath)) {
        Fail "no $BinPath -- build it in CLion with the WSL toolchain (Release, output dir cmake-build-release-wsl)"
    }
    $binItem = Get-Item $BinPath
    $problems = @()
    if (-not ((& findstr.exe /M /C:"v$Version" $BinPath) -ne $null)) {
        $problems += "the binary does not contain 'v$Version' (built from another branch or before the version bump)"
    }
    $coreTs = [int64](git log -1 --format=%ct $Branch -- src include GLSL CMakeLists.txt third_party resources)
    $coreDate = [DateTimeOffset]::FromUnixTimeSeconds($coreTs).LocalDateTime
    if ($binItem.LastWriteTime -lt $coreDate) {
        $problems += "the binary ($($binItem.LastWriteTime)) is older than the last core commit of $Branch ($coreDate)"
    }
    if ($problems.Count -gt 0) {
        $problems | ForEach-Object { Write-Host "[package_linux]   - $_" -ForegroundColor Yellow }
        if (-not $Force) { Fail "binary checks failed (use -Force to package anyway)" }
        Write-Host "[package_linux] -Force: continuing despite the checks" -ForegroundColor Yellow
    } else {
        Step "Linux binary OK: v$Version, built $($binItem.LastWriteTime)"
    }

    # ── 3. WSL, tools and build dependencies ──────────────────────────────────
    $probe = Wsl "echo wsl-ok"
    if ($LASTEXITCODE -ne 0 -or ($probe -notcontains "wsl-ok")) {
        Write-Host ($probe -join "`n")
        Fail "WSL '$Distro' does not start. If the error mentions virtualization (HCS_E_HYPERV_NOT_INSTALLED), enable Intel VT-x / AMD SVM in the BIOS and the 'Virtual Machine Platform' Windows feature"
    }
    $missingTools = Wsl 'for t in dpkg-buildpackage dh dpkg-shlibdeps; do command -v $t >/dev/null || echo $t; done'
    if ($missingTools) {
        Fail ("missing packaging tools in WSL: " + ($missingTools -join ", ") + " -- run in WSL: sudo apt install dpkg-dev debhelper")
    }
    Step "WSL '$Distro' ready"

    # ── 4. Clean staging tree inside WSL ──────────────────────────────────────
    $tar = Join-Path $env:TEMP "brakeza3d_deb_$Version.tar"
    # core.autocrlf=false: the archive keeps the LF endings stored in the repo (debian/rules must be LF)
    git -c core.autocrlf=false archive --format=tar -o $tar $Branch assets GLSL config debian
    if ($LASTEXITCODE -ne 0) { Fail "git archive failed" }
    $tarW = (Wsl "wslpath -a '$($tar -replace "'", "'\''")'") | Select-Object -Last 1
    $binW = (Wsl "wslpath -a '$($BinPath -replace "'", "'\''")'") | Select-Object -Last 1
    $src  = "`$HOME/brakeza3d-deb/brakeza3d-$Version"
    Step "Staging source tree in WSL ($src)"
    Wsl "set -e; rm -rf `$HOME/brakeza3d-deb; mkdir -p $src/cmake-build-release-wsl; tar -xf '$tarW' -C $src; cp '$binW' $src/cmake-build-release-wsl/Brakeza3D; chmod +x $src/cmake-build-release-wsl/Brakeza3D $src/debian/rules" | Out-Null
    if ($LASTEXITCODE -ne 0) { Fail "staging failed" }
    Remove-Item $tar -Force

    $deps = Wsl "cd $src && dpkg-checkbuilddeps 2>&1"
    if ($LASTEXITCODE -ne 0) {
        Write-Host ($deps -join "`n") -ForegroundColor Yellow
        Fail "missing build dependencies in WSL (see above) -- install them with sudo apt install ..."
    }

    # ── 5. dpkg-buildpackage ──────────────────────────────────────────────────
    Step "Running dpkg-buildpackage -us -uc -b"
    $log = Wsl "cd $src && dpkg-buildpackage -us -uc -b 2>&1"
    if ($LASTEXITCODE -ne 0) {
        $log | Select-Object -Last 30 | ForEach-Object { Write-Host $_ }
        Fail "dpkg-buildpackage failed"
    }
    $debW = "`$HOME/brakeza3d-deb/brakeza3d_${Version}_amd64.deb"
    if (-not ((Wsl "test -f $debW && echo yes") -contains "yes")) { Fail "no $debW produced" }

    # ── 6. Copy out and verify ────────────────────────────────────────────────
    New-Item -ItemType Directory -Force -Path $OutDir | Out-Null
    $outW = (Wsl "wslpath -a '$((Resolve-Path $OutDir).Path -replace "'", "'\''")'") | Select-Object -Last 1
    Wsl "cp $debW '$outW/$DebName'" | Out-Null
    if ($LASTEXITCODE -ne 0) { Fail "could not copy the .deb out of WSL" }
    $deb = Join-Path $OutDir $DebName

    $info = Wsl "dpkg-deb -f $debW Package Version Architecture Installed-Size Depends"
    $info | ForEach-Object { Write-Host "[package_linux]   $_" }
    $content = Wsl "dpkg-deb -c $debW"
    foreach ($must in @("./opt/brakeza3d/bin/Brakeza3D", "./usr/bin/brakeza3d", "./opt/brakeza3d/assets/", "./opt/brakeza3d/GLSL/")) {
        if (-not ($content | Where-Object { $_ -match [regex]::Escape($must) })) { Fail ".deb is missing $must" }
    }
    if ($content | Where-Object { $_ -match "/assets/.*/RTS/|\.bak" }) { Fail ".deb contains RTS files or backups" }
    Step ("Package ready: {0} ({1:N0} MB, {2} entries)" -f $deb, ((Get-Item $deb).Length / 1MB), $content.Count)
}
finally {
    Pop-Location
}
