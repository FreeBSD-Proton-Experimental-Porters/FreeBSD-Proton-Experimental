--- dlls/ntdll/unix/file.c.orig	2026-01-26 18:46:06.000000000 -0800
+++ dlls/ntdll/unix/file.c	2026-01-31 23:18:57.311680000 -0800
@@ -7861,12 +7861,34 @@ NTSTATUS WINAPI NtQueryObject( HANDLE handle, OBJECT_I
             status = wine_server_call( req );
             if (status == STATUS_SUCCESS)
             {
-                if (!reply->total)  /* no name */
-                {
-                    if (sizeof(*p) > len) status = STATUS_INFO_LENGTH_MISMATCH;
-                    else memset( p, 0, sizeof(*p) );
-                    if (used_len) *used_len = sizeof(*p);
-                }
+		if (!reply->total)  /* no name */
+		{
+		    if (len < sizeof(*p)) 
+		    {
+		        status = STATUS_INFO_LENGTH_MISMATCH;
+		    }
+		    else 
+		    {
+		        /* Return SUCCESS but with 0 length strings */
+		        p->Name.Length = 0;
+		        /* Some apps crash if Buffer is NULL. Point it to the memory immediately following the struct. */
+		        p->Name.Buffer = (WCHAR *)(p + 1);
+		        /* If we have space for at least one WCHAR (2 bytes), null-terminate and set MaximumLength. */
+		        if (len >= sizeof(*p) + sizeof(WCHAR))
+		        {
+		            p->Name.MaximumLength = sizeof(WCHAR);
+		            p->Name.Buffer[0] = 0;
+		        }
+		        else
+		        {
+		            p->Name.MaximumLength = 0;
+		        }
+
+		        status = STATUS_SUCCESS;
+		    }
+		    /* Total size must include the header */
+		    if (used_len) *used_len = sizeof(*p);
+		}
                 else if (sizeof(*p) + reply->total + sizeof(WCHAR) > len)
                 {
                     if (used_len) *used_len = sizeof(*p) + reply->total + sizeof(WCHAR);
