--- dlls/ntdll/unix/unix_private.h.orig	2026-04-24 08:28:08.000000000 -0700
+++ dlls/ntdll/unix/unix_private.h	2026-05-02 22:52:11.482219000 -0700
@@ -117,6 +117,7 @@ struct ntdll_thread_data
     void                     *param;         /* thread entry point parameter */
     void                     *jmp_buf;       /* setjmp buffer for exception handling */
     int                      *fsync_apc_futex;
+    BYTE               syscall_dispatch; /* SUD selector byte */
 };
 
 C_ASSERT( sizeof(struct ntdll_thread_data) <= sizeof(((TEB *)0)->GdiTebBatch) );
@@ -290,6 +291,11 @@ extern NTSTATUS system_time_precise( void *args );
 extern unsigned int alloc_object_attributes( const OBJECT_ATTRIBUTES *attr, struct object_attributes **ret,
                                              data_size_t *ret_len );
 extern NTSTATUS system_time_precise( void *args );
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+extern NTSTATUS set_thread_syscall_dispatcher(void *start, size_t len, BYTE *selector);
+extern NTSTATUS clear_thread_syscall_dispatcher(void);
+#endif
 
 extern void *steamclient_handle_fault( LPCVOID addr, DWORD err );
 extern void *anon_mmap_fixed( void *start, size_t size, int prot, int flags );
