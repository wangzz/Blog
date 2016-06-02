## Sublime Text

#### 1、构建 Javascript 编译器

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
