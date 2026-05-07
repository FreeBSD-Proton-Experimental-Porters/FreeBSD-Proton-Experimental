--- server/ptrace.c.orig	2026-04-24 08:28:08.000000000 -0700
+++ server/ptrace.c	2026-05-02 23:08:02.213278000 -0700
@@ -304,6 +304,32 @@ static int read_thread_long( struct thread *thread, vo
 /* read a long from a thread address space */
 static int read_thread_long( struct thread *thread, void *addr, unsigned long *data )
 {
+#if defined(__FreeBSD__) && defined(__x86_64__)
+    int lo, hi;
+
+    errno = 0;
+
+    lo = ptrace(PTRACE_PEEKDATA, get_ptrace_pid(thread), (caddr_t)addr, 0);
+    if (lo == -1 && errno)
+    {
+      *data = -1;
+      file_set_error();
+      return -1;
+    }
+
+    hi = ptrace(PTRACE_PEEKDATA, get_ptrace_pid(thread), (caddr_t)addr + 4, 0);
+    if (hi == -1 && errno)
+    {
+      *data = -1;
+      file_set_error();
+      return -1;
+    }
+
+    *data = hi;
+    *data = (*data << 32) + lo;
+
+    return 0;
+#else
     errno = 0;
     *data = ptrace( PTRACE_PEEKDATA, get_ptrace_pid(thread), (caddr_t)addr, 0 );
     if ( *data == -1 && errno)
@@ -312,6 +338,7 @@ static int read_thread_long( struct thread *thread, vo
         return -1;
     }
     return 0;
+#endif
 }
 
 /* write a long to a thread address space */
@@ -325,8 +352,16 @@ static long write_thread_long( struct thread *thread, 
         if (read_thread_long( thread, addr, &old_data ) == -1) return -1;
         data = (data & mask) | (old_data & ~mask);
     }
+#if defined(__FreeBSD__) && defined(__x86_64__)
+    if ((res = ptrace(PTRACE_POKEDATA, get_ptrace_pid(thread), (caddr_t)addr,     data))       == -1 ||
+        (res = ptrace(PTRACE_POKEDATA, get_ptrace_pid(thread), (caddr_t)addr + 4, data >> 32)) == -1)
+    {
+        file_set_error();
+    }
+#else
     if ((res = ptrace( PTRACE_POKEDATA, get_ptrace_pid(thread), (caddr_t)addr, data )) == -1)
         file_set_error();
+#endif
     return res;
 }
 
@@ -485,7 +520,22 @@ int read_process_memory( struct process *process, clie
         return 0;
     }
 
-    if ((ret = read_process_memory_vm( thread, ptr, size, dest )) != -1) return ret;
+    /* Perform the initial VM-based memory read */
+    ret = read_process_memory_vm( thread, ptr, size, dest );
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+    /* On FreeBSD, /proc/pid/mem throws ENOMEM on unmapped gaps. 
+       Map this to a partial copy so Windows DRM scanners don't panic. */
+    if (ret == -1 && errno == ENOMEM)
+    {
+        set_error( STATUS_PARTIAL_COPY );
+        resume_after_ptrace( thread );
+        return 0;
+    }
+#endif
+
+    /* Return result if successful, otherwise fall back to ptrace */
+    if (ret != -1) return ret;
     return read_process_memory_ptrace( thread, ptr, size, dest );
 }
 
