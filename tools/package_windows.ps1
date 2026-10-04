<#
.SYNOPSIS
    Builds the Windows distribution package of Brakeza3D from the master branch.

.DESCRIPTION
    Replaces the old ..\copy_files.bat. Steps:
      1. Reads the engine version from <Branch>:include/Config.h (ENGINE_VERSION).
      2. Checks that the compiled Brakeza3D.exe is that version and is newer than the last core
         commit of <Branch> (build it from CLion first, Release-MinGW-BrakezaBundle, with <Branch>
         checked out -- this script never compiles).
      3. Empties the package folder (..\build by default), or renames it with -KeepPrevious.
      4. Exports assets\, GLSL\ and config\ with "git archive <Branch>": only files tracked in
         <Branch> go in, never untracked leftovers (RTS backups, test fonts...) or another branch's files.
      5. Copies the DLLs from ..\dlls (minus the obsolete ones), Brakeza3D.exe and imgui.ini to bin\.
      6. Verifies that every DLL the exe imports is in bin\ or is a Windows system DLL.
      7. Zips the package to ..\Brakeza3D-<version>-windows.zip (skip with -NoZip).
      8. Builds the installer ..\Brakeza3D-x64-86-Windows-installer.exe with Inno Setup (ISCC.exe) from
         tools/installer/Brakeza3D.iss, stamped with the engine version (skip with -NoInstaller).

    Works from any branch: the assets always come from <Branch>.

.PARAMETER Branch          Branch to package. Default: master.
.PARAMETER OutDir          Package folder. Default: ..\build (next to the repo).
.PARAMETER DllDir          Folder with the runtime DLLs. Default: ..\dlls.
.PARAMETER BuildDir        CMake build folder with Brakeza3D.exe. Default: cmake-build-release-mingw-brakezabundle.
.PARAMETER KeepPrevious    Rename the existing package folder to <OutDir>_<old version> instead of deleting it.
.PARAMETER NoZip           Do not create the zip.
.PARAMETER Strip           Strip debug symbols from the packaged exe (smaller download; crash addresses can no
                           longer be resolved with addr2line against that copy). The exe in BuildDir is untouched.
.PARAMETER Force           Package even if the exe version/date checks fail.
.PARAMETER NoInstaller     Do not build the Inno Setup installer.
.PARAMETER InstallerDir    Output folder of the installer. Default: the folder that contains the repo (..).
.PARAMETER Iscc            Path to ISCC.exe. Default: Inno Setup 6 in Program Files (x86) / Program Files.

.EXAMPLE
    .\tools\package_windows.ps1
    .\tools\package_windows.ps1 -KeepPrevious -Strip
