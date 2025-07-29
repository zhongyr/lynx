// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#define private public
#define protected public

#include "devtool/lynx_devtool/message_handler/fetch_debug_info_handler.h"

#include "devtool/base_devtool/native/test/message_sender_mock.h"
#include "devtool/lynx_devtool/config/devtool_config.h"
#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace lynx {
namespace devtool {
namespace testing {

class FetchDebugInfoHandlerTest : public ::testing::Test {
 public:
  FetchDebugInfoHandlerTest() {}
  ~FetchDebugInfoHandlerTest() override {}
  void SetUp() override {
    handler_ = std::make_unique<FetchDebugInfoHandler>();
  }

 private:
  std::unique_ptr<FetchDebugInfoHandler> handler_;
};

TEST_F(FetchDebugInfoHandlerTest, handle) {
  std::string get_type = "GetFetchDebugInfo";
  std::string set_type = "SetFetchDebugInfo";
  std::string error_type = "Error";

  std::string key = "MTS";
  std::string error_key = "error_key";
  Json::Value get_msg(Json::ValueType::objectValue);
  get_msg["type"] = key;
  Json::Value set_msg(Json::ValueType::objectValue);
  set_msg["type"] = key;
  set_msg["value"] = false;
  Json::Value error_msg(Json::ValueType::objectValue);
  error_msg["type"] = error_key;
  error_msg["value"] = false;

  auto sender = std::make_shared<MessageSenderMock>();
  MockReceiver::GetInstance().received_message_ = {"", ""};
  DevToolConfig::should_fetch_mts_debug_info_ = true;

  handler_->handle(sender, error_type, get_msg);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, error_type, set_msg);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, get_type, error_msg);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, set_type, error_msg);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, get_type, get_msg);
  std::string expected = "{\n   \"type\" : \"MTS\",\n   \"value\" : true\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, get_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);

  handler_->handle(sender, set_type, set_msg);
  expected = "{\n   \"type\" : \"MTS\",\n   \"value\" : false\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, set_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);
}

}  // namespace testing
}  // namespace devtool
}  // namespace lynx
