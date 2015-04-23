## Xcode升级后插件失效解决办法

*  查看Xcode对应版本的UUID：

```
tail -f /var/log/system.log
```

*  更新插件配置文件

得到UUID后执行命令：

```
find ~/Library/Application\ Support/Developer/Shared/Xcode/Plug-ins -name Info.plist -maxdepth 3 | xargs -I{} defaults write {} DVTPlugInCompatibilityUUIDs -array-add XCode_UUID
```

* 参考链接：

[XCode升级后插件失效的原理与修复办法](http://joeshang.github.io/2015/04/10/fix-xcode-upgrade-plugin-invalid/)

