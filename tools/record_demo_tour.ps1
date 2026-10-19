<#
.SYNOPSIS
    Records a video of the TutorialsProject demo tour: it goes through every demo of the DemoBrowser and,
    on each one, presses the keys / moves the mouse described in tools/demo_tour_plan.json so you can see
    what the demo does.

.DESCRIPTION
    - Launches Brakeza3D with --set demoBrowserTour=true --set demoTourRecord=true (DemoBrowserTour.lua
      waits for this script to finish the actions of a demo before going to the next one).
    - Records the game window region of the desktop with ffmpeg (gdigrab, H.264, no audio).
    - Sends real keyboard / mouse input (SendInput) to the game window: DO NOT touch the keyboard or the
      mouse, and do not put other windows over the game while it runs (about 10 minutes).
    Never builds the project -- build it from CLion first.

.EXAMPLE
    .\tools\record_demo_tour.ps1
    .\tools\record_demo_tour.ps1 -Only 3,13,19       # only those demos (quick test)
    .\tools\record_demo_tour.ps1 -Fps 60 -Out C:\Videos\tour.mp4
#>
param(
    [int]$Fps = 30,
    [string]$Only = "",
    [string]$Out,
    [string]$ExePath,
    [string]$Ffmpeg,
    [switch]$NoVsync,           # start the game with --no-vsync (needed for a real 60 fps recording: with vsync it ran at 24-35 fps)
    [switch]$AllDemos,          # the 35 demos of the catalog instead of the 10 of the tour
    [switch]$Hd,                # sharp 1080p: the game window is always maximized (renders at ~1920x1008, not 1280x720), output 1920x1080, lower CRF
    [switch]$BurnSubs,          # burn the subtitles into the picture (1280x800 canvas); by default the video is 1280x720 and they go to a .srt next to -Out (YouTube)
    [double]$SubLag = 0.6,      # ffmpeg start-up delay: subtitles are shifted back by this much
    [switch]$Light,             # low-load mode for the final encode: BelowNormal priority, limited threads, faster preset (avoids freezing the PC while the video is assembled)
    [int]$Threads = 0,          # ffmpeg threads for the final encode (0 = auto; with -Light and 0 it uses half of the cores, max 4)
    [double]$MoveSpeed = 1.7,   # pointer speed multiplier (glides and rests before pressing); 1 = the old, slower hand
    [switch]$Safe               # slow but memory/CPU-safe 60 fps: MJPEG capture to disk (no x264 competing with the game), one low-priority encode at a time with bounded buffers, raw segment deleted after each encode, effective fps measured and reported. Implies -Light -NoVsync and 60 fps unless -Fps is given
)
$ErrorActionPreference = "Stop"
if ($Safe) {
    $Light = $true; $NoVsync = $true
    if (-not $PSBoundParameters.ContainsKey("Fps")) { $Fps = 60 }
}
# -File passes "21,23" as ONE string: split it
$OnlyList = @($Only | ForEach-Object { "$_" -split "[, ]" } | Where-Object { $_ -ne "" } | ForEach-Object { [int]$_ })
$RepoRoot = Split-Path -Parent $PSScriptRoot
$LogsDir  = Join-Path $RepoRoot "logs"
if (-not (Test-Path $LogsDir)) { New-Item -ItemType Directory -Force -Path $LogsDir | Out-Null }
if (-not $Out) { $Out = Join-Path $LogsDir "demo_tour.mp4" }
$Out = [IO.Path]::GetFullPath($(if ([IO.Path]::IsPathRooted($Out)) { $Out } else { Join-Path (Get-Location).Path $Out }))

if (-not $ExePath) {
    $ExePath = @(
        (Join-Path $RepoRoot "cmake-build-release-mingw-brakezabundle\Brakeza3D.exe"),
        (Join-Path $RepoRoot "cmake-build-debug-mingw-brakezabundle\Brakeza3D.exe")
    ) | Where-Object { Test-Path $_ } | ForEach-Object { Get-Item $_ } |
        Sort-Object LastWriteTime -Descending | Select-Object -First 1 | ForEach-Object { $_.FullName }
    if (-not $ExePath) { throw "No Brakeza3D.exe found. Build the project from CLion first, or pass -ExePath." }
}
$exeDir = Split-Path -Parent $ExePath

if (-not $Ffmpeg) {
    $cmd = Get-Command ffmpeg -ErrorAction SilentlyContinue
    if ($cmd) { $Ffmpeg = $cmd.Source }
    else {
        $Ffmpeg = Get-ChildItem "$env:LOCALAPPDATA\Microsoft\WinGet\Packages" -Recurse -Filter ffmpeg.exe -ErrorAction SilentlyContinue |
            Select-Object -First 1 -ExpandProperty FullName
    }
    if (-not $Ffmpeg) { throw "ffmpeg not found. Install it (winget install Gyan.FFmpeg) or pass -Ffmpeg." }
}

