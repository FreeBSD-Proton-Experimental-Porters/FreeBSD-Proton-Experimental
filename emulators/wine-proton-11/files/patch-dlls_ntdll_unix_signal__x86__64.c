--- dlls/ntdll/unix/signal_x86_64.c.orig	2026-04-15 08:45:36.000000000 -0700
+++ dlls/ntdll/unix/signal_x86_64.c	2026-04-17 09:04:42.821353000 -0700
@@ -2085,6 +2087,7 @@ static int sc_seccomp(unsigned int operation, unsigned
 }
 #endif
 
+#ifdef HAVE_SECCOMP
 static void check_bpf_jit_enable(void)
 {
     char enabled;
@@ -2110,6 +2113,7 @@ static void check_bpf_jit_enable(void)
     }
     close(fd);
 }
+#endif /* HAVE_SECCOMP */
 
 static void install_bpf(struct sigaction *sig_act)
 {
@@ -2279,7 +2283,7 @@ __ASM_GLOBAL_FUNC( dump_syscall_fault_return,
                    "movq %rdi,%rsp\n\t"
                    "movq %rsi,%rax\n\t"
                    "movq %rdx,%r13\n\t"
-                   "jmp %rcx")
+                   "jmp *%rcx")
 
 
 static void dump_syscall_fault( CONTEXT *context, DWORD exc_code )
@@ -3197,7 +3201,11 @@ void set_thread_teb( TEB *teb )
 
 void set_thread_teb( TEB *teb )
 {
+#if defined __linux__
     arch_prctl( ARCH_SET_GS, teb );
+#elif defined (__FreeBSD__)
+    amd64_set_gsbase( teb );
+#endif
 }
 
 /***********************************************************************
