--- dlls/ntdll/unix/signal_x86_64.c.orig	2026-03-20 12:14:04.734176000 -0700
+++ dlls/ntdll/unix/signal_x86_64.c	2026-03-20 19:22:02.980965000 -0700
@@ -151,10 +151,13 @@
 #define TRAP_sig(context)    ((context)->uc_mcontext.gregs[REG_TRAPNO])
 #define ERROR_sig(context)   ((context)->uc_mcontext.gregs[REG_ERR])
 #define FPU_sig(context)     ((XMM_SAVE_AREA32 *)((context)->uc_mcontext.fpregs))
-#define XState_sig(fpu)      (((unsigned int *)fpu->Reserved4)[12] == FP_XSTATE_MAGIC1 ? (XSAVE_AREA_HEADER *)(fpu + 1) : NULL)
+#define XState_sig(context)  (((unsigned int *)FPU_sig(context)->Reserved4)[12] == FP_XSTATE_MAGIC1 ? (XSAVE_AREA_HEADER *)(FPU_sig(context) + 1) : NULL)
 
 #elif defined(__FreeBSD__) || defined (__FreeBSD_kernel__)
 
+#include <machine/cpufunc.h>
+#include <machine/segments.h>
+#include <machine/specialreg.h>
 #include <machine/trap.h>
 
 #define RAX_sig(context)     ((context)->uc_mcontext.mc_rax)
@@ -184,7 +187,7 @@
 #define TRAP_sig(context)    ((context)->uc_mcontext.mc_trapno)
 #define ERROR_sig(context)   ((context)->uc_mcontext.mc_err)
 #define FPU_sig(context)     ((XMM_SAVE_AREA32 *)((context)->uc_mcontext.mc_fpstate))
-#define XState_sig(context)  NULL
+#define XState_sig(context)  ((context)->uc_mcontext.mc_xfpustate ? (XSAVE_AREA_HEADER *)((XMM_SAVE_AREA32 *)(uintptr_t)(context)->uc_mcontext.mc_xfpustate + 1) : NULL)
 
 #elif defined(__NetBSD__)
 
@@ -474,7 +477,7 @@
     return (struct amd64_thread_data *)ntdll_get_thread_data()->cpu_data;
 }
 
