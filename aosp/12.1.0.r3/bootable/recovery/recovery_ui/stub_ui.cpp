/*
 * Copyright (C) 2019 The Android Open Source Project
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

#include "recovery_ui/stub_ui.h"

#include <android-base/logging.h>

#include "recovery_ui/device.h"

size_t StubRecoveryUI::ShowMenu(const std::vector<std::string>& /* headers */,
                                const std::vector<std::string>& /* items */,
                                size_t /* initial_selection */, bool /* menu_only */,
                                const std::function<int(int, bool)>& /*key_handler*/) {
  while (true) {
        InputEvent evt = WaitInputEvent();
    if (evt.type() == EventType::EXTRA) {
      if (evt.key() == static_cast<int>(KeyError::INTERRUPTED) ||
        evt.key() == static_cast<int>(KeyError::TIMED_OUT))
          return static_cast<size_t>(evt.key());
    }
  }
  LOG(FATAL) << "Unreachable key selected in ShowMenu of stub UI";
}
