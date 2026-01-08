/*
 * ClashMonitorPlugin - 插件核心实现
 */
#include "pch.h"
#include "ClashMonitorPlugin.h"
#include "DataManager.h"
//#include "OptionsDlg.h"

CClashMonitorPlugin CClashMonitorPlugin::m_instance;

CClashMonitorPlugin::CClashMonitorPlugin()
{
}

CClashMonitorPlugin& CClashMonitorPlugin::Instance()
{
    return m_instance;
}

IPluginItem* CClashMonitorPlugin::GetItem(int index)
{
    switch (index)
    {
    case 0:
        return &m_node_item;
    case 1:
        return &m_latency_item;
    default:
        return nullptr;
    }
}

void CClashMonitorPlugin::DataRequired()
{
    // 节流检测：每 3 秒才真正更新一次数据
    static DWORD last_update_time = 0;
    DWORD current_time = GetTickCount();
    
    // 3000 毫秒 = 3 秒
    if (current_time - last_update_time < 3000 && last_update_time != 0)
    {
        return;  // 距离上次更新不足 3 秒，跳过
    }
    
    last_update_time = current_time;
    
    // 更新数据
    // 注意：DataRequired 在后台线程调用，不可以操作 UI
    m_api_client.UpdateStatus();
}

const wchar_t* CClashMonitorPlugin::GetInfo(PluginInfoIndex index)
{
    switch (index)
    {
    case TMI_NAME:
        return L"Clash Monitor";
    case TMI_DESCRIPTION:
        return L"Clash 节点和延迟监控";
    case TMI_AUTHOR:
        return L"Clash-mm";
    case TMI_COPYRIGHT:
        return L"Copyright (C) 2026";
    case TMI_VERSION:
        return L"1.0";
    case TMI_URL:
        return L"https://github.com/Dreamacro/clash";
    default:
        return L"";
    }
}

ITMPlugin::OptionReturn CClashMonitorPlugin::ShowOptionsDialog(void* hParent)
{
    // TODO: 实现选项对话框
    // AFX_MANAGE_STATE(AfxGetStaticModuleState());
    // COptionsDlg dlg(CWnd::FromHandle((HWND)hParent));
    // if (dlg.DoModal() == IDOK)
    //     return ITMPlugin::OR_OPTION_CHANGED;
    return ITMPlugin::OR_OPTION_UNCHANGED;
}

void CClashMonitorPlugin::OnExtenedInfo(ExtendedInfoIndex index, const wchar_t* data)
{
    if (index == EI_CONFIG_DIR)
    {
        // 加载配置
        g_data.LoadConfig(data);
    }
}

const wchar_t* CClashMonitorPlugin::GetTooltipInfo()
{
    static std::wstring tip;
    tip = L"Clash 监控\n";
    tip += L"节点: " + g_data.m_data.node_name + L"\n";
    tip += L"延迟: " + g_data.m_data.latency_text;
    return tip.c_str();
}

// 导出函数
ITMPlugin* TMPluginGetInstance()
{
    AFX_MANAGE_STATE(AfxGetStaticModuleState());
    return &CClashMonitorPlugin::Instance();
}
