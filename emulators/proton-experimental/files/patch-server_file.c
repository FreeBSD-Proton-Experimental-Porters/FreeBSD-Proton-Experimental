--- server/file.c
+++ server/file.c
@@ -548,6 +548,11 @@
     case ELOOP:     set_error( STATUS_REPARSE_POINT_NOT_RESOLVED ); break;
 #ifdef EOVERFLOW
     case EOVERFLOW: set_error( STATUS_INVALID_PARAMETER ); break;
 #endif
+    case ENOMEM:
+        /* On FreeBSD, /proc/pid/mem throws ENOMEM on unmapped gaps. 
+           Map this to a partial copy so Windows DRM scanners don't panic. */
+        set_error( STATUS_PARTIAL_COPY );
+        break;
     default:
         perror("wineserver: file_set_error() can't map error");
         set_error( STATUS_UNSUCCESSFUL );