--- server/file.c.orig	2026-03-20 13:33:36.000000000 -0700
+++ server/file.c	2026-03-28 04:48:25.934004000 -0700
@@ -620,6 +620,13 @@ void file_set_error(void)
 #ifdef EOVERFLOW
     case EOVERFLOW: set_error( STATUS_INVALID_PARAMETER ); break;
 #endif
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+    case ENOMEM:
+        /* On FreeBSD, /proc/pid/mem throws ENOMEM on unmapped gaps. 
+           Map this to a partial copy so Windows DRM scanners don't panic. */
+        set_error( STATUS_PARTIAL_COPY );
+        break;
+#endif
     default:
         perror("wineserver: file_set_error() can't map error");
         set_error( STATUS_UNSUCCESSFUL );
