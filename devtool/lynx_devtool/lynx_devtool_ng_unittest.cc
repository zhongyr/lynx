// Copyright 2025 The Lynx Authors. All rights reserved.
// Licensed under the Apache License Version 2.0 that can be found in the
// LICENSE file in the root directory of this source tree.

#define private public
#define protected public

#include "devtool/testing/mock/lynx_devtool_ng_mock.h"
#include "third_party/googletest/googletest/include/gtest/gtest.h"

namespace lynx {
namespace devtool {
namespace testing {

class LynxDevToolNGTest : public ::testing::Test {
 public:
  LynxDevToolNGTest() {}
  ~LynxDevToolNGTest() override {}
  void SetUp() override {}
};

TEST_F(LynxDevToolNGTest, GlobalMessageHandler) {
  constexpr char kTypeGetStopAtEntry[] = "GetStopAtEntry";
  constexpr char kTypeSetStopAtEntry[] = "SetStopAtEntry";
  constexpr char kTypeGetFetchDebugInfo[] = "GetFetchDebugInfo";
  constexpr char kTypeSetFetchDebugInfo[] = "SetFetchDebugInfo";

  auto& global_dispatcher =
      lynx::devtool::AbstractDevTool::GetGlobalMessageDispatcherInstance();

  auto devtool1 = std::make_shared<lynx::testing::LynxDevToolNGMock>();
  auto it = global_dispatcher.handler_map_.find(kTypeGetStopAtEntry);
  EXPECT_NE(it, global_dispatcher.handler_map_.end());
  const auto& get_stop_at_entry_handler = it->second;
  it = global_dispatcher.handler_map_.find(kTypeSetStopAtEntry);
  EXPECT_NE(it, global_dispatcher.handler_map_.end());
  const auto& set_stop_at_entry_handler = it->second;
  it = global_dispatcher.handler_map_.find(kTypeGetFetchDebugInfo);
  EXPECT_NE(it, global_dispatcher.handler_map_.end());
  const auto& get_fetch_debug_info_handler = it->second;
  it = global_dispatcher.handler_map_.find(kTypeSetFetchDebugInfo);
  EXPECT_NE(it, global_dispatcher.handler_map_.end());
  const auto& set_fetch_debug_info_handler = it->second;

  auto devtool2 = std::make_shared<lynx::testing::LynxDevToolNGMock>();
  it = global_dispatcher.handler_map_.find(kTypeGetStopAtEntry);
  EXPECT_EQ(it->second, get_stop_at_entry_handler);
  it = global_dispatcher.handler_map_.find(kTypeSetStopAtEntry);
  EXPECT_EQ(it->second, set_stop_at_entry_handler);
  it = global_dispatcher.handler_map_.find(kTypeGetFetchDebugInfo);
  EXPECT_EQ(it->second, get_fetch_debug_info_handler);
  it = global_dispatcher.handler_map_.find(kTypeSetFetchDebugInfo);
  EXPECT_EQ(it->second, set_fetch_debug_info_handler);
}

}  // namespace testing
}  // namespace devtool
}  // namespace lynx
