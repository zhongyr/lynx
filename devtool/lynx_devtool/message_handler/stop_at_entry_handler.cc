// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "devtool/lynx_devtool/message_handler/stop_at_entry_handler.h"

#include "base/include/log/logging.h"
#include "devtool/lynx_devtool/config/devtool_config.h"

namespace lynx {
namespace devtool {

namespace {
constexpr char kTypeGetStopAtEntry[] = "GetStopAtEntry";
constexpr char kTypeSetStopAtEntry[] = "SetStopAtEntry";
constexpr char kKeyType[] = "type";
constexpr char kKeyValue[] = "value";
constexpr char kKeyDefault[] = "DEFAULT";
constexpr char kKeyMTS[] = "MTS";
constexpr char kKeyBTS[] = "BTS";
}  // namespace

void StopAtEntryHandler::handle(const std::shared_ptr<MessageSender>& sender,
                                const std::string& type,
                                const Json::Value& message) {
  if (type != kTypeGetStopAtEntry && type != kTypeSetStopAtEntry) {
    LOGE(
        "StopAtEntryHandler::handle error, unsupported message type: " << type);
    return;
  }
  Json::Value content = message;
  std::string key = message[kKeyType].asString();
  if (key != kKeyMTS && key != kKeyBTS && key != kKeyDefault) {
    LOGE("StopAtEntryHandler::handle error, unsupported key: " << key);
    return;
  }
  if (type == kTypeGetStopAtEntry) {
    content[kKeyValue] = DevToolConfig::ShouldStopAtEntry(key == kKeyMTS);
  } else if (type == kTypeSetStopAtEntry) {
    bool value = message[kKeyValue].asBool();
    DevToolConfig::SetStopAtEntry(value, key == kKeyMTS);
  }
  sender->SendMessage(type, content);
}

}  // namespace devtool
}  // namespace lynx
