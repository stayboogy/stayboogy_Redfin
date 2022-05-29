/*
 * Copyright (C) 2016 - 2020 The TeamWin Recovery Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Decrypt.h"
#include "FsCrypt.h"
#include <fscrypt/fscrypt.h>

#include <map>
#include <string>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef HAVE_LIBKEYUTILS
#include "key_control.h"
#else
#include <keyutils.h>
#endif
#include "keystore_client.pb.h"
#include "Weaver1.h"
#include "cutils/properties.h"

#include <openssl/sha.h>
#include <openssl/aes.h>
#include <openssl/evp.h>
#include <openssl/rand.h>

#include <dirent.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <fstream>
#include <future>
#include <algorithm>

#include <android-base/file.h>
#include <base/threading/platform_thread.h>
#include <android/hardware/confirmationui/1.0/types.h>
#include <android/security/BnConfirmationPromptCallback.h>
#include <android/security/keystore/IKeystoreService.h>
#include <android/hardware/gatekeeper/1.0/IGatekeeper.h>

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <hardware/hw_auth_token.h>

#include <keystore/keystore.h>
#include <keystore/keystore_client.h>
#include <keystore/keystore_client_impl.h>
#include <keystore/KeystoreResponse.h>
#include <keystore/keystore_hidl_support.h>
#include <keystore/keystore_promises.h>
#include <keystore/keystore_return_types.h>
#include <keystore/keymaster_types.h>
#include <keymasterV4_1/Keymaster.h>
#include <keystore/OperationResult.h>
#include "keystore_client.pb.h"

#include <keymasterV4_1/authorization_set.h>
#include <keymasterV4_1/keymaster_utils.h>

extern "C" {
#include "crypto_scrypt.h"
}

#include "fscrypt_policy.h"
#include "fscrypt-common.h"
#include "HashPassword.h"
#include "KeyStorage.h"
#include "android/os/IVold.h"

using android::security::keystore::IKeystoreService;
using keystore::KeystoreResponsePromise;
using keystore::OperationResultPromise;
using android::security::keymaster::OperationResult;
using android::hardware::keymaster::V4_1::support::blob2hidlVec;


inline std::string hidlVec2String(const ::keystore::hidl_vec<uint8_t>& value) {
    return std::string(reinterpret_cast<const std::string::value_type*>(&value[0]), value.size());
}

static bool lookup_ref_key_internal(std::map<userid_t, android::fscrypt::EncryptionPolicy> key_map, const uint8_t* policy, userid_t* user_id) {
#ifdef USE_FSCRYPT_POLICY_V1
	char policy_string_hex[FS_KEY_DESCRIPTOR_SIZE_HEX];
	char key_map_hex[FS_KEY_DESCRIPTOR_SIZE_HEX];
	bytes_to_hex(policy, FS_KEY_DESCRIPTOR_SIZE, policy_string_hex);
#else
	char policy_string_hex[FSCRYPT_KEY_IDENTIFIER_HEX_SIZE];
	char key_map_hex[FSCRYPT_KEY_IDENTIFIER_HEX_SIZE];
	bytes_to_hex(policy, FSCRYPT_KEY_IDENTIFIER_SIZE, policy_string_hex);
#endif

    for (std::map<userid_t, android::fscrypt::EncryptionPolicy>::iterator it=key_map.begin(); it!=key_map.end(); ++it) {
#ifdef USE_FSCRYPT_POLICY_V1
		bytes_to_hex(reinterpret_cast<const uint8_t*>(&it->second.key_raw_ref[0]), FS_KEY_DESCRIPTOR_SIZE, key_map_hex);
#else
		bytes_to_hex(reinterpret_cast<const uint8_t*>(&it->second.key_raw_ref[0]), FSCRYPT_KEY_IDENTIFIER_SIZE, key_map_hex);
#endif
		std::string key_map_hex_string = std::string(key_map_hex);
		if (key_map_hex_string == policy_string_hex) {
            *user_id = it->first;
            return true;
        }
    }
    return false;
}

#ifdef USE_FSCRYPT_POLICY_V1
extern "C" bool lookup_ref_key(fscrypt_policy_v1* fep, uint8_t* policy_type) {
#else
extern "C" bool lookup_ref_key(fscrypt_policy_v2* fep, uint8_t* policy_type) {
#endif
	userid_t user_id = 0;
	std::string policy_type_string;

#ifdef USE_FSCRYPT_POLICY_V1
	char policy_hex[FS_KEY_DESCRIPTOR_SIZE_HEX];
	bytes_to_hex(fep->master_key_descriptor, FS_KEY_DESCRIPTOR_SIZE, policy_hex);
	if (std::strncmp((const char*)fep->master_key_descriptor, de_key_raw_ref.c_str(), FS_KEY_DESCRIPTOR_SIZE) == 0) {
		policy_type_string = SYSTEM_DE_FSCRYPT_POLICY;
		memcpy(policy_type, policy_type_string.data(), policy_type_string.size());
		return true;
	}
    if (!lookup_ref_key_internal(s_de_policies, fep->master_key_descriptor, &user_id)) {
        if (!lookup_ref_key_internal(s_ce_policies, fep->master_key_descriptor, &user_id)) {
            return false;
		} else {
			policy_type_string = USER_CE_FSCRYPT_POLICY + std::to_string(user_id);
		}
    } else {
			policy_type_string = USER_DE_FSCRYPT_POLICY + std::to_string(user_id);
	}
#else
	char policy_hex[FSCRYPT_KEY_IDENTIFIER_HEX_SIZE];
	bytes_to_hex(fep->master_key_identifier, FSCRYPT_KEY_IDENTIFIER_SIZE, policy_hex);
	if (std::strncmp((const char*)fep->master_key_identifier, de_key_raw_ref.c_str(), FSCRYPT_KEY_IDENTIFIER_SIZE) == 0) {
		policy_type_string = SYSTEM_DE_FSCRYPT_POLICY;
		memcpy(policy_type, policy_type_string.data(), policy_type_string.size());
		return true;
	}
    if (!lookup_ref_key_internal(s_de_policies, fep->master_key_identifier, &user_id)) {
        if (!lookup_ref_key_internal(s_ce_policies, fep->master_key_identifier, &user_id)) {
            return false;
		} else {
			policy_type_string = USER_CE_FSCRYPT_POLICY + std::to_string(user_id);
		}
    } else {
			policy_type_string = USER_DE_FSCRYPT_POLICY + std::to_string(user_id);
	}
#endif

	memcpy(policy_type, policy_type_string.data(), policy_type_string.size());
	LOG(INFO) << "storing policy type: " << policy_type;
    return true;
}

extern "C" bool lookup_ref_tar(const uint8_t* policy_type, uint8_t* policy) {
	std::string policy_type_string = std::string((char *) policy_type);
#ifdef USE_FSCRYPT_POLICY_V1
	char policy_hex[FS_KEY_DESCRIPTOR_SIZE_HEX];
	bytes_to_hex(policy_type, FS_KEY_DESCRIPTOR_SIZE, policy_hex);
#else
	char policy_hex[FSCRYPT_KEY_IDENTIFIER_HEX_SIZE];
	bytes_to_hex(policy_type, FSCRYPT_KEY_IDENTIFIER_SIZE, policy_hex);
#endif

	userid_t user_id = atoi(policy_type_string.substr(3, 4).c_str());

	// TODO Update version # and make magic strings
#ifdef USE_FSCRYPT_POLICY_V1
	if (policy_type_string.substr(0,1) != FSCRYPT_V1) {
#else
	if (policy_type_string.substr(0,1) != FSCRYPT_V2) {
#endif
        LOG(ERROR) << "Unexpected version:" << policy_type[0];
        return false;
    }

	if (policy_type_string.substr(1, 2) == SYSTEM_DE_KEY) {
        memcpy(policy, de_key_raw_ref.data(), de_key_raw_ref.size());
        return true;
    }

    std::string raw_ref;

	if (policy_type_string.substr(1, 1) == USER_DE_KEY) {
        if (lookup_key_ref(s_de_policies, user_id, &raw_ref)) {
            memcpy(policy, raw_ref.data(), raw_ref.size());
        } else
            return false;
    } else if (policy_type_string.substr(1, 1) == USER_CE_KEY) {
        if (lookup_key_ref(s_ce_policies, user_id, &raw_ref)) {
            memcpy(policy, raw_ref.data(), raw_ref.size());
        } else
            return false;
    } else {
        LOG(ERROR) << "unknown policy type: " << policy_type;
        return false;
    }
    return true;
}

bool Decrypt_DE() {
	if (!fscrypt_initialize_systemwide_keys()) { // this deals with the overarching device encryption
		printf("fscrypt_initialize_systemwide_keys returned fail\n");
		return false;
	}
	if (!fscrypt_init_user0()) {
		printf("fscrypt_init_user0 returned fail\n");
		return false;
	}
	return true;
}

// Crappy functions for debugging, please ignore unless you need to debug
// void output_hex(const std::string& in) {
// 	const char *buf = in.data();
// 	char hex[in.size() * 2 + 1];
// 	unsigned int index;
// 	for (index = 0; index < in.size(); index++)
// 		sprintf(&hex[2 * index], "%02X", buf[index]);
// 	printf("%s", hex);
// }

// void output_hex(const char* buf, const int size) {
// 	char hex[size * 2 + 1];
// 	int index;
// 	for (index = 0; index < size; index++)
// 		sprintf(&hex[2 * index], "%02X", buf[index]);
// 	printf("%s", hex);
// }

// void output_hex(const unsigned char* buf, const int size) {
// 	char hex[size * 2 + 1];
// 	int index;
// 	for (index = 0; index < size; index++)
// 		sprintf(&hex[2 * index], "%02X", buf[index]);
// 	printf("%s", hex);
// }

// void output_hex(std::vector<uint8_t>* vec) {
// 	char hex[3];
// 	unsigned int index;
// 	for (index = 0; index < vec->size(); index++) {
// 		sprintf(&hex[0], "%02X", vec->at(index));
// 		printf("%s", hex);
// 	}
// }

/* An alternative is to use:
 * sqlite3 /data/system/locksettings.db "SELECT value FROM locksettings WHERE name='sp-handle' AND user=0;"
 * but we really don't want to include the 1.1MB libsqlite in TWRP. We scan the spblob folder for the
 * password data file (*.pwd) and get the handle from the filename instead. This is a replacement for
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/LockSettingsService.java#2017
 * We never use this data as an actual long. We always use it as a string. */
