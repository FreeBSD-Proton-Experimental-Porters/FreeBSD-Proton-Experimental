--- dlls/ntdll/unix/signal_x86_64.c.orig	2026-10-30 22:34:26.028963000 -0700
+++ dlls/ntdll/unix/signal_x86_64.c	2026-10-30 22:45:24.885144000 -0700
@@ -3410,7 +3410,11 @@ void set_thread_teb( TEB *teb )
 
 void set_thread_teb( TEB *teb )
 {
+#if defined __linux__
     arch_prctl( ARCH_SET_GS, teb );
+#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+    amd64_set_gsbase( teb );
+#endif
 }
 
 /***********************************************************************
