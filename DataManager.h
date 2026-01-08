/*
 * ClashMonitorPlugin - 数据管理器
 * 管理插件配置和运行时数据
 */
#pragma once
#include <string>
#include <vector>
#include <map>

// 外部图像基地址，用于获取DLL路径
EXTERN_C IMAGE_DOS_HEADER __ImageBase;

// 插件配置
struct ClashConfig
{
    std::wstring api_url = L"http://127.0.0.1:2831";    // Clash API 地址
    std::wstring secret;                                  // API 密钥
    int timeout_ms = 3000;                                // 请求超时(毫秒)
    int max_latency = 2000;                               // 最大延迟(毫秒)
    bool show_node_name = true;                           // 显示节点名称
    bool show_latency = true;                             // 显示延迟
    bool short_node_name = true;                          // 使用简短节点名
    int max_node_length = 12;                             // 节点名称最大长度
    std::vector<std::wstring> proxy_groups = { L"Proxy", L"GLOBAL" };  // 代理组
};

// 运行时数据
struct ClashData
{
    std::wstring node_name = L"--";         // 当前节点名称
    std::wstring node_display = L"--";      // 显示用的节点名称(可能被截断)
    int latency = -1;                        // 延迟(ms), -1表示未知
    std::wstring latency_text = L"--";       // 延迟文本
    bool connected = false;                  // 是否已连接
};

// 数据管理器单例
class CDataManager
{
public:
    static CDataManager& Instance();
    ~CDataManager();

    // 配置
    void LoadConfig(const std::wstring& config_dir);
    void SaveConfig() const;

    // 字符串资源
    const CString& StringRes(UINT id);

    // 公共数据
    ClashConfig m_config;
    ClashData m_data;
    std::wstring m_config_path;

private:
    CDataManager();
    static CDataManager m_instance;
    std::map<UINT, CString> m_string_table;
};

// 全局访问
#define g_data CDataManager::Instance()
