--- include/wine/asm.h.orig	2024-02-24 10:23:40.551780000 +0300
+++ include/wine/asm.h	2024-02-24 10:23:43.285931000 +0300
@@ -242,9 +242,9 @@
                        ".byte 0x75,0x03\n\t"      /* jne 1f */ \
                        ".byte 0x0f,0x05\n\t"      /* syscall */ \
                        ".byte 0xc3\n\t"           /* ret */ \
-                       "jmp 1f\n\t" \
+                       ".byte 0xeb,0x01\n\t"      /* jmp 1f */ \
                        ".byte 0xc3\n"             /* ret */ \
-                       "1:\t.byte 0xff,0x14,0x25\n\t" /* 1: callq *(0x7ffe1000) */ \
+                       ".byte 0xff,0x14,0x25\n\t" /* 1: callq *(0x7ffe1000) */ \
                        ".long 0x7ffe1000\n\t" \
                        "ret" )
 # else
