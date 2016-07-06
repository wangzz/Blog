## 配置 gitconfig 文件

该文件用于记录 ~/.gitconfig 文件中的常用配置：

1、alias

```
[alias]
	co = checkout
	br = branch
	ci = commit
	st = status
	logg = log --graph --pretty=format:'%Cred%h%Creset %Cgreen%cd%Creset -%C(yellow)%d%Creset %s [%an]' --abbrev-commit --date=short
```