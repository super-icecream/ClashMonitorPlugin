/*
 * ClashMonitorPlugin - 数据管理器实现
 */
#include "pch.h"
#include "DataManager.h"
#include "resource.h"

CDataManager CDataManager::m_instance;

CDataManager::CDataManager()
{
}

CDataManager::~CDataManager()
{
    SaveConfig();
}

CDataManager& CDataManager::Instance()
{
    return m_instance;
}

void CDataManager::LoadConfig(const std::wstring& config_dir)
{
    // 获取DLL模块路径
    HMODULE hModule = reinterpret_cast<HMODULE>(&__ImageBase);
    wchar_t path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);
    std::wstring module_path = path;
    m_config_path = module_path;

    // 如果提供了配置目录，使用它
    if (!config_dir.empty())
    {
        size_t index = module_path.find_last_of(L"\\/");
        std::wstring module_file_name = module_path.substr(index + 1);
        m_config_path = config_dir + module_file_name;
    }
    m_config_path += L".ini";

    // 读取配置
    wchar_t buffer[1024];
    
    // API 配置
    GetPrivateProfileStringW(L"clash", L"api_url", L"http://127.0.0.1:9090", 
                             buffer, 1024, m_config_path.c_str());
    m_config.api_url = buffer;

    GetPrivateProfileStringW(L"clash", L"secret", L"", 
                             buffer, 1024, m_config_path.c_str());
    m_config.secret = buffer;

    m_config.timeout_ms = GetPrivateProfileIntW(L"clash", L"timeout_ms", 3000, m_config_path.c_str());
    m_config.max_latency = GetPrivateProfileIntW(L"clash", L"max_latency", 2000, m_config_path.c_str());

    // 显示配置
    m_config.show_node_name = GetPrivateProfileIntW(L"display", L"show_node_name", 1, m_config_path.c_str()) != 0;
    m_config.show_latency = GetPrivateProfileIntW(L"display", L"show_latency", 1, m_config_path.c_str()) != 0;
    m_config.short_node_name = GetPrivateProfileIntW(L"display", L"short_node_name", 1, m_config_path.c_str()) != 0;
    m_config.max_node_length = GetPrivateProfileIntW(L"display", L"max_node_length", 12, m_config_path.c_str());

    // 代理组配置
    GetPrivateProfileStringW(L"clash", L"proxy_groups", L"Proxy,GLOBAL", 
                             buffer, 1024, m_config_path.c_str());
    m_config.proxy_groups.clear();
    std::wstring groups_str = buffer;
    size_t pos = 0;
    while ((pos = groups_str.find(L",")) != std::wstring::npos)
    {
        std::wstring group = groups_str.substr(0, pos);
        if (!group.empty())
            m_config.proxy_groups.push_back(group);
        groups_str.erase(0, pos + 1);
    }
    if (!groups_str.empty())
        m_config.proxy_groups.push_back(groups_str);
}

static void WritePrivateProfileIntW(const wchar_t* app_name, const wchar_t* key_name, 
                                    int value, const wchar_t* file_path)
{
    wchar_t buff[32];
    swprintf_s(buff, L"%d", value);
    WritePrivateProfileStringW(app_name, key_name, buff, file_path);
}

void CDataManager::SaveConfig() const
{
    if (m_config_path.empty())
        return;

    // API 配置
    WritePrivateProfileStringW(L"clash", L"api_url", m_config.api_url.c_str(), m_config_path.c_str());
    WritePrivateProfileStringW(L"clash", L"secret", m_config.secret.c_str(), m_config_path.c_str());
    WritePrivateProfileIntW(L"clash", L"timeout_ms", m_config.timeout_ms, m_config_path.c_str());
    WritePrivateProfileIntW(L"clash", L"max_latency", m_config.max_latency, m_config_path.c_str());

    // 显示配置
    WritePrivateProfileIntW(L"display", L"show_node_name", m_config.show_node_name ? 1 : 0, m_config_path.c_str());
    WritePrivateProfileIntW(L"display", L"show_latency", m_config.show_latency ? 1 : 0, m_config_path.c_str());
    WritePrivateProfileIntW(L"display", L"short_node_name", m_config.short_node_name ? 1 : 0, m_config_path.c_str());
    WritePrivateProfileIntW(L"display", L"max_node_length", m_config.max_node_length, m_config_path.c_str());

    // 代理组
    std::wstring groups_str;
    for (size_t i = 0; i < m_config.proxy_groups.size(); ++i)
    {
        if (i > 0) groups_str += L",";
        groups_str += m_config.proxy_groups[i];
    }
    WritePrivateProfileStringW(L"clash", L"proxy_groups", groups_str.c_str(), m_config_path.c_str());
}

const CString& CDataManager::StringRes(UINT id)
{
    auto iter = m_string_table.find(id);
    if (iter != m_string_table.end())
    {
        return iter->second;
    }
    else
    {
        AFX_MANAGE_STATE(AfxGetStaticModuleState());
        m_string_table[id].LoadString(id);
        return m_string_table[id];
    }
}
