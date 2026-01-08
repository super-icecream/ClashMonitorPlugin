#include "pch.h"
#include "ClashApiClient.h"
#include "DataManager.h"
#include <sstream>
#include <iomanip>
#include <winhttp.h>

#pragma comment(lib, "winhttp.lib")

// 简单的文件日志
void LogDebug(const std::string& msg)
{
    FILE* fp = nullptr;
    if (fopen_s(&fp, "C:\\Clash-mm\\plugin_debug.log", "a+") == 0 && fp)
    {
        SYSTEMTIME st;
        GetLocalTime(&st);
        fprintf(fp, "[%02d:%02d:%02d.%03d] %s\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, msg.c_str());
        fclose(fp);
    }
}

// --- SimpleJson 实现 ---

// 改进的 JSON 字符串提取，支持冒号后的空格
std::wstring SimpleJson::GetString(const std::string& json, const std::string& key)
{
    std::string search = "\"" + key + "\"";
    size_t key_pos = json.find(search);
    if (key_pos == std::string::npos) return L"";
    
    // 找到冒号
    size_t col_pos = json.find(":", key_pos + search.length());
    if (col_pos == std::string::npos) return L"";
    
    // 找到值的开始引号
    size_t start = json.find("\"", col_pos + 1);
    if (start == std::string::npos) return L"";
    
    // 找到值的结束引号
    size_t end = start + 1;
    while (end < json.length()) {
        if (json[end] == '\"' && json[end-1] != '\\') break;
        end++;
    }
    if (end >= json.length()) return L"";
    
    std::string value = json.substr(start + 1, end - start - 1);
    return CClashApiClient::Utf8ToWString(value);
}

int SimpleJson::GetInt(const std::string& json, const std::string& key)
{
    std::string search = "\"" + key + "\"";
    size_t key_pos = json.find(search);
    if (key_pos == std::string::npos) return -1;
    
    size_t col_pos = json.find(":", key_pos + search.length());
    if (col_pos == std::string::npos) return -1;
    
    size_t start = col_pos + 1;
    while (start < json.length() && (isspace(json[start]))) start++;
    
    size_t end = start;
    while (end < json.length() && (isdigit(json[end]) || json[end] == '-')) 
        end++;
    
    if (start >= end) return -1;

    std::string value = json.substr(start, end - start);
    try {
        return std::stoi(value);
    } catch (...) {
        return -1;
    }
}

// --- CClashApiClient 实现 ---

CClashApiClient::CClashApiClient()
{
    // 初始化 Session - 强制直连，不使用代理，避免回环
    m_hSession = WinHttpOpen(L"ClashMonitorPlugin/1.0", 
                           WINHTTP_ACCESS_TYPE_NO_PROXY,
                           WINHTTP_NO_PROXY_NAME, 
                           WINHTTP_NO_PROXY_BYPASS, 0);
}

CClashApiClient::~CClashApiClient()
{
    if (m_hSession)
        WinHttpCloseHandle(m_hSession);
}

std::wstring CClashApiClient::Utf8ToWString(const std::string& str)
{
    if (str.empty()) return L"";
    int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
    return wstrTo;
}

std::string CClashApiClient::WStringToUtf8(const std::wstring& str)
{
    if (str.empty()) return "";
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, &str[0], (int)str.size(), &strTo[0], size_needed, NULL, NULL);
    return strTo;
}

std::wstring CClashApiClient::UrlEncode(const std::wstring& str)
{
    std::string utf8 = WStringToUtf8(str);
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (char c : utf8) {
        if (isalnum((unsigned char)c) || c == '-' || c == '_' || c == '.' || c == '~') {
            escaped << c;
        } else {
            escaped << std::uppercase;
            escaped << '%' << std::setw(2) << int((unsigned char)c);
            escaped << std::nouppercase;
        }
    }
    return Utf8ToWString(escaped.str());
}

