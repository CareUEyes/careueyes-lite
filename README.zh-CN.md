# CareUEyes Lite

[English](README.md)

CareUEyes Lite 是 CareUEyes 推出的开源 Windows 屏幕调光工具。它面向长时间使用电脑的人群，提供一个轻量、直接的方式，让屏幕亮度更舒适。

CareUEyes Lite 是一个刻意保持轻量的开源版本，只保留基于 gamma 的屏幕亮度调节能力。

CareUEyes Lite 使用软件 gamma 调光，不会直接改变显示器的物理亮度或 OSD 亮度值。

## 下载

你可以从 GitHub Releases 下载最新版本，也可以访问 CareUEyes 官网：

https://care-eyes.com/?lite

## 截图

![CareUEyes Lite - Windows 屏幕调光工具](assets/careueyes-lite.jpg)

## 主要功能

- 通过系统托盘调节屏幕亮度
- 提供 20%、40%、60%、80%、100% 亮度档位
- 支持多显示器 gamma 调光
- 在显示器、电源、会话状态变化后恢复亮度设置
- 只保留核心亮度调节能力，不包含其它附加功能

## Lite 与 Pro

CareUEyes Lite 提供开源的核心屏幕调光体验。

如果你需要蓝光过滤、色温调节、休息提醒、专注工具和亮度自动化，可以使用 CareUEyes Pro：

https://care-eyes.com/?lite

![CareUEyes Pro - 完整护眼套件](assets/careueyes-pro.jpg)

## 构建

CareUEyes Lite 是一个 Win32 项目，使用 Visual Studio 2005 构建。

打开：

```text
careueyes-lite.sln
```

或使用命令行构建：

```text
"C:\Program Files (x86)\Microsoft Visual Studio 8\Common7\IDE\devenv.exe" careueyes-lite.sln /Build Release
```

生成文件位于：

```text
Release/CareUEyes.exe
```

当前输出文件名仍为 `CareUEyes.exe`，这是为了兼容原有项目结构。

## 项目边界

这个仓库用于 CareUEyes Lite。贡献内容应保持在轻量屏幕调光体验、兼容性、构建稳定性和文档完善范围内。

蓝光过滤、休息提醒、专注工具、高级计划任务和更完整的多屏工作流属于 CareUEyes Pro。

## 许可证

源代码使用 MIT License 开源。

CareUEyes 名称、Logo、官网、产品截图以及其它品牌资产不属于 MIT 授权范围，除非另有明确说明。
