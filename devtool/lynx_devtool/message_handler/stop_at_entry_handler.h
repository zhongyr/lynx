// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#ifndef DEVTOOL_LYNX_DEVTOOL_MESSAGE_HANDLER_STOP_AT_ENTRY_HANDLER_H_
#define DEVTOOL_LYNX_DEVTOOL_MESSAGE_HANDLER_STOP_AT_ENTRY_HANDLER_H_

#include "devtool/base_devtool/native/public/devtool_message_handler.h"
#include "devtool/base_devtool/native/public/message_sender.h"

namespace lynx {
namespace devtool {

class StopAtEntryHandler : public DevToolMessageHandler {
 public:
  StopAtEntryHandler() = default;
  ~StopAtEntryHandler() override = default;

  void handle(const std::shared_ptr<MessageSender>& sender,
              const std::string& type, const Json::Value& message) override;
};

}  // namespace devtool
}  // namespace lynx

#endif  // DEVTOOL_LYNX_DEVTOOL_MESSAGE_HANDLER_STOP_AT_ENTRY_HANDLER_H_