std::string CClashApiClient::HttpGet(const std::wstring& full_url, int timeout_ms)
{
    if (!m_hSession) {
        LogDebug("Error: WinHttp Session is invalid");
        return "";
    }

    // 可选：记录请求URL (如果日志太多可注释掉)
    // LogDebug("Requesting: " + WStringToUtf8(full_url));

    URL_COMPONENTS urlComp;
    ZeroMemory(&urlComp, sizeof(urlComp));
    urlComp.dwStructSize = sizeof(urlComp);
    urlComp.dwSchemeLength    = (DWORD)-1;
    urlComp.dwHostNameLength  = (DWORD)-1;
    urlComp.dwUrlPathLength   = (DWORD)-1;
    urlComp.dwExtraInfoLength = (DWORD)-1;

    wchar_t* urlBuffer = _wcsdup(full_url.c_str());
    if (!WinHttpCrackUrl(urlBuffer, (DWORD)wcslen(urlBuffer), 0, &urlComp))
    {
        LogDebug("Error: WinHttpCrackUrl failed");
        free(urlBuffer);
        return "";
    }

    std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
    std::wstring urlPath(urlComp.lpszUrlPath, urlComp.dwUrlPathLength);
    std::wstring extraInfo(urlComp.lpszExtraInfo, urlComp.dwExtraInfoLength);
    std::wstring fullPath = urlPath + extraInfo;
    if (fullPath.empty()) fullPath = L"/";

    // Debug: Check Host and Port
    LogDebug("Connecting to: " + WStringToUtf8(hostName) + " Port: " + std::to_string(urlComp.nPort));

    HINTERNET hConnect = WinHttpConnect(m_hSession, hostName.c_str(), urlComp.nPort, 0);
    if (!hConnect)
    {
        LogDebug("Error: WinHttpConnect failed");
        free(urlBuffer);
        return "";
    }

    HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", fullPath.c_str(),
                                          NULL, WINHTTP_NO_REFERER, 
                                          WINHTTP_DEFAULT_ACCEPT_TYPES, 
                                          WINHTTP_FLAG_REFRESH); 
    
    std::string responseData;
    if (hRequest)
    {
        WinHttpSetTimeouts(hRequest, timeout_ms, timeout_ms, timeout_ms, timeout_ms);

        std::wstring secret = g_data.m_config.secret;
        if (!secret.empty())
        {
            std::wstring header = L"Authorization: Bearer " + secret;
            if (!WinHttpAddRequestHeaders(hRequest, header.c_str(), (DWORD)-1L, WINHTTP_ADDREQ_FLAG_ADD)) {
                LogDebug("Warning: Failed to add Auth header");
            }
        }

        if (WinHttpSendRequest(hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0,
                             WINHTTP_NO_REQUEST_DATA, 0, 
                             0, 0))
        {
            if (WinHttpReceiveResponse(hRequest, NULL))
            {
                DWORD dwStatusCode = 0;
                DWORD dwSize = sizeof(dwStatusCode);
                WinHttpQueryHeaders(hRequest, WINHTTP_QUERY_STATUS_CODE | WINHTTP_QUERY_FLAG_NUMBER, 
                                  WINHTTP_HEADER_NAME_BY_INDEX, &dwStatusCode, &dwSize, WINHTTP_NO_HEADER_INDEX);

                if (dwStatusCode == 200)
                {
                    DWORD dwDownloaded = 0;
                    do 
                    {
                        dwSize = 0;
                        if (!WinHttpQueryDataAvailable(hRequest, &dwSize)) break;
                        if (dwSize == 0) break;

                        char* pszOutBuffer = new char[dwSize + 1];
                        if (!pszOutBuffer) break;
                        ZeroMemory(pszOutBuffer, dwSize + 1);

                        if (WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
                        {
                            responseData.append(pszOutBuffer, dwDownloaded);
                        }
                        delete[] pszOutBuffer;
                    } while (dwSize > 0);
                }
                else
                {
                    LogDebug("HTTP Status Error: " + std::to_string(dwStatusCode));
                }
            }
            else
            {
                LogDebug("Error: WinHttpReceiveResponse failed");
            }
        }
        else
        {
             LogDebug("Error: WinHttpSendRequest failed. Error Code: " + std::to_string(GetLastError()));
        }
        WinHttpCloseHandle(hRequest);
    }
    else
    {
        LogDebug("Error: WinHttpOpenRequest failed");
    }

    WinHttpCloseHandle(hConnect);
    free(urlBuffer);

    return responseData;
}

std::wstring CClashApiClient::SimplifyNodeName(const std::wstring& name)
{
    std::wstring simplified = name;
    
    // 移除常见前缀
    const wchar_t* prefixes[] = { L"[", L"【" };
    const wchar_t* suffixes[] = { L"]", L"】" };
    
    for (int i = 0; i < 2; ++i)
    {
        size_t start = simplified.find(prefixes[i]);
        if (start != std::wstring::npos)
        {
            size_t end = simplified.find(suffixes[i], start);
            if (end != std::wstring::npos)
            {
                simplified = simplified.substr(end + 1);
                // 去除开头的空格
                size_t first_char = simplified.find_first_not_of(L" ");
                if (first_char != std::wstring::npos)
                    simplified = simplified.substr(first_char);
            }
        }
    }

    if (g_data.m_config.short_node_name && simplified.length() > g_data.m_config.max_node_length)
    {
        simplified = simplified.substr(0, g_data.m_config.max_node_length) + L"..";
    }
    
    return simplified;
}

