P4Merge 是一款非常优秀的 git merge 工具，且跨平台兼容。尽管 git 亦有内部实现的 merge 工具，但并不如 P4Merge 易用。我们可以通过配置 .gitconfig 文件来设置 git 使用外部 merge 工具。

首先，下载安装 [P4Merge](https://www.perforce.com/product/components/perforce-visual-merge-and-diff-tools)。MAC 下可以通过：

```
brew cask install p4merge
```

来安装。

在系统可访问的目录下（我们这里使用 `/usr/local/bin/`）创建两个可执行文件 extMerge 和 extDiff，其内容如下：

```
$ cat /usr/local/bin/extMerge
#!/bin/sh
/Applications/p4merge.app/Contents/MacOS/p4merge $*
```

```
$ cat /usr/local/bin/extDiff
#!/bin/sh
[ $# -eq 7 ] && /usr/local/bin/extMerge "$2" "$5"
```

别忘了添加可执行权限：

```
$ sudo chmod +x /usr/local/bin/extMerge
$ sudo chmod +x /usr/local/bin/extDiff
```

使用 extMerge和extDiff的好处是，我们可以很方便的切换 merge 工具。
最后，在你的 .gitconfig 文件里添加如下配置：

```
[merge]
    tool = extMerge
[mergetool "extMerge"]
    cmd = extMerge "$BASE" "$LOCAL" "$REMOTE" "$MERGED"
[diff]
    external = extDiff
[mergetool]
    trustExitCode = false
    keepTemporaries = false
    keepBackup = false
    prompt = false
```

好了，当合并（merge/rebase） 出现冲突时，执行git mergetool，即可调出 P4Merge 来解决冲突了。

