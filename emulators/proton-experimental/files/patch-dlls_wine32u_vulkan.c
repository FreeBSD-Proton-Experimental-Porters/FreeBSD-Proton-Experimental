--- dlls/win32u/vulkan.c.orig	2026-09-19 01:11:08.000000000 -0700
+++ dlls/win32u/vulkan.c	2026-09-24 09:02:06.642829000 -0700
@@ -29,6 +29,9 @@
 #include <pthread.h>
 #include <unistd.h>
 #include <assert.h>
+#ifdef __FreeBSD__
+#include <pthread_np.h>
+#endif
 
 #include "ntstatus.h"
 #define WIN32_NO_STATUS
@@ -646,7 +649,13 @@ static void *signaller_worker( void *arg )
 
 static void *signaller_worker( void *arg )
 {
+#if defined(__linux__)
     int unix_tid = gettid();
+#elif defined(__FreeBSD__)
+    int unix_tid = pthread_getthreadid_np();
+#else
+    int unix_tid = 0;
+#endif
     struct vulkan_device *device = arg;
     struct semaphore *sem;
     VkSemaphoreWaitInfo wait_info = { 0 };
