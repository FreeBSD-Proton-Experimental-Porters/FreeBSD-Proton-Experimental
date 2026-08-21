--- dlls/wow64/system.c.orig	2026-08-13 21:00:11.000000000 -0700
+++ dlls/wow64/system.c	2026-08-20 18:35:34.899666000 -0700
@@ -517,7 +517,7 @@ NTSTATUS WINAPI wow64_NtQuerySystemInformation( UINT *
         info32->HighestNodeNumber = min( info.HighestNodeNumber, max_node_number );
         if (retlen)
             *retlen = offsetof(SYSTEM_NUMA_INFORMATION32, ActiveProcessorsGroupAffinity[info32->HighestNodeNumber + 1]);
-        for (i = 0; i <= info32->HighestNodeNumber; ++i)
+        for (i = 0; i <= info32->HighestNodeNumber && i <= max_node_number; ++i)
         {
             info32->ActiveProcessorsGroupAffinity[i].Group = info.ActiveProcessorsGroupAffinity[i].Group;
             info32->ActiveProcessorsGroupAffinity[i].Mask = info.ActiveProcessorsGroupAffinity[i].Mask;