-#ifdef __linux__
+#if defined(__linux__) || defined(__FreeBSD__)
 static inline TEB *get_current_teb(void)
 {
     unsigned long rsp;
@@ -933,7 +936,7 @@
         context->ContextFlags |= CONTEXT_FLOATING_POINT;
         context->FltSave = *FPU_sig(sigcontext);
         context->MxCsr = context->FltSave.MxCsr;
-        if (xstate_extended_features() && (xs = XState_sig(FPU_sig(sigcontext))))
+        if (xstate_extended_features() && (xs = XState_sig(sigcontext)))
         {
             /* xcontext and sigcontext are both on the signal stack, so we can
              * just reference sigcontext without overflowing 32 bit XState.Offset */
@@ -1686,12 +1689,15 @@
                    "movq %rsp,0x328(%r8)\n\t"  /* amd64_thread_data()->syscall_frame */
                    /* switch to user stack */
                    "movq %rdi,%rsp\n\t"        /* user_rsp */
-#ifdef __linux__
+#if defined(__linux__) || defined(__FreeBSD__)
                    "testl $12,%r14d\n\t"       /* SYSCALL_HAVE_PTHREAD_TEB | SYSCALL_HAVE_WRFSGSBASE */
                    "jz 1f\n\t"
                    "movw 0x338(%r8),%fs\n"     /* amd64_thread_data()->fs */
                    "1:\n\t"
 #endif
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+                   "movb $1,%gs:0x3d0\n\t"       /* SUD: BLOCK (1) Windows syscalls */
+#endif
                    "movq 0x348(%r8),%r10\n\t"    /* amd64_thread_data()->instrumentation_callback */
                    "movq (%r10),%r10\n\t"
                    "test %r10,%r10\n\t"
@@ -1706,6 +1712,9 @@
 extern void DECLSPEC_NORETURN user_mode_callback_return( void *ret_ptr, ULONG ret_len,
                                                          NTSTATUS status, TEB *teb );
 __ASM_GLOBAL_FUNC( user_mode_callback_return,
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+                   "movb $0,%gs:0x3d0\n\t"       /* SUD: ALLOW (0) native Unix syscalls */
+#endif
                    "movq 0x328(%rcx),%r10\n\t" /* amd64_thread_data()->syscall_frame */
                    "movq 0xa0(%r10),%r11\n\t"  /* frame->prev_frame */
                    "movq %r11,0x328(%rcx)\n\t" /* amd64_thread_data()->syscall_frame = prev_frame */
@@ -2667,8 +2676,190 @@
     EFL_sig(ucontext) &= ~0x100;  /* clear single-step flag */
     RIP_sig(ucontext) = (ULONG64)__wine_syscall_dispatcher_prolog_end_ptr;
 }
+#endif
+
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+#ifndef SIGSYS_DISPATCH
+#define SIGSYS_DISPATCH 1
+#endif
+#ifndef SYSCALL_DISPATCH_FILTER_ALLOW
+#define SYSCALL_DISPATCH_FILTER_ALLOW 0
+#define SYSCALL_DISPATCH_FILTER_BLOCK 1
 #endif
+
+/* Syscall Translation for FreeBSD SUD (Win10 22H2 - Build 19045) */
+static struct
+{
+    unsigned int win_syscall_nr;
+    unsigned int wine_syscall_nr;
+    void *function;
+}
+fbsd_syscall_nr_translation[] =
+{
+    /* Core Process & Thread Information */
+    {0x10, ~0u, NtQueryObject},
+    {0x19, ~0u, NtQueryInformationProcess},
+    {0x1c, ~0u, NtSetInformationProcess},
+    {0x25, ~0u, NtQueryInformationThread},
+    {0x0d, ~0u, NtSetInformationThread},
+    {0x36, ~0u, NtQuerySystemInformation},
+    {0x0f3, ~0u, NtGetContextThread},
+
+    /* Basic File & Memory I/O */
+    {0x33, ~0u, NtOpenFile},
+    {0x55, ~0u, NtCreateFile},
+    {0x4a, ~0u, NtCreateSection},
+    {0x28, ~0u, NtMapViewOfSection},
+    {0x2a, ~0u, NtUnmapViewOfSection},
+    {0x08, ~0u, NtWriteFile},
+    {0x06, ~0u, NtReadFile},
+    {0x0f, ~0u, NtClose},
+    {0x23, ~0u, NtQueryVirtualMemory},
+    {0x50, ~0u, NtProtectVirtualMemory},
+    {0x0a6, ~0u, NtCreateDebugObject},
+
+    /* Chromium IPC & Named Pipes Base */
+    {0x39, ~0u, NtFsControlFile},
+    {0x07, ~0u, NtDeviceIoControlFile},
+    {0x11, ~0u, NtQueryInformationFile},
+    {0x27, ~0u, NtSetInformationFile},
+    {0x04, ~0u, NtWaitForSingleObject},
+    {0x3c, ~0u, NtDuplicateObject},
+    /* NtWaitForMultipleObjects: 0x5b verified. NtQuerySystemTime stub starts with
+     * JMP (E9); its 0x5b pattern is dead code. Real owner is NtWaitForMultipleObjects. */
+    {0x05b, ~0u, NtWaitForMultipleObjects},
+
+    /* Pipeline Creation and Asynchronous I/O Cancellation */
+    {0x0b5, ~0u, NtCreateNamedPipeFile},
+    {0x0b3, ~0u, NtCreateMailslotFile},
+    {0x05d, ~0u, NtCancelIoFile},
+    {0x092, ~0u, NtCancelIoFileEx},
+
+    /* Foolproofing for pipe path verification and thread sleeping */
+    {0x3d, ~0u, NtQueryAttributesFile},
+    {0x34, ~0u, NtDelayExecution},
+
+    /* Memory Manipulation (Denuvo / VM Unpacking) */
+    {0x18, ~0u, NtAllocateVirtualMemory},
+    {0x1e, ~0u, NtFreeVirtualMemory},
+    {0x3f, ~0u, NtReadVirtualMemory},
+    {0x3a, ~0u, NtWriteVirtualMemory},
+
+    /* Thread State Manipulation & Hardware Breakpoints */
+    {0x18d, ~0u, NtSetContextThread},
+    {0x52, ~0u, NtResumeThread},
+    {0x1be, ~0u, NtSuspendThread},
+
+    /* Execution Timing and Evasion */
+    {0x046, ~0u, NtYieldExecution},
+
+    /* Anti-Debug & Process Monitoring */
+    {0x26, ~0u, NtOpenProcess},
+    {0x12f, ~0u, NtOpenThread},
+    {0x2c, ~0u, NtTerminateProcess},
+    {0x0f9, ~0u, NtGetNextThread},
+    {0xc2, ~0u, NtCreateThreadEx},
+    {0x0c9, ~0u, NtCreateUserProcess},
+    {0x1b8, ~0u, NtSignalAndWaitForSingleObject},
+
+    /* Timing Attacks & System Info */
+    /* NtQuerySystemTime removed: stub is a JMP thunk to KUSER_SHARED_DATA;
+     * the 0x5b pattern found by the scanner is dead code after the JMP.
+     * Real owner of 0x5b is NtWaitForMultipleObjects (entry above). */
+    {0x31, ~0u, NtQueryPerformanceCounter},
+    {0x163, ~0u, NtQueryTimerResolution},
+    {0x49, ~0u, NtQueryVolumeInformationFile},
+
+    /* Code Integrity & Unpacking */
+    {0x0e9, ~0u, NtFlushInstructionCache},
+    {0x51, ~0u, NtQuerySection},
+
+    /* Registry & Environment Sniffing */
+    {0x12, ~0u, NtOpenKey},
+    {0x017, ~0u, NtQueryValueKey},
+
+    /* Advanced Hijacking / Process Hollowing */
+    {0x45, ~0u, NtQueueApcThread},
+    {0x06e, ~0u, NtAlertResumeThread},
+    {0x06f, ~0u, NtAlertThread},
+
+};
+
+static int fbsd_sud_translate_syscalls = 0;
+
+/**********************************************************************
+ *		sigsys_handler
+ *
+ * Handler for SIGSYS, intercepts FreeBSD SUD syscall traps.
+ */
+static void sigsys_handler( int signal, siginfo_t *siginfo, void *sigcontext )
+{
+    extern const void *__wine_syscall_dispatcher_prolog_end_ptr;
+    struct ntdll_thread_data *thread_data = (struct ntdll_thread_data *)&get_current_teb()->GdiTebBatch;
+    ucontext_t *ucontext;
+    struct syscall_frame *frame;
+    unsigned int i;
+
+    /* Must be first: clear the SUD selector so any syscall issued by
+     * init_handler() or other C runtime code within this handler cannot
+     * be re-intercepted by SUD.  The selector is BLOCK on entry because
+     * we were executing Windows PE code when SUD fired.  No restoration
+     * is needed: prolog_end sets ALLOW on Wine dispatch entry, and the
+     * dispatcher return path sets BLOCK when returning to Windows code. */
+    thread_data->syscall_dispatch = SYSCALL_DISPATCH_FILTER_ALLOW;
+
+    /* Restore the correct pthread fsbase. FreeBSD signal delivery
+     * restores the saved user-mode %%fs from the signal frame; when SUD
+     * fires from a Windows PE stub, the saved %%fs may be GUFS32_SEL
+     * (the LDT selector) rather than the pthread TLS base, causing any
+     * %%fs-relative C runtime access inside this handler to read garbage. */
+    amd64_set_fsbase(((struct amd64_thread_data *)thread_data->cpu_data)->pthread_teb);
+
+    ucontext = init_handler( sigcontext );
+    frame = amd64_thread_data()->syscall_frame;
+
+    if (siginfo->si_code == SIGSYS_DISPATCH)
+    {
+        ULONG64 syscall_nr = siginfo->si_syscall;
+
+        if (fbsd_sud_translate_syscalls)
+        {
+            for (i = 0; i < ARRAY_SIZE(fbsd_syscall_nr_translation); ++i)
+            {
+                if (syscall_nr == fbsd_syscall_nr_translation[i].win_syscall_nr)
+                {
+                    syscall_nr = fbsd_syscall_nr_translation[i].wine_syscall_nr;
+                    break;
+                }
+            }
+        }
+
+        RAX_sig(ucontext) = syscall_nr;
+
+	frame->rip = RIP_sig(ucontext) + 0xb;
+        frame->rcx = RIP_sig(ucontext);
+        frame->eflags = EFL_sig(ucontext);
+        frame->restore_flags = 0;
+        if (instrumentation_callback) frame->restore_flags |= RESTORE_FLAGS_INSTRUMENTATION;
+
+        /* FreeBSD's kernel moves R10 to RCX on syscall entry. Because SUD
+         * interrupts this, the first argument is stuck in RCX. Restore it! */
+        R10_sig(ucontext) = RCX_sig(ucontext);
+
+        /* Explicitly map and preserve the argument registers into the Wine frame
+         * to prevent bleeding during the context switch. */
+        frame->r10 = R10_sig(ucontext);
+        frame->r8  = R8_sig(ucontext);
+        frame->r9  = R9_sig(ucontext);
+        frame->rdx = RDX_sig(ucontext);
 
+        RCX_sig(ucontext) = (ULONG_PTR)frame;
+        R11_sig(ucontext) = frame->eflags;
+        EFL_sig(ucontext) &= ~0x100;  /* clear single-step flag */
+        RIP_sig(ucontext) = (ULONG64)__wine_syscall_dispatcher_prolog_end_ptr;
+    }
+}
+#endif
 
 /***********************************************************************
  *           LDT support
@@ -2731,6 +2922,16 @@
 
 #if defined(__APPLE__)
     if (i386_set_ldt(index, (union ldt_entry *)&entry, 1) < 0) perror("i386_set_ldt");
+#elif defined(__FreeBSD__)
+    struct i386_ldt_args p;
+    p.start = index;
+    p.descs = (struct user_segment_descriptor *)&entry;
+    p.num   = 1;
+    if (sysarch(I386_SET_LDT, &p) == -1)
+    {
+        perror("i386_set_ldt");
+        exit(1);
+    }
 #else
     fprintf( stderr, "No LDT support on this platform\n" );
     exit(1);
@@ -2879,7 +3080,47 @@
 }
 #endif
 
+#ifdef __FreeBSD__
+static __siginfohandler_t *libthr_signal_handlers[_SIG_MAXSIG];
 
+/* occasionally signals happen right between %fs reset to GUFS32_SEL and fsbase correction,
+which results in fsbase being wrong on handler entry; we'll just restore fsbase ourselves */
+static void libthr_sighandler_wrapper(int sig, siginfo_t *info, void *_ucp) {
+    struct ntdll_thread_data *thread_data = (struct ntdll_thread_data *)&get_current_teb()->GdiTebBatch;
+    BYTE old_sud = thread_data->syscall_dispatch;
+
+    /* Disable SUD so native syscalls (sysarch, etc.) in the handler succeed */
+    thread_data->syscall_dispatch = 0;
+
+    amd64_set_fsbase(((struct amd64_thread_data *)thread_data->cpu_data)->pthread_teb);
+    libthr_signal_handlers[sig - 1](sig, info, _ucp);
+
+    /* Restore SUD state before libc executes the sigreturn() syscall */
+    thread_data->syscall_dispatch = old_sud;
+}
+
+extern int __sys_sigaction(int, const struct sigaction *, struct sigaction *);
+
+static int wrap_libthr_signal_handlers(void) {
+    struct sigaction act;
+    int sig;
+
+    for (sig = 1; sig <= _SIG_MAXSIG; sig++) {
+
+        if (__sys_sigaction(sig, NULL, &act) == -1) return -1;
+        if ((act.sa_flags & SA_SIGINFO) && act.sa_sigaction != NULL) {
+
+            libthr_signal_handlers[sig - 1] = act.sa_sigaction;
+            act.sa_sigaction = libthr_sighandler_wrapper;
+
+            if (__sys_sigaction(sig, &act, NULL) == -1) return -1;
+        }
+    }
+
+    return 0;
+}
+#endif
+
 /**********************************************************************
  *		signal_init_process
  */
@@ -2942,6 +3183,37 @@
             break;
         }
     }
