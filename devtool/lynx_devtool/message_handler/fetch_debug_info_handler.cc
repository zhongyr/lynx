// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#include "devtool/lynx_devtool/message_handler/fetch_debug_info_handler.h"

#include "base/include/log/logging.h"
#include "devtool/lynx_devtool/config/devtool_config.h"

namespace lynx {
namespace devtool {

namespace {
constexpr char kTypeGetFetchDebugInfo[] = "GetFetchDebugInfo";
constexpr char kTypeSetFetchDebugInfo[] = "SetFetchDebugInfo";
constexpr char kKeyType[] = "type";
constexpr char kKeyValue[] = "value";
constexpr char kKeyMTS[] = "MTS";
}  // namespace

void FetchDebugInfoHandler::handle(const std::shared_ptr<MessageSender>& sender,
                                   const std::string& type,
                                   const Json::Value& message) {
  if (type != kTypeGetFetchDebugInfo && type != kTypeSetFetchDebugInfo) {
    LOGE("FetchDebugInfoHandler::handle error, unsupported message type: "
         << type);
    return;
  }
  Json::Value content = message;
  std::string key = message[kKeyType].asString();
  if (key != kKeyMTS) {
    LOGE("FetchDebugInfoHandler::handle error, unsupported key: " << key);
    return;
  }
  if (type == kTypeGetFetchDebugInfo) {
    content[kKeyValue] = DevToolConfig::ShouldFetchDebugInfo(true);
  } else if (type == kTypeSetFetchDebugInfo) {
    bool value = message[kKeyValue].asBool();
    DevToolConfig::SetFetchDebugInfo(value, true);
  }
  sender->SendMessage(type, content);
}

}  // namespace devtool
}  // namespace lynx
