# Gaokun3-AlternativeLid

华为高坤 (Huawei Gaokun) Windows on ARM64 设备的替代盖子驱动

## 问题描述

华为高坤设备的霍尔传感器 (Hall Sensor) 损坏,导致系统错误地检测盖子状态,使得键盘在 Windows 11 下无法正常工作。

## 解决方案

本项目提供一个最小化的内核驱动 `SimpleLidDriver.sys`,用于替换系统默认的 `ACPI\ID9001` 盖子驱动,阻止错误的盖子关闭信号。

### 驱动功能

- **占位驱动**: 接管 `ACPI\ID9001` 设备,但不响应任何盖子事件
- **极简设计**: 仅约 40 行代码,最小化系统影响
- **ARM64 原生**: 专为 Windows on ARM64 编译

## 自动构建

[![Build ARM64 Driver](https://github.com/Fectxd/Gaokun3-AlternativeLid/actions/workflows/build-driver.yml/badge.svg)](https://github.com/Fectxd/Gaokun3-AlternativeLid/actions/workflows/build-driver.yml)

驱动由 GitHub Actions 自动编译,无需本地安装 WDK。

### 下载已编译驱动

1. 访问 [Actions](https://github.com/Fectxd/Gaokun3-AlternativeLid/actions) 页面
2. 点击最新的成功构建 (绿色 ✓)
3. 在 Artifacts 部分下载 `SimpleLidDriver-Release-Package`

## 安装步骤

### 1. 启用测试模式 (必需)

以管理员身份运行 PowerShell:

```powershell
# 启用测试签名
bcdedit /set testsigning on

# 重启电脑
Restart-Computer
```

### 2. 安装驱动

解压下载的驱动包,以管理员身份运行:

```powershell
# 进入驱动目录
cd <驱动解压路径>

# 安装驱动
pnputil /add-driver SimpleLidDriver.inf /install

# 禁用原有的盖子驱动
$deviceId = "ACPI\ID9001\2&DABA3FF&1"
Disable-PnpDevice -InstanceId $deviceId -Confirm:$false

# 更新驱动为新的 SimpleLidDriver
pnputil /scan-devices
```

### 3. 验证安装

打开设备管理器,在 "系统设备" 下应该看到:

```
SimpleLidDriver
    位置: ACPI\ID9001
    驱动程序: SimpleLidDriver.sys
    状态: 正常工作
```

## 卸载驱动

以管理员身份运行:

```powershell
# 恢复为原驱动
pnputil /delete-driver <OEM编号>.inf /uninstall

# 禁用测试模式 (可选)
bcdedit /set testsigning off

# 重启
Restart-Computer
```

## 本地构建 (可选)

### 前置要求

- Visual Studio 2022 或更高版本
- Windows Driver Kit (WDK) 11
- Windows 11 SDK

### 构建步骤

```powershell
cd SimpleLidDriver
powershell -ExecutionPolicy Bypass -File build_arm64.ps1
```

输出文件:
- `ARM64\Release\SimpleLidDriver.sys` - 驱动文件
- `ARM64\Release\SimpleLidDriver.inf` - 安装配置文件

## 项目结构

```
.
├── .github/
│   └── workflows/
│       └── build-driver.yml        # GitHub Actions 构建配置
├── SimpleLidDriver/
│   ├── SimpleLidDriver.c           # 驱动源代码
│   ├── SimpleLidDriver.inf         # 驱动安装配置
│   ├── SimpleLidDriver.vcxproj     # Visual Studio 项目文件
│   └── build_arm64.ps1             # 本地构建脚本
└── README.md
```

## 技术细节

### 目标设备

- **设备 ID**: `ACPI\ID9001\2&DABA3FF&1`
- **设备名称**: GPIO Laptop or Slate Indicator Driver
- **原驱动**: `msgpiowin32.sys` (Microsoft GPIO Driver)

### 驱动实现

驱动使用 KMDF (Kernel-Mode Driver Framework) 1.15:

- **DriverEntry**: 初始化 WDF 驱动框架
- **EvtDeviceAdd**: 创建设备对象,但不注册任何事件处理
- **关键特性**: 完全静默,不响应任何盖子事件

### 安全性

- 仅接管单一设备 (`ACPI\ID9001`)
- 不修改系统文件
- 不包含网络功能
- 开源,可完整审查代码

## 替代方案

### PowerShell 脚本 (临时)

如果不想安装驱动,可以使用 PowerShell 脚本临时禁用设备:

```powershell
# 每次启动后运行
Disable-PnpDevice -InstanceId "ACPI\ID9001\2&DABA3FF&1" -Confirm:$false
```

**局限性**:
- 需要每次启动手动运行
- 睡眠唤醒后可能失效

### 智能检测脚本

使用加速度计和光线传感器判断盖子状态:

参见: `smart_lid_detection.ps1`

**判断逻辑**:
- Z 轴 > 0.93 (屏幕朝下) **且** 光线 < 0.3 lux (极暗) = 盖子关闭

## 常见问题

### Q: 安装驱动后键盘仍不工作?

A: 尝试以下步骤:
1. 在设备管理器中完全卸载 `ACPI\ID9001` 设备
2. 重启电脑
3. 重新安装 SimpleLidDriver

### Q: 驱动会影响其他硬件吗?

A: 不会。驱动只接管 `ACPI\ID9001` 设备,不影响其他硬件。

### Q: 为什么需要测试模式?

A: 驱动未经过 Microsoft WHQL 认证签名,Windows 默认不允许加载。测试模式允许加载测试签名的驱动。

### Q: 如何判断盖子是否真的关闭?

A: 可以运行 `smart_lid_detection.ps1` 脚本,通过传感器组合判断。

## 贡献

欢迎提交 Issue 和 Pull Request!

## 许可证

MIT License

## 致谢

感谢所有为 Windows on ARM 生态做出贡献的开发者。

---

**免责声明**: 本驱动仅供学习和研究使用。安装驱动有一定风险,请谨慎操作。作者不对因使用本驱动造成的任何损失负责。
