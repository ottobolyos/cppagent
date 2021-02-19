// Ensure that gtest is the first header otherwise Windows raises an error
#include <gtest/gtest.h>
// Keep this comment to keep gtest.h above. (clang-format off/on is not working here!)

#include "adapter/adapter.hpp"
#include "agent.hpp"
#include "agent_test_helper.hpp"
#include "json_helper.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using json = nlohmann::json;
using namespace std;
using namespace mtconnect;
using namespace mtconnect::adapter;

class SensorConfigurationTest : public testing::Test
{
protected:
  void SetUp() override
  {  // Create an agent with only 16 slots and 8 data items.
    m_agentTestHelper = make_unique<AgentTestHelper>();
    m_agentTestHelper->createAgent("/samples/configuration.xml", 8, 4, "1.6", 25);
    m_agentId = to_string(getCurrentTimeInSec());
    m_device = m_agentTestHelper->m_agent->getDeviceByName("LinuxCNC");
  }

  void TearDown() override { m_agentTestHelper.reset(); }

  Adapter *m_adapter{nullptr};
  std::string m_agentId;
  Device *m_device{nullptr};
  std::unique_ptr<AgentTestHelper> m_agentTestHelper;
};

TEST_F(SensorConfigurationTest, ParseSensorConfiguration)
{
  ASSERT_NE(nullptr, m_device);

  auto &configurations_list = m_device->getConfiguration();

  ASSERT_TRUE(!configurations_list.empty());

  auto configurations_entity = configurations_list.front()->getEntity();

  auto &configuration = configurations_entity->get<entity::EntityList>("LIST");

  ASSERT_EQ(2, configuration.size());
  
  auto config = configuration.begin();
  config++;
  
  EXPECT_EQ("SensorConfiguration", (*config)->getName());

  auto channels = (*config)->getList("Channels");
  auto channel = channels->front();

  EXPECT_EQ("A/D:1", get<string>(channel->getProperty("name")));  
}

