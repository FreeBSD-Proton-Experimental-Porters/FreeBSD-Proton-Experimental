--- dlls/ntdll/unix/system.c.orig	2026-03-15 11:34:34.635336000 -0700
+++ dlls/ntdll/unix/system.c	2026-03-15 14:40:25.542936000 -0700
@@ -43,6 +43,26 @@
 #ifdef HAVE_SYS_SYSCTL_H
 # include <sys/sysctl.h>
 #endif
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+# include <sys/procctl.h>
+
+/* Fallback definitions for FreeBSD Syscall User Dispatch (SUD) */
+#ifndef PROC_SUD_SET
+#define PROC_SUD_SET   25
+#define PROC_SUD_CLEAR 26
+
+#define SYSCALL_DISPATCH_FILTER_ALLOW 0
+#define SYSCALL_DISPATCH_FILTER_BLOCK 1
+
+struct proc_sud_ctl {
+    uintptr_t sud_start;
+    size_t    sud_len;
+    uint8_t  *sud_selector;
+};
+#endif
+#endif
+
 #ifdef HAVE_SYS_UTSNAME_H
 # include <sys/utsname.h>
 #endif
@@ -2170,8 +2190,66 @@
     struct smbios_boot_info boot = { .hdr.type = SMBIOS_TYPE_BOOTINFO, .hdr.length = sizeof(boot) };
 
     return append_smbios( buf, &boot.hdr, NULL, 0 );
+}
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+
+/***********************************************************************
+ * set_thread_syscall_dispatcher
+ *
+ * Registers the syscall dispatcher allow-range for the calling thread.
+ */
+NTSTATUS set_thread_syscall_dispatcher(void *dispatcher_start, size_t dispatcher_len, uint8_t *selector)
+{
+    struct proc_sud_ctl ctl;
+
+    if (!dispatcher_start || !dispatcher_len || !selector)
+        return STATUS_INVALID_PARAMETER;
+
+    ctl.sud_start = (uintptr_t)dispatcher_start;
+    ctl.sud_len = dispatcher_len;
+    ctl.sud_selector = selector;
+
+    /* * P_PID with getpid() resolves to td->td_proc in the kernel.
+     * The kernel patch applies this exclusively to the calling thread.
+     */
+    if (procctl(P_PID, getpid(), PROC_SUD_SET, &ctl) == -1)
+    {
+        switch (errno)
+        {
+        case ENOMEM:
+            return STATUS_NO_MEMORY;
+        case EINVAL:
+            return STATUS_INVALID_PARAMETER;
+        case EPERM:
+            return STATUS_ACCESS_DENIED;
+        default:
+            return STATUS_NOT_SUPPORTED;
+        }
+    }
+
+    return STATUS_SUCCESS;
+}
+
+/***********************************************************************
+ * clear_thread_syscall_dispatcher
+ *
+ * Disables SUD for the calling thread.
+ */
+NTSTATUS clear_thread_syscall_dispatcher(void)
+{
+    if (procctl(P_PID, getpid(), PROC_SUD_CLEAR, NULL) == -1)
+    {
+        if (errno == EPERM)
+            return STATUS_ACCESS_DENIED;
+        return STATUS_INVALID_PARAMETER;
+    }
+
+    return STATUS_SUCCESS;
 }
 
+#endif
+
 #ifdef __aarch64__
 #ifdef linux
 
