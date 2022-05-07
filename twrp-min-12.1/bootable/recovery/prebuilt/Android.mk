LOCAL_PATH := $(call my-dir)

RELINK := $(LOCAL_PATH)/relink.sh

#dummy file to trigger required modules
include $(CLEAR_VARS)

LOCAL_MODULE := teamwin
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/

# Manage list
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/dump_image
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/flash_image
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/erase_image
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/bu

RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/sh

LOCAL_POST_INSTALL_CMD += $(hide) if [ -e "$(TARGET_RECOVERY_ROOT_OUT)/system/bin/egrep" ]; then \
							rm $(TARGET_RECOVERY_ROOT_OUT)/system/bin/egrep; fi; ln -s $(TARGET_RECOVERY_ROOT_OUT)/system/bin/grep $(TARGET_RECOVERY_ROOT_OUT)/system/bin/egrep; \
							if [ -e "$(TARGET_RECOVERY_ROOT_OUT)/system/bin/fgrep" ]; then \
							rm $(TARGET_RECOVERY_ROOT_OUT)/system/bin/fgrep; fi; ln -s $(TARGET_RECOVERY_ROOT_OUT)/system/bin/grep $(TARGET_RECOVERY_ROOT_OUT)/system/bin/fgrep;

ifneq ($(wildcard external/zip/Android.mk),)
	RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_OPTIONAL_EXECUTABLES)/zip
endif
ifneq ($(wildcard external/unzip/Android.mk),)
	RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_OPTIONAL_EXECUTABLES)/unzip
endif
ifneq ($(wildcard system/libziparchive/Android.bp),)
	RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/unzip
endif
ifneq ($(wildcard external/one-true-awk/Android.bp),)
	RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/awk
endif

RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/fastbootd
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/bc
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/e2fsck
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/e2fsdroid
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/mke2fs
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/tune2fs
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/resize2fs

#RELINK_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/twrpmtp
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/android.hardware.fastboot@1.0.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/ld-android.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libandroid_runtime_lazy.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libc.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libdl.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libm.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libfs_mgr.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libfscrypt.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libgsi.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libkeyutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/liblogwrap.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/liblp.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libprocessgroup.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libprocessgroup_setup.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libadbd.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libadbd_services.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libcap.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libminijail.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libunwindstack.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libasyncio.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libbinderthreadstate.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libmdnssd.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libinit.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/bootstrap/libdl_android.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libprotobuf-cpp-lite.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libbinder.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libbinder_ndk.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libchrome.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libevent.so
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/keystore
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/keystore_cli_v2
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/hwservicemanager
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/servicemanager
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/vold_prepare_subdirs
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_EXECUTABLES)/hw/android.hardware.boot@1.0-service
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_EXECUTABLES)/hw/android.hardware.boot@1.1-service
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_EXECUTABLES)/vndservicemanager
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/toybox
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_EXECUTABLES)/hw/android.hardware.health@2.0-service
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_EXECUTABLES)/hw/android.hardware.health@2.1-service
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/charger
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/ueventd
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/watchdogd
RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/minadbd

RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcrecovery.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libusbhost.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_com_err.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_e2p.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2fs.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_profile.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_uuid.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_misc.so
ifneq ($(wildcard external/e2fsprogs/lib/quota/Android.mk),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_quota.so
endif
ifneq ($(wildcard external/e2fsprogs/lib/ext2fs/Android.*),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2fs.so
endif
ifneq ($(wildcard external/e2fsprogs/lib/blkid/Android.*),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_blkid.so
endif
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libpng.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/liblog.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libstdc++.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libminuitwrp.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libminadbd.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libz.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libmtdutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libtar.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libtwadbbu.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libtwrpdigest.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libutil-linux.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libblkid.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libmmcutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libbmlutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libflashutils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libfusesideload.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libbootloader_message.so

RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcrypto.so \
$(if $(WITH_CRYPTO_UTILS),$(TARGET_OUT_SHARED_LIBRARIES)/libcrypto_utils.so)

RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/lib64/libbacktrace.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libunwind.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libbase.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libc++.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libnetd_client.so

RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/toolbox


ifneq ($(TW_OEM_BUILD),true)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/twrp
else
    TW_EXCLUDE_MTP := true
endif

RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libtwrpmtp-ffs.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext4_utils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libaosprecovery.so
ifneq ($(TW_INCLUDE_JPEG),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libjpeg.so
endif
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libselinux.so

ifneq ($(TW_NO_EXFAT), true)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libexfat_twrp.so
else
    TW_NO_EXFAT_FUSE := true
endif
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhardware.so
ifeq ($(TW_INCLUDE_CRYPTO), true)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcryptfsfde.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libdexfile_support.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libf2fs_sparseblock.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.authsecret@1.0.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.oemlock@1.0.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libnos_transport.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libnos_datagram.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.keymaster@3.0.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libgpt_twrp.so
    ifeq ($(TARGET_HW_DISK_ENCRYPTION),true)
        ifeq ($(TARGET_CRYPTFS_HW_PATH),)
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_VENDOR_SHARED_LIBRARIES)/libcryptfs_hw.so
        else
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcryptfs_hw.so
        endif
    endif
    # FBE files
    ifeq ($(TW_INCLUDE_CRYPTO_FBE), true)
         RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libtwrpfscrypt.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.security.maintenance-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.system.keystore2-V1-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.security.apc-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.security.authorization-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymint_support.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.security.keymint-V1-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.frameworks.stats-V1-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.security.secureclock-V1-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libgatekeeper.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymaster_messages.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libbinder.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libprotobuf-cpp-lite.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libsoftkeymasterdevice.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.gatekeeper@1.0.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeystore2_aaid.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeystore2_apc_compat.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeystore2_crypto.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkm_compat_service.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeystore2_vintf_cpp.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.security.compat-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkm_compat.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymint.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/lib_android_keymaster_keymint_utils.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcppbor_external.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcppbor.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libpuresoftkeymasterdevice.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcppcose_rkp.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.security.sharedsecret-V1-ndk_platform.so
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/avbctl
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/keystore
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/keystore2
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/keystore_cli
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.system.wifi.keystore@1.0.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.vibrator@1.0.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.vibrator@1.1.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.vibrator@1.2.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.vibrator-V1-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.vibrator-V1-cpp.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libstatslog.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libsoft_attestation_cert.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libxml2.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libdiskconfig.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhardware_legacy.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libincfs.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.health.storage@1.0.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.health.storage@1.0.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.health.storage-V1-ndk_platform.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhardware_legacy.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.system.suspend@1.0.so
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/fscryptpolicyget

        ifneq ($(wildcard system/keymaster/keymaster_stl.cpp),)
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymaster_portable.so
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymaster_staging.so
        endif
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libwifikeystorehal.so
        ifneq ($(wildcard hardware/interfaces/weaver/Android.bp),)
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.weaver@1.0.so
        endif
        ifneq ($(wildcard hardware/interfaces/weaver/1.0/Android.bp),)
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.weaver@1.0.so
        endif
        ifneq ($(wildcard hardware/interfaces/confirmationui/1.0/Android.bp),)
            RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.confirmationui@1.0.so
        endif
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libsoftkeymaster.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.keymaster@4.0.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.keymaster@4.1.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymaster4support.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymaster4_1support.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libutilscallstack.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libdexfile.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libservices.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeymaster_portable.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhwbinder.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeystore-attestation-application-id.so
         # lshal can be useful for seeing if you have things like the keymaster working properly, but it isn't needed for TWRP to work
         #RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/lshal
         #RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/liblshal.so
         #RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libssl.so
         #RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhidl-gen-hash.so
    endif
endif
ifeq ($(AB_OTA_UPDATER), true)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/update_engine_sideload
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.boot@1.0.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hardware.boot@1.1.so
endif
ifeq ($(PRODUCT_USE_DYNAMIC_PARTITIONS),true)
    ifeq ($(TW_EXCLUDE_LPDUMP),)
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/bootctl
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/lpdump
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/lpdumpd
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/liblpdump.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/liblpdump_interface-V1-cpp.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libprotobuf-cpp-full.so
    endif
    ifeq ($(TW_EXCLUDE_LPTOOLS),)
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/lptools
    endif
endif

ifneq ($(wildcard system/core/libsparse/Android.*),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libsparse.so
endif
ifneq ($(TW_EXCLUDE_ENCRYPTED_BACKUPS), true)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libopenaes.so
endif
ifeq ($(TARGET_USERIMAGES_USE_F2FS), true)
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/make_f2fs
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/fsck.f2fs
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/sload_f2fs
endif
ifneq ($(wildcard system/core/reboot/Android.*),)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_RECOVERY_ROOT_OUT)/system/bin/reboot
endif
ifneq ($(TW_DISABLE_TTF), true)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libft2.so
endif
ifneq ($(TW_RECOVERY_ADDITIONAL_RELINK_BINARY_FILES),)
    RECOVERY_BINARY_SOURCE_FILES += $(TW_RECOVERY_ADDITIONAL_RELINK_BINARY_FILES)
endif
ifneq ($(TW_RECOVERY_ADDITIONAL_RELINK_LIBRARY_FILES),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TW_RECOVERY_ADDITIONAL_RELINK_LIBRARY_FILES)
endif
ifneq ($(wildcard external/pcre/Android.mk),)
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libpcre.so
endif
ifeq ($(TW_INCLUDE_NTFS_3G),true)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/mount.ntfs
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/fsck.ntfs
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/mkfs.ntfs
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libntfs-3g.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libfuse-lite.so
else
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/ntfs-3g
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/ntfsfix
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/mkntfs
endif
ifeq ($(BOARD_HAS_NO_REAL_SDCARD),)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/sgdisk
endif
ifneq ($(TW_OZIP_DECRYPT_KEY),)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/ozip_decrypt
endif
ifeq ($(TWRP_INCLUDE_LOGCAT), true)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/logcat
    ifeq ($(TARGET_USES_LOGD), true)
        RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_EXECUTABLES)/logd
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libsysutils.so
        RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libnl.so
    endif
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libpcrecpp.so
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/liblogcat.so
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libcap.so
endif
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libvndksupport.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhidlbase.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhidltransport.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.frameworks.stats@1.0.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libziparchive.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_blkid.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_quota.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libhidl-gen-utils.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libvintf.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libtinyxml2.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/android.hidl.token@1.0.so
ifneq ($(wildcard system/core/libkeyutils/Android.bp),)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libkeyutils.so
endif
ifeq ($(TARGET_ARCH), arm64)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libclang_rt.ubsan_standalone-aarch64-android.so
endif
ifeq ($(TARGET_ARCH), arm)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libclang_rt.ubsan_standalone-arm-android.so
endif
ifeq ($(TARGET_ARCH), x86_64)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libclang_rt.ubsan_standalone-x86_64-android.so
endif
ifeq ($(TARGET_ARCH), x86)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libclang_rt.ubsan_standalone-i686-android.so
endif
ifeq ($(TARGET_ARCH), mips)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libclang_rt.ubsan_standalone-mips-android.so
endif
ifeq ($(TARGET_ARCH), mips64)
	RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libclang_rt.ubsan_standalone-mips64-android.so
endif
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/liblogwrap.so
RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libext2_misc.so

ifneq ($(TW_EXCLUDE_NANO), true)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_OPTIONAL_EXECUTABLES)/nano
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libncurses.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libssh.so
    RECOVERY_LIBRARY_SOURCE_FILES += $(TARGET_OUT_SHARED_LIBRARIES)/libssl.so
endif

ifneq ($(TW_EXCLUDE_BASH), true)
    RECOVERY_BINARY_SOURCE_FILES += $(TARGET_OUT_OPTIONAL_EXECUTABLES)/bash
endif

include $(CLEAR_VARS)
LOCAL_MODULE := relink_libraries
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/
ifneq ($(TARGET_ARCH), arm64)
    ifneq ($(TARGET_ARCH), x86_64)
        LOCAL_POST_INSTALL_CMD += $(RELINK) $(TARGET_RECOVERY_ROOT_OUT)/system/lib $(RECOVERY_LIBRARY_SOURCE_FILES)
    else
        LOCAL_POST_INSTALL_CMD += $(RELINK) $(TARGET_RECOVERY_ROOT_OUT)/system/lib64 $(RECOVERY_LIBRARY_SOURCE_FILES)
    endif
else
LOCAL_POST_INSTALL_CMD += $(RELINK) $(TARGET_RECOVERY_ROOT_OUT)/system/lib64 $(RECOVERY_LIBRARY_SOURCE_FILES)
endif
TARGET_LIBRARY_RELINK_FILES := $(notdir $(RECOVERY_LIBRARY_SOURCE_FILES))
TARGET_BASE_LIBRARY_RELINK_MODULES := $(basename $(TARGET_LIBRARY_RELINK_FILES))
TARGET_RELINK_LIBRARY_MODULES :=  $(filter-out libdexfile, $(TARGET_BASE_LIBRARY_RELINK_MODULES))
LOCAL_REQUIRED_MODULES += $(TARGET_RELINK_LIBRARY_MODULES)
include $(BUILD_PHONY_PACKAGE)


include $(CLEAR_VARS)
LOCAL_MODULE := relink_binaries
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/
LOCAL_POST_INSTALL_CMD += $(RELINK) $(TARGET_RECOVERY_ROOT_OUT)/system/bin $(RECOVERY_BINARY_SOURCE_FILES)
TARGET_BINARY_RELINK_FILES := $(filter-out bu, $(notdir $(RECOVERY_BINARY_SOURCE_FILES)))
LOCAL_REQUIRED_MODULES += $(TARGET_BINARY_RELINK_FILES)
include $(BUILD_PHONY_PACKAGE)

#build out TWRP ramdisk
include $(CLEAR_VARS)
LOCAL_MODULE := twrp_ramdisk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)
LOCAL_POST_INSTALL_CMD += \
    mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/sbin; ln -sf /system/bin/sh $(TARGET_RECOVERY_ROOT_OUT)/sbin/sh && \
    mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/system/etc/selinux/ && \
    mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/vendor/etc/selinux/ && \
    cp $(TARGET_OUT_ETC)/selinux/plat_service_contexts $(TARGET_RECOVERY_ROOT_OUT)/system/etc/selinux/plat_service_contexts && \
    cp $(TARGET_OUT_ETC)/selinux/plat_hwservice_contexts $(TARGET_RECOVERY_ROOT_OUT)/system/etc/selinux/plat_hwservice_contexts && \
    cp $(TARGET_OUT_VENDOR_ETC)/selinux/vndservice_contexts $(TARGET_RECOVERY_ROOT_OUT)/vendor/etc/selinux/vndservice_contexts && \
    cp $(TARGET_OUT_VENDOR_ETC)/selinux/vendor_hwservice_contexts $(TARGET_RECOVERY_ROOT_OUT)/vendor/etc/selinux/vendor_hwservice_contexts && \
    cp $(TARGET_OUT_ETC)/selinux/plat_keystore2_key_contexts $(TARGET_RECOVERY_ROOT_OUT)/system/etc/selinux/plat_keystore2_key_contexts
    ifeq ($(TARGET_USES_MKE2FS), true)
        LOCAL_POST_INSTALL_CMD += \
            && cp $(TARGET_OUT_ETC)/mke2fs.conf $(TARGET_RECOVERY_ROOT_OUT)/system/etc/mke2fs.conf
    endif
