# stayboogy

include vendor/stayboogy/opensource/prebuilt/audio.mk

# custom colorful bootanimation
$(call inherit-product, vendor/stayboogy/bootanimation/bootanimation.mk)

PRODUCT_PACKAGES += \
	ThemePicker \
	Backgrounds \
	ExactCalculator \
	Recorder

PRODUCT_PRODUCT_PROPERTIES +=\
	ro.build.version.custom=

# Lawnchair Prebuilt
#include vendor/stayboogy/opensource/prebuilt/lawnchair/lawnchair.mk

# Pixel Launcher Prebuilt
#include vendor/stayboogy/opensource/prebuilt/PixelLauncher/PixelLauncher.mk

# AuroraStore Prebuilt
#include vendor/stayboogy/opensource/prebuilt/AuroraStore/Aurora.mk

