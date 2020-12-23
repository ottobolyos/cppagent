// Ensure that gtest is the first header otherwise Windows raises an error
#include <gtest/gtest.h>
// Keep this comment to keep gtest.h above. (clang-format off/on is not working here!)

#include "adapter.hpp"
#include "agent.hpp"
#include "agent_test_helper.hpp"
#include "json_helper.hpp"
#include "solid_model.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

using json = nlohmann::json;
using namespace std;
using namespace mtconnect;

class CuttingToolTest : public testing::Test
{
 protected:
  void SetUp() override
  {  // Create an agent with only 16 slots and 8 data items.
    m_agent = make_unique<Agent>(PROJECT_ROOT_DIR "/samples/solid_model.xml", 4, 4, "1.7");
    m_agentId = int64ToString(getCurrentTimeInSec());
    m_adapter = nullptr;

    m_agentTestHelper = make_unique<AgentTestHelper>();
    m_agentTestHelper->m_agent = m_agent.get();

    m_device = m_agent->getDeviceByName("LinuxCNC");
  }

  void TearDown() override
  {
    m_agent.reset();
    m_agentTestHelper.reset();
  }

  void addAdapter()
  {
    ASSERT_FALSE(m_adapter);
    m_adapter = new Adapter("LinuxCNC", "server", 7878);
    m_agent->addAdapter(m_adapter);
    ASSERT_TRUE(m_adapter);
  }
  
  std::unique_ptr<Agent> m_agent;
  std::string m_agentId;
  Device *m_device{nullptr};
  Adapter *m_adapter{nullptr};

  std::unique_ptr<AgentTestHelper> m_agentTestHelper;
};

