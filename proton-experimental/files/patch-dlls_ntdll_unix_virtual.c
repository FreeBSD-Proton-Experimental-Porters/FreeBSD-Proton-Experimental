--- dlls/ntdll/unix/virtual.c.orig	2025-04-02 09:17:16.000000000 -0700
+++ dlls/ntdll/unix/virtual.c	2026-01-04 17:16:24.910270000 -0800
@@ -70,9 +70,11 @@
 # include <mach/mach_vm.h>
 #endif
 
+#ifdef __linux__
 #include <linux/userfaultfd.h>
 #include <sys/ioctl.h>
 #include "uffd_tmp_defs.h"
+#endif
 
 #include <sys/uio.h>
 
@@ -156,11 +158,11 @@ static void r_debug_add_module( void *module, int fd, 
 
     r_debug_set_state( RT_ADD );
 
-    if (ptr) entry->map.l_addr = offset;
+    if (ptr) entry->map.l_addr = (caddr_t)offset;
     else if ((entry = calloc( 1, sizeof(*entry) )))
     {
         entry->module = module;
-        entry->map.l_addr = offset;
+	 entry->map.l_addr = (caddr_t)offset;
         entry->map.l_name = r_debug_path_from_fd( fd );
 
         entry->map.l_next = link_map.l_next;
@@ -379,6 +381,7 @@ void *anon_mmap_alloc( size_t size, int prot )
     return mmap( NULL, size, prot, MAP_PRIVATE | MAP_ANON, -1, 0 );
 }
 
+#ifdef __linux__
 static void kernel_writewatch_softdirty_init(void)
 {
     if ((pagemap_reset_fd = open( "/proc/self/pagemap_reset", O_RDONLY | O_CLOEXEC )) == -1) return;
@@ -615,8 +618,28 @@ static NTSTATUS kernel_get_write_watches( void *base, 
     }
     return STATUS_SUCCESS;
 }
+#else
+static void kernel_writewatch_init(void)
+{
+    use_kernel_writewatch = 0;
+}
 
+static void kernel_writewatch_register_range(struct file_view*, void*, size_t)
+{
+    /* do nothing */
+}
 
+static void kernel_writewatch_reset(void*, SIZE_T)
+{
+    abort();
+}
+
+static NTSTATUS kernel_get_write_watches(void*, SIZE_T, void**, ULONG_PTR*, BOOL)
+{
+    abort();
+}
+#endif
+
 static void mmap_add_reserved_area( void *addr, SIZE_T size )
 {
     struct reserved_area *area;