bool Find_Handle(const std::string& spblob_path, std::string& handle_str) {
	DIR* dir = opendir(spblob_path.c_str());
	if (!dir) {
		printf("Error opening '%s'\n", spblob_path.c_str());
		return false;
	}

	struct dirent* de = 0;

	while ((de = readdir(dir)) != 0) {
		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;
		size_t len = strlen(de->d_name);
		if (len <= 4)
			continue;
		char* p = de->d_name;
		p += len - 4;
		if (strncmp(p, ".pwd", 4) == 0) {
			handle_str = de->d_name;
			handle_str = handle_str.substr(0, len - 4);
			//*handle = strtoull(handle_str.c_str(), 0 , 16);
			closedir(dir);
			return true;
		}
	}
	closedir(dir);
	return false;
}

/* This is the structure of the data in the password data (*.pwd) file which the structure can be found
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#187 */
struct password_data_struct {
	int password_type;
	unsigned char scryptN;
	unsigned char scryptR;
	unsigned char scryptP;
	int salt_len;
	void* salt;
	int handle_len;
	void* password_handle;
};

/* C++ replacement for
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#764 */
bool Get_Password_Data(const std::string& spblob_path, const std::string& handle_str, password_data_struct *pwd) {
	std::string pwd_file = spblob_path + handle_str + ".pwd";
	std::string pwd_data;
	if (!android::base::ReadFileToString(pwd_file, &pwd_data)) {
		printf("Failed to read '%s'\n", pwd_file.c_str());
		return false;
	}
	// output_hex(pwd_data.data(), pwd_data.size());printf("\n");
	const int* intptr = (const int*)pwd_data.data();
	pwd->password_type = *intptr;
	endianswap(&pwd->password_type);
	//printf("password type %i\n", pwd->password_type); // 2 was PIN, 1 for pattern, 2 also for password, -1 for default password
	const unsigned char* byteptr = (const unsigned char*)pwd_data.data() + sizeof(int);
	pwd->scryptN = *byteptr;
	byteptr++;
	pwd->scryptR = *byteptr;
	byteptr++;
	pwd->scryptP = *byteptr;
	byteptr++;
	intptr = (const int*)byteptr;
	pwd->salt_len = *intptr;
	endianswap(&pwd->salt_len);
	if (pwd->salt_len != 0) {
		pwd->salt = malloc(pwd->salt_len);
		if (!pwd->salt) {
			printf("Get_Password_Data malloc salt\n");
			return false;
		}
		memcpy(pwd->salt, intptr + 1, pwd->salt_len);
		intptr++;
		byteptr = (const unsigned char*)intptr;
		byteptr += pwd->salt_len;
	} else {
		printf("Get_Password_Data salt_len is 0\n");
		return false;
	}
	intptr = (const int*)byteptr;
	pwd->handle_len = *intptr;
	endianswap(&pwd->handle_len);
	if (pwd->handle_len != 0) {
		pwd->password_handle = malloc(pwd->handle_len);
		if (!pwd->password_handle) {
			printf("Get_Password_Data malloc password_handle\n");
			return false;
		}
		memcpy(pwd->password_handle, intptr + 1, pwd->handle_len);
	} else {
		printf("Get_Password_Data handle_len is 0\n");
		// Not an error if using weaver
	}
	return true;
}

