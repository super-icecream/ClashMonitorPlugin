# ClashMonitorPlugin

A TrafficMonitor plugin to display Clash proxy node and latency information on the Windows taskbar.

[English](#english) | [中文](#中文)

---

## English

### Features

- Display current active proxy node name
- Display real-time latency (ping)
- Auto-detect active connections via Clash API
- Configurable update interval (default: 3 seconds)

### Requirements

- Windows 10/11
- [TrafficMonitor](https://github.com/zhongyang219/TrafficMonitor) v1.85.1 or later (x64)
- Clash/Clash Meta/Mihomo with RESTful API enabled

### Installation

1. Download `ClashMonitorPlugin.dll` from [Releases](../../releases)
2. Copy the DLL to `TrafficMonitor\plugins\` folder
3. Restart TrafficMonitor
4. Right-click on TrafficMonitor → Display Settings → Enable "Clash 节点" and "Clash 延迟"

### Configuration

The plugin will automatically create a configuration file `ClashMonitorPlugin.dll.ini` in the plugins folder.

```ini
[clash]
api_url=http://127.0.0.1:2831    ; Clash API address
secret=your_api_secret           ; API secret (if configured)
timeout_ms=3000                  ; API request timeout
max_latency=2000                 ; Maximum latency display value

[display]
show_node_name=1                 ; Show node name
show_latency=1                   ; Show latency
short_node_name=1                ; Use shortened node name
max_node_length=12               ; Maximum node name length
```

### Building from Source

1. Open `ClashMonitorPlugin.sln` in Visual Studio 2022
2. Select `Release` configuration and `x64` platform
3. Build the solution

### License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

This project uses the TrafficMonitor Plugin Interface, which is licensed under the Anti-996 License.
See [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md) for third-party licenses.

---

## 中文

### 功能特性

- 显示当前使用的代理节点名称
- 显示实时延迟（ping）
- 通过 Clash API 自动检测活跃连接
- 可配置的更新间隔（默认：3 秒）

### 系统要求

- Windows 10/11
- [TrafficMonitor](https://github.com/zhongyang219/TrafficMonitor) v1.85.1 或更高版本 (x64)
- Clash/Clash Meta/Mihomo 并启用 RESTful API

### 安装方法

1. 从 [Releases](../../releases) 下载 `ClashMonitorPlugin.dll`
2. 将 DLL 复制到 `TrafficMonitor\plugins\` 文件夹
3. 重启 TrafficMonitor
4. 右键点击 TrafficMonitor → 显示设置 → 启用"Clash 节点"和"Clash 延迟"

### 配置说明

插件会自动在 plugins 文件夹中创建配置文件 `ClashMonitorPlugin.dll.ini`。

```ini
[clash]
api_url=http://127.0.0.1:2831    ; Clash API 地址
secret=your_api_secret           ; API 密钥（如果配置了的话）
timeout_ms=3000                  ; API 请求超时时间
max_latency=2000                 ; 最大延迟显示值

[display]
show_node_name=1                 ; 显示节点名称
show_latency=1                   ; 显示延迟
short_node_name=1                ; 使用简短节点名
max_node_length=12               ; 节点名称最大长度
```

### 从源码构建

1. 使用 Visual Studio 2022 打开 `ClashMonitorPlugin.sln`
2. 选择 `Release` 配置和 `x64` 平台
3. 生成解决方案

### 许可证

本项目采用 MIT 许可证 - 详见 [LICENSE](LICENSE) 文件。

本项目使用了 TrafficMonitor 的插件接口，该接口采用 Anti-996 许可证。
第三方许可证详见 [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md)。

---

## Acknowledgments

- [TrafficMonitor](https://github.com/zhongyang219/TrafficMonitor) by zhongyang219
- [Clash](https://github.com/Dreamacro/clash) / [Clash Meta](https://github.com/MetaCubeX/mihomo)
