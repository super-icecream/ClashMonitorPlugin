# ClashMonitorPlugin

**TrafficMonitor 插件：在 Windows 任务栏实时显示 Clash 当前代理节点和延迟**

[![GitHub Release](https://img.shields.io/github/v/release/super-icecream/YASB-clash)](https://github.com/super-icecream/YASB-clash/releases)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

---

## 📖 项目说明

本项目是一个为 [TrafficMonitor](https://github.com/zhongyang219/TrafficMonitor) 开发的**第三方插件**。

**重要声明**：
- 本插件是基于 TrafficMonitor 提供的**公开插件接口 (Plugin Interface)** 独立开发的扩展功能
- 本项目**不是** TrafficMonitor 的分支或修改版本
- 本项目**不包含** TrafficMonitor 的任何源代码（仅使用其公开的插件接口头文件）
- 本项目遵循 TrafficMonitor 的 Anti-996 许可证要求，在 [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md) 中完整保留了其版权声明

---

## ✨ 功能特性

- 📍 **显示当前代理节点** - 实时显示正在使用的 Clash 代理节点名称
- ⚡ **显示实时延迟** - 显示当前节点的延迟（ping）
- 🔄 **自动检测** - 通过 Clash API 自动检测活跃的代理连接
- ⏱️ **低资源占用** - 每 3 秒更新一次，不频繁请求 API

---

## 📋 系统要求

- **操作系统**: Windows 10/11 (64 位)
- **TrafficMonitor**: [v1.85.1](https://github.com/zhongyang219/TrafficMonitor/releases) 或更高版本 (x64)
- **Clash 客户端**: Clash/Clash Meta/Mihomo，并启用 RESTful API

---

## 📥 安装教程

### 步骤 1：下载插件

1. 前往 [Releases 页面](https://github.com/super-icecream/YASB-clash/releases)
2. 下载最新版本的 `ClashMonitorPlugin.dll`

### 步骤 2：安装插件

1. 找到你的 TrafficMonitor 安装目录
2. 在安装目录下找到（或创建）`plugins` 文件夹
   ```
   TrafficMonitor/
   ├── TrafficMonitor.exe
   ├── plugins/                 ← 将 DLL 放在这里
   │   └── ClashMonitorPlugin.dll
   └── ...
   ```
3. 将下载的 `ClashMonitorPlugin.dll` 复制到 `plugins` 文件夹中

### 步骤 3：重启 TrafficMonitor

1. 如果 TrafficMonitor 正在运行，请右键点击任务栏图标 → **退出**
2. 重新启动 TrafficMonitor

### 步骤 4：启用插件显示项

1. 右键点击 TrafficMonitor（任务栏或悬浮窗）
2. 选择 **显示设置** 或 **选项**
3. 在显示项目中找到并勾选：
   - ✅ **Clash 节点**
   - ✅ **Clash 延迟**
4. 点击确定

### 步骤 5：配置 Clash API

插件首次运行时会自动创建配置文件 `ClashMonitorPlugin.dll.ini`（位于 `plugins` 文件夹中）。

**你需要根据自己的 Clash 配置修改此文件：**

```ini
[clash]
api_url=http://127.0.0.1:9090    ; 改成你的 Clash API 地址和端口
secret=your_api_secret           ; 改成你的 API 密钥（如果设置了的话）
timeout_ms=3000                  ; API 请求超时时间（毫秒）
max_latency=2000                 ; 最大延迟显示值（毫秒）

[display]
show_node_name=1                 ; 是否显示节点名称
show_latency=1                   ; 是否显示延迟
short_node_name=1                ; 使用简短节点名
max_node_length=12               ; 节点名称最大长度
```

> **如何找到 Clash 的 API 地址和密钥？**
> 
> 打开你的 Clash 配置文件（通常是 `config.yaml`），找到如下配置：
> ```yaml
> external-controller: 127.0.0.1:9090
> secret: "your-secret-key"
> ```

<p align="center">
  <img src="https://github.com/user-attachments/assets/8c978518-fe45-4c89-92bc-6b92e2c960a9" alt="Clash配置示例" style="max-width: 100%; height: auto;" />
</p>

<p align="center">
  <img src="https://github.com/user-attachments/assets/8bc88714-576c-44ff-9a33-3e6a2471030c" alt="效果展示" style="max-width: 100%; height: auto;" />
</p>

---

## 🔧 从源码构建

### 环境要求

- Visual Studio 2022（需安装 C++ 桌面开发工作负载和 MFC 组件）
- Windows SDK

### 构建步骤

1. 克隆仓库
   ```bash
   git clone https://github.com/super-icecream/YASB-clash.git
   cd YASB-clash
   ```

2. 使用 Visual Studio 打开 `ClashMonitorPlugin.vcxproj`

3. 选择配置：
   - 配置：`Release`
   - 平台：`x64`

4. 生成解决方案（Ctrl + Shift + B）

5. 编译好的 DLL 位于 `x64/Release/ClashMonitorPlugin.dll`

---

## ❓ 常见问题

### Q: 显示"API连接失败"或"API错误"
**A**: 
1. 检查 Clash 是否正在运行
2. 检查配置文件中的 `api_url` 是否正确
3. 检查防火墙是否阻止了连接

### Q: 显示"待机"或"无代理连接"
**A**: 这表示当前没有活跃的代理连接。尝试访问一个需要代理的网站，然后等待 3 秒刷新。

### Q: 节点名称显示乱码
**A**: 确保 Clash API 返回的是 UTF-8 编码的数据。

### Q: 延迟显示"--"
**A**: 这可能是因为延迟测试超时或节点暂时无法连接。

---

## 🙏 致谢

本项目的实现离不开以下优秀的开源项目：

### TrafficMonitor

**本插件是基于 [TrafficMonitor](https://github.com/zhongyang219/TrafficMonitor) 的插件接口开发的。**

- **作者**: [zhongyang219](https://github.com/zhongyang219)
- **项目地址**: https://github.com/zhongyang219/TrafficMonitor
- **许可证**: Anti-996 License

TrafficMonitor 是一款用于 Windows 的网络流量监控悬浮窗软件，可以在任务栏或桌面显示网速、CPU、内存等系统信息。本插件利用其提供的插件接口，扩展了 Clash 代理信息显示功能。

**特别感谢 zhongyang219 开发了如此优秀的软件，并提供了清晰、完善的插件开发接口！**

### Clash / Mihomo

- **Clash**: https://github.com/Dreamacro/clash
- **Mihomo (Clash Meta)**: https://github.com/MetaCubeX/mihomo

本插件通过 Clash 的 RESTful API 获取代理信息，不包含任何 Clash 源代码。

---

## 📄 许可证

本项目采用 [MIT 许可证](LICENSE)。

**第三方许可证声明**：
- 本项目使用了 TrafficMonitor 的插件接口文件 (`PluginInterface.h`)，该文件遵循 Anti-996 许可证
- 完整的第三方许可证信息请参见 [THIRD_PARTY_LICENSES.md](THIRD_PARTY_LICENSES.md)

---

## ⚖️ 免责声明

- 本项目为个人开发的第三方插件，与 TrafficMonitor、Clash 官方无关
- 本项目不保证与所有版本的 TrafficMonitor 或 Clash 兼容
- 使用本插件产生的任何问题，作者不承担任何责任
- 本项目仅供学习和个人使用

---

## 📬 反馈与贡献

如果你发现了 Bug 或有功能建议，欢迎：
- 提交 [Issue](https://github.com/super-icecream/YASB-clash/issues)
- 提交 Pull Request

---

*Made with ❤️ for the Clash community*
