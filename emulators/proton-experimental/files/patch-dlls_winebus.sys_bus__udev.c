--- dlls/winebus.sys/bus_udev.c.orig	2026-03-20 13:33:36.000000000 -0700
+++ dlls/winebus.sys/bus_udev.c	2026-04-25 12:00:00.000000000 -0700
@@ -1364,7 +1364,30 @@ static void udev_add_device(struct udev_device *dev, int fd)
     get_device_subsystem_info(dev, "hid", NULL, &desc, &bus);
     get_device_subsystem_info(dev, "input", NULL, &desc, &bus);
     get_device_subsystem_info(dev, "usb", "usb_device", &desc, &bus);
     if (bus == BUS_BLUETOOTH) desc.bus_type = BUS_TYPE_BLUETOOTH;
     else if (bus == BUS_USB) desc.bus_type = BUS_TYPE_USB;
+
+#if defined(HAVE_LINUX_HIDRAW_H) && defined(__FreeBSD__)
+    /* On FreeBSD, libudev-devd does not expose the Linux-style sysfs parent
+     * hierarchy for hidraw devices, so get_device_subsystem_info() leaves
+     * vid/pid at zero. Read them directly from the open hidraw fd via
+     * HIDIOCGRAWINFO. The header at <dev/hid/hidraw.h> defines the ioctl
+     * with FreeBSD's 'U' type number; struct hidraw_devinfo layout matches. */
+    if ((!desc.vid || !desc.pid || !bus) && fd >= 0)
+    {
+        const char *sub = udev_device_get_subsystem(dev);
+        if (sub && !strcmp(sub, "hidraw"))
+        {
+            struct hidraw_devinfo hdi = {0};
+            if (ioctl(fd, HIDIOCGRAWINFO, &hdi) == 0)
+            {
+                if (!bus)      bus      = hdi.bustype;
+                if (!desc.vid) desc.vid = (unsigned short)hdi.vendor;
+                if (!desc.pid) desc.pid = (unsigned short)hdi.product;
+                if (bus == BUS_USB) desc.bus_type = BUS_TYPE_USB;
+            }
+        }
+    }
+#endif
 
     if (!(subsystem = udev_device_get_subsystem(dev)))
