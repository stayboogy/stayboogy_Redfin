# stayboogy

PRODUCT_PACKAGES += \
	ThemePicker \
	Backgrounds \
	ExactCalculator

include vendor/stayboogy/opensource/prebuilt/audio.mk

# lawnicons (not sure if this even works honestly)
include vendor/stayboogy/opensource/lawnicons/overlay.mk

PRODUCT_PACKAGE_OVERLAYS += vendor/stayboogy/opensource/lawnicons/overlay
# lawnicons end
