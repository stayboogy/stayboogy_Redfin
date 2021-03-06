/*
 * Copyright (C) 2007 The Android Open Source Project
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

#ifndef OTAUTIL_DIRUTIL_H_
#define OTAUTIL_DIRUTIL_H_

#include <limits.h>    // PATH_MAX
#include <sys/stat.h>  // mode_t
#include <utime.h>     // utime/utimbuf

#include <string>

struct selabel_handle;

// Like "mkdir -p", try to guarantee that all directories specified in path are present, creating as
// many directories as necessary. The specified mode is passed to all mkdir calls; no modifications
// are made to umask.
//
// If strip_filename is set, everything after the final '/' is stripped before creating the
// directory
// hierarchy.
//
// Returns 0 on success; returns -1 (and sets errno) on failure (usually if some element of path is
// not a directory).
int mkdir_recursively(const std::string& path, mode_t mode, bool strip_filename,
                      const struct selabel_handle* sehnd);

// As above, but if timestamp is non-NULL, directories will be timestamped accordingly.
int mkdir_recursively(const std::string& input_path, mode_t mode, bool strip_filename,
                      const selabel_handle* sehnd, const struct utimbuf* timestamp);

// rm -rf <path>
int dirUnlinkHierarchy(const char* path);


#endif  // OTAUTIL_DIRUTIL_H_
