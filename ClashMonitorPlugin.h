/*
 * ClashMonitorPlugin - 插件核心实现
 * 实现 ITMPlugin 接口
 */
#pragma once
#include <afxwin.h>
#include "../include/PluginInterface.h"
#include "ClashMonitorItem.h"
#include "ClashApiClient.h"

class CClashMonitorPlugin : public ITMPlugin
{
public:
    static CClashMonitorPlugin& Instance();

    // ITMPlugin 实现
    virtual IPluginItem* GetItem(int index) override;
    virtual void DataRequired() override;
    virtual const wchar_t* GetInfo(PluginInfoIndex index) override;
    virtual OptionReturn ShowOptionsDialog(void* hParent) override;
    virtual void OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data) override;
    virtual const wchar_t* GetTooltipInfo() override;
    virtual int GetAPIVersion() const override { return 7; }

private:
    CClashMonitorPlugin();

    static CClashMonitorPlugin m_instance;

    CClashNodeItem m_node_item;
    CClashLatencyItem m_latency_item;
    CClashApiClient m_api_client;
};