/* C++ replacement for
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#765
 * called here
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#1050 */
bool Get_Password_Token(const password_data_struct *pwd, const std::string& Password, unsigned char* password_token) {
	if (!password_token) {
		printf("password_token is null\n");
		return false;
	}
	unsigned int N = 1 << pwd->scryptN;
	unsigned int r = 1 << pwd->scryptR;
	unsigned int p = 1 << pwd->scryptP;
	//printf("N %i r %i p %i\n", N, r, p);
	int ret = crypto_scrypt(reinterpret_cast<const uint8_t*>(Password.data()), Password.size(),
                          reinterpret_cast<const uint8_t*>(pwd->salt), pwd->salt_len,
                          N, r, p,
                          password_token, 32);
	if (ret != 0) {
		printf("scrypt error\n");
		return false;
	}
	return true;
}

// Data structure for the *.weaver file, see Get_Weaver_Data below
struct weaver_data_struct {
	unsigned char version;
	int slot;
};

/* C++ replacement for
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#501
 * called here
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#768 */
bool Get_Weaver_Data(const std::string& spblob_path, const std::string& handle_str, weaver_data_struct *wd) {
	std::string weaver_file = spblob_path + handle_str + ".weaver";
	std::string weaver_data;
	if (!android::base::ReadFileToString(weaver_file, &weaver_data)) {
		printf("Failed to read '%s'\n", weaver_file.c_str());
		return false;
	}
	// output_hex(weaver_data.data(), weaver_data.size());printf("\n");
	const unsigned char* byteptr = (const unsigned char*)weaver_data.data();
	wd->version = *byteptr;
	// printf("weaver version %i\n", wd->version);
	const int* intptr = (const int*)weaver_data.data() + sizeof(unsigned char);
	wd->slot = *intptr;
	//endianswap(&wd->slot); not needed
	// printf("weaver slot %i\n", wd->slot);
	return true;
}