+#elif defined(__FreeBSD__)
+    if (wow_teb)
+    {
+        u_int p[4];
+        u_int cpu_stdext_feature;
+
+        LDT_ENTRY fs32_entry = ldt_make_entry(wow_teb, page_size - 1, LDT_FLAGS_DATA | LDT_FLAGS_32BIT);
+
+        cs32_sel = GSEL(GUCODE32_SEL, SEL_UPL);
+
+        amd64_thread_data()->fs = LSEL(first_ldt_entry, SEL_UPL);
+        ldt_set_entry(amd64_thread_data()->fs, fs32_entry);
+
+        syscall_flags |= SYSCALL_HAVE_PTHREAD_TEB;
+
+        do_cpuid(0, p);
+        if (p[0] >= 7)
+        {
+            cpuid_count(7, 0, p);
+            cpu_stdext_feature = p[1];
+        }
+        else
+        {
+            cpu_stdext_feature = 0;
+        }
+
+        if (cpu_stdext_feature & CPUID_STDEXT_FSGSBASE)
+        {
+            syscall_flags |= SYSCALL_HAVE_WRFSGSBASE;
+        }
+    }
 #endif
 
     sig_act.sa_mask = server_block_set;
@@ -2964,8 +3236,38 @@
     if (sigaction( SIGILL, &sig_act, NULL ) == -1) goto error;
     if (sigaction( SIGBUS, &sig_act, NULL ) == -1) goto error;
 #ifdef __APPLE__