LOCAL_REQUIRED_MODULES += init_second_stage.recovery reboot.recovery plat_service_contexts plat_hardware_contexts vndservice_contexts plat_keystore2_key_contexts
include $(BUILD_PHONY_PACKAGE)

# copy license file for OpenAES
ifneq ($(TW_EXCLUDE_ENCRYPTED_BACKUPS), true)
    include $(CLEAR_VARS)
    LOCAL_MODULE := openaes_license
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/license/openaes
    LOCAL_SRC_FILES := ../openaes/LICENSE
    include $(BUILD_PREBUILT)
endif

ifeq ($(TW_INCLUDE_DUMLOCK), true)
    #htcdumlock for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := htcdumlocksys
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #flash_image for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := flash_imagesys
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #dump_image for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := dump_imagesys
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #libbmlutils for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := libbmlutils.so
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #libflashutils for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := libflashutils.so
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #libmmcutils for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := libmmcutils.so
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #libmtdutils for /system for dumlock
    include $(CLEAR_VARS)
    LOCAL_MODULE := libmtdutils.so
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)

    #HTCDumlock.apk
    include $(CLEAR_VARS)
    LOCAL_MODULE := HTCDumlock.apk
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := EXECUTABLES
    LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)$(TWHTCD_PATH)
    LOCAL_SRC_FILES := $(LOCAL_MODULE)
    include $(BUILD_PREBUILT)
