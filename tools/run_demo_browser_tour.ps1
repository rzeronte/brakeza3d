<#
.SYNOPSIS
    Launches TutorialsProject with the demo browser tour (DemoBrowserTour.lua) and captures the window
    on every demo.

.DESCRIPTION
    --set demoBrowserTour=true loads assets/scripts/Browser/DemoBrowserTour.lua, which goes
    through every demo of DemoBrowser and prints "[DemoTour] ready <NN>" when each one has settled.
    The engine's own screenshot does not include the UI, so this script captures the game window
    (PrintWindow) at each "ready": logs/demo_tour_<NN>.png and logs/demo_tour_list.png (demo list
    open). A run that does not reach "[DemoTour] DONE" failed on the demo after the last capture.
    Never builds the project -- build it from CLion first.

.EXAMPLE
    .\tools\run_demo_browser_tour.ps1
    .\tools\run_demo_browser_tour.ps1 -Dwell 6
#>
param(
    [double]$Dwell = 4,
    [int]$ExitAfter = 0,
    [string]$ExePath
)
$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$LogsDir  = Join-Path $RepoRoot "logs"
if ($ExitAfter -le 0) { $ExitAfter = [int](11 * ($Dwell + 2.5 + 3) + 60) }

if (-not $ExePath) {
    $ExePath = @(
        (Join-Path $RepoRoot "cmake-build-release-mingw-brakezabundle\Brakeza3D.exe"),
        (Join-Path $RepoRoot "cmake-build-debug-mingw-brakezabundle\Brakeza3D.exe")
    ) | Where-Object { Test-Path $_ } | ForEach-Object { Get-Item $_ } |
        Sort-Object LastWriteTime -Descending | Select-Object -First 1 | ForEach-Object { $_.FullName }
    if (-not $ExePath) { throw "No Brakeza3D.exe found. Build the project from CLion first, or pass -ExePath." }
}
$exeDir = Split-Path -Parent $ExePath

# MinGW runtime DLLs (SDL2/glew/libstdc++) on PATH, as CLion does
$cache = Join-Path $exeDir "CMakeCache.txt"
if (Test-Path $cache) {
    $line = Select-String -Path $cache -Pattern '^CMAKE_MAKE_PROGRAM:FILEPATH=(.+)$' | Select-Object -First 1
    if ($line) { $env:PATH = (Split-Path -Parent $line.Matches[0].Groups[1].Value) + ";" + $env:PATH }
}

if (-not (Test-Path $LogsDir)) { New-Item -ItemType Directory -Force -Path $LogsDir | Out-Null }
$stdout = Join-Path $LogsDir "demo_tour_stdout.txt"
$stderr = Join-Path $LogsDir "demo_tour_stderr.txt"
Get-ChildItem $LogsDir -Filter "demo_tour_*.png" -ErrorAction SilentlyContinue | Remove-Item -Force

Add-Type -AssemblyName System.Drawing
Add-Type -Namespace Win32 -Name TourCap -MemberDefinition @"
[DllImport("user32.dll")] public static extern bool PrintWindow(System.IntPtr h, System.IntPtr hdc, uint flags);
[DllImport("user32.dll")] public static extern bool GetClientRect(System.IntPtr h, out RECT r);
public struct RECT { public int Left, Top, Right, Bottom; }
"@

# Captures the game WINDOW (works even if it is behind other windows)
function Save-Window($proc, $file) {
    $proc.Refresh()
    $h = $proc.MainWindowHandle
    if ($h -eq [System.IntPtr]::Zero) { Write-Warning "game window handle not found"; return }
    $r = New-Object Win32.TourCap+RECT
    [Win32.TourCap]::GetClientRect($h, [ref]$r) | Out-Null
    $w = [Math]::Max(1, $r.Right - $r.Left); $hh = [Math]::Max(1, $r.Bottom - $r.Top)
    $bmp = New-Object System.Drawing.Bitmap $w, $hh
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $hdc = $g.GetHdc()
    [Win32.TourCap]::PrintWindow($h, $hdc, 3) | Out-Null   # PW_CLIENTONLY | PW_RENDERFULLCONTENT
    $g.ReleaseHdc($hdc)
    $bmp.Save($file, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose()
    Write-Host "[run_demo_browser_tour] captured $file"
}

Write-Host "[run_demo_browser_tour] Using $ExePath"
$proc = Start-Process -FilePath $ExePath -WorkingDirectory $exeDir -PassThru -NoNewWindow `
    -ArgumentList @("-p", "TutorialsProject.json", "--set", "demoBrowserTour=true", "--set", "demoTourDwell=$Dwell",
                    "--log-console", "--exit-after", "$ExitAfter") `
    -RedirectStandardOutput $stdout -RedirectStandardError $stderr

$captured = @{}
$deadline = (Get-Date).AddSeconds($ExitAfter + 10)
while (-not $proc.HasExited -and (Get-Date) -lt $deadline) {
    Start-Sleep -Milliseconds 300
    if (-not (Test-Path $stdout)) { continue }
    $hits = Select-String -Path $stdout -Pattern '\[DemoTour\] ready (\S+)' -ErrorAction SilentlyContinue
    foreach ($m in $hits) {
        $tag = $m.Matches[0].Groups[1].Value
        if ($captured.ContainsKey($tag)) { continue }
        $captured[$tag] = $true
        Start-Sleep -Milliseconds 400
        Save-Window $proc (Join-Path $LogsDir "demo_tour_$tag.png")
    }
}
$proc.WaitForExit()
Write-Host "[run_demo_browser_tour] Exit code $($proc.ExitCode)"
Select-String -Path $stdout -Pattern '\[DemoTour\]|\[DemoBrowser\]|\[Error\]' | ForEach-Object { $_.Line }
exit $proc.ExitCode