+    sig_act.sa_sigaction = sigsys_handler;
+    if (sigaction( SIGSYS, &sig_act, NULL ) == -1) goto error;
+#endif
+#ifdef __FreeBSD__
+    /* Wrap libthr internal signal handlers before installing our own SIGSYS
+     * handler, so that wrap_libthr_signal_handlers() does not also wrap
+     * sigsys_handler itself (SIGSYS has no libthr handler to wrap anyway,
+     * but doing it first is cleaner and avoids the ordering dependency). */
+    if (wrap_libthr_signal_handlers() == -1) goto error;
+
+    /* Register the SUD intercept handler */
     sig_act.sa_sigaction = sigsys_handler;
     if (sigaction( SIGSYS, &sig_act, NULL ) == -1) goto error;
+
+    {
+        const char *sgi = getenv("SteamGameId");
+        if (sgi && (!strcmp(sgi, "1174180") || !strcmp(sgi, "1404210") || !strcmp(sgi, "1418100") || !strcmp(sgi, "2767030")
+                   || !strcmp(sgi, "2853730") || !strcmp( sgi, "298110" )))
+        {
+            unsigned int i, j;
+            fbsd_sud_translate_syscalls = 1;
+            for (i = 0; i < KeServiceDescriptorTable->ServiceLimit; ++i)
+            {
+                for (j = 0; j < ARRAY_SIZE(fbsd_syscall_nr_translation); ++j)
+                    if ((void *)KeServiceDescriptorTable->ServiceTable[i] == fbsd_syscall_nr_translation[j].function)
+                    {
+                        fbsd_syscall_nr_translation[j].wine_syscall_nr = i;
+                        break;
+                    }
+            }
+        }
+    }
 #endif
     install_bpf(&sig_act);
     return;
