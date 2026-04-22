--- dlls/ntdll/unix/loader.c.orig	2026-04-15 08:45:36.000000000 -0700
+++ dlls/ntdll/unix/loader.c	2026-04-19 20:48:06.884367000 -0700
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
@@ -2459,7 +2478,9 @@ static void start_main_thread(void)
     load_ntdll();
     load_wow64_ntdll( main_image_info.Machine );
     load_apiset_dll();
+#ifdef M_PERTURB
     mallopt( M_PERTURB, 0 );
+#endif
     server_init_process_done();
 }
 