#>
param(
    [string]$Branch = "master",
    [string]$OutDir,
    [string]$DllDir,
    [string]$BuildDir,
    [switch]$KeepPrevious,
    [switch]$NoZip,
    [switch]$Strip,
    [switch]$Force,
    [switch]$NoInstaller,
    [string]$InstallerDir,
    [string]$Iscc
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$Parent   = Split-Path -Parent $RepoRoot
if (-not $OutDir)   { $OutDir   = Join-Path $Parent "build" }
if (-not $DllDir)   { $DllDir   = Join-Path $Parent "dlls" }
if (-not $BuildDir) { $BuildDir = Join-Path $RepoRoot "cmake-build-release-mingw-brakezabundle" }
if (-not $InstallerDir) { $InstallerDir = $Parent }

# DLLs from ..\dlls that the engine no longer uses (audio moved from SDL2_mixer to miniaudio)
$ObsoleteDlls = @("SDL2_mixer.dll")
# Windows system DLLs: never shipped
$SystemDlls = @("KERNEL32.dll", "USER32.dll", "GDI32.dll", "OPENGL32.dll", "dbghelp.dll", "ucrtbase.dll",
                "ADVAPI32.dll", "SHELL32.dll", "ole32.dll", "WS2_32.dll", "msvcrt.dll", "WINMM.dll",
                "IMM32.dll", "VERSION.dll", "SETUPAPI.dll", "OLEAUT32.dll", "CRYPT32.dll", "bcrypt.dll")

function Step($msg) { Write-Host "[package_windows] $msg" -ForegroundColor Cyan }
function Fail($msg) { Write-Host "[package_windows] ERROR: $msg" -ForegroundColor Red; exit 1 }

Push-Location $RepoRoot
try {
    # ── 1. Version from <Branch> ──────────────────────────────────────────────
    git rev-parse --verify --quiet "$Branch" | Out-Null
    if ($LASTEXITCODE -ne 0) { Fail "branch '$Branch' not found" }
    $configH = git show "${Branch}:include/Config.h" | Out-String
    if ($configH -notmatch 'ENGINE_VERSION\s*=\s*"v?([0-9][0-9.]*)"') { Fail "ENGINE_VERSION not found in ${Branch}:include/Config.h" }
    $Version = $Matches[1]
    Step "Branch '$Branch', engine version $Version"

    # ── 2. Executable checks ──────────────────────────────────────────────────
    $exe = Join-Path $BuildDir "Brakeza3D.exe"
    if (-not (Test-Path $exe)) { Fail "no $exe -- build Release-MinGW-BrakezaBundle in CLion first" }
    $exeItem = Get-Item $exe
    $problems = @()

    $hasVersion = (& findstr.exe /M /C:"v$Version" $exe) -ne $null
    if (-not $hasVersion) { $problems += "the exe does not contain 'v$Version' (built from another branch or before the version bump)" }

    $coreTs = [int64](git log -1 --format=%ct $Branch -- src include GLSL CMakeLists.txt third_party resources)
    $coreDate = [DateTimeOffset]::FromUnixTimeSeconds($coreTs).LocalDateTime
    if ($exeItem.LastWriteTime -lt $coreDate) {
        $problems += "the exe ($($exeItem.LastWriteTime)) is older than the last core commit of $Branch ($coreDate)"
    }
    $current = (git rev-parse --abbrev-ref HEAD).Trim()
    if ($current -ne $Branch) {
        Write-Host "[package_windows] Note: checked-out branch is '$current'. Assets come from '$Branch' anyway; make sure the exe was compiled with '$Branch' checked out." -ForegroundColor Yellow
    }
    if ($problems.Count -gt 0) {
        $problems | ForEach-Object { Write-Host "[package_windows]   - $_" -ForegroundColor Yellow }
        if (-not $Force) { Fail "executable checks failed (use -Force to package anyway)" }
        Write-Host "[package_windows] -Force: continuing despite the checks" -ForegroundColor Yellow
    } else {
        Step "Executable OK: v$Version, built $($exeItem.LastWriteTime)"
    }

    # ── 3. Package folder ─────────────────────────────────────────────────────
    if (Test-Path $OutDir) {
        if ($KeepPrevious) {
            $prev = "old"
            $prevExe = Join-Path $OutDir "bin\Brakeza3D.exe"
            if (Test-Path $prevExe) {
                $prev = (Get-Item $prevExe).LastWriteTime.ToString("yyyyMMdd_HHmm")
            }
            $dest = "${OutDir}_$prev"
            Step "Keeping previous package as $dest"
            Rename-Item -Path $OutDir -NewName (Split-Path -Leaf $dest)
        } else {
            Step "Emptying $OutDir"
            Remove-Item -Recurse -Force $OutDir
        }
    }
    New-Item -ItemType Directory -Force -Path (Join-Path $OutDir "bin") | Out-Null

    # ── 4. assets / GLSL / config from <Branch> (tracked files only) ─────────
    Step "Exporting assets, GLSL and config from '$Branch' (git archive)"
    $tar = Join-Path $env:TEMP "brakeza3d_package_$Version.tar"
    git archive --format=tar -o $tar $Branch assets GLSL config
    if ($LASTEXITCODE -ne 0) { Fail "git archive failed" }
    & tar.exe -xf $tar -C $OutDir
    if ($LASTEXITCODE -ne 0) { Fail "tar extraction failed" }
    Remove-Item $tar -Force

    # ── 5. bin: DLLs, exe, imgui.ini ──────────────────────────────────────────
    if (-not (Test-Path $DllDir)) { Fail "DLL folder not found: $DllDir" }
    $bin = Join-Path $OutDir "bin"
    Get-ChildItem $DllDir -Filter *.dll | Where-Object { $ObsoleteDlls -notcontains $_.Name } |
        ForEach-Object { Copy-Item $_.FullName $bin -Force }
    Copy-Item $exe $bin -Force
    $ini = Join-Path $BuildDir "imgui.ini"
    if (Test-Path $ini) { Copy-Item $ini $bin -Force } else { Write-Host "[package_windows] Warning: no imgui.ini in $BuildDir" -ForegroundColor Yellow }

    # MinGW tools (objdump/strip): from the toolchain recorded in CMakeCache.txt
    $mingwBin = $null
    $cache = Join-Path $BuildDir "CMakeCache.txt"
    if (Test-Path $cache) {
        $line = Select-String -Path $cache -Pattern '^CMAKE_MAKE_PROGRAM:[A-Z]+=(.+)$' | Select-Object -First 1
        if ($line) { $mingwBin = Split-Path -Parent $line.Matches[0].Groups[1].Value }
    }

    if ($Strip) {
        $stripExe = if ($mingwBin) { Join-Path $mingwBin "strip.exe" } else { $null }
        if ($stripExe -and (Test-Path $stripExe)) {
            $before = (Get-Item (Join-Path $bin "Brakeza3D.exe")).Length
            & $stripExe --strip-debug (Join-Path $bin "Brakeza3D.exe")
            $after = (Get-Item (Join-Path $bin "Brakeza3D.exe")).Length
            Step ("Stripped exe: {0:N0} MB -> {1:N0} MB" -f ($before / 1MB), ($after / 1MB))
        } else {
            Write-Host "[package_windows] Warning: strip.exe not found, exe left as is" -ForegroundColor Yellow
        }
    }

    # ── 6. Every imported DLL present ─────────────────────────────────────────
    $objdump = if ($mingwBin) { Join-Path $mingwBin "objdump.exe" } else { $null }
    if ($objdump -and (Test-Path $objdump)) {
        $imports = & $objdump -p (Join-Path $bin "Brakeza3D.exe") | Select-String "DLL Name:\s*(\S+)" |
                   ForEach-Object { $_.Matches[0].Groups[1].Value } | Sort-Object -Unique
        $missing = $imports | Where-Object {
            ($SystemDlls -notcontains $_) -and -not (Test-Path (Join-Path $bin $_))
        }
        if ($missing) { Fail ("missing DLLs in bin: " + ($missing -join ", ")) }
        Step "All $($imports.Count) imported DLLs are present or are system DLLs"
    } else {
        Write-Host "[package_windows] Warning: objdump not found, DLL check skipped" -ForegroundColor Yellow
    }

    # ── 7. Zip ────────────────────────────────────────────────────────────────
    $size = (Get-ChildItem $OutDir -Recurse -File | Measure-Object Length -Sum).Sum
    Step ("Package ready: {0} ({1:N0} MB)" -f $OutDir, ($size / 1MB))
    if (-not $NoZip) {
        $zip = Join-Path (Split-Path -Parent $OutDir) "Brakeza3D-$Version-windows.zip"
        if (Test-Path $zip) { Remove-Item $zip -Force }
        Step "Zipping to $zip"
        & tar.exe -a -c -f $zip -C $OutDir bin assets GLSL config
        if ($LASTEXITCODE -ne 0) { Fail "zip creation failed" }
        Step ("Zip ready: {0:N0} MB" -f ((Get-Item $zip).Length / 1MB))
    }

    # ── 8. Installer (Inno Setup) ─────────────────────────────────────────────
    if (-not $NoInstaller) {
        if (-not $Iscc) {
            $Iscc = @("${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe", "$env:ProgramFiles\Inno Setup 6\ISCC.exe") |
                    Where-Object { Test-Path $_ } | Select-Object -First 1
        }
        if (-not $Iscc -or -not (Test-Path $Iscc)) { Fail "ISCC.exe (Inno Setup 6) not found -- install it, pass -Iscc <path> or use -NoInstaller" }
        $iss = Join-Path $RepoRoot "tools\installer\Brakeza3D.iss"
        # The .iss is exported from <Branch> too, so the installer definition matches the packaged branch
        $issTmp = Join-Path $env:TEMP "brakeza3d_installer_$Version.iss"
        if (git ls-tree --name-only $Branch tools/installer/Brakeza3D.iss) {
            git show "${Branch}:tools/installer/Brakeza3D.iss" | Out-File -FilePath $issTmp -Encoding utf8
        } else {
            Write-Host "[package_windows] Note: tools/installer/Brakeza3D.iss is not committed in '$Branch' yet, using the working copy" -ForegroundColor Yellow
            Copy-Item $iss $issTmp -Force
        }
        New-Item -ItemType Directory -Force -Path $InstallerDir | Out-Null
        $installer = Join-Path $InstallerDir "Brakeza3D-x64-86-Windows-installer.exe"
        if (Test-Path $installer) { Remove-Item $installer -Force }
        Step "Building installer with Inno Setup (version $Version)"
        & $Iscc /Q "/DMyAppVersion=$Version" "/DSourceDir=$((Resolve-Path $OutDir).Path)" `
                "/DOutputDir=$((Resolve-Path $InstallerDir).Path)" "/DRepoDir=$RepoRoot" $issTmp
        $isccExit = $LASTEXITCODE
        Remove-Item $issTmp -Force -ErrorAction SilentlyContinue
        if ($isccExit -ne 0 -or -not (Test-Path $installer)) { Fail "Inno Setup failed (exit $isccExit)" }
        Step ("Installer ready: {0} ({1:N0} MB)" -f $installer, ((Get-Item $installer).Length / 1MB))
    }
}
finally {
    Pop-Location
}
