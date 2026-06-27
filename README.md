# 🎓 Student Life Simulator v3.0

### *一款基于 C++17 和 SFML 的校园生活模拟游戏*

[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![SFML](https://img.shields.io/badge/SFML-2.6-green.svg)](https://www.sfml-dev.org/)
[![License](https://img.shields.io/badge/License-Apache%202.0-orange.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()
[![GitHub stars](https://img.shields.io/github/stars/你的用户名/student-simulator?style=social)](https://github.com/你的用户名/student-simulator/stargazers)

---

## 📖 项目简介

**Student Life Simulator** 是一款沉浸式的大学生活模拟游戏。你控制一个角色在可视化校园中探索、学习、社交、触发随机事件、提升NPC好感度，体验从大一新生到毕业的完整大学生活。

### 🎯 核心特色

| 特色 | 说明 |
|------|------|
| 🗺️ **2D 校园地图** | 宿舍、图书馆、教室、食堂、健身房等 5 个场景 |
| 🎲 **随机事件系统** | 10+ 种随机事件，每次游玩体验不同 |
| 💬 **NPC 好感度** | 3 个 NPC（学姐/室友/导师），好感度解锁特殊奖励 |
| 🏆 **技能系统** | 考试成绩优秀可获得额外技能 |
| 💾 **JSON 存档** | 人类可读的存档格式，跨平台兼容 |
| 📊 **Buff 系统** | 临时增益效果（学习效率+20%、精力恢复+50%） |
| 🎓 **毕业目标** | 8 学期 + GPA ≥ 2.0 + 20 门课程 |

---

## 🏗️ 系统架构

### 类结构

```
┌─────────────────────────────────────────────────────────────────┐
│                       GameManager                                │
│  - 游戏主循环 / 事件处理 / 渲染                                  │
│  - UI 管理 / 输入处理                                           │
└──────────────────────────┬──────────────────────────────────────┘
                           │
        ┌──────────────────┼──────────────────┐
        ▼                  ▼                  ▼
┌──────────────────┐ ┌──────────────────┐ ┌──────────────────┐
│     Stats        │ │  CourseManager   │ │  SkillManager    │
│  核心属性/状态    │ │  课程管理/成绩    │ │  技能解锁/查询    │
└──────────────────┘ └──────────────────┘ └──────────────────┘
        │                  │                  │
        └──────────────────┼──────────────────┘
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                    辅助系统                                       │
├──────────────────┬──────────────────┬──────────────────────────┤
│  EventSystem     │   NPCManager     │   SaveManager            │
│  随机事件生成     │   NPC好感度管理   │   JSON 存档/读档         │
└──────────────────┴──────────────────┴──────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                    Interactable (多态)                           │
├──────────┬──────────┬──────────┬──────────┬──────────────────┤
│ Dormitory │ Library  │ Classroom│ Canteen  │      Gym         │
│  宿舍     │  图书馆   │  教室    │  食堂    │     健身房        │
└──────────┴──────────┴──────────┴──────────┴──────────────────┘
```

### 技术栈

| 技术 | 用途 |
|------|------|
| C++17 | 核心语言 |
| SFML 2.6 | 图形 / 窗口 / 事件 |
| nlohmann/json | JSON 存档 |
| STL | 容器 / 算法 / 文件系统 |

---

## 🚀 快速开始

### 环境准备

#### Windows
```bash
# 1. 安装 MinGW-w64
# 下载: https://www.mingw-w64.org/

# 2. 安装 SFML
# 下载: https://www.sfml-dev.org/download/sfml/2.6.1/
# 解压到 C:\SFML-2.6.1

# 3. 安装 nlohmann/json（可选，也可以直接下载 single header）
# vcpkg install nlohmann-json
# 或下载: https://github.com/nlohmann/json/releases
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install g++ make libsfml-dev
# nlohmann/json 通常可通过 apt 安装
sudo apt install nlohmann-json3-dev
```

#### macOS
```bash
brew install sfml
brew install nlohmann-json
```

### 编译与运行

#### 使用 CMake（推荐）
```bash
mkdir build && cd build
cmake ..
cmake --build .
./bin/student_simulator
```

#### 手动编译（Windows/MinGW）
```bash
g++ -o student_simulator.exe main.cpp -std=c++17 \
    -I C:\SFML-2.6.1\include \
    -I C:\vcpkg\installed\x64-windows\include \
    -L C:\SFML-2.6.1\lib \
    -L C:\vcpkg\installed\x64-windows\lib \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -lstdc++fs
```

#### 手动编译（Linux）
```bash
g++ -o student_simulator main.cpp -std=c++17 \
    -lsfml-graphics -lsfml-window -lsfml-system \
    -lstdc++fs
```

### 运行
```bash
# Windows
student_simulator.exe

# Linux/macOS
./student_simulator
```

---

## 🎮 游戏指南

### 操作说明

| 按键 | 功能 |
|------|------|
| ⬆⬇⬅➡ | 角色移动 |
| Enter | 与建筑交互（靠近建筑时） |
| 1-9 | 执行菜单操作 |
| S | 手动存档 |
| L | 读取存档 |
| R | 修改角色名 |
| E | 触发随机事件 |
| ESC | 退出游戏 |

### 校园建筑

| 建筑 | 功能 | 效果 |
|------|------|------|
| 🏠 宿舍 | 睡觉 | 精力 +40（有 Buff 时 +60） |
| 📚 图书馆 | 学习 | 提升课程成绩 |
| 🏫 教室 | 考试周 | 集中考试所有课程 |
| 🍜 食堂 | 吃饭 | 精力 +20，快乐 +8 |
| 💪 健身房 | 运动 | 精力 +15，快乐 +10 |

### NPC 系统

| NPC | 好感度奖励 |
|-----|-----------|
| 学姐 | 30: 复习资料 → 学习效率+20% (2天) |
| 学姐 | 60: 考试重点 → 随机课程加分 |
| 室友 | 30: 一起吃饭 → 精力+10，快乐+15 |
| 室友 | 60: 组队学习 → 学习效率+20% (1天) |
| 导师 | 30: 学术指导 → 快乐+10 |
| 导师 | 60: 推荐信 → 毕业竞争力提升 |

### 随机事件（部分列表）

| 事件 | 效果 |
|------|------|
| 📚 捡到笔记 | 学习效率+20% (3天) |
| 🎮 室友通宵 | 精力 -15，快乐 -5 |
| 💼 找到兼职 | 收入 +300，快乐 +10 |
| 📝 期中考试 | 精力 -10，触发考试 |
| 🏆 竞赛获奖 | 快乐 +20，解锁"编程能力"技能 |
| ☕ 咖啡店偶遇 | 快乐 +15，解锁"项目管理"技能 |

### 毕业条件

- ✅ 完成 **8 个学期**
- ✅ GPA ≥ **2.0**
- ✅ 完成 **20 门课程**

---

## 📁 项目结构

```
student-simulator/
├── main.cpp                 # 主程序（所有代码合并）
├── CMakeLists.txt           # CMake 构建配置
├── README.md                # 项目说明
├── LICENSE                  # Apache-2.0 许可证
├── .gitignore               # Git 忽略文件
├── screenshots/             # 游戏截图
│   ├── campus.png
│   ├── status.png
│   └── event.png
└── saves/                   # 存档目录（自动创建）
    └── autosave.json
```

---

## 📊 数据统计

### 课程列表

| 课程 | 学分 | 难度 |
|------|------|------|
| 高等数学 | 4 | ⭐⭐⭐ |
| C++ 编程 | 3 | ⭐⭐⭐ |
| 数据结构 | 3 | ⭐⭐⭐⭐ |
| 操作系统 | 3 | ⭐⭐⭐⭐ |
| 计算机网络 | 3 | ⭐⭐⭐ |
| 数据库原理 | 3 | ⭐⭐⭐ |
| 软件工程 | 2 | ⭐⭐ |
| 人工智能 | 4 | ⭐⭐⭐⭐⭐ |
| 机器学习 | 3 | ⭐⭐⭐⭐ |
| 英语 | 2 | ⭐⭐ |
| 离散数学 | 3 | ⭐⭐⭐ |
| 编译原理 | 3 | ⭐⭐⭐⭐⭐ |

### 状态影响

| 活动 | 精力 | 快乐 | 学习效率 |
|------|------|------|----------|
| 学习 (1h) | -2 | +3 | +3 分/小时 |
| 睡眠 | +40 | +5 | - |
| 吃饭 | +20 | +8 | - |
| 社交 | -10 | +20 | - |
| 运动 | +15 | +10 | - |
| 摸鱼 | -5 | +5 | - |

---

## 📝 更新日志

### v3.0.0 (2026-06-27) - 重大重构

- ✅ 随机事件系统（10+ 种事件）
- ✅ NPC 好感度系统（3 个 NPC）
- ✅ 技能系统
- ✅ JSON 格式存档（人类可读）
- ✅ Buff 系统（临时增益）
- ✅ 多态建筑交互（易于扩展）
- ✅ 跨平台字体加载
- ✅ 代码架构重构（拆分上帝类）

### v2.0.0 (2026-06-26)

- ✅ 2D 图形界面 (SFML)
- ✅ 可视化校园地图
- ✅ 实时角色移动控制

### v1.0.0 (2026-06-26)

- ✅ 控制台版本发布
- ✅ 课程 / 考试 / 技能系统

---

## 🤝 贡献指南

欢迎贡献代码！请遵循以下流程：

1. Fork 本仓库
2. 创建功能分支 (`git checkout -b feature/AmazingFeature`)
3. 提交更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开 Pull Request

### 代码风格

```cpp
// 类名：驼峰命名法
class GameManager { };

// 函数名：驼峰命名法
void handleKeyPress() { }

// 变量名：下划线命名法
int energy_value = 80;

// 常量：全大写
const int MAX_ENERGY = 100;

// 充分注释
/**
 * 处理键盘输入事件
 * @param key SFML 键盘按键枚举
 */
void handleKeyPress(sf::Keyboard::Key key) { }
```

---

## 📄 许可证

本项目采用 **Apache License 2.0**。

```
Copyright © 2026 Magician's Wand

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

---

## 👨‍💻 作者

- **开发者**: [Duck]
- [MagiciansWand](https://2015137.xyz)-MagiciansWand官网
---

## 🙏 致谢

- [SFML](https://www.sfml-dev.org/) - 优秀的 2D 图形库
- [nlohmann/json](https://github.com/nlohmann/json) - JSON 库
- [MinGW-w64](https://www.mingw-w64.org/) - Windows 编译环境
- [虚舟实验室](https://www.caellab.com/)-虚舟实验室官网
---

## ⭐ Star History

如果这个项目对你有帮助，请点亮右上角的 Star ⭐ ！

[![Star History Chart](https://api.star-history.com/svg?repos=MC-MCL/student-simulator&type=Date)](https://star-history.com/#你的用户名/student-simulator&Date)

---

**祝你大学生活愉快！🎉**