endif

ifeq ($(TW_USE_TOOLBOX), true)
   include $(CLEAR_VARS)
   LOCAL_MODULE := mkshrc_twrp
   LOCAL_MODULE_STEM := mkshrc
   LOCAL_MODULE_TAGS := optional
   LOCAL_MODULE_CLASS := ETC
   LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/system/etc
   LOCAL_SRC_FILES := $(LOCAL_MODULE)
   include $(BUILD_PREBUILT)
endif

#TWRP App "placeholder"
include $(CLEAR_VARS)
LOCAL_MODULE := me.twrp.twrpapp.apk
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/system/bin
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

#TWRP App permissions for Android 9+
include $(CLEAR_VARS)
LOCAL_MODULE := privapp-permissions-twrpapp.xml
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := EXECUTABLES
LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/system/bin
LOCAL_SRC_FILES := $(LOCAL_MODULE)
include $(BUILD_PREBUILT)

ifeq ($(TW_INCLUDE_CRYPTO), true)
    ifneq ($(TW_CRYPTO_USE_SYSTEM_VOLD),)
        # Prebuilt vdc_pie for pre-Pie SDK Platforms
        include $(CLEAR_VARS)
        LOCAL_MODULE := vdc_pie
        LOCAL_MODULE_TAGS := optional
        LOCAL_MODULE_CLASS := EXECUTABLES
        LOCAL_MODULE_PATH := $(TARGET_RECOVERY_ROOT_OUT)/system/bin
        LOCAL_SRC_FILES := vdc_pie-$(TARGET_ARCH)
        include $(BUILD_PREBUILT)
    endif
