## iOS图像解压缩

一般我们使用的图片格式有两种：jpeg和png。

#### Apple对png的优化

Apple推荐使用png作为用户界面的图片格式，这些图片会被一个叫[pngcrush](http://pmt.sourceforge.net/pngcrush/)的开元工具优化.
该工具位于目录：

```
/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin
```

详情可见苹果官方的Q&A文档：[Viewing iOS-Optimized PNGs](https://developer.apple.com/library/ios/qa/qa1681/_index.html)

这样iOS设备就能在显示时更快的解压和渲染图片。

OS X 10.8之后系统preview.app自己能够识别优化后的图片，在此之前优化后的图片是无法通过其预览的，详情见：[PNG与iOS优化选项](http://dingtianran.com/blog/2012/02/17/pngyu-iosyou-hua-xuan-xiang/)


#### png和jpeg优缺点对比

虽然png图片会被事先优化好，但这并不意味着在所有情况下png都是最佳的图片格式，png对于那些app中自带的图片来说非常好，但是对于要从internet上down下来的图片来说又会怎样呢。png和jpeg这两种格式都有各自的优缺点：

* png格式的图片有alpha通道，jpeg则没有；

* png无损压缩，jpeg允许选择0-100%的压缩质量（peg格式会忽略那些你看不到的信息，对于大部分的图片，使用60-70%的压缩治疗对图片不会造成明显影响）；

因此，如果需要alpha，就只能用png格式；如果不需要一个完美的图片，就可以使用jpeg格式。

#### 显示图片所需消耗时间

* 从磁盘上alloc/init UIImage的时间

* 解压缩的时间

* 将解压缩后的比特转换成CGContext的时间，通常需要改变尺寸，混合，抗锯齿工作。

#### 结论

对于需要从服务器下载的图片，如果我们需要alpha通道或者必须使用png格式时，应该在web服务器上安装pngcrush并处理好所有的png图片。其他情况下，高质量的jpeg能带来较小的文件大小以及更快的解压缩和渲染。

事实证明，png格式对于那些使用在UI元素中的小图片来说非常好，但是对于那些全屏显示图片的应用来说则完全不是。替代png的通常是60-80%压缩质量的jpeg，至于压缩质量取多少合适，这取决于你的图片内容。

你可能希望所有显示过的图片都能保持他们的解压缩版本，但是这也将带来大量的内存开销并导致你的App进程被杀掉。此时使用NSCache就是一个很好的解决方案。它可以自动在内存短缺的时候照看好这些图片。


#### 参考文档

* [iOS如何避免图像解压缩的时间开销](http://www.taofengping.com/2013/05/10/ios_uiimage_optimizer/#.VT3WbCGeDRY)

* [Avoiding Image Decompression Sickness](http://www.cocoanetics.com/2011/10/avoiding-image-decompression-sickness/)

* [Viewing iOS-Optimized PNGs](https://developer.apple.com/library/ios/qa/qa1681/_index.html)

* [PNG与iOS优化选项](http://dingtianran.com/blog/2012/02/17/pngyu-iosyou-hua-xuan-xiang/)