namespace android {

// On Android 8.0 for some reason init can't seem to completely stop keystore
// so we have to kill it too if it doesn't die on its own.
static void kill_keystore() {
    DIR* dir = opendir("/proc");
    if (dir) {
        struct dirent* de = 0;

        while ((de = readdir(dir)) != 0) {
            if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
                continue;

            int pid = -1;
            int ret = sscanf(de->d_name, "%d", &pid);

            if (ret == 1) {
                char cmdpath[PATH_MAX];
                sprintf(cmdpath, "/proc/%d/cmdline", pid);

                FILE* file = fopen(cmdpath, "r");
                size_t task_size = PATH_MAX;
                char task[PATH_MAX];
                char* p = task;
                if (getline(&p, &task_size, file) > 0) {
                    if (strstr(task, "keystore") != 0) {
                        printf("keystore pid %d found, sending kill.\n", pid);
                        kill(pid, SIGINT);
                        usleep(5000);
                        kill(pid, SIGKILL);
                    }
                }
                fclose(file);
            }
        }
        closedir(dir);
    }
}

// The keystore holds a file open on /data so we have to stop / kill it
// if we want to be able to unmount /data for things like formatting.
static void stop_keystore() {
    printf("Stopping keystore...\n");
    property_set("ctl.stop", "keystore");
    usleep(5000);
    kill_keystore();
}

/* These next 2 functions try to get the keystore service 50 times because
 * the keystore is not always ready when TWRP boots */
android::sp<IBinder> getKeystoreBinder() {
	android::sp<IServiceManager> sm = android::defaultServiceManager();
    return sm->getService(String16("android.security.keystore"));
}

android::sp<IBinder> getKeystoreBinderRetry() {
	printf("Starting keystore...\n");
    property_set("ctl.start", "keystore");
	int retry_count = 50;
	android::sp<IBinder> binder = getKeystoreBinder();
	while (binder == NULL && retry_count) {
		printf("Waiting for keystore service... %i\n", retry_count--);
		sleep(1);
		binder = getKeystoreBinder();
	}
	return binder;
}

namespace keystore {

#define SYNTHETIC_PASSWORD_VERSION_V1 1
#define SYNTHETIC_PASSWORD_VERSION_V2 2
#define SYNTHETIC_PASSWORD_VERSION_V3 3
#define SYNTHETIC_PASSWORD_PASSWORD_BASED 0
#define SYNTHETIC_PASSWORD_KEY_PREFIX "USRSKEY_synthetic_password_"
#define USR_PRIVATE_KEY_PREFIX "USRPKEY_synthetic_password_"

static std::string mKey_Prefix;

/* The keystore alias subid is sometimes the same as the handle, but not always.
 * In the case of handle 0c5303fd2010fe29, the alias subid used c5303fd2010fe29
 * without the leading 0. We could try to parse the data from a previous
 * keystore request, but I think this is an easier solution because there
 * is little to no documentation on the format of data we get back from
 * the keystore in this instance. We also want to copy everything to a temp
 * folder so that any key upgrades that might take place do not actually
 * upgrade the keys on the data partition. We rename all 1000 uid files to 0
 * to pass the keystore permission checks. */
bool Find_Keystore_Alias_SubID_And_Prep_Files(const userid_t user_id, std::string& keystoreid, const std::string& handle_str) {
	char path_c[PATH_MAX];
	sprintf(path_c, "/data/misc/keystore/user_%d", user_id);
	char user_dir[PATH_MAX];
	sprintf(user_dir, "user_%d", user_id);
	std::string source_path = "/data/misc/keystore/";
	source_path += user_dir;
	std::string handle_sub = handle_str;
	while (handle_sub.substr(0,1) == "0") {
		std::string temp = handle_sub.substr(1);
		handle_sub = temp;
	}
	mKey_Prefix = "";

	mkdir("/tmp/misc", 0755);
	mkdir("/tmp/misc/keystore", 0755);
	std::string destination_path = "/tmp/misc/keystore/";
	destination_path += user_dir;
	if (mkdir(destination_path.c_str(), 0755) && errno != EEXIST) {
		printf("failed to mkdir '%s' %s\n", destination_path.c_str(), strerror(errno));
		return false;
	}
	destination_path += "/";

	DIR* dir = opendir(source_path.c_str());
	if (!dir) {
		printf("Error opening '%s'\n", source_path.c_str());
		return false;
	}
	source_path += "/";

	struct dirent* de = 0;
	size_t prefix_len = strlen(SYNTHETIC_PASSWORD_KEY_PREFIX);
	bool found_subid = false;
	bool has_pkey = false; // PKEY has priority over SKEY

	while ((de = readdir(dir)) != 0) {
		if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
			continue;
		if (!found_subid) {
			size_t len = strlen(de->d_name);
			if (len <= prefix_len)
				continue;
			if (strstr(de->d_name, SYNTHETIC_PASSWORD_KEY_PREFIX) && !has_pkey)
				mKey_Prefix = SYNTHETIC_PASSWORD_KEY_PREFIX;
			else if (strstr(de->d_name, USR_PRIVATE_KEY_PREFIX)) {
				mKey_Prefix = USR_PRIVATE_KEY_PREFIX;
				has_pkey = true;
			} else
				continue;
			if (strstr(de->d_name, handle_sub.c_str())) {
				keystoreid = handle_sub;
				printf("keystoreid matched handle_sub: '%s'\n", keystoreid.c_str());
				found_subid = true;
			} else {
				std::string file = de->d_name;
				std::size_t found = file.find_last_of("_");
				if (found != std::string::npos) {
					keystoreid = file.substr(found + 1);
					// printf("possible keystoreid: '%s'\n", keystoreid.c_str());
					//found_subid = true; // we'll keep going in hopes that we find a pkey or a match to the handle_sub
				}
			}
		}
		std::string src = source_path;
		src += de->d_name;
		std::ifstream srcif(src.c_str(), std::ios::binary);
		std::string dst = destination_path;
		dst += de->d_name;
		std::size_t source_uid = dst.find("1000");
		if (source_uid != std::string::npos)
			dst.replace(source_uid, 4, "0");
		std::ofstream dstof(dst.c_str(), std::ios::binary);
		printf("copying '%s' to '%s'\n", src.c_str(), dst.c_str());
		dstof << srcif.rdbuf();
		srcif.close();
		dstof.close();
	}
	closedir(dir);
	if (!found_subid && !mKey_Prefix.empty() && !keystoreid.empty())
		found_subid = true;
	return found_subid;
}

/* C++ replacement for function of the same name
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#867
 * returning an empty string indicates an error */
std::string unwrapSyntheticPasswordBlob(const std::string& spblob_path, const std::string& handle_str, const userid_t user_id,
	const void* application_id, const size_t application_id_size, uint32_t auth_token_len) {
	std::string disk_decryption_secret_key = "";

	android::ProcessState::self()->startThreadPool();

	std::string keystore_alias_subid;
	// Can be stored in user 0, so check for both.
	if (!Find_Keystore_Alias_SubID_And_Prep_Files(user_id, keystore_alias_subid, handle_str) &&
		!Find_Keystore_Alias_SubID_And_Prep_Files(0, keystore_alias_subid, handle_str))
	{
		printf("failed to scan keystore alias subid and prep keystore files\n");
		return disk_decryption_secret_key;
	}

	// First get the keystore service
    android::sp<IBinder> binder = getKeystoreBinderRetry();
	android::sp<IKeystoreService> service = interface_cast<IKeystoreService>(binder);

	if (service == NULL) {
		printf("error: could not connect to keystore service\n");
		return disk_decryption_secret_key;
	}

	if (auth_token_len > 0) {
		printf("Starting keystore_auth service...\n");
		property_set("ctl.start", "keystore_auth");
	}

	// Read the data from the .spblob file per: https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#869
	std::string spblob_file = spblob_path + handle_str + ".spblob";
	std::string spblob_data;
	if (!android::base::ReadFileToString(spblob_file, &spblob_data)) {
		printf("Failed to read '%s'\n", spblob_file.c_str());
		return disk_decryption_secret_key;
	}
	unsigned char* byteptr = (unsigned char*)spblob_data.data();
	if (*byteptr != SYNTHETIC_PASSWORD_VERSION_V2 && *byteptr != SYNTHETIC_PASSWORD_VERSION_V1
			&& *byteptr != SYNTHETIC_PASSWORD_VERSION_V3) {
		printf("Unsupported synthetic password version %i\n", *byteptr);
		return disk_decryption_secret_key;
	}
	const unsigned char* synthetic_password_version = byteptr;
	byteptr++;
	if (*byteptr != SYNTHETIC_PASSWORD_PASSWORD_BASED) {
		printf("spblob data is not SYNTHETIC_PASSWORD_PASSWORD_BASED\n");
		return disk_decryption_secret_key;
	}
	byteptr++; // Now we're pointing to the blob data itself
	if (*synthetic_password_version == SYNTHETIC_PASSWORD_VERSION_V2
			|| *synthetic_password_version == SYNTHETIC_PASSWORD_VERSION_V3) {
		printf("spblob v2 / v3\n");
		/* Version 2 / 3 of the spblob is basically the same as version 1, but the order of getting the intermediate key and disk decryption key have been flip-flopped
		 * as seen in https://android.googlesource.com/platform/frameworks/base/+/5025791ac6d1538224e19189397de8d71dcb1a12
		 */
		/* First decrypt call found in
		 * https://android.googlesource.com/platform/frameworks/base/+/android-8.1.0_r18/services/core/java/com/android/server/locksettings/SyntheticPasswordCrypto.java#135
		 * We will use https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/keystore/java/android/security/keystore/AndroidKeyStoreCipherSpiBase.java
		 * and https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/keystore/java/android/security/keystore/AndroidKeyStoreAuthenticatedAESCipherSpi.java
		 * First we set some algorithm parameters as seen in two places:
		 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/keystore/java/android/security/keystore/AndroidKeyStoreAuthenticatedAESCipherSpi.java#297
		 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/keystore/java/android/security/keystore/AndroidKeyStoreAuthenticatedAESCipherSpi.java#216 */
		// When using secdis (aka not weaver) you must supply an auth token to the keystore prior to the begin operation
		if (auth_token_len > 0) {
			/*::keystore::KeyStoreServiceReturnCode auth_result = service->addAuthToken(auth_token, auth_token_len);
			if (!auth_result.isOk()) {
				// The keystore checks the uid of the calling process and will return a permission denied on this operation for user 0
				printf("keystore error adding auth token\n");
				return disk_decryption_secret_key;
			}*/
			// The keystore refuses to allow the root user to supply auth tokens, so we write the auth token to a file earlier and
			// run a separate service that runs user the system user to add the auth token. We wait for the auth token file to be
			// deleted by the keymaster_auth service and check for a /auth_error file in case of errors. We quit after after a while if
			// the /auth_token file never gets deleted.
			int auth_wait_count = 20;
			while (access("/auth_token", F_OK) == 0 && auth_wait_count-- > 0)
				usleep(5000);
			if (auth_wait_count == 0 || access("/auth_error", F_OK) == 0) {
				printf("error during keymaster_auth service\n");
				/* If you are getting this error, make sure that you have the keymaster_auth service defined in your init scripts, preferrably in init.recovery.{ro.hardware}.rc
				 * service keystore_auth /system/bin/keystore_auth
				 *     disabled
				 *     oneshot
				 *     user system
				 *     group root
				 *     seclabel u:r:recovery:s0
				 *
				 * And check dmesg for error codes regarding this service if needed. */
				return disk_decryption_secret_key;
			}
		}
		int32_t ret;
		size_t maclen = 128;
		unsigned char* iv = (unsigned char*)byteptr; // The IV is the first 12 bytes of the spblob
		::keystore::hidl_vec<uint8_t> iv_hidlvec;
		iv_hidlvec.setToExternal((unsigned char*)byteptr, 12);
		// printf("iv: "); output_hex((const unsigned char*)iv, 12); printf("\n");
		std::string keystore_alias = mKey_Prefix;
		keystore_alias += keystore_alias_subid;
		String16 keystore_alias16(keystore_alias.data(), keystore_alias.size());
		int32_t error_code;
		unsigned char* cipher_text = (unsigned char*)byteptr + 12; // The cipher text comes immediately after the IV
		std::string cipher_text_str(byteptr, byteptr + spblob_data.size() - 14);

		::keystore::hidl_vec<uint8_t> cipher_text_hidlvec;
		::keystore::AuthorizationSetBuilder begin_params;

        cipher_text_hidlvec.setToExternal(cipher_text, spblob_data.size() - 14 /* 1 each for version and SYNTHETIC_PASSWORD_PASSWORD_BASED and 12 for the iv */);
		begin_params.Authorization(::keystore::TAG_ALGORITHM, ::keystore::Algorithm::AES);
		begin_params.Authorization(::keystore::TAG_BLOCK_MODE, ::keystore::BlockMode::GCM);
		begin_params.Padding(::keystore::PaddingMode::NONE);
		begin_params.Authorization(::keystore::TAG_NONCE, iv_hidlvec);
		begin_params.Authorization(::keystore::TAG_MAC_LENGTH, maclen);

		::keystore::hidl_vec<uint8_t> entropy; // No entropy is needed for decrypt
		entropy.resize(0);
		android::security::keymaster::KeymasterArguments empty_params;
		android::hardware::keymaster::V4_0::KeyPurpose decryptPurpose  = android::hardware::keymaster::V4_0::KeyPurpose::DECRYPT;
		android::sp<android::IBinder> decryptAuthToken(new android::BBinder);

		android::sp<OperationResultPromise> promise = new OperationResultPromise;
		auto future = promise->get_future();
		auto binder_result = service->begin(promise, decryptAuthToken, keystore_alias16, (int32_t)decryptPurpose, true,
			android::security::keymaster::KeymasterArguments(begin_params.hidl_data()), 
			entropy, -1, &error_code);
	    if (!binder_result.isOk()) {
        	printf("communication error while calling keystore\n");
			return disk_decryption_secret_key;
   		}
		::keystore::KeyStoreNativeReturnCode rc(error_code);
		if (!rc.isOk()) {
			printf("Keystore begin returned: %u\n", error_code);
        	return disk_decryption_secret_key;
    	}
		OperationResult result = future.get();
		std::map<uint64_t, android::sp<android::IBinder>> active_operations_;
		uint64_t next_virtual_handle_ = 1;
		active_operations_[next_virtual_handle_] = result.token;

		// The cipher.doFinal call triggers an update to the keystore followed by a finish https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordCrypto.java#64
		// See also https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/keystore/java/android/security/keystore/KeyStoreCryptoOperationChunkedStreamer.java#208
		future = {};
		promise = new OperationResultPromise();
	    future = promise->get_future();
		binder_result = service->update(promise, active_operations_[next_virtual_handle_], empty_params, cipher_text_hidlvec, &error_code);
		rc = ::keystore::KeyStoreNativeReturnCode(error_code);
        if (!rc.isOk()) {
            printf("Keystore update returned: %d\n", error_code);
            return disk_decryption_secret_key;
        }
		result = future.get();
        if (!result.resultCode.isOk()) {
            printf("update failed: %d\n", error_code);
            return disk_decryption_secret_key;
        }

		size_t keystore_result_size = result.data.size();
		unsigned char* keystore_result = (unsigned char*)malloc(keystore_result_size);
		if (!keystore_result) {
			printf("malloc on keystore_result\n");
			return disk_decryption_secret_key;
		}
		memcpy(keystore_result, &result.data[0], result.data.size());
		future = {};
		promise = new OperationResultPromise();
		future = promise->get_future();

		auto hidlSignature = blob2hidlVec("");
		auto hidlInput = blob2hidlVec(disk_decryption_secret_key);
		binder_result = service->finish(promise, active_operations_[next_virtual_handle_], empty_params, hidlInput, hidlSignature, ::keystore::hidl_vec<uint8_t>(), &error_code);
		if (!binder_result.isOk()) {
			printf("communication error while calling keystore\n");
			free(keystore_result);
			return disk_decryption_secret_key;
		}
		rc = ::keystore::KeyStoreNativeReturnCode(error_code);
		if (!rc.isOk()) {
			printf("Keystore finish returned: %d\n", error_code);
			return disk_decryption_secret_key;
		}
		result = future.get();
		if (!result.resultCode.isOk()) {
			printf("finish failed: %d\n", error_code);
			return disk_decryption_secret_key;
		}
		stop_keystore();
		/* Now we do the second decrypt call as seen in:
		 * https://android.googlesource.com/platform/frameworks/base/+/android-8.1.0_r18/services/core/java/com/android/server/locksettings/SyntheticPasswordCrypto.java#136
		 */
		const unsigned char* intermediate_iv = keystore_result;
		// printf("intermediate_iv: "); output_hex((const unsigned char*)intermediate_iv, 12); printf("\n");
		const unsigned char* intermediate_cipher_text = (const unsigned char*)keystore_result + 12; // The cipher text comes immediately after the IV
		int cipher_size = keystore_result_size - 12;
		// First we personalize as seen https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordCrypto.java#102
		void* personalized_application_id = PersonalizedHashBinary(PERSONALISATION_APPLICATION_ID, (const char*)application_id, application_id_size);
		if (!personalized_application_id) {
			return disk_decryption_secret_key;
		}
		// printf("personalized application id: "); output_hex((unsigned char*)personalized_application_id, SHA512_DIGEST_LENGTH); printf("\n");
		// Now we'll decrypt using openssl AES/GCM/NoPadding
		OpenSSL_add_all_ciphers();
		int actual_size=0, final_size=0;
		EVP_CIPHER_CTX *d_ctx = EVP_CIPHER_CTX_new();
		const unsigned char* key = (const unsigned char*)personalized_application_id; // The key is the now personalized copy of the application ID
		// printf("key: "); output_hex((const unsigned char*)key, 32); printf("\n");
		EVP_DecryptInit(d_ctx, EVP_aes_256_gcm(), key, intermediate_iv);
		unsigned char* secret_key = (unsigned char*)malloc(cipher_size);
		if (!secret_key) {
			printf("malloc failure on secret key\n");
			return disk_decryption_secret_key;
		}
		EVP_DecryptUpdate(d_ctx, secret_key, &actual_size, intermediate_cipher_text, cipher_size);
		unsigned char tag[AES_BLOCK_SIZE];
		EVP_CIPHER_CTX_ctrl(d_ctx, EVP_CTRL_GCM_SET_TAG, 16, tag);
		EVP_DecryptFinal_ex(d_ctx, secret_key + actual_size, &final_size);
		EVP_CIPHER_CTX_free(d_ctx);
		free(personalized_application_id);
		free(keystore_result);
		int secret_key_real_size = actual_size - 16;
		// printf("secret key:  "); output_hex((const unsigned char*)secret_key, secret_key_real_size); printf("\n");
		// The payload data from the keystore update is further personalized at https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#153
		// We now have the disk decryption key!
		if (*synthetic_password_version == SYNTHETIC_PASSWORD_VERSION_V3) {
			// V3 uses SP800 instead of SHA512
			disk_decryption_secret_key = PersonalizedHashSP800(PERSONALIZATION_FBE_KEY, PERSONALISATION_CONTEXT, (const char*)secret_key, secret_key_real_size);
		} else {
			disk_decryption_secret_key = PersonalizedHash(PERSONALIZATION_FBE_KEY, (const char*)secret_key, secret_key_real_size);
		}
		// printf("disk_decryption_secret_key: '%s'\n", disk_decryption_secret_key.c_str());
		free(secret_key);
		return disk_decryption_secret_key;
	}
	return disk_decryption_secret_key;
}

}}

