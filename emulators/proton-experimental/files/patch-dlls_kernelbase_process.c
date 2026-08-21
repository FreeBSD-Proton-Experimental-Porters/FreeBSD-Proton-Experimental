--- dlls/kernelbase/process.c.orig	2026-08-13 21:00:11.000000000 -0700
+++ dlls/kernelbase/process.c	2026-08-20 17:52:06.579897000 -0700
@@ -599,7 +599,7 @@ static const WCHAR *hack_append_command_line( const WC
     }
     options[] =
     {
-#if defined(__aarch64__) || defined(__arm64ec__)
+#if defined(__aarch64__) || defined(__arm64ec__) || defined(__x86_64__) || defined(__amd64__)
         {L"msedgewebview2.exe", L" --no-sandbox"},
 #endif
         {L"Elisa The Innkeeper - Prequel\\nw.exe", L" --disable_direct_composition=1"},