endif

ifneq (,$(filter $(TW_INCLUDE_REPACKTOOLS) $(TW_INCLUDE_RESETPROP) $(TW_INCLUDE_LIBRESETPROP), true))
    ifeq ($(wildcard external/magisk-prebuilt/Android.mk),)
        $(warning Magisk prebuilt tools not found!)
        $(warning Please place https://github.com/TeamWin/external_magisk-prebuilt)
        $(warning into external/magisk-prebuilt)
        $(error magiskboot prebuilts not present; exiting)
    endif
endif

# Include tzdata in TWRP to fix "__bionic_open_tzdata" log spam
# Dummy file to apply post-install patch
ifneq ($(TW_EXCLUDE_TZDATA), true)
    include $(CLEAR_VARS)
    LOCAL_MODULE := tzdata_twrp
    LOCAL_MODULE_TAGS := optional
    LOCAL_MODULE_CLASS := ETC
    LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/system/bin
    LOCAL_REQUIRED_MODULES := tzdata

    LOCAL_POST_INSTALL_CMD += \
        mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/system/usr/share/zoneinfo; \
        cp -f $(TARGET_OUT)/usr/share/zoneinfo/tzdata $(TARGET_RECOVERY_ROOT_OUT)/system/usr/share/zoneinfo/;
    include $(BUILD_PHONY_PACKAGE)
endif

include $(CLEAR_VARS)
LOCAL_MODULE := nano_twrp
LOCAL_MODULE_TAGS := optional
LOCAL_MODULE_CLASS := ETC
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/system/bin
LOCAL_REQUIRED_MODULES := nano libncurses
LOCAL_POST_INSTALL_CMD += \
    cp -rf $(TARGET_OUT_ETC)/nano $(TARGET_RECOVERY_ROOT_OUT)/system/etc/; \
    cp -rf external/libncurses/lib/terminfo $(TARGET_RECOVERY_ROOT_OUT)/system/etc/;
include $(BUILD_PHONY_PACKAGE)

ifneq ($(TW_EXCLUDE_BASH), true)
	include $(CLEAR_VARS)
	LOCAL_MODULE := bash_twrp
	LOCAL_MODULE_TAGS := optional
	LOCAL_MODULE_CLASS := ETC
	LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT)/system/bin
	LOCAL_REQUIRED_MODULES := bash

	LOCAL_POST_INSTALL_CMD += \
		mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/system/etc/bash/; \
		cp -rf external/bash/etc/* $(TARGET_RECOVERY_ROOT_OUT)/system/etc/bash/; \
        sed -i 's/ro.lineage.device/ro.product.device/' $(TARGET_RECOVERY_ROOT_OUT)/system/etc/bash/bashrc; \
        sed -i '/export TERM/d' $(TARGET_RECOVERY_ROOT_OUT)/system/etc/bash/bashrc; \
        mkdir -p $(TARGET_RECOVERY_ROOT_OUT)/sbin; \
        ln -sf /system/bin/bash $(TARGET_RECOVERY_ROOT_OUT)/sbin/bash;
	include $(BUILD_PHONY_PACKAGE)
endif

