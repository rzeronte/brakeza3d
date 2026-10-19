# GUI probe: launches the game (as a child, so it may take the foreground) on the tour and executes the commands dropped, one per line, in cmd.txt next to this script;
# results / screenshots (client area of the GAME window only, never the desktop) land next to it too. Commands: focus, key VK [rctrl], click fx fy, move fx fy, drag x0 y0 x1 y1,
# wheel fx fy notches, wait s, ack N, pos, shot name, quit. Used to measure the editor GUI before writing demo_tour plans. Write cmd.txt atomically (cmd.tmp + rename).
$S = Split-Path -Parent $MyInvocation.MyCommand.Path
Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Windows.Forms
Add-Type -TypeDefinition @"
using System; using System.Runtime.InteropServices;
public static class P {
 [DllImport("user32.dll")] public static extern bool SetProcessDPIAware();
 [DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr h);
 [DllImport("user32.dll")] public static extern bool ShowWindow(IntPtr h,int c);
 [DllImport("user32.dll")] public static extern bool GetClientRect(IntPtr h,out RECT r);
 [DllImport("user32.dll")] public static extern bool ClientToScreen(IntPtr h,ref POINT p);
 [DllImport("user32.dll")] public static extern bool GetCursorPos(out POINT p);
 [DllImport("user32.dll")] public static extern bool SetCursorPos(int x,int y);
 [DllImport("user32.dll")] public static extern void mouse_event(uint f,int dx,int dy,int d,UIntPtr e);
 [DllImport("user32.dll")] public static extern void keybd_event(byte vk,byte sc,uint f,UIntPtr e);
 [DllImport("user32.dll")] public static extern uint MapVirtualKey(uint c,uint t);
 [DllImport("user32.dll")] public static extern IntPtr GetForegroundWindow();
 [StructLayout(LayoutKind.Sequential)] public struct RECT{public int L,T,R,B;}
 [StructLayout(LayoutKind.Sequential)] public struct POINT{public int X,Y;}
}
"@
[P]::SetProcessDPIAware() | Out-Null
$repo = "C:\Users\darkh\CLionProjects\brakeza3d"
$exeDir = "$repo\cmake-build-release-mingw-brakezabundle"
$l = Select-String -Path "$exeDir\CMakeCache.txt" -Pattern '^CMAKE_MAKE_PROGRAM:FILEPATH=(.+)$' | Select-Object -First 1
$env:PATH = (Split-Path -Parent $l.Matches[0].Groups[1].Value) + ";" + $env:PATH
Set-Content "$repo\logs\demo_tour_ack.txt" "0"
$args2 = @("-p","TutorialsProject.json","--set","demoBrowserTour=true","--set","demoTourRecord=true","--set","demoTourTourOnly=true","--log-console","--exit-after","1500") + @($Extra -split ' ' | Where-Object { $_ })
$proc = Start-Process "$exeDir\Brakeza3D.exe" -WorkingDirectory $exeDir -PassThru -NoNewWindow -ArgumentList $args2 -RedirectStandardOutput "$repo\logs\probe_out.txt" -RedirectStandardError "$repo\logs\probe_err.txt"
function Win { $proc.Refresh(); $h = $proc.MainWindowHandle; $r = New-Object P+RECT; [P]::GetClientRect($h,[ref]$r)|Out-Null; $pt = New-Object P+POINT; [P]::ClientToScreen($h,[ref]$pt)|Out-Null; @{h=$h;x=$pt.X;y=$pt.Y;w=$r.R-$r.L;hh=$r.B-$r.T} }
function Focus { $w = Win; if ([P]::GetForegroundWindow() -ne $w.h) { [P]::keybd_event(0x12,0,0,[UIntPtr]::Zero); [P]::keybd_event(0x12,0,2,[UIntPtr]::Zero) }; [P]::SetForegroundWindow($w.h)|Out-Null; Start-Sleep -Milliseconds 300 }
function Key($vk,$ctrl) { $sc = { param($v) [byte][P]::MapVirtualKey($v,0) }
  if ($ctrl) { [P]::keybd_event(0xA3,(& $sc 0xA3),1,[UIntPtr]::Zero); Start-Sleep -Milliseconds 140 }
  [P]::keybd_event([byte]$vk,(& $sc $vk),0,[UIntPtr]::Zero); Start-Sleep -Milliseconds 90; [P]::keybd_event([byte]$vk,(& $sc $vk),2,[UIntPtr]::Zero)
  if ($ctrl) { [P]::keybd_event(0xA3,(& $sc 0xA3),3,[UIntPtr]::Zero) } }