#define PASSWORD_TOKEN_SIZE 32

/* C++ replacement for
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#992
 * called here
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#813 */
bool Get_Secdis(const std::string& spblob_path, const std::string& handle_str, std::string& secdis_data) {
	std::string secdis_file = spblob_path + handle_str + ".secdis";
	if (!android::base::ReadFileToString(secdis_file, &secdis_data)) {
		printf("Failed to read '%s'\n", secdis_file.c_str());
		return false;
	}
	// output_hex(secdis_data.data(), secdis_data.size());printf("\n");
	return true;
}

// C++ replacement for https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#1033
userid_t fakeUid(const userid_t uid) {
    return 100000 + uid;
}

bool Is_Weaver(const std::string& spblob_path, const std::string& handle_str) {
	std::string weaver_file = spblob_path + handle_str + ".weaver";
	struct stat st;
	if (stat(weaver_file.c_str(), &st) == 0)
		return true;
	return false;
}

bool Free_Return(bool retval, void* weaver_key, password_data_struct* pwd) {
	if (weaver_key)
		free(weaver_key);
	if (pwd->salt)
		free(pwd->salt);
	if (pwd->password_handle)
		free(pwd->password_handle);
	return retval;
}

/* Decrypt_User_Synth_Pass is the TWRP C++ equivalent to spBasedDoVerifyCredential
 * https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/LockSettingsService.java#1998 */
