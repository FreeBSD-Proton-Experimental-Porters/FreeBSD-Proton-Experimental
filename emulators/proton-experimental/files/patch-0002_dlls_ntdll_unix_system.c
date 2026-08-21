--- dlls/ntdll/unix/system.c.orig	2026-08-20 17:48:16.269433000 -0700
+++ dlls/ntdll/unix/system.c	2026-08-20 18:30:20.087816000 -0700
@@ -1915,8 +1915,14 @@ static void init_logical_proc_info(void)
         {
             if (p->Relationship == RelationNumaNode || p->Relationship == RelationNumaNodeEx)
             {
-                numa_info.ActiveProcessorsGroupAffinity[p->NumaNode.NodeNumber] = p->NumaNode.GroupMask;
-                ++numa_node_count;
+                if (p->NumaNode.NodeNumber < ARRAY_SIZE(numa_info.ActiveProcessorsGroupAffinity))
+                {
+                    numa_info.ActiveProcessorsGroupAffinity[p->NumaNode.NodeNumber] = p->NumaNode.GroupMask;
+                    ++numa_node_count;
+                }
+                else
+                    FIXME( "NUMA node number %lu exceeds MAXIMUM_NUMA_NODE_COUNT (%u), ignoring.\n",
+                           p->NumaNode.NodeNumber, (unsigned int)ARRAY_SIZE(numa_info.ActiveProcessorsGroupAffinity) );
             }
             p = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)((char *)p + p->Size);
         }
