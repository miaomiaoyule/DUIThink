# XUI

X，不被定义，无限可能。

XUI（前身 [DUIThink](https://github.com/miaomiaoyule/XUI)）是带可视化设计器的跨平台 C++ 界面开发框架，许可证为 [LGPL-2.1](LICENSE)。界面用工程文件描述，业务代码可以像 MFC 一样自动生成窗体类、控件变量和消息映射，并带上线程池、文件、网络、监听等常用部件，用来直接做应用。

控件没有独立窗口句柄，整棵界面挂在一个窗口上绘制。Windows 使用 Win32；Linux、macOS、Android 使用 SDL3。

| 平台 | 窗口后端 |
| --- | --- |
| Windows | Win32 |
| Linux | SDL3 |
| macOS | SDL3 |
| Android | SDL3 |

- 官网：<http://49.235.209.245/XUI>
- 源码：<https://github.com/miaomiaoyule/XUI>
- QQ 群：885314879
- 视频教程：<https://space.bilibili.com/322458488>

## 为什么做

MFC 做复杂皮肤时，很多地方要自己画，改样式成本高。纯手写 XML 又缺少预览，属性名也不好记。XUI 把这两件事分开：

- 结构、布局、颜色、图片、字体放在 `.DuiProj` 工程里，用设计器维护。
- 窗口生命周期、控件查找、点击和定时器放在 C++ 类里，写法接近 MFC 的控件变量和消息映射。

## 特性

### 布局与绘制

- 布局：靠上、靠下、靠左、靠右、居中、相对、绝对
- 尺寸：固定宽高、最大最小宽高、按文本或子控件自动调整
- 背景：颜色、图片、圆角、渐变，以及自定义 Bitmap
- SVG 图片，覆盖比常见看图工具和部分开源库更全的格式
- RichText：同一段文字分字体、分颜色，支持多行与六向对齐
- DPI 切换一行代码完成，设计器里可预览各 DPI
- 窗体特效、控件水波纹，以及一行代码换肤

### 控件

- Clone：复制出相同控件；Attach / Detach 随意挂到父节点
- 自绘 Edit：可插入图片、动图和带透明通道的文字，适合聊天输入框
- 自绘 IP 控件、日历控件、Clock 时钟、3D 旋转菜单
- ComboBox 支持梯度子项滑动
- Animate 支持 Gif 与序列帧
- ListView 改一个属性即可在 List、Grid、TileH、TileV 之间切换，子项可拖动换位
- 树控件：选中按钮、无选中按钮、表头树、单选树，改属性即可切换

### 开发部件

线程池、文件操作、网络、文件监听、注册表监听、文件拖拽、命令行解析、磁吸停靠、内存池、分辨率转换、字符串操作。这些部件和界面库在同一套框架里，可以直接用来做完整应用。

### 可视化设计器

设计器单独提供，运行时库开源。绑定 Visual Studio 的解决方案后：

- 像 MFC 一样自动生成窗体类、控件变量和控件事件代码
- 任意拖放、对齐控件，修改属性立即看到结果
- 图片、字体、颜色资源集中查看
- 控件树里用 Shift + Up / Down 实时调整 Z 序
- 菜单可按一级、二级、三级实时编辑
- 支持 Ctrl + C / V / Z / Y，也能把界面复制到另一个视图
- 支持开发者自定义控件，并实时预览各 DPI

## 快速开始

先跑通 `Demo/XUIDemo_C++`。程序从可执行文件的上一级读取 `XUIDemo_C++.DuiProj`。

**Windows。** 用 Visual Studio 打开 `XUI.2017.sln`，配置选 `Release_Unicode | Win32`，先编出库。`MMHelper/DuiPlatformSDL.props` 里的 `DuiPlatformSDL` 保持默认 `false`，这一端不需要 SDL3。再打开 `Demo/XUIDemo_C++/XUIDemo_C++.sln`，配置选 `Release | Win32`。

**Linux、macOS、Android。** 使用 CMake，打开 `XUI_PLATFORM_SDL`，并安装 SDL3。

```bash
cmake -S . -B build -DXUI_PLATFORM_SDL=ON
cmake --build build --target XUI XUIDemo_C++
```

头文件入口是 `XUI/XUIHead.h`。应用工程按 DLL 使用时不要定义 `XUISDK` / `XUILIB`；静态链接时定义 `XUILIB`。

## 示例

| 目录 | 内容 |
| --- | --- |
| `Demo/XUIDemo_C++` | 主示例：基础控件、DPI、换肤、SVG、聊天界面 |
| `Demo/FlowSheet` | 流程类小示例 |
| `Demo/Scratch` | 更小的启动样例 |

界面工程以 `.DuiProj` 为入口，里面索引皮肤、字体、颜色、属性表和各个界面 XML。新界面用设计器制作，C++ 侧在 `OnFindControl` 里绑定控件，用消息映射处理点击和定时器。

## 许可证

[LGPL-2.1](LICENSE)
