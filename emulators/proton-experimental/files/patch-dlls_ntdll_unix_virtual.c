--- dlls/ntdll/unix/virtual.c.orig	2026-04-15 08:45:36.000000000 -0700
+++ dlls/ntdll/unix/virtual.c	2026-04-20 23:43:50.594377000 -0700
@@ -554,14 +554,17 @@ static void kernel_writewatch_init(void)
 #else
 static void kernel_writewatch_init(void)
 {
+    use_kernel_writewatch = 0;
 }
 
 static void kernel_writewatch_reset( void *start, SIZE_T len )
 {
+    abort();
 }
 
 static void kernel_writewatch_register_range( struct file_view *view, void *base, size_t size )
 {
+    /* do nothing */
 }
 
 static void kernel_get_write_watches( void *base, SIZE_T size, void **buffer, ULONG_PTR *count, BOOL reset )
@@ -2445,6 +2448,12 @@ failed:
     {
         ERR( "out of memory for %p-%p\n", base, (char *)base + size );
         status = STATUS_NO_MEMORY;
+        if ((uintptr_t)base == 0x400000)
+        {
+            char buf[100];
+            snprintf(buf, sizeof(buf), "procstat -v %d", getpid());
+            system(buf);
+        }
     }
     else if (errno == EEXIST) status = STATUS_CONFLICTING_ADDRESSES;
     else
