/*
 * ClashMonitorPlugin - Clash API 客户端
 * 负责与 Clash RESTful API 通信
 */
#pragma once
#include <string>
#include <winhttp.h>

// 简单的 JSON 解析辅助类
class SimpleJson
{
public:
    static std::wstring GetString(const std::string& json, const std::string& key);
    static int GetInt(const std::string& json, const std::string& key);
};

// Clash API 客户端
class CClashApiClient
{
public:
    CClashApiClient();
    ~CClashApiClient();

    // 更新状态（主入口）
    void UpdateStatus();

    // 静态工具函数
    static std::wstring Utf8ToWString(const std::string& str);
    static std::string WStringToUtf8(const std::wstring& str);
    static std::wstring UrlEncode(const std::wstring& str);

private:
    // HTTP 请求
    std::string HttpGet(const std::wstring& url, int timeout_ms);

    // 从活跃连接中获取当前节点（新方法，最准确）
    std::wstring GetNodeFromConnections();

    // 从代理列表中获取当前节点（回退方法）
    std::wstring GetCurrentNode(const std::string& proxies_json);

    // 简化节点名称
    std::wstring SimplifyNodeName(const std::wstring& name);

    // 测试延迟
    int TestLatency(const std::wstring& node_name);

    HINTERNET m_hSession;
};
