--- dlls/ntdll/unix/thread.c.orig	2026-03-11 17:46:31.000000000 -0700
+++ dlls/ntdll/unix/thread.c	2026-03-15 15:38:21.309004000 -0700
@@ -1149,6 +1149,17 @@
     thread_data->pthread_id = pthread_self();
     pthread_setspecific( teb_key, teb );
     server_init_thread( thread_data->start, &suspend );
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+    /* Dynamically calculate the size of the dispatcher thunk */
+    size_t dispatcher_len = (const char *)&__wine_syscall_dispatcher_return - (const char *)&__wine_syscall_dispatcher;
+    
+    /* Initialize to ALLOW (0) because we are currently executing Wine Unix code */
+    thread_data->syscall_dispatch = 0; /* SYSCALL_DISPATCH_FILTER_ALLOW */
+    
+    set_thread_syscall_dispatcher(&__wine_syscall_dispatcher, dispatcher_len, (BYTE *)&thread_data->syscall_dispatch);
+#endif
+
     signal_start_thread( thread_data->start, thread_data->param, suspend, teb );
 }
 