@@ -3005,7 +3307,8 @@
     arch_prctl( ARCH_GET_FS, &thread_data->pthread_teb );
     if (fs32_sel) alloc_fs_sel( fs32_sel >> 3, get_wow_teb( teb ));
 #elif defined (__FreeBSD__) || defined (__FreeBSD_kernel__)
-    amd64_set_gsbase( teb );
+    amd64_set_gsbase(teb);
+    amd64_get_fsbase(&thread_data->pthread_teb);
 #elif defined(__NetBSD__)
     sysarch( X86_64_SET_GSBASE, &teb );
 #elif defined (__APPLE__)
@@ -3149,6 +3452,9 @@
                    __ASM_CFI(".cfi_adjust_cfa_offset -8\n\t")
                    "movl $0,0xb4(%rcx)\n\t"        /* frame->restore_flags */
                    __ASM_LOCAL_LABEL("__wine_syscall_dispatcher_prolog_end") ":\n\t"
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+                   "movb $0,%gs:0x3d0\n\t"       /* SUD: ALLOW (0) native Unix syscalls */
+#endif
                    "movq %rax,0x00(%rcx)\n\t"
                    "movq %rbx,0x08(%rcx)\n\t"
                    __ASM_CFI_REG_IS_AT1(rbx, rcx, 0x08)
