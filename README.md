
# FocusGuard / 专注小卫士
现代数字化学习守护者 | Digital Guardian for Focused Learning
无害化的关闭前台应用程序 | Harmless closure of front-end applications
只关闭前台的窗口，并不会终止后台的程序或服务 | Only closing the front-end window will not terminate the back-end application or services
---

## 📖 使用场景 / Use Cases
- 写代码时想开微信的手贱时刻**物理阻断**🧤
- 备战考研考公时想玩**steam**的时候自动消灭💥
- 🔥 Code Protection: Programmatically blocks **Telegram** during coding sessions via window handle termination
- 🎯 Exam Mode: Auto-terminates **Steam** processes when detected during national exam preparation
---

## 🚦 极速配置 / Quick Start

### 克隆仓库 / Clone Repo
```bash
git clone https://github.com/Waller-Man/FlowFocus
```

### 配置白名单 / Whitelist Setup
```cpp
const std::vector<std::string> whitelist = {
    "cherry studio.exe",
    "explorer.exe", //Windows资源管理器 Windows Explorer
    //"focus.exe",
    "msedge.exe",   //edge浏览器  Microsoft Edge Browser
    "devenv.exe"    //VS
};
```

## 🔍 How to Find Process Names / 如何查找进程名
1. Open Task Manager → Navigate to "Details" tab / 打开任务管理器 → 详情选项卡  
2. Right-click column headers → Check "Process Name" / 右键列头 → 选择"进程名"列  
3. Right-click suspicious process → "Open file location" to verify / 右键可疑进程 → "打开文件所在位置" 确认 

## 🛠️ Build Solution / 编译生成
1. Open `focus.sln` with Visual Studio / 用 Visual Studio 打开解决方案文件  
   (Requires C++ desktop development components / 需安装C++桌面开发组件)  
2. Select **x64 Release** configuration / 选择x64 Release模式  
3. Menu → Build → **Rebuild Solution** / 菜单栏 → 生成 → 重新生成解决方案  

##  武装启动 / Launch
📂 导航到“x64 \Release”文件夹
🖱️ 右键单击“FocusGuard.exe”→**以管理员身份运行**
📂 Navigate to the `x64\Release` folder  
🖱️ Right-click `FocusGuard.exe` → **Run as administrator**

##   运行截图 / Run screenshot
![捕获](https://github.com/user-attachments/assets/270b52a3-ea6e-43da-b057-ddbccb8bfe1d)


##   🛡️ 安全退出 / Safe Exit
Method 1️⃣: Press Ctrl+C in the console window  
方法一：在控制窗口按 Ctrl+C  
Method 2️⃣: Terminate process directly via Task Manager  
方法二：通过任务管理器直接关闭进程  
Surf freely! 🏄♀️  
立即恢复自由冲浪模式



