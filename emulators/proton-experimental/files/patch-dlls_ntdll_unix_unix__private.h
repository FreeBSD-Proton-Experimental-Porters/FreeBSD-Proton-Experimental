--- dlls/ntdll/unix/unix_private.h.orig	2026-03-11 17:46:31.000000000 -0700
+++ dlls/ntdll/unix/unix_private.h	2026-03-15 09:55:27.719046000 -0700
@@ -114,6 +114,7 @@
     PRTL_THREAD_START_ROUTINE start;  /* thread entry point */
     void              *param;         /* thread entry point parameter */
     void              *jmp_buf;       /* setjmp buffer for exception handling */
+    BYTE               syscall_dispatch; /* SUD selector byte */
 };
 
 C_ASSERT( sizeof(struct ntdll_thread_data) <= sizeof(((TEB *)0)->GdiTebBatch) );
@@ -283,6 +284,11 @@
                                              data_size_t *ret_len );
 extern NTSTATUS system_time_precise( void *args );
 extern void get_random( void *buf, ULONG len );
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+extern NTSTATUS set_thread_syscall_dispatcher(void *start, size_t len, BYTE *selector);
+extern NTSTATUS clear_thread_syscall_dispatcher(void);
+#endif
 
 extern void *steamclient_handle_fault( LPCVOID addr, DWORD err );
 extern void *anon_mmap_fixed( void *start, size_t size, int prot, int flags );
