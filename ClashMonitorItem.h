/*
 * ClashMonitorPlugin - 插件显示项目
 * 实现 IPluginItem 接口
 */
#pragma once
#include "../include/PluginInterface.h"
#include "DataManager.h"

// 节点名称显示
class CClashNodeItem : public IPluginItem
{
public:
    const wchar_t* GetItemName() const override;
    const wchar_t* GetItemId() const override;
    const wchar_t* GetItemLableText() const override;
    const wchar_t* GetItemValueText() const override;
    const wchar_t* GetItemValueSampleText() const override;
};

// 延迟显示
class CClashLatencyItem : public IPluginItem
{
public:
    const wchar_t* GetItemName() const override;
    const wchar_t* GetItemId() const override;
    const wchar_t* GetItemLableText() const override;
    const wchar_t* GetItemValueText() const override;
    const wchar_t* GetItemValueSampleText() const override;

    // 支持资源占用图 (显示延迟条)
    int IsDrawResourceUsageGraph() const override;
    float GetResourceUsageGraphValue() const override;
};