// 辅助函数：查找并提取所有 Selector 类型的组及其当前选择
// 辅助函数：查找并提取所有 Selector 类型的组及其当前选择
std::wstring CClashApiClient::GetCurrentNode(const std::string& proxies_json)
{
    // 暴力启发式搜索：
    // 查找所有 "now": "Value" 
    // 如果 Value 不是 "DIRECT" 也不包含 "REJECT"，就返回它。
    
    size_t pos = 0;
    while ((pos = proxies_json.find("\"now\"", pos)) != std::string::npos)
    {
        // 找到 "now"，提取后面的值
        // 格式可能是 "now": "Value" 或 "now":"Value"
        
        size_t col_pos = proxies_json.find(":", pos + 5);
        if (col_pos != std::string::npos)
        {
            size_t start = proxies_json.find("\"", col_pos + 1);
            if (start != std::string::npos)
            {
                size_t end = start + 1;
                while (end < proxies_json.length()) {
                     if (proxies_json[end] == '\"' && proxies_json[end-1] != '\\') break;
                     end++;
                }
                
                if (end < proxies_json.length())
                {
                    std::string val = proxies_json.substr(start + 1, end - start - 1);
                    
                    // 过滤掉系统保留值
                    if (val != "DIRECT" && val != "REJECT" && val != "GLOBAL" && val != "Compatible")
                    {
                        // 找到一个有效的节点名！
                        return Utf8ToWString(val);
                    }
                }
            }
        }
        pos += 5;
    }

    // 如果没找到任何有效节点，但有 DIRECT，就返回 DIRECT
    if (proxies_json.find("\"now\": \"DIRECT\"") != std::string::npos || 
        proxies_json.find("\"now\":\"DIRECT\"") != std::string::npos)
    {
        return L"DIRECT";
    }

    return L"";
}

int CClashApiClient::TestLatency(const std::wstring& node_name)
{
    if (node_name.empty() || node_name == L"DIRECT") return -1;
    
    std::wstring encoded_name = UrlEncode(node_name);
    std::wstring url = g_data.m_config.api_url + L"/proxies/" + encoded_name + 
                       L"/delay?url=http://www.gstatic.com/generate_204&timeout=5000";

    std::string resp = HttpGet(url, 5500);  // 5.5 秒超时
    if (!resp.empty())
    {
        return SimpleJson::GetInt(resp, "delay");
    }
    return -1;
}

// 从活跃连接中获取当前使用的节点（最准确的方法）
std::wstring CClashApiClient::GetNodeFromConnections()
{
    std::wstring url = g_data.m_config.api_url + L"/connections";
    std::string resp = HttpGet(url, g_data.m_config.timeout_ms);
    
    if (resp.empty()) return L"";
    
    // 解析 JSON，查找 "chains" 数组
    // 找到第一个非 DIRECT 的 chains[0]
    
    size_t pos = 0;
    while ((pos = resp.find("\"chains\"", pos)) != std::string::npos)
    {
        // 定位到 "[" 
        size_t arr_start = resp.find("[", pos);
        if (arr_start == std::string::npos) { pos += 8; continue; }
        
        // 找到第一个元素 (第一个引号内的内容)
        size_t first_quote = resp.find("\"", arr_start + 1);
        if (first_quote == std::string::npos) { pos += 8; continue; }
        
        size_t second_quote = resp.find("\"", first_quote + 1);
        if (second_quote == std::string::npos) { pos += 8; continue; }
        
        std::string node = resp.substr(first_quote + 1, second_quote - first_quote - 1);
        
        // 过滤掉系统保留值
        if (node != "DIRECT" && node != "REJECT" && !node.empty())
        {
            return Utf8ToWString(node);  // 找到了！返回节点名
        }
        
        pos = second_quote;  // 继续找下一个
    }
    
    return L"";  // 没找到代理连接
}

void CClashApiClient::UpdateStatus()
{
    // 方法一：从活跃连接中获取（最准确）
    std::wstring node_name = GetNodeFromConnections();
    
    // 如果没有活跃的代理连接，回退到 /proxies 方式
    if (node_name.empty())
    {
        std::wstring url_proxies = g_data.m_config.api_url + L"/proxies";
        std::string resp = HttpGet(url_proxies, g_data.m_config.timeout_ms);
        if (!resp.empty())
        {
            node_name = GetCurrentNode(resp);  // 旧的逻辑作为回退
        }
    }
    
    // 更新显示数据
    if (!node_name.empty())
    {
        g_data.m_data.node_name = node_name;
        g_data.m_data.node_display = SimplifyNodeName(node_name);
        g_data.m_data.connected = true;
        
        // 测试延迟
        int latency = TestLatency(node_name);
        g_data.m_data.latency = latency;
        
        // 更新延迟文本
        if (latency >= 0)
            g_data.m_data.latency_text = std::to_wstring(latency) + L" ms";
        else
            g_data.m_data.latency_text = L"--";
    }
    else
    {
        g_data.m_data.node_name = L"无代理连接";
        g_data.m_data.node_display = L"待机";
        g_data.m_data.latency = -1;
        g_data.m_data.latency_text = L"--";
        g_data.m_data.connected = true;
    }
}

