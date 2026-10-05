--- dlls/ntdll/unix/signal_x86_64.c.orig	2026-10-05 01:24:23.674656000 -0700
+++ dlls/ntdll/unix/signal_x86_64.c	2026-10-05 01:24:23.682799000 -0700
@@ -2217,6 +2217,7 @@ static int sc_seccomp(unsigned int operation, unsigned
 }
 #endif
 
+#ifdef HAVE_SECCOMP
 static void check_bpf_jit_enable(void)
 {
     char enabled;
@@ -2242,6 +2243,7 @@ static void check_bpf_jit_enable(void)
     }
     close(fd);
 }
+#endif /* HAVE_SECCOMP */
 
 static void install_bpf(struct sigaction *sig_act)
 {
@@ -3520,7 +3522,11 @@ void set_thread_teb( TEB *teb )
 
 void set_thread_teb( TEB *teb )
 {
+#if defined __linux__
     arch_prctl( ARCH_SET_GS, teb );
+#elif defined (__FreeBSD__)
+    amd64_set_gsbase( teb );
+#endif
 }
 
 /***********************************************************************
