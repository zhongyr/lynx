// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#define private public
#define protected public

#include "devtool/lynx_devtool/message_handler/stop_at_entry_handler.h"

#include "devtool/base_devtool/native/test/message_sender_mock.h"
#include "devtool/lynx_devtool/config/devtool_config.h"
#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace lynx {
namespace devtool {
namespace testing {

class StopAtEntryHandlerTest : public ::testing::Test {
 public:
  StopAtEntryHandlerTest() {}
  ~StopAtEntryHandlerTest() override {}
  void SetUp() override { handler_ = std::make_unique<StopAtEntryHandler>(); }

 private:
  std::unique_ptr<StopAtEntryHandler> handler_;
};

TEST_F(StopAtEntryHandlerTest, handle) {
  std::string get_type = "GetStopAtEntry";
  std::string set_type = "SetStopAtEntry";
  std::string error_type = "Error";

  std::string key1 = "MTS";
  std::string key2 = "BTS";
  std::string key3 = "DEFAULT";
  std::string error_key = "error_key";
  Json::Value get_msg1(Json::ValueType::objectValue);
  get_msg1["type"] = key1;
  Json::Value get_msg2(Json::ValueType::objectValue);
  get_msg2["type"] = key2;
  Json::Value get_msg3(Json::ValueType::objectValue);
  get_msg3["type"] = key3;
  Json::Value set_msg1(Json::ValueType::objectValue);
  set_msg1["type"] = key1;
  set_msg1["value"] = false;
  Json::Value set_msg2(Json::ValueType::objectValue);
  set_msg2["type"] = key2;
  set_msg2["value"] = false;
  Json::Value set_msg3(Json::ValueType::objectValue);
  set_msg3["type"] = key3;
  set_msg3["value"] = false;
  Json::Value error_msg(Json::ValueType::objectValue);
  error_msg["type"] = error_key;
  error_msg["value"] = false;

  auto sender = std::make_shared<MessageSenderMock>();
  MockReceiver::GetInstance().received_message_ = {"", ""};
  DevToolConfig::should_stop_at_entry_ = true;
  DevToolConfig::should_stop_mts_at_entry_ = true;

  handler_->handle(sender, error_type, get_msg1);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, get_type, error_msg);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, set_type, error_msg);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, "");
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, "");

  handler_->handle(sender, get_type, get_msg1);
  std::string expected = "{\n   \"type\" : \"MTS\",\n   \"value\" : true\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, get_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);

  handler_->handle(sender, get_type, get_msg2);
  expected = "{\n   \"type\" : \"BTS\",\n   \"value\" : true\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, get_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);

  handler_->handle(sender, get_type, get_msg3);
  expected = "{\n   \"type\" : \"DEFAULT\",\n   \"value\" : true\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, get_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);

  handler_->handle(sender, set_type, set_msg1);
  expected = "{\n   \"type\" : \"MTS\",\n   \"value\" : false\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, set_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);

  handler_->handle(sender, set_type, set_msg2);
  expected = "{\n   \"type\" : \"BTS\",\n   \"value\" : false\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, set_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);

  handler_->handle(sender, set_type, set_msg3);
  expected = "{\n   \"type\" : \"DEFAULT\",\n   \"value\" : false\n}\n";
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.first, set_type);
  EXPECT_EQ(MockReceiver::GetInstance().received_message_.second, expected);
}

}  // namespace testing
}  // namespace devtool
}  // namespace lynx
