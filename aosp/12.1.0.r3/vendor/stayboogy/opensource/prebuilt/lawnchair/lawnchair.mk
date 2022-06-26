include $(call first-makefiles-under,$(LOCAL_PATH))

PRODUCT_COPY_FILES += \
    vendor/stayboogy/opensource/prebuilt/lawnchair/etc/permissions/privapp-permissions-app.lawnchair.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/permissions/privapp-permissions-app.lawnchair.xml \
    vendor/stayboogy/opensource/prebuilt/lawnchair/etc/sysconfig/app.lawnchair-hiddenapi-package-whitelist.xml:$(TARGET_COPY_OUT_SYSTEM)/etc/sysconfig/app.lawnchair-hiddenapi-package-whitelist.xml \
    vendor/stayboogy/opensource/prebuilt/lawnchair/priv-app/Lawnchair/lib/arm64/libnrb.so:$(TARGET_COPY_OUT_SYSTEM)/priv-app/Lawnchair/lib/arm64/libnrb.so

PRODUCT_PACKAGES += \
    QuickstepSwitcherOverlay \
    Lawnchair