$cache = Join-Path $exeDir "CMakeCache.txt"
if (Test-Path $cache) {
    $line = Select-String -Path $cache -Pattern '^CMAKE_MAKE_PROGRAM:FILEPATH=(.+)$' | Select-Object -First 1
    if ($line) { $env:PATH = (Split-Path -Parent $line.Matches[0].Groups[1].Value) + ";" + $env:PATH }
}

Add-Type -TypeDefinition @"
using System;
using System.Runtime.InteropServices;
public static class TourInput {
    [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
    [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
    [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
    [DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr h);
    [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h, int cmd);
    [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h, out RECT r);
    [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h, ref POINT p);
    [DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
    [DllImport("user32.dll")] public static extern bool GetCursorPos(out POINT p);
    [DllImport("user32.dll")] public static extern void mouse_event(uint flags, int dx, int dy, int data, UIntPtr extra);
    [DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
    [DllImport("user32.dll")] public static extern uint MapVirtualKey(uint code, uint type);
    [StructLayout(LayoutKind.Sequential)] public struct RECT { public int Left, Top, Right, Bottom; }
    [StructLayout(LayoutKind.Sequential)] public struct POINT { public int X, Y; }
    public static void Key(byte vk, bool down, bool ext) {
        uint f = (down ? 0u : 2u) | (ext ? 1u : 0u);
        keybd_event(vk, (byte)MapVirtualKey(vk, 0), f, UIntPtr.Zero);
    }
    public static void Mouse(uint flags, int data) { mouse_event(flags, 0, 0, data, UIntPtr.Zero); }
}
"@
[TourInput]::SetProcessDPIAware() | Out-Null

$VK = @{ RCTRL=0xA3; F2=0x71; F5=0x74; F6=0x75; F7=0x76; F8=0x77; F9=0x78; F10=0x79; F11=0x7A; F12=0x7B; SPACE=0x20; LEFT=0x25; UP=0x26; RIGHT=0x27; DOWN=0x28; LCTRL=0xA2; LSHIFT=0xA0; DELETE=0x2E; TAB=0x09; ESC=0x1B }
$EXT = @("LEFT","UP","RIGHT","DOWN","DELETE","RCTRL")
function Get-Vk($name) {
    if ($VK.ContainsKey($name)) { return [byte]$VK[$name] }
    if ($name.Length -eq 1) { return [byte][char]$name.ToUpper() }
    throw "unknown key '$name'"
}
function Send-Key($name, $down) { [TourInput]::Key((Get-Vk $name), $down, ($EXT -contains $name)) }

# --- window / region ---------------------------------------------------------------------------------------
$script:winRect = $null
function Update-WinRect($proc) {
    $proc.Refresh()
    $h = $proc.MainWindowHandle
    if (-not $h -or $h -eq [IntPtr]::Zero) { return $false }   # null once the game has exited
    $r = New-Object TourInput+RECT
    [TourInput]::GetClientRect($h, [ref]$r) | Out-Null
    $p = New-Object TourInput+POINT
    [TourInput]::ClientToScreen($h, [ref]$p) | Out-Null
    $new = @{ X = $p.X; Y = $p.Y; W = ($r.Right - $r.Left); H = ($r.Bottom - $r.Top); Handle = $h }
    if ($new.W -lt 400 -or $new.H -lt 300) {     # minimized / mid-resize / not yet a real window: keep the last good rectangle
        [TourInput]::ShowWindow($h, 9) | Out-Null; [TourInput]::SetForegroundWindow($h) | Out-Null
        return $false
    }
    $script:winRect = $new
    return $true
}
# Foreground (an occluded GL window stops presenting frames and the loading stalls); Alt tap unlocks SetForegroundWindow
$script:editorOn = $false
function Focus-Game {
    if (-not $script:winRect) { return }
    # The Alt tap that unlocks SetForegroundWindow also puts the editor's ImGui in keyboard-navigation mode, which swallows the
    # RCtrl+F-key shortcuts: only do it when the game really is not in the foreground.
    if ([TourInput]::GetForegroundWindow() -ne $script:winRect.Handle) {
        [TourInput]::Key(0x12, $true, $false); [TourInput]::Key(0x12, $false, $false)
    }
    [TourInput]::ShowWindow($script:winRect.Handle, $(if ($script:editorOn -or $Hd) { 3 } else { 9 })) | Out-Null   # 3 = maximized (editor), 9 = restore
    [TourInput]::BringWindowToTop($script:winRect.Handle) | Out-Null
    [TourInput]::SetForegroundWindow($script:winRect.Handle) | Out-Null
}
function To-Screen($fx, $fy) {
    @([int]($script:winRect.X + $fx * $script:winRect.W), [int]($script:winRect.Y + $fy * $script:winRect.H))
}

# --- plan -> list of timed events ---------------------------------------------------------------------------
$script:rng = New-Object System.Random 7
$script:cur = @(0.5, 0.5)      # last mouse position (fractions of the window), updated while expanding a plan

function Smooth($f) { return $f * $f * (3 - 2 * $f) }     # ease in / out: a hand accelerates and brakes

# Plan -> events. The pointer always GLIDES to where it has to click (curved path, ease in/out, a little tremor)
# and rests a moment before pressing, like a person would; keys get a slightly irregular hold time.
function Expand-Plan($steps, $subList) {
    $ev = New-Object System.Collections.ArrayList
    function Add($t, $kind, $a, $b) { [void]$ev.Add([pscustomobject]@{ T = [double][Math]::Max(0.0, $t); K = $kind; A = $a; B = $b; N = $ev.Count }) }
    # glide from the current position to (x,y), arriving at time $tEnd
    function Glide($tEnd, $x, $y, $minDur) {
        $x0 = $script:cur[0]; $y0 = $script:cur[1]
        $dist = [Math]::Sqrt(($x - $x0) * ($x - $x0) + ($y - $y0) * ($y - $y0))
        $dur = [Math]::Max($minDur, 0.35 + 1.1 * $dist) / $MoveSpeed
        $n = [Math]::Max(6, [int]($dur * 60))
        $side = if ($script:rng.Next(2) -eq 0) { 1 } else { -1 }
        $bow = 0.10 * $dist * $side * (0.6 + 0.8 * $script:rng.NextDouble())   # sideways bulge of the path
        $nx = -($y - $y0); $ny = ($x - $x0)
        $len = [Math]::Max(1e-6, [Math]::Sqrt($nx * $nx + $ny * $ny)); $nx /= $len; $ny /= $len
        $cx = ($x0 + $x) / 2 + $nx * $bow; $cy = ($y0 + $y) / 2 + $ny * $bow
        for ($i = 1; $i -le $n; $i++) {
            $f = Smooth ($i / $n)
            $px = (1 - $f) * (1 - $f) * $x0 + 2 * (1 - $f) * $f * $cx + $f * $f * $x
            $py = (1 - $f) * (1 - $f) * $y0 + 2 * (1 - $f) * $f * $cy + $f * $f * $y
            if ($i -lt $n) { $px += ($script:rng.NextDouble() - 0.5) * 0.0015; $py += ($script:rng.NextDouble() - 0.5) * 0.0015 }
            Add ($tEnd - $dur + $dur * ($i / $n)) "mv" $px $py
        }
        $script:cur = @($x, $y)
    }
    foreach ($sb in @($subList)) { if ($sb.text) { Add ([double]$sb.t) "sub" $sb.text ([double]$sb.d) } }
    foreach ($s in $steps) {
        $t = [double]$s.t
        $btnDown = if ($s.button -eq "right") { 0x0008 } else { 0x0002 }
        $btnUp   = if ($s.button -eq "right") { 0x0010 } else { 0x0004 }
        $press   = 0.07 + 0.06 * $script:rng.NextDouble()
        if ($null -ne $s.editor) {
            Add $t "editor" $s.editor 0
        } elseif ($s.drag) {
            $d = [double]$s.dur
            if ($s.keydrag) { Add ($t - 0.35) "kd" $s.keydrag 0 }
            $pre = $(if ($s.pre) { [double]$s.pre } else { 0.2 }) / [Math]::Sqrt($MoveSpeed)      # rest on the start point before pressing (gizmo handles need the hover first)
            Glide ($t) $s.drag[0] $s.drag[1] 0.5
            Add ($t + $pre) "md" $btnDown 0
            $steps2 = [Math]::Max(6, [int]($d * 60))
            for ($i = 1; $i -le $steps2; $i++) {
                $f = Smooth ($i / $steps2)
                Add ($t + $pre + $d * ($i / $steps2)) "mv" ($s.drag[0] + ($s.drag[2] - $s.drag[0]) * $f) ($s.drag[1] + ($s.drag[3] - $s.drag[1]) * $f)
            }
            $script:cur = @($s.drag[2], $s.drag[3])
            Add ($t + $pre + 0.15 + $d) "mu" $btnUp 0
            if ($s.keydrag) { Add ($t + 0.6 + $d) "ku" $s.keydrag 0 }
        } elseif ($s.move) {
            Glide ($t + 0.1) $s.move[0] $s.move[1] 0.4
            $d = [double]$s.dur; $n2 = [Math]::Max(6, [int]($d * 60))
            for ($i = 1; $i -le $n2; $i++) {
                $f = Smooth ($i / $n2)
                $wob = [Math]::Sin($f * 6.283 * 1.5) * 0.015
                Add ($t + 0.1 + $d * ($i / $n2)) "mv" ($s.move[0] + ($s.move[2] - $s.move[0]) * $f + $wob) ($s.move[1] + ($s.move[3] - $s.move[1]) * $f - $wob)
            }
            $script:cur = @($s.move[2], $s.move[3])
        } elseif ($null -ne $s.wheel) {
            Glide ($t) $s.at[0] $s.at[1] 0.5
            $n = [int]$s.wheel; $dir = if ($n -ge 0) { 120 } else { -120 }
            for ($i = 0; $i -lt [Math]::Abs($n); $i++) { Add ($t + 0.3 + 0.12 * $i) "mw" $dir 0 }
        } elseif ($s.click) {
            # "pre" = how long the pointer rests on the target before pressing (menus of the editor need it to highlight the item)
            $pre = $(if ($s.pre) { [double]$s.pre } else { 0.18 }) / [Math]::Sqrt($MoveSpeed)
            Glide ($t) $s.click[0] $s.click[1] 0.5
            Add ($t + $pre) "md" $btnDown 0
            Add ($t + $pre + $press) "mu" $btnUp 0
        } elseif ($s.key -like "*+click") {
            $mod = ($s.key -split '\+')[0]
            Add ($t - 0.3) "kd" $mod 0
            Glide ($t) $s.pos[0] $s.pos[1] 0.5
            Add ($t + 0.18) "md" 0x0002 0
            Add ($t + 0.18 + $press) "mu" 0x0004 0
            Add ($t + 0.45) "ku" $mod 0
        } elseif ($s.key) {
            $keys = $s.key -split '\+'
            $hold = if ($s.hold) { [double]$s.hold } else { 0.15 }
            $hold = $hold * (0.9 + 0.25 * $script:rng.NextDouble())
            $jit  = 0.06 * $script:rng.NextDouble()
            $i = 0
            # combos (RCTRL+F7): the modifier goes down 0.12 s before the key, or the engine can see the key without it
            foreach ($k in $keys) { Add ($t + $jit + 0.12 * $i) "kd" $k 0; $i++ }
            [array]::Reverse($keys)
            foreach ($k in $keys) { Add ($t + $jit + $hold + 0.12 * $i) "ku" $k 0; $i++ }
        }
    }
    return ($ev | Sort-Object T, N)
}

# --- recording in segments: the window changes size when the editor GUI is toggled, so each size is its own file ---
$ff = $null
$recSw = $null
$segs = New-Object System.Collections.ArrayList
$subs = New-Object System.Collections.ArrayList

function Start-Segment {
    $w = $script:winRect.W - ($script:winRect.W % 2); $h = $script:winRect.H - ($script:winRect.H % 2)
    $file = Join-Path $LogsDir ("demo_tour_seg{0:00}.{1}" -f $segs.Count, $(if ($Safe) { "mkv" } else { "mp4" }))
    Write-Host "[record_demo_tour] segment $($segs.Count): region $($script:winRect.X),$($script:winRect.Y) ${w}x${h}"
    $psi = New-Object Diagnostics.ProcessStartInfo
    $psi.FileName = $Ffmpeg
    $capPreset = if ($Light) { "ultrafast" } else { "veryfast" }   # ultrafast: far less CPU while the game runs (bigger temp file, re-encoded later anyway)
    $capCrf = if ($Hd) { 16 } else { 20 }
    $capThr = if ($Light) { "-threads 2" } else { "" }
    if ($Safe) {
        # MJPEG: intra-only, almost no CPU and no lookahead/rate-control buffers while the game runs; the big input queue absorbs hiccups
        $psi.Arguments = "-y -thread_queue_size 1024 -f gdigrab -framerate $Fps -offset_x $($script:winRect.X) -offset_y $($script:winRect.Y) -video_size ${w}x${h} -draw_mouse 1 -i desktop -c:v mjpeg -q:v 3 -pix_fmt yuvj420p -threads 2 `"$file`""
    } else {
    $psi.Arguments = "-y -f gdigrab -framerate $Fps -offset_x $($script:winRect.X) -offset_y $($script:winRect.Y) -video_size ${w}x${h} -draw_mouse 1 -i desktop -c:v libx264 -preset $capPreset $capThr -crf $capCrf -pix_fmt yuv420p `"$file`""
    }
    $psi.UseShellExecute = $false; $psi.RedirectStandardInput = $true; $psi.RedirectStandardError = $true
    $script:ff = [Diagnostics.Process]::Start($psi)
    if ($Safe) { try { $script:ff.PriorityClass = "AboveNormal" } catch {} }   # the capture must never be starved by the game
    $script:ff.BeginErrorReadLine() 2>$null
    if (-not $script:recSw) { $script:recSw = [Diagnostics.Stopwatch]::StartNew() }
    [void]$segs.Add([pscustomobject]@{ File = $file; Start = $script:recSw.Elapsed.TotalSeconds; End = 1e9 })
    Start-Sleep -Milliseconds 1200
}
function Stop-Segment {
    if ($script:ff -and -not $script:ff.HasExited) {
        $segs[$segs.Count - 1].End = $script:recSw.Elapsed.TotalSeconds
        $script:ff.StandardInput.WriteLine("q")
        $script:ff.WaitForExit(20000) | Out-Null
        if (-not $script:ff.HasExited) { $script:ff.Kill() }
    }
}
# F5 with Right Ctrl: shows / hides the engine's editor GUI (the window resizes), so a new segment starts
function Toggle-Editor($proc, $on) {
    Stop-Segment
    Focus-Game; Start-Sleep -Milliseconds 300
    if ([TourInput]::GetForegroundWindow() -ne $script:winRect.Handle) {   # never type into another application
        Write-Warning "the game window is not in the foreground: editor toggle skipped"
        Start-Segment
        return
    }
    $script:editorOn = [bool]$on
    Send-Key "RCTRL" $true; Start-Sleep -Milliseconds 120
    Send-Key "F5" $true; Start-Sleep -Milliseconds 90; Send-Key "F5" $false; Start-Sleep -Milliseconds 90
    Send-Key "RCTRL" $false
    Focus-Game                                   # maximizes (editor on) or restores (editor off): the editor needs room
    Start-Sleep -Milliseconds 3000
    for ($i = 0; $i -lt 10 -and -not (Update-WinRect $proc); $i++) { Start-Sleep -Milliseconds 500 }   # wait for a real window size (a 36x16 region was captured once)
    Start-Sleep -Milliseconds 300
    Start-Segment
}

$script:skippedKeys = 0
$script:btnDown = @{}
$script:lastMv = @(0.0, 0.0)
function Run-Events($events, $endAt, $proc) {
    $sw = [Diagnostics.Stopwatch]::StartNew()
    $shift = 0.0     # time spent in barriers (editor toggles): the plan clock does not run during them
    $events = @($events | Where-Object { $null -ne $_ })
    $idx = 0; $n = $events.Count
    $lastCheck = -1.0; $gameHasFocus = $true
    while (($sw.Elapsed.TotalSeconds - $shift) -lt $endAt) {
        if ($proc.HasExited) { break }          # the game closed: do not keep clicking on whatever is behind it
        # Input goes only to the game window: if something else took the focus, try to get it back and hold the input meanwhile
        if (($sw.Elapsed.TotalSeconds - $lastCheck) -gt 0.25) {
            $lastCheck = $sw.Elapsed.TotalSeconds
            $gameHasFocus = ([TourInput]::GetForegroundWindow() -eq $script:winRect.Handle)
            if (-not $gameHasFocus) {
                Focus-Game; Start-Sleep -Milliseconds 120
                $gameHasFocus = ([TourInput]::GetForegroundWindow() -eq $script:winRect.Handle)
            }
        }
        $now = $sw.Elapsed.TotalSeconds - $shift
        while ($idx -lt $n -and $events[$idx].T -le $now) {
            $e = $events[$idx]; $idx++
            if ($e.K -in @("md", "mu", "mw")) {     # input trace: who pressed what, and where the pointer was (logs/tour_input_trace.txt)
                try { Add-Content -Path (Join-Path $PSScriptRoot "..\logs\tour_input_trace.txt") -Value ("{0:N2} plan={1:N2} {2} A={3} focus={4} cur={5:N4},{6:N4}" -f $sw.Elapsed.TotalSeconds, $e.T, $e.K, $e.A, $gameHasFocus, $script:lastMv[0], $script:lastMv[1]) } catch {}
            }
            switch ($e.K) {
                "kd" {
                    if ($gameHasFocus) { Send-Key $e.A $true } else { $script:skippedKeys++ }
                    # Real-time pacing: if the loop was late, a modifier and its key (or a key and its release) would arrive in the same
                    # millisecond, and the engine only sees the modifier on the NEXT frame: hold every key down for a while before going on
                    if ($e.A -in @("RCTRL", "LCTRL", "LSHIFT")) { Start-Sleep -Milliseconds 140 } else { Start-Sleep -Milliseconds 90 }
                }
                "ku" { Send-Key $e.A $false }
                "mv" { if ($gameHasFocus) { $script:lastMv = @($e.A, $e.B); $p = To-Screen $e.A $e.B; [TourInput]::SetCursorPos($p[0], $p[1]) | Out-Null } }
                "md" { if ($gameHasFocus) { [TourInput]::Mouse([uint32]$e.A, 0); $script:btnDown[[int]$e.A] = $true; Start-Sleep -Milliseconds 70 } }   # real-time hold: a late loop must not press and release in the same frame (ImGui drops that click)
                "mu" { $d = [int]($e.A / 2); if ($script:btnDown.ContainsKey($d)) { [TourInput]::Mouse([uint32]$e.A, 0); $script:btnDown.Remove($d) } }
                "mw" { if ($gameHasFocus) { [TourInput]::Mouse([uint32]0x0800, [int]$e.A) } }
                "sub" {
                    $t = $script:recSw.Elapsed.TotalSeconds
                    [void]$subs.Add([pscustomobject]@{ Start = $t; End = $t + [double]$e.B; Text = $e.A })
                }
                "editor" {
                    $before = $sw.Elapsed.TotalSeconds
                    Toggle-Editor $proc $e.A
                    $shift += ($sw.Elapsed.TotalSeconds - $before)
                }
            }
        }
        Start-Sleep -Milliseconds 8
    }
    # nothing is left pressed: release ONLY what is still down (left up = 4 for a left down = 2; right up = 16 for a right down = 8)
    foreach ($down in @($script:btnDown.Keys)) { [TourInput]::Mouse([uint32]($down * 2), 0) }
    $script:btnDown.Clear()
    if ($script:skippedKeys -gt 0) { Write-Warning "$($script:skippedKeys) key events skipped: the game window was not in the foreground"; $script:skippedKeys = 0 }
}

# --- launch ---------------------------------------------------------------------------------------------------
$plan = Get-Content (Join-Path $PSScriptRoot "demo_tour_plan.json") -Raw | ConvertFrom-Json
$stdout = Join-Path $LogsDir "demo_tour_stdout.txt"
$stderr = Join-Path $LogsDir "demo_tour_stderr.txt"
$ackFile = Join-Path $LogsDir "demo_tour_ack.txt"
# the game reads this file with io.open: retry if it happens to have it open at that very moment
function Write-Ack($n) {
    for ($i = 0; $i -lt 50; $i++) {
        try { [IO.File]::WriteAllText($ackFile, "$n"); return } catch { Start-Sleep -Milliseconds 40 }
    }
    Write-Warning "could not write $ackFile"
}
Write-Ack 0
Get-ChildItem $LogsDir -Filter "demo_tour_seg*" -ErrorAction SilentlyContinue | Remove-Item -Force

Write-Host "[record_demo_tour] Using $ExePath"
$proc = Start-Process -FilePath $ExePath -WorkingDirectory $exeDir -PassThru -NoNewWindow `
    -ArgumentList (@("-p", "TutorialsProject.json", "--set", "demoBrowserTour=true", "--set", "demoTourRecord=true",
                    "--set", "demoTourDwell=2") + $(if ($AllDemos) { @() } else { @("--set", "demoTourTourOnly=true") }) + $(if ($NoVsync) { @("--no-vsync") } else { @() }) + @("--log-console", "--exit-after", "1500")) `
    -RedirectStandardOutput $stdout -RedirectStandardError $stderr

function Format-AssTime([double]$t) {
    if ($t -lt 0) { $t = 0 }
    $h = [int][Math]::Floor($t / 3600); $m = [int][Math]::Floor(($t % 3600) / 60); $sec = $t % 60
    return ("{0}:{1:00}:{2:00.00}" -f $h, $m, $sec).Replace(",", ".")
}
function Format-SrtTime([double]$t) {
    if ($t -lt 0) { $t = 0 }
    $ms = [long][Math]::Round($t * 1000)
    return ("{0:00}:{1:00}:{2:00},{3:000}" -f [int][Math]::Floor($ms / 3600000), [int]([Math]::Floor($ms / 60000) % 60), [int]([Math]::Floor($ms / 1000) % 60), [int]($ms % 1000))
}
function Write-Subtitles($file, $list, $segStart) {
    $head = @"
[Script Info]
ScriptType: v4.00+
PlayResX: 1280
PlayResY: 800
WrapStyle: 0

[V4+ Styles]
Format: Name, Fontname, Fontsize, PrimaryColour, SecondaryColour, OutlineColour, BackColour, Bold, Italic, Underline, StrikeOut, ScaleX, ScaleY, Spacing, Angle, BorderStyle, Outline, Shadow, Alignment, MarginL, MarginR, MarginV, Encoding
Style: Default,Segoe UI,27,&H00FFFFFF,&H000000FF,&H00000000,&H00000000,0,0,0,0,100,100,0,0,1,2,0,2,60,60,14,1

[Events]
Format: Layer, Start, End, Style, Name, MarginL, MarginR, MarginV, Effect, Text
"@
    $lines = foreach ($e in $list) {
        $txt = ($e.Text -replace '\{', '(' -replace '\}', ')')
        "Dialogue: 0,$(Format-AssTime ($e.Start - $segStart - $SubLag)),$(Format-AssTime ($e.End - $segStart - $SubLag)),Default,,0,0,0,,$txt"
    }
    Set-Content -Path $file -Value ($head + "`n" + ($lines -join "`n")) -Encoding UTF8
}

try {
    $done = @{}
    $deadline = (Get-Date).AddSeconds(1500)
    $lastFocus = Get-Date
    while (-not $proc.HasExited -and (Get-Date) -lt $deadline) {
        Start-Sleep -Milliseconds 200
        if (((Get-Date) - $lastFocus).TotalSeconds -gt 2) { $lastFocus = Get-Date; if (Update-WinRect $proc) { Focus-Game } }
        $text = if (Test-Path $stdout) { Get-Content $stdout -Tail 400 -ErrorAction SilentlyContinue | Select-String -Pattern '\[DemoTour\] (ready \S+|DONE.*)' } else { @() }
        foreach ($m in $text) {
            $line = $m.Matches[0].Groups[1].Value
            if ($done.ContainsKey($line)) { continue }
            $done[$line] = $true

            if ($line -like "DONE*") { Start-Sleep -Seconds 1; break }
            if ($line -like "ready list") {
                if ($ff) {
                    Start-Sleep -Milliseconds 3200   # the demo list is shown for about 4 s: stop BEFORE the game exits, or the
                    Stop-Segment                     # recording would catch whatever is on the desktop behind the window
                }
                continue
            }
            $num = [int](($line -split ' ')[1])
            if ($OnlyList.Count -gt 0 -and ($OnlyList -notcontains $num)) {
                Write-Ack $num      # skipped demo: straight to the next one (not recorded)
                continue
            }
            if (-not $ff) {
                if (-not (Update-WinRect $proc)) { continue }
                Focus-Game
                Start-Sleep -Milliseconds 500
                Update-WinRect $proc | Out-Null
                Start-Segment
            }
            Update-WinRect $proc | Out-Null
            Focus-Game
            $entry = $plan."$num"
            $steps = if ($entry) { @($entry.steps) } else { @() }
            $end   = if ($entry) { [double]$entry.end } else { 5.0 }
            Write-Host "[record_demo_tour] demo $num ($($steps.Count) actions, ${end}s)"
            $cp = New-Object TourInput+POINT
            [TourInput]::GetCursorPos([ref]$cp) | Out-Null
            if ($script:winRect.W -gt 0) {
                $script:cur = @([Math]::Min(1, [Math]::Max(0, ($cp.X - $script:winRect.X) / $script:winRect.W)),
                                [Math]::Min(1, [Math]::Max(0, ($cp.Y - $script:winRect.Y) / $script:winRect.H)))
            }
            $ev = Expand-Plan $steps $(if ($entry) { $entry.subs } else { @() })
            Run-Events $ev $end $proc
            if ($OnlyList.Count -gt 0 -and $num -eq $OnlyList[-1]) { Stop-Segment; $script:onlyDone = $true; break }
            Write-Ack $num
        }
        if ($script:onlyDone) { break }
        if ($done.Keys | Where-Object { $_ -like "DONE*" }) { break }
    }
} finally {
    Stop-Segment
    if ($script:onlyDone -and -not $proc.HasExited) { $proc.Kill() }
    if (-not $proc.HasExited) { $proc.WaitForExit(15000) | Out-Null }
    if (-not $proc.HasExited) { $proc.Kill() }
}

# --- every segment: game scaled into 1280x720 on a 1280x800 canvas (the 80 px strip below is for the subtitles, so they never
# --- cover the demo's own HUD), subtitles burned in, then joined ------------------------------------
$encPreset = "medium"
$encThreads = @()
if ($Light) {
    $encPreset = if ($Safe) { "medium" } else { "veryfast" }
    try { (Get-Process -Id $PID).PriorityClass = "BelowNormal" } catch {}   # ffmpeg children inherit it
    if ($Threads -le 0) { $Threads = [Math]::Max(1, [Math]::Min(4, [int]($env:NUMBER_OF_PROCESSORS) / 2)) }
}
if ($Threads -gt 0) { $encThreads = @("-threads", "$Threads", "-filter_threads", "1") }
$encMem = if ($Safe) { @("-x264-params", "rc-lookahead=20:ref=3:sync-lookahead=0") } else { @() }   # bounded lookahead / frame buffers
$ffprobe = Join-Path (Split-Path -Parent $Ffmpeg) "ffprobe.exe"
$lowFps = New-Object System.Collections.Generic.List[string]
Push-Location $LogsDir
try {
    $parts = @()
    $srt = New-Object System.Collections.Generic.List[string]
    $srtIdx = 0
    $offset = 0.0     # start of the current segment inside the joined video
    for ($i = 0; $i -lt $segs.Count; $i++) {
        $sg = $segs[$i]
        if (-not (Test-Path $sg.File)) { continue }
        $mine = @($subs | Where-Object { $_.Start -ge $sg.Start - 0.01 -and $_.Start -lt $sg.End })
        $assName = "demo_tour_seg{0:00}.ass" -f $i
        $finalName = "demo_tour_seg{0:00}_final.mp4" -f $i
        $outW = if ($Hd) { 1920 } else { 1280 }; $outH = if ($Hd) { 1080 } else { 720 }
        $finalCrf = if ($Hd) { 15 } else { 18 }
        if ($BurnSubs) {
            Write-Subtitles (Join-Path $LogsDir $assName) $mine $sg.Start
            $vf = if ($Hd) { "scale=1920:1080:force_original_aspect_ratio=decrease:flags=lanczos,pad=1920:1200:(ow-iw)/2:0:black,ass=$assName" } else { "scale=1280:720:force_original_aspect_ratio=decrease,pad=1280:800:(ow-iw)/2:0:black,ass=$assName" }
        } else {
            $vf = "scale=${outW}:${outH}:force_original_aspect_ratio=decrease:flags=lanczos,pad=${outW}:${outH}:(ow-iw)/2:(oh-ih)/2:black"
        }
        & $Ffmpeg -y -v error @encThreads -i (Split-Path -Leaf $sg.File) -vf $vf -r $Fps -c:v libx264 -preset $encPreset @encMem -crf $finalCrf -pix_fmt yuv420p $finalName
        if ($LASTEXITCODE -ne 0) { throw "ffmpeg failed encoding segment $i (raw file kept: $($sg.File))" }
        $parts += $finalName
        if ($Safe) {
            # real frames captured per second of the segment: below 95 % of -Fps means the capture dropped frames (the encode only duplicated them)
            $span = $sg.End - $sg.Start
            if ((Test-Path $ffprobe) -and $span -gt 1) {
                $pk = & $ffprobe -v error -select_streams v:0 -count_packets -show_entries stream=nb_read_packets -of csv=p=0 (Split-Path -Leaf $sg.File)
                $eff = [double]::Parse(("$pk").Trim().TrimEnd(","), [Globalization.CultureInfo]::InvariantCulture) / $span
                Write-Host ("[record_demo_tour] segment {0}: {1:N1} captured fps (target {2})" -f $i, $eff, $Fps)
                if ($eff -lt 0.95 * $Fps) { $lowFps.Add(("segment {0}: {1:N1} fps" -f $i, $eff)) }
            }
            Remove-Item -Force $sg.File    # free the disk before the next (bigger) encode
        }
        foreach ($e in $mine) {
            $srtIdx++
            $a = $offset + [Math]::Max(0, $e.Start - $sg.Start - $SubLag)
            $b = $offset + [Math]::Max(0, $e.End - $sg.Start - $SubLag)
            $srt.Add("$srtIdx`n$(Format-SrtTime $a) --> $(Format-SrtTime $b)`n$($e.Text)`n")
        }
        $ErrorActionPreference = "Continue"      # ffmpeg -i prints to stderr and exits 1: not an error here
        $info = & $Ffmpeg -hide_banner -i $finalName 2>&1 | Out-String
        $ErrorActionPreference = "Stop"
        if ($info -match 'Duration: (\d+):(\d+):(\d+\.\d+)') { $offset += [int]$Matches[1] * 3600 + [int]$Matches[2] * 60 + [double]::Parse($Matches[3], [Globalization.CultureInfo]::InvariantCulture) }
        Write-Host "[record_demo_tour] segment $i : $($mine.Count) subtitles"
    }
    if ($parts.Count -gt 0) {
        Set-Content "demo_tour_concat.txt" (($parts | ForEach-Object { "file '$_'" }) -join "`n") -Encoding ASCII
        & $Ffmpeg -y -v error -f concat -safe 0 -i demo_tour_concat.txt -c copy $Out
        if (-not $BurnSubs) {
            $srtPath = [IO.Path]::ChangeExtension($Out, ".srt")
            Set-Content -Path $srtPath -Value ($srt -join "`n") -Encoding UTF8
            Write-Host "[record_demo_tour] subtitles: $srtPath ($srtIdx cues)"
        }
    }
} finally { Pop-Location }
if ($lowFps.Count -gt 0) { Write-Warning ("capture below 95 % of {0} fps (frames were dropped while recording, the video repeats frames there): {1}" -f $Fps, ($lowFps -join "; ")) }
Write-Host "[record_demo_tour] done: $Out"
Select-String -Path $stdout -Pattern '\[DemoTour\] DONE|\[Error\]' | ForEach-Object { $_.Line }
