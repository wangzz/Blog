## homebrew

#### 关于

* 安装

```
/usr/bin/ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
```

* 更新

```
brew update
```

* 卸载

```
cd `brew --prefix`
rm -rf Cellar
brew prune
rm `git ls-files`
rm -r Library/Homebrew Library/Aliases Library/Formula Library/Contributions
rm -rf .git
rm -rf ~/Library/Caches/Homebrew
```


#### 使用

* 查找

```
brew search [name]
```

* 安装

```
brew install [name]
```

* 卸载

```
brew uninstall [name]
```

* 查看

```
brew list
```

* 其他

如果不想每次都 sudo，可执行已下命令：

```
sudo chown -R [username] /usr/local
```

#### 官网

* [homebrew官网](http://brew.sh/)