#if 0
TEST_F(CuttingToolTest, AssetRefCounts)
{
  addAdapter();

  const auto assets = m_agent->getAssets();

  m_adapter->parseBuffer(
      R"ASSET(2018-02-19T22:54:03.0738Z|@ASSET@|M8010N9172N:1.0|CuttingTool|--multiline--SMOOTH
<CuttingTool toolId="0" serialNumber="0" removed="False" assetId="M8010N9172N:1.0"><CuttingToolLifeCycle><ToolLife type="MINUTES" countDirection="UP" initial="0" limit="">1</ToolLife><ToolLife type="PART_COUNT" countDirection="UP" initial="0" limit="">1</ToolLife><Location type="POT" positiveOverlap="0" negativeOverlap="0">1</Location><ProgramToolGroup>0</ProgramToolGroup><ProgramToolNumber>1.0</ProgramToolNumber><CutterStatus><Status>USED</Status><Status>ALLOCATED</Status><Status>AVAILABLE</Status></CutterStatus><Measurements><FunctionalLength code="LF" nominal="0">0</FunctionalLength><CuttingDiameterMax code="DC" nominal="0">200</CuttingDiameterMax></Measurements></CuttingToolLifeCycle></CuttingTool>
--multiline--SMOOTH
)ASSET");

  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  // Asset has two secondary indexes
  AssetPtr first(*(assets->front()));
  ASSERT_EQ((unsigned int)4, first.getObject()->refCount());

  m_adapter->parseBuffer(
      R"ASSET(2018-02-19T22:54:03.1749Z|@ASSET@|M8010N9172N:1.2|CuttingTool|--multiline--SMOOTH
<CuttingTool toolId="0" serialNumber="1" removed="False" assetId="M8010N9172N:1.2"><CuttingToolLifeCycle><ToolLife type="MINUTES" countDirection="UP" initial="0" limit=""></ToolLife><ToolLife type="PART_COUNT" countDirection="UP" initial="0" limit=""></ToolLife><Location type="POT" positiveOverlap="0" negativeOverlap="0">1</Location><ProgramToolGroup>0</ProgramToolGroup><ProgramToolNumber>1.2</ProgramToolNumber><CutterStatus><Status>NEW</Status><Status>ALLOCATED</Status><Status>AVAILABLE</Status></CutterStatus><Measurements><FunctionalLength code="LF" nominal="649640">649640</FunctionalLength><CuttingDiameterMax code="DC" nominal="-177708">100</CuttingDiameterMax></Measurements></CuttingToolLifeCycle></CuttingTool>
--multiline--SMOOTH
)ASSET");

  ASSERT_EQ((unsigned int)2, m_agent->getAssetCount());
  ASSERT_EQ((unsigned int)2, first.getObject()->refCount());

  m_adapter->parseBuffer(
      R"ASSET(2018-02-19T22:54:03.2760Z|@ASSET@|M8010N9172N:1.0|CuttingTool|--multiline--SMOOTH
<CuttingTool toolId="0" serialNumber="0" removed="False" assetId="M8010N9172N:1.0"><CuttingToolLifeCycle><ToolLife type="MINUTES" countDirection="UP" initial="0" limit=""></ToolLife><ToolLife type="PART_COUNT" countDirection="UP" initial="0" limit=""></ToolLife><Location type="POT" positiveOverlap="0" negativeOverlap="0">1</Location><ProgramToolGroup>0</ProgramToolGroup><ProgramToolNumber>1.0</ProgramToolNumber><CutterStatus><Status>NEW</Status><Status>ALLOCATED</Status><Status>AVAILABLE</Status></CutterStatus><Measurements><FunctionalLength code="LF" nominal="0">0</FunctionalLength><CuttingDiameterMax code="DC" nominal="0">0</CuttingDiameterMax></Measurements></CuttingToolLifeCycle></CuttingTool>
--multiline--SMOOTH
)ASSET");

  m_adapter->parseBuffer(
      R"ASSET(2018-02-19T22:54:03.3771Z|@ASSET@|M8010N9172N:2.5|CuttingTool|--multiline--SMOOTH
<CuttingTool toolId="0" serialNumber="0" removed="False" assetId="M8010N9172N:2.5"><CuttingToolLifeCycle><ToolLife type="MINUTES" countDirection="UP" initial="0" limit="">11</ToolLife><ToolLife type="PART_COUNT" countDirection="UP" initial="0" limit="">4</ToolLife><Location type="POT" positiveOverlap="0" negativeOverlap="0">2</Location><ProgramToolGroup>0</ProgramToolGroup><ProgramToolNumber>2.5</ProgramToolNumber><CutterStatus><Status>USED</Status><Status>ALLOCATED</Status><Status>AVAILABLE</Status></CutterStatus><Measurements><FunctionalLength code="LF" nominal="615207">615207</FunctionalLength><CuttingDiameterMax code="DC" nominal="-174546">200</CuttingDiameterMax></Measurements></CuttingToolLifeCycle></CuttingTool>
--multiline--SMOOTH
)ASSET");

  m_adapter->parseBuffer(
      R"ASSET(2018-02-19T22:54:03.4782Z|@ASSET@|M8010N9172N:2.2|CuttingTool|--multiline--SMOOTH
<CuttingTool toolId="0" serialNumber="0" removed="False" assetId="M8010N9172N:2.2"><CuttingToolLifeCycle><ToolLife type="MINUTES" countDirection="UP" initial="0" limit="">11</ToolLife><ToolLife type="PART_COUNT" countDirection="UP" initial="0" limit="">4</ToolLife><Location type="POT" positiveOverlap="0" negativeOverlap="0">2</Location><ProgramToolGroup>0</ProgramToolGroup><ProgramToolNumber>2.2</ProgramToolNumber><CutterStatus><Status>USED</Status><Status>ALLOCATED</Status><Status>AVAILABLE</Status></CutterStatus><Measurements><FunctionalLength code="LF" nominal="615207">615207</FunctionalLength><CuttingDiameterMax code="DC" nominal="174546">200</CuttingDiameterMax></Measurements></CuttingToolLifeCycle></CuttingTool>
--multiline--SMOOTH
)ASSET");

  // First asset should now be removed (we are holding the one ref)
  ASSERT_EQ((unsigned int)1, first.getObject()->refCount());

  // Check next asset
  AssetPtr second(*(assets->front()));
  ASSERT_EQ((unsigned int)2, second.getObject()->refCount());
  ASSERT_EQ(string("M8010N9172N:1.2"), second.getObject()->getAssetId());

  // Update the asset
  m_adapter->parseBuffer(
      R"ASSET(2018-02-19T22:54:03.1749Z|@ASSET@|M8010N9172N:1.2|CuttingTool|--multiline--SMOOTH
<CuttingTool toolId="0" serialNumber="1" removed="False" assetId="M8010N9172N:1.2"><CuttingToolLifeCycle><ToolLife type="MINUTES" countDirection="UP" initial="0" limit=""></ToolLife><ToolLife type="PART_COUNT" countDirection="UP" initial="0" limit=""></ToolLife><Location type="POT" positiveOverlap="0" negativeOverlap="0">1</Location><ProgramToolGroup>0</ProgramToolGroup><ProgramToolNumber>1.2</ProgramToolNumber><CutterStatus><Status>NEW</Status><Status>ALLOCATED</Status><Status>AVAILABLE</Status></CutterStatus><Measurements><FunctionalLength code="LF" nominal="649640">649640</FunctionalLength><CuttingDiameterMax code="DC" nominal="-177708">100</CuttingDiameterMax></Measurements></CuttingToolLifeCycle></CuttingTool>
--multiline--SMOOTH
)ASSET");

  // should be deleted
  ASSERT_EQ((unsigned int)1, second.getObject()->refCount());
}

