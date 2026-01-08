/*
 * ClashMonitorPlugin - 插件显示项目实现
 */
#include "pch.h"
#include "ClashMonitorItem.h"

// --- CClashNodeItem ---

const wchar_t* CClashNodeItem::GetItemName() const
{
    return L"Clash 节点";
}

const wchar_t* CClashNodeItem::GetItemId() const
{
    return L"clash_node";
}

const wchar_t* CClashNodeItem::GetItemLableText() const
{
    return L"节点";
}

const wchar_t* CClashNodeItem::GetItemValueText() const
{
    return g_data.m_data.node_display.c_str();
}

const wchar_t* CClashNodeItem::GetItemValueSampleText() const
{
    return L"香港节点 01";
}

// --- CClashLatencyItem ---

const wchar_t* CClashLatencyItem::GetItemName() const
{
    return L"Clash 延迟";
}

const wchar_t* CClashLatencyItem::GetItemId() const
{
    return L"clash_latency";
}

const wchar_t* CClashLatencyItem::GetItemLableText() const
{
    return L"延迟";
}

const wchar_t* CClashLatencyItem::GetItemValueText() const
{
    return g_data.m_data.latency_text.c_str();
}

const wchar_t* CClashLatencyItem::GetItemValueSampleText() const
{
    return L"100ms";
}

int CClashLatencyItem::IsDrawResourceUsageGraph() const
{
    return 1; // 允许绘制图表
}

float CClashLatencyItem::GetResourceUsageGraphValue() const
{
    // 将延迟映射到 0.0 - 1.0 (例如 0-1000ms)
    int lat = g_data.m_data.latency;
    if (lat < 0) return 0.0f;
    if (lat > 1000) return 1.0f;
    return (float)lat / 500.0f; // 500ms 满格
}
