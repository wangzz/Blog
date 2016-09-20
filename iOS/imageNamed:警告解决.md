
控制台打印 log :

```
CUICatalog: Invalid asset name supplied: (null)
```

问题定位方法如下：

Add symbolic breakpoint for [UIImage imageNamed:]

![](http://i.stack.imgur.com/ATz38.png)

Add $arg3 == nil condition (on Simulator) or $r0 == nil condition on iPhone device
Run your application and see where is the problem persist