TEST_F(CuttingToolTest, BadAsset)
{
  addAdapter();

  m_adapter->parseBuffer("TIME|@ASSET@|111|CuttingTool|--multiline--AAAA\n");
  m_adapter->parseBuffer((getFile("asset4.xml") + "\n").c_str());
  m_adapter->parseBuffer("--multiline--AAAA\n");
  ASSERT_EQ((unsigned int)0, m_agent->getAssetCount());
}

TEST_F(CuttingToolTest, AssetProbe)
{
  m_agent->enablePut();
  m_agentTestHelper->m_path = "/asset/1";
  string body = "<Part>TEST 1</Part>";
  key_value_map queries;

  queries["device"] = "LinuxCNC";
  queries["type"] = "Part";

  m_agentTestHelper->m_path = "/asset/1";
  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());
  }
  m_agentTestHelper->m_path = "/asset/2";
  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)2, m_agent->getAssetCount());
  }

  {
    m_agentTestHelper->m_path = "/probe";
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header/m:AssetCounts/m:AssetCount@assetType", "Part");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header/m:AssetCounts/m:AssetCount", "2");
  }
}

TEST_F(CuttingToolTest, AssetRemoval)
{
  m_agent->enablePut();
  m_agentTestHelper->m_path = "/asset/1";
  string body = "<Part>TEST 1</Part>";
  key_value_map queries;

  queries["device"] = "LinuxCNC";
  queries["type"] = "Part";

  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());
  ASSERT_EQ((unsigned int)0, m_agent->getAssetCount());

  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());
    ASSERT_EQ(1, m_agent->getAssetCount("Part"));
  }

  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Part", "TEST 1");
  }

  // Make sure replace works properly
  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());
    ASSERT_EQ(1, m_agent->getAssetCount("Part"));
  }

  m_agentTestHelper->m_path = "/asset/2";
  body = "<Part>TEST 2</Part>";

  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)2, m_agent->getAssetCount());
    ASSERT_EQ(2, m_agent->getAssetCount("Part"));
  }

  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "2");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Part", "TEST 2");
  }

  m_agentTestHelper->m_path = "/asset/3";
  body = "<Part>TEST 3</Part>";

  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)3, m_agent->getAssetCount());
    ASSERT_EQ(3, m_agent->getAssetCount("Part"));
  }

  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Part", "TEST 3");
  }

  m_agentTestHelper->m_path = "/asset/2";
  body = "<Part removed='true'>TEST 2</Part>";

  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)3, m_agent->getAssetCount());
    ASSERT_EQ(3, m_agent->getAssetCount("Part"));
  }

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved", "2");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved@assetType", "Part");
  }

  m_agentTestHelper->m_path = "/assets";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:Assets/*", 2);
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[2]", "TEST 1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[1]", "TEST 3");
  }

  m_agentTestHelper->m_queries["removed"] = "true";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:Assets/*", 3);
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[1]", "TEST 3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[2]", "TEST 2");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[2]@removed", "true");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[3]", "TEST 1");
  }
}

TEST_F(CuttingToolTest, AssetRemovalByAdapter)
{
  addAdapter();

  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());

  m_adapter->processData("TIME|@ASSET@|111|Part|<Part>TEST 1</Part>");
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_adapter->processData("TIME|@ASSET@|112|Part|<Part>TEST 2</Part>");
  ASSERT_EQ((unsigned int)2, m_agent->getAssetCount());

  m_adapter->processData("TIME|@ASSET@|113|Part|<Part>TEST 3</Part>");
  ASSERT_EQ((unsigned int)3, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "113");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "Part");
  }

  m_adapter->processData("TIME|@REMOVE_ASSET@|112\r");
  ASSERT_EQ((unsigned int)3, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved", "112");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved@assetType", "Part");
  }

  m_agentTestHelper->m_path = "/assets";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:Assets/*", 2);
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[2]", "TEST 1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[1]", "TEST 3");
  }

  // TODO: When asset is removed and the content is literal, it will
  // not regenerate the attributes for the asset.
  m_agentTestHelper->m_queries["removed"] = "true";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:Assets/*", 3);
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[3]", "TEST 1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[2]", "TEST 2");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[1]", "TEST 3");
  }
}

TEST_F(CuttingToolTest, AssetStorageWithoutType)
{
  m_agent->enablePut();
  m_agentTestHelper->m_path = "/asset/123";
  string body = "<Part>TEST</Part>";
  key_value_map queries;

  queries["device"] = "LinuxCNC";

  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());
  ASSERT_EQ((unsigned int)0, m_agent->getAssetCount());

  {
    PARSE_XML_RESPONSE_PUT(body, queries);
    ASSERT_EQ((unsigned int)0, m_agent->getAssetCount());
  }
}

TEST_F(CuttingToolTest, AssetAdditionOfAssetChanged12)
{
  m_agent.reset();
  m_agent = make_unique<Agent>(PROJECT_ROOT_DIR "/samples/min_config.xml", 8, 4, "1.2", 25);
  m_agentTestHelper->m_agent = m_agent.get();

  {
    m_agentTestHelper->m_path = "/LinuxCNC/probe";
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:DataItem[@type='ASSET_CHANGED']", 1);
    ASSERT_XML_PATH_EQUAL(doc, "//m:DataItem[@type='ASSET_CHANGED']@discrete", nullptr);
    ASSERT_XML_PATH_COUNT(doc, "//m:DataItem[@type='ASSET_REMOVED']", 0);
  }
}

TEST_F(CuttingToolTest, AssetAdditionOfAssetRemoved13)
{
  m_agent.reset();
  m_agent = make_unique<Agent>(PROJECT_ROOT_DIR "/samples/min_config.xml", 8, 4, "1.3", 25);
  m_agentTestHelper->m_agent = m_agent.get();

  {
    m_agentTestHelper->m_path = "/LinuxCNC/probe";
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:DataItem[@type='ASSET_CHANGED']", 1);
    ASSERT_XML_PATH_EQUAL(doc, "//m:DataItem[@type='ASSET_CHANGED']@discrete", nullptr);
    ASSERT_XML_PATH_COUNT(doc, "//m:DataItem[@type='ASSET_REMOVED']", 1);
  }
}

TEST_F(CuttingToolTest, AssetAdditionOfAssetRemoved15)
{
  m_agent.reset();
  m_agent = make_unique<Agent>(PROJECT_ROOT_DIR "/samples/min_config.xml", 8, 4, "1.5", 25);
  m_agentTestHelper->m_agent = m_agent.get();

  {
    m_agentTestHelper->m_path = "/LinuxCNC/probe";
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:DataItem[@type='ASSET_CHANGED']", 1);
    ASSERT_XML_PATH_EQUAL(doc, "//m:DataItem[@type='ASSET_CHANGED']@discrete", "true");
    ASSERT_XML_PATH_COUNT(doc, "//m:DataItem[@type='ASSET_REMOVED']", 1);
  }
}

TEST_F(CuttingToolTest, AssetPrependId)
{
  addAdapter();

  m_adapter->processData("TIME|@ASSET@|@1|Part|<Part>TEST 1</Part>");
  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/asset/0001";

  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Part", "TEST 1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Part@assetId", "0001");
  }
}

TEST_F(CuttingToolTest, AssetWithSimpleCuttingItems)
{
  XmlPrinter *printer = dynamic_cast<XmlPrinter *>(m_agent->getPrinter("xml"));
  ASSERT_TRUE(printer != nullptr);

  printer->clearAssetsNamespaces();
  printer->addAssetsNamespace("urn:machine.com:MachineAssets:1.3",
                              "http://www.machine.com/schemas/MachineAssets_1.3.xsd", "x");

  addAdapter();

  m_adapter->parseBuffer("TIME|@ASSET@|XXX.200|CuttingTool|--multiline--AAAA\n");
  m_adapter->parseBuffer((getFile("asset5.xml") + "\n").c_str());
  m_adapter->parseBuffer("--multiline--AAAA\n");
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/asset/XXX.200";

  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='1']/m:ItemLife", "0");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='1']/m:ItemLife@type", "PART_COUNT");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='1']/m:ItemLife@countDirection", "UP");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='1']/m:ItemLife@initial", "0");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='1']/m:ItemLife@limit", "0");

    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='1']/x:ItemCutterStatus/m:Status",
                          "AVAILABLE");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='2']/x:ItemCutterStatus/m:Status", "USED");

    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='4']/m:ItemLife", "0");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='4']/m:ItemLife@type", "PART_COUNT");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='4']/m:ItemLife@countDirection", "UP");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='4']/m:ItemLife@initial", "0");
    ASSERT_XML_PATH_EQUAL(doc, "//m:CuttingItem[@indices='4']/m:ItemLife@limit", "0");
  }

  printer->clearAssetsNamespaces();
}

TEST_F(CuttingToolTest, RemoveLastAssetChanged)
{
  addAdapter();

  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());

  m_adapter->processData("TIME|@ASSET@|111|Part|<Part>TEST 1</Part>");
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "111");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "Part");
  }

  m_adapter->processData("TIME|@REMOVE_ASSET@|111");
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved", "111");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved@assetType", "Part");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "UNAVAILABLE");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "Part");
  }
}

TEST_F(CuttingToolTest, RemoveAssetUsingHttpDelete)
{
  addAdapter();
  m_agent->enablePut();


  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());

  m_adapter->processData("TIME|@ASSET@|111|Part|<Part>TEST 1</Part>");
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "111");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "Part");
  }

  m_agentTestHelper->m_path = "/asset/111";
  {
    PARSE_XML_RESPONSE_DELETE;
  }
  
  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved", "111");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved@assetType", "Part");
  }
}


TEST_F(CuttingToolTest, AssetChangedWhenUnavailable)
{
  addAdapter();

  {
    m_agentTestHelper->m_path = "/current";
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "UNAVAILABLE");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved", "UNAVAILABLE");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved@assetType", "");
  }
}

TEST_F(CuttingToolTest, RemoveAllAssets)
{
  addAdapter();

  ASSERT_EQ((unsigned int)4, m_agent->getMaxAssets());

  m_adapter->processData("TIME|@ASSET@|111|Part|<Part>TEST 1</Part>");
  ASSERT_EQ((unsigned int)1, m_agent->getAssetCount());

  m_adapter->processData("TIME|@ASSET@|112|Part|<Part>TEST 2</Part>");
  ASSERT_EQ((unsigned int)2, m_agent->getAssetCount());

  m_adapter->processData("TIME|@ASSET@|113|Part|<Part>TEST 3</Part>");
  ASSERT_EQ((unsigned int)3, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "113");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "Part");
  }

  m_adapter->processData("TIME|@REMOVE_ALL_ASSETS@|Part");
  ASSERT_EQ((unsigned int)3, m_agent->getAssetCount());

  m_agentTestHelper->m_path = "/current";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved", "111");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetRemoved@assetType", "Part");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged", "UNAVAILABLE");
    ASSERT_XML_PATH_EQUAL(doc, "//m:AssetChanged@assetType", "Part");
  }

  m_agentTestHelper->m_path = "/assets";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:Assets/*", 0);
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
  }

  // TODO: When asset is removed and the content is literal, it will
  // not regenerate the attributes for the asset.
  m_agentTestHelper->m_queries["removed"] = "true";
  {
    PARSE_XML_RESPONSE;
    ASSERT_XML_PATH_COUNT(doc, "//m:Assets/*", 3);
    ASSERT_XML_PATH_EQUAL(doc, "//m:Header@assetCount", "3");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[3]", "TEST 1");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[2]", "TEST 2");
    ASSERT_XML_PATH_EQUAL(doc, "//m:Assets/*[1]", "TEST 3");
  }
}
#endif

