--- dlls/dsound/primary.c.orig	2025-07-23 03:45:33.314139000 +0300
+++ dlls/dsound/primary.c	2025-07-23 04:22:39.177103000 +0300
@@ -194,10 +194,10 @@
         if (FAILED(hr))
             return hr;
 
-        if (device->num_speakers == 0 || mixwfe->Format.nChannels < device->num_speakers) {
-            device->speaker_config = DSOUND_FindSpeakerConfig(device->mmdevice, mixwfe->Format.nChannels);
-            DSOUND_ParseSpeakerConfig(device);
-        } else if (mixwfe->Format.nChannels > device->num_speakers) {
+        device->speaker_config = DSOUND_FindSpeakerConfig(device->mmdevice, mixwfe->Format.nChannels);
+        DSOUND_ParseSpeakerConfig(device);
+
+        if (mixwfe->Format.nChannels > device->num_speakers) {
             mixwfe->Format.nChannels = device->num_speakers;
             mixwfe->Format.nBlockAlign = mixwfe->Format.nChannels * mixwfe->Format.wBitsPerSample / 8;
             mixwfe->Format.nAvgBytesPerSec = mixwfe->Format.nSamplesPerSec * mixwfe->Format.nBlockAlign;
