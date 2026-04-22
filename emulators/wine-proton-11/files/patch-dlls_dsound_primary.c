--- dlls/dsound/primary.c.orig	2026-04-15 08:45:36.000000000 -0700
+++ dlls/dsound/primary.c	2026-04-19 21:39:20.178397000 -0700
@@ -200,10 +200,10 @@ static HRESULT DSOUND_WaveFormat(DirectSoundDevice *de
         wfe.SubFormat = KSDATAFORMAT_SUBTYPE_IEEE_FLOAT;
         wfe.Samples.wValidBitsPerSample = wfe.Format.wBitsPerSample = 32;
 
-        if (device->num_speakers == 0 || wfe.Format.nChannels < device->num_speakers) {
-            device->speaker_config = DSOUND_FindSpeakerConfig(device->mmdevice, wfe.Format.nChannels);
-            DSOUND_ParseSpeakerConfig(device);
-        } else if (wfe.Format.nChannels > device->num_speakers) {
+        device->speaker_config = DSOUND_FindSpeakerConfig(device->mmdevice, wfe.Format.nChannels);
+        DSOUND_ParseSpeakerConfig(device);
+
+	if (wfe.Format.nChannels > device->num_speakers) {
             wfe.Format.nChannels = device->num_speakers;
             wfe.dwChannelMask = speaker_config_to_channel_mask(device->speaker_config);
         }