bool Decrypt_User_Synth_Pass(const userid_t user_id, const std::string& Password) {
	bool retval = false;
	void* weaver_key = NULL;
	password_data_struct pwd;
	pwd.salt = NULL;
	pwd.salt_len = 0;
	pwd.password_handle = NULL;
	pwd.handle_len = 0;
	char application_id[PASSWORD_TOKEN_SIZE + SHA512_DIGEST_LENGTH];

    uint32_t auth_token_len = 0;

	std::string secret; // this will be the disk decryption key that is sent to vold
	std::string token = "!"; // there is no token used for this kind of decrypt, key escrow is handled by weaver
	int flags = android::os::IVold::STORAGE_FLAG_CE;
	char spblob_path_char[PATH_MAX];
	sprintf(spblob_path_char, "/data/system_de/%d/spblob/", user_id);
	std::string spblob_path = spblob_path_char;
	long handle = 0;
	std::string handle_str;
	// Get the handle: https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/LockSettingsService.java#2017
	if (!Find_Handle(spblob_path, handle_str)) {
		printf("Error getting handle\n");
		return Free_Return(retval, weaver_key, &pwd);
	}
	// printf("Handle is '%s'\n", handle_str.c_str());
	// Now we begin driving unwrapPasswordBasedSyntheticPassword from: https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#758
	// First we read the password data which contains scrypt parameters
	if (!Get_Password_Data(spblob_path, handle_str, &pwd)) {
		printf("Failed to Get_Password_Data\n");
		return Free_Return(retval, weaver_key, &pwd);
	}
	// printf("pwd N %i R %i P %i salt ", pwd.scryptN, pwd.scryptR, pwd.scryptP); output_hex((char*)pwd.salt, pwd.salt_len); printf("\n");
	unsigned char password_token[PASSWORD_TOKEN_SIZE];
	// printf("Password: '%s'\n", Password.c_str());
	// The password token is the password scrypted with the parameters from the password data file
	if (!Get_Password_Token(&pwd, Password, &password_token[0])) {
		printf("Failed to Get_Password_Token\n");
		return Free_Return(retval, weaver_key, &pwd);
	}
	// output_hex(&password_token[0], PASSWORD_TOKEN_SIZE);printf("\n");
	if (Is_Weaver(spblob_path, handle_str)) {
		printf("using weaver\n");
		// BEGIN PIXEL 2 WEAVER
		// Get the weaver data from the .weaver file which tells us which slot to use when we ask weaver for the escrowed key
		// https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#768
		weaver_data_struct wd;
		if (!Get_Weaver_Data(spblob_path, handle_str, &wd)) {
			printf("Failed to get weaver data\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		// The weaver key is the the password token prefixed with "weaver-key" padded to 128 with nulls with the password token appended then SHA512
		// https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#1059
		weaver_key = PersonalizedHashBinary(PERSONALISATION_WEAVER_KEY, (char*)&password_token[0], PASSWORD_TOKEN_SIZE);
		if (!weaver_key) {
			printf("malloc error getting weaver_key\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		// Now we start driving weaverVerify: https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#343
		// Called from https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#776
		android::vold::Weaver weaver;
		if (!weaver) {
			printf("Failed to get weaver service\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		// Get the key size from weaver service
		uint32_t weaver_key_size = 0;
		if (!weaver.GetKeySize(&weaver_key_size)) {
			printf("Failed to get weaver key size\n");
			return Free_Return(retval, weaver_key, &pwd);
		} else {
			printf("weaver key size is %u\n", weaver_key_size);
		}
		// printf("weaver key: "); output_hex((unsigned char*)weaver_key, weaver_key_size); printf("\n");
		// Send the slot from the .weaver file, the computed weaver key, and get the escrowed key data
		std::vector<uint8_t> weaver_payload;
		// TODO: we should return more information about the status including time delays before the next retry
		if (!weaver.WeaverVerify(wd.slot, weaver_key, &weaver_payload)) {
			printf("failed to weaver verify\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		// printf("weaver payload: "); output_hex(&weaver_payload); printf("\n");
		// Done with weaverVerify
		// Now we will compute the application ID
		// https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#964
		// Called from https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#780
		// The escrowed weaver key data is prefixed with "weaver-pwd" padded to 128 with nulls with the weaver payload appended then SHA512
		void* weaver_secret = PersonalizedHashBinary(PERSONALISATION_WEAVER_PASSWORD, (const char*)weaver_payload.data(), weaver_payload.size());
		// printf("weaver secret: "); output_hex((unsigned char*)weaver_secret, SHA512_DIGEST_LENGTH); printf("\n");
		// The application ID is the password token and weaver secret appended to each other
		memcpy((void*)&application_id[0], (void*)&password_token[0], PASSWORD_TOKEN_SIZE);
		memcpy((void*)&application_id[PASSWORD_TOKEN_SIZE], weaver_secret, SHA512_DIGEST_LENGTH);
		// printf("application ID: "); output_hex((unsigned char*)application_id, PASSWORD_TOKEN_SIZE + SHA512_DIGEST_LENGTH); printf("\n");
		// END PIXEL 2 WEAVER
	} else {
		printf("using secdis\n");
		std::string secdis_data;
		if (!Get_Secdis(spblob_path, handle_str, secdis_data)) {
			printf("Failed to get secdis data\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		void* secdiscardable = PersonalizedHashBinary(PERSONALISATION_SECDISCARDABLE, (char*)secdis_data.data(), secdis_data.size());
		if (!secdiscardable) {
			printf("malloc error getting secdiscardable\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		memcpy((void*)&application_id[0], (void*)&password_token[0], PASSWORD_TOKEN_SIZE);
		memcpy((void*)&application_id[PASSWORD_TOKEN_SIZE], secdiscardable, SHA512_DIGEST_LENGTH);

		int ret = -1;
		bool request_reenroll = false;
		android::sp<android::hardware::gatekeeper::V1_0::IGatekeeper> gk_device;
		gk_device = ::android::hardware::gatekeeper::V1_0::IGatekeeper::getService();
		if (gk_device == nullptr) {
			printf("failed to get gatekeeper service\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		if (pwd.handle_len <= 0) {
			printf("no password handle supplied\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		android::hardware::hidl_vec<uint8_t> pwd_handle_hidl;
		pwd_handle_hidl.setToExternal(const_cast<uint8_t *>((const uint8_t *)pwd.password_handle), pwd.handle_len);
		void* gk_pwd_token = PersonalizedHashBinary(PERSONALIZATION_USER_GK_AUTH, (char*)&password_token[0], PASSWORD_TOKEN_SIZE);
		if (!gk_pwd_token) {
			printf("malloc error getting gatekeeper_key\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
		android::hardware::hidl_vec<uint8_t> gk_pwd_token_hidl;
		gk_pwd_token_hidl.setToExternal(const_cast<uint8_t *>((const uint8_t *)gk_pwd_token), SHA512_DIGEST_LENGTH);
		android::hardware::Return<void> hwRet =
			gk_device->verify(fakeUid(user_id), 0 /* challange */,
							  pwd_handle_hidl,
							  gk_pwd_token_hidl,
							  [&ret, &request_reenroll, &auth_token_len]
								(const android::hardware::gatekeeper::V1_0::GatekeeperResponse &rsp) {
									ret = static_cast<int>(rsp.code); // propagate errors
									if (rsp.code >= android::hardware::gatekeeper::V1_0::GatekeeperStatusCode::STATUS_OK) {
										auth_token_len = rsp.data.size();
										request_reenroll = (rsp.code == android::hardware::gatekeeper::V1_0::GatekeeperStatusCode::STATUS_REENROLL);
										ret = 0; // all success states are reported as 0
										// The keystore refuses to allow the root user to supply auth tokens, so we write the auth token to a file here and later
										// run a separate service that runs as the system user to add the auth token. We wait for the auth token file to be
										// deleted by the keymaster_auth service and check for a /auth_error file in case of errors. We quit after a while seconds if
										// the /auth_token file never gets deleted.
										unlink("/auth_token");
										FILE* auth_file = fopen("/auth_token","wb");
										if (auth_file != NULL) {
											fwrite(rsp.data.data(), sizeof(uint8_t), rsp.data.size(), auth_file);
											fclose(auth_file);
										} else {
											printf("failed to open /auth_token for writing\n");
											ret = -2;
										}
									} else if (rsp.code == android::hardware::gatekeeper::V1_0::GatekeeperStatusCode::ERROR_RETRY_TIMEOUT && rsp.timeout > 0) {
										ret = rsp.timeout;
									}
								}
							 );
		free(gk_pwd_token);
		if (!hwRet.isOk() || ret != 0) {
			printf("gatekeeper verification failed\n");
			return Free_Return(retval, weaver_key, &pwd);
		}
	}
	// Now we will handle https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#816
	// Plus we will include the last bit that computes the disk decrypt key found in:
	// https://android.googlesource.com/platform/frameworks/base/+/android-8.0.0_r23/services/core/java/com/android/server/locksettings/SyntheticPasswordManager.java#153
	secret = android::keystore::unwrapSyntheticPasswordBlob(spblob_path, handle_str, user_id, (const void*)&application_id[0], 
		PASSWORD_TOKEN_SIZE + SHA512_DIGEST_LENGTH, auth_token_len);
	if (!secret.size()) {
		printf("failed to unwrapSyntheticPasswordBlob\n");
		return Free_Return(retval, weaver_key, &pwd);
	}

	if (!fscrypt_unlock_user_key(user_id, 0, token.c_str(), secret.c_str())) {
		printf("fscrypt_unlock_user_key returned fail\n");
		return Free_Return(retval, weaver_key, &pwd);
	}

	if (!fscrypt_prepare_user_storage("", user_id, 0, flags)) {
		printf("failed to fscrypt_prepare_user_storage\n");
		return Free_Return(retval, weaver_key, &pwd);
	}
	printf("User %i Decrypted Successfully!\n", user_id);
	retval = true;
	return Free_Return(retval, weaver_key, &pwd);
}

int Get_Password_Type(const userid_t user_id, std::string& filename) {
	struct stat st;
	char spblob_path_char[PATH_MAX];
	sprintf(spblob_path_char, "/data/system_de/%d/spblob/", user_id);
	if (stat(spblob_path_char, &st) == 0) {
		printf("Using synthetic password method\n");
		std::string spblob_path = spblob_path_char;
		std::string handle_str;
		if (!Find_Handle(spblob_path, handle_str)) {
			printf("Error getting handle\n");
			return 0;
		}
		printf("Handle is '%s'\n", handle_str.c_str());
		password_data_struct pwd;
		if (!Get_Password_Data(spblob_path, handle_str, &pwd)) {
			printf("Failed to Get_Password_Data\n");
			return 0;
		}
		// In Android type 1 is pattern
		// In Android <11 type 2 is PIN or password
		// In Android 11+ type 3 is PIN and type 4 is password
		if (pwd.password_type == 2) {
			printf("password type: password/PIN\n");
			return 1; // In TWRP this means password or PIN (Android <11)
		} else if (pwd.password_type == 4) {
			printf("password type: password\n");
			return 1; // In TWRP this means password
		} else if (pwd.password_type == 1) {
			printf("password type: pattern\n");
			return 2; // In TWRP this means pattern
		} else if (pwd.password_type == 3) {
			printf("password type: PIN\n");
			return 3; // In TWRP this means PIN
		}
		printf("using default password\n");
		return 0; // We'll try the default password
	}
	std::string path;
    if (user_id == 0) {
		path = "/data/system/";
	} else {
		char user_id_str[5];
		sprintf(user_id_str, "%i", user_id);
		path = "/data/system/users/";
		path += user_id_str;
		path += "/";
	}
	filename = path + "gatekeeper.password.key";
	if (stat(filename.c_str(), &st) == 0 && st.st_size > 0)
		return 1;
	filename = path + "gatekeeper.pattern.key";
	if (stat(filename.c_str(), &st) == 0 && st.st_size > 0)
		return 2;
	printf("Unable to locate gatekeeper password file '%s'\n", filename.c_str());
	filename = "";
	return 0;
}

bool Decrypt_User(const userid_t user_id, const std::string& Password) {
    uint8_t *auth_token;
    uint32_t auth_token_len;
    int ret;

    struct stat st;
    if (user_id > 9999) {
		printf("user_id is too big\n");
		return false;
	}
    std::string filename;
    bool Default_Password = (Password == "!");
    if (Get_Password_Type(user_id, filename) == 0 && !Default_Password) {
		printf("Unknown password type\n");
		return false;
	}

	int flags = android::os::IVold::STORAGE_FLAG_CE;

	if (Default_Password) {
		if (!fscrypt_unlock_user_key(user_id, 0, "!", "!")) {
			printf("unlock_user_key returned fail\n");
			return false;
		}
		if (!fscrypt_prepare_user_storage("", user_id, 0, flags)) {
			printf("failed to fscrypt_prepare_user_storage\n");
			return false;
		}
		printf("User %i Decrypted Successfully!\n", user_id);
		return true;
	}
	if (stat("/data/system_de/0/spblob", &st) == 0) {
		printf("Using synthetic password method\n");
		return Decrypt_User_Synth_Pass(user_id, Password);
	}
	// printf("password filename is '%s'\n", filename.c_str());
	if (stat(filename.c_str(), &st) != 0) {
		printf("error stat'ing key file: %s\n", strerror(errno));
		return false;
	}
	std::string handle;
    if (!android::base::ReadFileToString(filename, &handle)) {
		printf("Failed to read '%s'\n", filename.c_str());
		return false;
	}
    bool should_reenroll;
	bool request_reenroll = false;
	android::sp<android::hardware::gatekeeper::V1_0::IGatekeeper> gk_device;
	gk_device = ::android::hardware::gatekeeper::V1_0::IGatekeeper::getService();
	if (gk_device == nullptr)
		return false;
	android::hardware::hidl_vec<uint8_t> curPwdHandle;
	curPwdHandle.setToExternal(const_cast<uint8_t *>((const uint8_t *)handle.c_str()), st.st_size);
	android::hardware::hidl_vec<uint8_t> enteredPwd;
	enteredPwd.setToExternal(const_cast<uint8_t *>((const uint8_t *)Password.c_str()), Password.size());

	android::hardware::Return<void> hwRet =
		gk_device->verify(user_id, 0 /* challange */,
						  curPwdHandle,
						  enteredPwd,
						  [&ret, &request_reenroll, &auth_token, &auth_token_len]
							(const android::hardware::gatekeeper::V1_0::GatekeeperResponse &rsp) {
								ret = static_cast<int>(rsp.code); // propagate errors
								if (rsp.code >= android::hardware::gatekeeper::V1_0::GatekeeperStatusCode::STATUS_OK) {
									auth_token = new uint8_t[rsp.data.size()];
									auth_token_len = rsp.data.size();
									memcpy(auth_token, rsp.data.data(), auth_token_len);
									request_reenroll = (rsp.code == android::hardware::gatekeeper::V1_0::GatekeeperStatusCode::STATUS_REENROLL);
									ret = 0; // all success states are reported as 0
								} else if (rsp.code == android::hardware::gatekeeper::V1_0::GatekeeperStatusCode::ERROR_RETRY_TIMEOUT && rsp.timeout > 0) {
									ret = rsp.timeout;
								}
							}
						 );
	if (!hwRet.isOk()) {
		return false;
	}

	char token_hex[(auth_token_len*2)+1];
	token_hex[(auth_token_len*2)] = 0;
	uint32_t i;
	for (i=0;i<auth_token_len;i++) {
		sprintf(&token_hex[2*i], "%02X", auth_token[i]);
	}
	// The secret is "Android FBE credential hash" plus appended 0x00 to reach 128 bytes then append the user's password then feed that to sha512sum
	std::string secret = HashPassword(Password);
	if (!fscrypt_unlock_user_key(user_id, 0, token_hex, secret.c_str())) {
		printf("fscrypt_unlock_user_key returned fail\n");
		return false;
	}

	if (!fscrypt_prepare_user_storage("", user_id, 0, flags)) {
		printf("failed to fscrypt_prepare_user_storage\n");
		return false;
	}
	printf("User %i Decrypted Successfully!\n", user_id);
	return true;
}