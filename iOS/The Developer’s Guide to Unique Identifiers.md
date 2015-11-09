## UUID 编程指南

#### 0x01 概念介绍

* UUID (Universally Unique Identifier)

通用唯一识别码。机器根据网卡 MAC 地址、时间戳、命名空间（Namespace）、随机或伪随机数、时序等要素，通过一定的算法生成，由 [RFC4122](http://www.ietf.org/rfc/rfc4122.txt) 定义。

* GUID (Globally Unique Identifier)

全局唯一标识。微软版的 UUID ，通常可以和 UUID 互换使用。

* UDID (Unique Device Identifier)

机器唯一标识符。用来唯一标识一台 iOS 设备。

#### 0x02 被禁掉的 UDID

在实际应用过程中，经常需要通过唯一标识来识别一个设备，最早 Apple 是开放 UDID 的，用法如下：

```
NSString *udid = [[UIDevice currentDevice] uniqueIdentifier];
```

举例：

```
bb4d786633053a0b9c0da20d54ea7e38e8776da4
```

但是出于用户隐私考虑，苹果先是在 iOS5 系统中废弃了该方法，随后又强制要求新提交的应用禁用该方法，否组不通过审核。于是开发者就不得不寻求下面几种 UUID 替代方案。

#### 0x03 CFUUID

该方法在 iOS 2 就可用了，用法如下：

```
CFUUIDRef cfuuid = CFUUIDCreate(kCFAllocatorDefault);
NSString *cfuuidString = (NSString*)CFBridgingRelease(CFUUIDCreateString(kCFAllocatorDefault, cfuuid));
```

我们需要将获取的结果保存在 NSUserDefaults、Keychain、 Pasteboard 或者其它地方。

举例： 

``` 68753A44-4D6F-1226-9C60-0050E4C00067 ```

#### 0x04 NSUUID

这是比 CFUUID 更高层级的方法，iOS 6 可用，用法如下：

```
NSString *uuid = [[NSUUID UUID] UUIDString];
```

其用法也和 CFUUID 类似。

举例： 

```
68753A44-4D6F-1226-9C60-0050E4C00067
```

#### 0x05 Advertiser Identifier

这也是个 iOS 6 可用的包含在 AdSupport.framework 中的方法，方法如下：

```
NSString *adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];
```

CFUUID 和 NSUUID 都需要我们自己存储，而 Advertiser Identifier 是由系统存储的，但是在以下几种情况下它也会发生变化：

* 还原了设备 (设置程序 -> 通用 -> 还原 -> 还原位置与隐私)
* 重置了广告标识符 (设置程序-> 通用 -> 关于本机 -> 广告 -> 还原广告标示符)

举例：

```
1E2DFA89-496A-47FD-9941-DF1FC4E6484A
```

#### 0x06 Identifier for Vendor

iOS 6 新增了另外一个方法：

```
NSString *idfv = [[[UIDevice currentDevice] identifierForVendor] UUIDString];
```

其结果是根据 vendor 生成的，[《UIDevice Class Reference》](https://developer.apple.com/library/prerelease/ios/documentation/UIKit/Reference/UIDevice_Class/#//apple_ref/occ/instp/UIDevice/identifierForVendor) 中关于生成规则的描述如下：

* iOS 6, bundle ID 的前两个组成部分用来生成 vendor ID 。如果 bundle ID 只有一位，那么整个 bundle ID 都被用于生成 vendor ID 。
* iOS 7 或更新系统中，bundle ID 除了最后一部分以外都被用于生成 vendor ID 。如果 bundle ID 只有一位，那么整个 bundle ID 都被用于生成 vendor ID 。

示例如下：

![UUID2VendorID](https://github.com/wangzz/Blog/blob/master/image/UUID2VendorID/UUID2VendorID.png)

特别需要指出，如果用户上安装有相同 vendor ID 的产品，其中一个产品被删除了重装时，获取的 UUID 仍然是相同的。

举例：

```
599F9C00-92DC-4B5C-9464-7971F01F8370
```

#### 0x07 OpenUDID

在 UDID 被弃用以后，OpenUDID 成为了当时使用最广泛的开源 UDID 替代方案。 OpenUDID 在工程中实现起来非常简单：

```
NSString *openUDID = [OpenUDID value];
```

OpenUDID 利用了一个非常巧妙的方法在不同程序间存储标示符：在粘贴板中用了一个特殊的名称来存储标示符。通过这种方法，别的程序（同样使用了 OpenUDID ）知道去什么地方获取已经生成的标示符（而不用再生成一个新的）。

举例：

```
0d943976b24c85900c764dd9f75ce054dc5986ff
```

#### 0x08 参考文档

* [NSUUID /CFUUIDRef /UIDevice -unique​Identifier /-identifier​For​Vendor](http://nshipster.com/uuid-udid-unique-identifier/)
* [The Developer’s Guide to Unique Identifiers](https://possiblemobile.com/2013/04/unique-identifiers/)
* [UIDevice Class Reference](https://developer.apple.com/library/prerelease/ios/documentation/UIKit/Reference/UIDevice_Class/#//apple_ref/occ/instp/UIDevice/identifierForVendor)