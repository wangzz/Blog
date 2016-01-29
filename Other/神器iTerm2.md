神器iTerm2

1、窗口分割(Split Panes)：
* command + d：垂直分割
* command + shift + d：水平分割

2、全局热键(Hotkey Window)：
command + \

3、切换标签：
* command + 数字 
* command + 左右方向键

4、切换全屏：
command + enter

5、查看历史命令：
command + ;

6、查看剪贴板历史：
command + shift + h

7、清除当前行：
ctrl + u

8、到行首：
ctrl + a

9、到行尾：
ctrl + e

10、前进后退：
ctrl + f/b (相当于左右方向键)

11、上一条命令：
ctrl + p

12、搜索命令历史：
ctrl + r

13、删除当前光标的字符：
ctrl + d

14、删除光标之前的字符：
ctrl + h

15、删除光标之前的单词：
ctrl + w

16、删除到文本末尾：
ctrl + k

17、交换光标处文本：
ctrl + t

18、清屏：
* command + r
* command + k
* ctrl + l

19、选中即复制：
* command + f
输入tab，将选中查找内容右侧的部分并复制；输入shift + tab将选中查找内容左侧的部分并复制。

20、展示所有屏幕：
command + option + e
同时可以搜索

21、快照：
Window > Save Window Arrangement
Window > Restore Window Arrangement
Preferences > General > Open saved window arrangement

22、Profiles：
不同的任务下可以做不同的界面样式及其它设置项的配置，比如写代码要纯黑底，看文档要白底黑字。这时候就可以针对不同的任务创建不同的Profiles

23、Expose Tabs：
option + command + e
一次性展示所有的窗口，同时能对这些窗口搜索。

24、寻找光标：
command + /

25、回放功能：
option + command + b

26、Preferences：
command + ,
快捷调出Preferences设置

27、随时响应：
设置开机启动后，可以随时响应之前设置的快捷键

28、隐藏Dock图标：
隐藏：

```
/usr/libexec/PlistBuddy  -c "Add :LSUIElement bool true" /Applications/iTerm.app/Contents/Info.plist
```

展示：

```
/usr/libexec/PlistBuddy  -c "Delete :LSUIElement" /Applications/iTerm.app/Contents/Info.plist
```

这个方法是通用的，LSUIElement 可控制 app 以无Dock，无菜单栏的方式运行，另外 LSBackgroundOnly 可让 app 以无窗口的方式在后台运行。详细说明可查看 [LaunchServicesKeys](https://developer.apple.com/library/ios/documentation/General/Reference/InfoPlistKeyReference/Articles/LaunchServicesKeys.html)


#### 官网

* [iTerm2](https://www.iterm2.com/features.html)



