## Sublime Text

#### 一、构建编译器

* Javascript 编译器

选择：
Tools –> Build System –> new Build System… 

在新创建的文件中输入：

```
{
    "cmd": ["/usr/local/Cellar/node/0.10.35_1/bin/node", "$file"],
    "file_regex": "^[ ]*File \"(...*?)\", line ([0-9]*)",
    "working_dir": "${project_path:${folder}}",
    "selector": "source.js",
    "shell": true,
    "encoding": "utf-8",
    "windows": {
        "cmd": ["taskkill /F /IM node.exe >nul 2>nul & node", "$file"]
    },
    "linux": {
        "cmd": ["killall node; node", "$file"]
    },
    "osx": {
        "cmd": ["killall node >/dev/null 2>&1; /usr/local/Cellar/node/0.10.35_1/bin/node $file"]
    }
}
```

直接保存为名称 Javascript.sublime-build 即可。

需要注意的是，一定要写对自己电脑上 node 的安装路径（通过 homebrew 安装成功后会打印路径），比如我的是： /usr/local/Cellar/node/0.10.35_1

#### 二、快捷键

* Jump To Definition

先打开：

```
Sublime Text -> Preferences -> Key Bindings - User
```

添加：

```
[
    { "keys": ["shift+command+m"], "command": "goto_definition" }
]
```

即可使用。


#### 三、常用插件

* Javascript 相关

JavaScript Completions

### 四、列编辑模式

* 先选中要编辑的数据，可以通过 command + A 选中全部数据
* 通过 command + shift + L 进入列编辑模式，进入列编辑模式后可批量修改每一列
 

