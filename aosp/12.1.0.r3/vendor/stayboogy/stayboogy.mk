# stayboogy

PRODUCT_PACKAGES += \
	ThemePicker \
	Backgrounds \
	ExactCalculator \
	Recorder

# AuroraStore Prebuilt
include vendor/stayboogy/opensource/prebuilt/AuroraStore/Android.mk
PRODUCT_PACKAGES += AuroraDroid AuroraStore AuroraServices

include vendor/stayboogy/opensource/prebuilt/audio.mk

# lawnicons (not sure if this even works honestly)
include vendor/stayboogy/opensource/lawnicons/overlay.mk

PRODUCT_PACKAGE_OVERLAYS += vendor/stayboogy/opensource/lawnicons/overlay
# lawnicons end