@@ -3241,6 +3547,30 @@
                    "1:\tmov $0x1002,%edi\n\t"      /* ARCH_SET_FS */
                    "mov $158,%eax\n\t"             /* SYS_arch_prctl */
                    "syscall\n\t"
+                   "leaq -0x98(%rbp),%rcx\n"
+                   "2:\n\t"
+#elif defined(__FreeBSD__)
+                   "testl $12,%r14d\n\t"           /* SYSCALL_HAVE_PTHREAD_TEB | SYSCALL_HAVE_WRFSGSBASE */
+                   "jz 2f\n\t"
+                   "movq $0x13,%rsi\n\t"           /* GSEL(GUFS32_SEL, SEL_UPL) */
+                   "movq %rsi,%fs\n\t"
+                   "movq %gs:0x320,%rsi\n\t"       /* amd64_thread_data()->pthread_teb */
+                   "testl $8,%r14d\n\t"            /* SYSCALL_HAVE_WRFSGSBASE */
+                   "jz 1f\n\t"
+                   "wrfsbase %rsi\n\t"
+                   "jmp 2f\n"
+                   "1:\n\t"
+                   "pushq %r8\n\t"
+                   "pushq %r9\n\t"
+                   "pushq %r10\n\t"
+                   "pushq %r11\n\t"
+                   "movq $0xa5,%rax\n\t"           /* sysarch */
+                   "movq $0x81,%rdi\n\t"           /* AMD64_SET_FSBASE */
+                   "syscall\n\t"
+                   "popq %r11\n\t"
+                   "popq %r10\n\t"
+                   "popq %r9\n\t"
+                   "popq %r8\n\t"
                    "leaq -0x98(%rbp),%rcx\n"
                    "2:\n\t"
 #endif
@@ -3322,12 +3652,20 @@
                    "movq 0x20(%rcx),%rsi\n\t"
                    "movq 0x08(%rcx),%rbx\n\t"
                    "leaq 0x70(%rcx),%rsp\n\t"      /* %rsp > frame means no longer inside syscall */
-#ifdef __linux__
+#if defined(__linux__) || defined(__FreeBSD__)
                    "testl $12,%r14d\n\t"           /* SYSCALL_HAVE_PTHREAD_TEB | SYSCALL_HAVE_WRFSGSBASE */
                    "jz 1f\n\t"
                    "movw %gs:0x338,%fs\n"          /* amd64_thread_data()->fs */
+# ifdef __FreeBSD__
+                   /* reset %ss (after sysret) for AMD */
+                   "movq $0x3b,%r14\n\t"           /* GSEL(GUDATA_SEL, SEL_UPL) */
+                   "movq %r14,%ss\n\t"
+# endif
                    "1:\n\t"
 #endif
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+                   "movb $1,%gs:0x3d0\n\t"       /* SUD: BLOCK (1) Windows syscalls */
+#endif
                    "testl $0x10000,%edx\n\t"       /* RESTORE_FLAGS_INSTRUMENTATION */
                    "movq 0x60(%rcx),%r14\n\t"
                    "jnz 2f\n\t"
