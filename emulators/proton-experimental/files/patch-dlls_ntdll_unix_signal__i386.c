--- dlls/ntdll/unix/signal_i386.c.orig	2026-03-23 15:08:59.000000000 -0700
+++ dlls/ntdll/unix/signal_i386.c	2026-04-02 14:38:26.397407000 -0700
@@ -213,9 +213,9 @@ static inline int set_thread_area( struct modify_ldt_s
 #define EIP_sig(context)     ((context)->uc_mcontext.mc_eip)
 #define ESP_sig(context)     ((context)->uc_mcontext.mc_esp)
 
-#define FPU_sig(context)     NULL  /* FIXME */
-#define FPUX_sig(context)    NULL  /* FIXME */
-#define XState_sig(context)  NULL  /* FIXME */
+#define FPU_sig(context)     NULL
+#define FPUX_sig(context)    ((XSAVE_FORMAT *)&(context)->uc_mcontext.mc_fpstate)
+#define XState_sig(context)  NULL
 
 #elif defined (__OpenBSD__)
 
@@ -860,7 +860,7 @@ static void fixup_frame_fpu_state( struct syscall_fram
     if (FPUX_sig(sigcontext))
     {
         if (user_shared_data->ProcessorFeatures[PF_XMMI_INSTRUCTIONS_AVAILABLE])
-            frame->u.xsave = *FPUX_sig(sigcontext);
+            frame->u.xsave = *((XSAVE_FORMAT *) FPUX_sig(sigcontext));
         else
             fpux_to_fpu( &frame->u.fsave, FPUX_sig(sigcontext) );
         frame->xstate.Mask = XSTATE_MASK_LEGACY;
