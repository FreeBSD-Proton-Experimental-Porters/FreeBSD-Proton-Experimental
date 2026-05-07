--- dlls/ntdll/unix/loader.c.orig	2026-04-24 08:28:08.000000000 -0700
+++ dlls/ntdll/unix/loader.c	2026-05-02 22:42:46.555312000 -0700
@@ -524,7 +524,24 @@ char *get_alternate_wineloader( WORD machine )
     const char *arch;
     BOOL force_wow64 = (arch = getenv( "WINEARCH" )) && !strcmp( arch, "wow64" );
     char *ret = NULL;
+
+    if (machine == current_machine) return NULL;
 
+    char* wineserver_path = getenv("WINESERVER");
+    if (wineserver_path != NULL && wineserver_path[0] == '/') {
+        char* s = remove_tail(wineserver_path, "server");
+        if (s != NULL) {
+            if (machine == IMAGE_FILE_MACHINE_AMD64) {
+                ret = malloc(strlen(s) + 3);
+                strcpy(ret, s);
+                strcat(ret, "64");
+            } else {
+                ret = s;
+            }
+            return ret;
+        }
+    }
+
     if (is_win64)
     {
         if (force_wow64) return NULL;
@@ -2450,7 +2467,9 @@ static void start_main_thread(void)
     set_thread_teb( teb );
 #endif
 
+#ifdef M_PERTURB
     mallopt( M_PERTURB, 0xff );
+#endif
     init_startup_info();
     *(ULONG_PTR *)&peb->CloudFileFlags = get_image_address();
     set_load_order_app_name( main_wargv[0] );
@@ -2459,7 +2478,25 @@ static void start_main_thread(void)
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
+#ifdef M_PERTURB
     mallopt( M_PERTURB, 0 );
+#endif
+
     server_init_process_done();
 }
 