function MoveTo($fx,$fy) { $w = Win; $x=[int]($w.x+$fx*$w.w); $y=[int]($w.y+$fy*$w.hh)
  $cp = New-Object P+POINT; [P]::GetCursorPos([ref]$cp)|Out-Null; $x0=$cp.X; $y0=$cp.Y; $n=25
  foreach ($k in 1..$n) { $f=$k/$n; $f=$f*$f*(3-2*$f); [P]::SetCursorPos([int]($x0+($x-$x0)*$f),[int]($y0+($y-$y0)*$f))|Out-Null; Start-Sleep -Milliseconds 20 }
  Start-Sleep -Milliseconds 300 }
while (-not $proc.HasExited) {
  Start-Sleep -Milliseconds 150
  if (-not (Test-Path "$S\cmd.txt")) { continue }
  $lines = Get-Content "$S\cmd.txt"; Remove-Item "$S\cmd.txt" -Force
  $res = @()
  foreach ($ln in $lines) {
    $a = $ln.Trim() -split '\s+'
    switch ($a[0]) {
      "focus" { Focus; $res += "fg=" + ([P]::GetForegroundWindow() -eq (Win).h) }
      "key"   { Focus; Key ([int]$a[1]) ($a[2] -eq "rctrl") }
      "move"  { MoveTo ([double]$a[1]) ([double]$a[2]) }
      "click" { Focus; MoveTo ([double]$a[1]) ([double]$a[2]); Start-Sleep -Milliseconds 700; [P]::mouse_event(2,0,0,0,[UIntPtr]::Zero); Start-Sleep -Milliseconds 90; [P]::mouse_event(4,0,0,0,[UIntPtr]::Zero) }
      "drag"  { Focus; MoveTo ([double]$a[1]) ([double]$a[2]); Start-Sleep -Milliseconds 500; [P]::mouse_event(2,0,0,0,[UIntPtr]::Zero); Start-Sleep -Milliseconds 150; MoveTo ([double]$a[3]) ([double]$a[4]); Start-Sleep -Milliseconds 300; [P]::mouse_event(4,0,0,0,[UIntPtr]::Zero) }
      "mdown" { [P]::mouse_event(2,0,0,0,[UIntPtr]::Zero) }
      "mup"   { [P]::mouse_event(4,0,0,0,[UIntPtr]::Zero) }
      "wheel" { Focus; MoveTo ([double]$a[1]) ([double]$a[2]); Start-Sleep -Milliseconds 300; [P]::mouse_event(0x800,0,0,[int]$a[3]*120,[UIntPtr]::Zero) }
      "wait"  { Start-Sleep -Milliseconds ([int]([double]$a[1]*1000)) }
      "ack"   { Set-Content "$repo\logs\demo_tour_ack.txt" $a[1] }
      "pos"   { $cp = New-Object P+POINT; [P]::GetCursorPos([ref]$cp)|Out-Null; $res += "pos $($cp.X),$($cp.Y)" }
      "shot"  { $w = Win; $bm = New-Object System.Drawing.Bitmap($w.w,$w.hh); $g = [System.Drawing.Graphics]::FromImage($bm); $g.CopyFromScreen($w.x,$w.y,0,0,$bm.Size); $g.Dispose(); $bm.Save("$S\$($a[1]).png",[System.Drawing.Imaging.ImageFormat]::Png); $bm.Dispose(); $res += "shot $($a[1]) $($w.w)x$($w.hh) fg=" + ([P]::GetForegroundWindow() -eq $w.h) }
      "quit"  { $proc.Kill() }
    }
  }
  Set-Content "$S\res.txt" ($res -join "`n")
}