@@ -3444,6 +3782,9 @@
                    __ASM_CFI_REG_IS_AT2(rip, rcx, 0xf0,0x00)
                    "movl $0x20000,0xb4(%rcx)\n\t"  /* frame->restore_flags <- RESTORE_FLAGS_INCOMPLETE_FRAME_CONTEXT */
                    __ASM_LOCAL_LABEL("__wine_unix_call_dispatcher_prolog_end") ":\n\t"
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+                   "movb $0,%gs:0x3d0\n\t"       /* SUD: ALLOW (0) native Unix syscalls */
+#endif
                    "movq %rbx,0x08(%rcx)\n\t"
                    __ASM_CFI_REG_IS_AT1(rbx, rcx, 0x08)
                    "movq %rsi,0x20(%rcx)\n\t"
@@ -3499,6 +3840,29 @@
                    "mov $158,%eax\n\t"             /* SYS_arch_prctl */
                    "syscall\n\t"
                    "2:\n\t"
+#elif defined(__FreeBSD__)
+                   "testl $12,%r14d\n\t"           /* SYSCALL_HAVE_PTHREAD_TEB | SYSCALL_HAVE_WRFSGSBASE */
+                   "jz 2f\n\t"
+                   "movq $0x13,%rsi\n\t"           /* GSEL(GUFS32_SEL, SEL_UPL) */
+                   "movq %rsi,%fs\n\t"
+                   "movq %gs:0x320,%rsi\n\t"       /* amd64_thread_data()->pthread_teb */
+                   "testl $8,%r14d\n\t"            /* SYSCALL_HAVE_WRFSGSBASE */
+                   "jz 1f\n\t"
+                   "wrfsbase %rsi\n\t"
+                   "jmp 2f\n"
+                   "1:\n\t"
+                   "pushq %r8\n\t"
+                   "pushq %r9\n\t"
+                   "pushq %r10\n\t"
+                   "pushq %r11\n\t"
+                   "movq $0xa5,%rax\n\t"           /* sysarch */
+                   "movq $0x81,%rdi\n\t"           /* AMD64_SET_FSBASE */
+                   "syscall\n\t"
+                   "popq %r11\n\t"
+                   "popq %r10\n\t"
+                   "popq %r9\n\t"
+                   "popq %r8\n\t"
+                   "2:\n\t"
 #endif
                    "movq %r8,%rdi\n\t"             /* args */
                    "callq *(%r10,%rdx,8)\n\t"
@@ -3518,12 +3882,20 @@
                    /* switch to user stack */
                    "movq 0x88(%rcx),%rsp\n\t"
                    __ASM_CFI(".cfi_restore_state\n\t")
-#ifdef __linux__
+#if defined(__linux__) || defined(__FreeBSD__)
                    "testl $12,%r14d\n\t"           /* SYSCALL_HAVE_PTHREAD_TEB | SYSCALL_HAVE_WRFSGSBASE */
                    "jz 1f\n\t"
                    "movw %gs:0x338,%fs\n"          /* amd64_thread_data()->fs */
+# ifdef __FreeBSD__
+                   /* reset %ss (after sysret) for AMD */
+                   "movq $0x3b,%r14\n\t"           /* GSEL(GUDATA_SEL, SEL_UPL) */
+                   "movq %r14,%ss\n\t"
+# endif
                    "1:\n\t"
 #endif
+#if defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
+                   "movb $1,%gs:0x3d0\n\t"       /* SUD: BLOCK (1) Windows syscalls */
+#endif
                    "movq 0x60(%rcx),%r14\n\t"
                    "movq 0x28(%rcx),%rdi\n\t"
                    "movq 0x20(%rcx),%rsi\n\t"
