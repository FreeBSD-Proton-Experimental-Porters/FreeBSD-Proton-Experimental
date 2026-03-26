--- dlls/ntdll/unix/loader.c.orig	2026-03-15 11:34:34.631291000 -0700
+++ dlls/ntdll/unix/loader.c	2026-03-15 15:38:00.420648000 -0700
@@ -2434,6 +2434,21 @@
     load_ntdll();
     load_wow64_ntdll( main_image_info.Machine );
     load_apiset_dll();
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+    /* The kernel SUD implementation is self-only; the main thread must register itself
+     * before transitioning into the NT environment. */
+    struct ntdll_thread_data *thread_data = (struct ntdll_thread_data *)&teb->GdiTebBatch;
+
+    /* Dynamically calculate the size of the dispatcher thunk */
+    size_t dispatcher_len = (const char *)&__wine_syscall_dispatcher_return - (const char *)&__wine_syscall_dispatcher;
+
+    /* Initialize to ALLOW (0) because we are currently executing Wine Unix code */
+    thread_data->syscall_dispatch = 0; /* SYSCALL_DISPATCH_FILTER_ALLOW */
+    
+    set_thread_syscall_dispatcher(&__wine_syscall_dispatcher, dispatcher_len, (BYTE *)&thread_data->syscall_dispatch);
+#endif
+
     server_init_process_done();
 }
 
