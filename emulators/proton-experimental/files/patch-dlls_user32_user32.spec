--- dlls/user32/user32.spec.orig	2025-06-27 14:21:15.000000000 -0700
+++ dlls/user32/user32.spec	2025-07-25 15:11:42.030956000 -0700
@@ -1074,7 +1074,7 @@
 @ stdcall SetProcessDefaultLayout(long) NtUserSetProcessDefaultLayout
 @ stdcall SetProcessDpiAwarenessContext(long)
 @ stdcall SetProcessDpiAwarenessInternal(long)
-@ stdcall SetProcessLaunchForegroundPolicy(long long)
+# @ stub SetProcessLaunchForegroundPolicy
 # @ stub SetProcessRestrictionExemption
 @ stdcall SetProcessWindowStation(long) NtUserSetProcessWindowStation
 @ stdcall SetProgmanWindow(long) NtUserSetProgmanWindow
