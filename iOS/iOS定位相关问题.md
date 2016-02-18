## iOS定位相关问题

### 一、Heading vs. Course

Core Location提供了两种获取方向相关信息的方法：

* 装有磁力仪(magnetometer)的机器可以提供当前设备所指方向信息，又被称为heading。

* 装有GPS的机器可以提供当前设备的移动方向信息，又被称为course。

Heading和course是有区别的。Heading表示当前设备实际指向和地理北极或地磁北极之间的角度，course表示当前设备移动方向和地理北极之间的角度。

我们的app可以根据使用场景来选择到底用哪个数据，比如在步行的时候heading可以更加真实的体现出用户的运行方向信息；而在驾车的时候course能更好的表示出用户的运行方向信息。

### 二、定位权限

##### 1、分类

iOS8 之前，只有始终允许定位（NSLocationUsageDescription）和不允许定位两个选择； iOS8+ 系统中，始终允许定位又被细分成以下两种：

* 使用应用期间（NSLocationWhenInUseUsageDescription）
* 始终（NSLocationAlwaysUsageDescription）

##### 2、使用

想使用哪种权限（或者二者同时使用），在 Info.plist 中添加对应条目即可。设置里出现哪几个权限，取决于 plist 里添加了几个权限条目。

同时，需要在开启定位的地方添加以下代码：

```
    CLAuthorizationStatus authorizationStatus = [CLLocationManager authorizationStatus];
    if (authorizationStatus == kCLAuthorizationStatusNotDetermined) {
        if ([_locationManager respondsToSelector:@selector(requestWhenInUseAuthorization)]) {
            [_locationManager requestWhenInUseAuthorization];
        }
        
// 或者
//        if ([_locationManager respondsToSelector:@selector(requestAlwaysAuthorization)]) {
//            [_locationManager requestAlwaysAuthorization];
//        }
    }
```

##### 3、定位蓝条

* iOS8
 
在 iOS8 系统中，将定位权限设置成 NSLocationWhenInUseUsageDescription 时，应用切后台会有蓝条出现。

* iOS9+
 
在 iOS9 或更新系统中，要想出现定位蓝条，在将定位权限设置成 NSLocationWhenInUseUsageDescription 的同时，还需要调用以下代码：

```
if ([_locationManager respondsToSelector:@selector(allowsBackgroundLocationUpdates)]) {
        _locationManager.allowsBackgroundLocationUpdates = YES;
    }
```

而且，如果不调用以上代码，应用切后台后，定位点会停止更新，哪怕并未调用关闭定位的代码。

##### 4、其它

通常一个应用只需使用 WhenInUse/Always 两种权限中的一个。此时，在 plist 中设置对应的 key，在开始定位的时候调用对应的权限申请方法即可； 

但也有某些应用在不同的场景下需要的定位权限，就是说这个应用同时需要 WhenInUse/Always 两种权限。这时就需要在 plist 中同时设置二者的 key，然后在进入不同场景开始定位的时候，分别调用对应的权限申请方法。

### 三、iOS定位权限进化史

定位作为 iOS 系统提供的基础功能之一，在每个版本都有改动，下面说说最近几个版本中定位权限的差异。

0x01 iOS7

要想开启定位，需要在 Background Modes 中增加 Location updates 选项。在 `设置 -> 隐私 -> 定位服务` 中，用户可以通过 switch 开关选择是否允许指定应用获取定位权限。

只要从用户那获取了权限，应用无论在前台后台都能定位。

0x02 iOS8

从这个版本开始，将定位权限分成 Always/WhenInUse 两种。实际上这两种权限都能在前后台获得定位更新，区别是 WhenInUse 权限在应用切后台后仍使用定位时，顶部会有一个 Blue Bar 提醒用户；而 Always 权限则不会有任何提醒，因为  Always 指的就是在用户当前并未使用应用的时候，仍能使用定位。

0x03 iOS9

定位权限仍然只有 Always/WhenInUse 两种，区别是 Always 权限应用无论在前台还是后台都能获得定位更新，但在默认情况下 WhenInUse 变成了则变成了真正的 WhenInUse，即这种权限在应用切后台以后就无法获得定位更新。

你以为这样就完了？

前面说过默认情况下 WhenInUse 权限无法在后台获得定位更新，但其实苹果在 CLLocationManager 中还为我们准备了另一个新属性：allowsBackgroundLocationUpdates。该属性默认为 NO，将其设置成 YES 以后，就算应用当前获得的是 WhenInUse 权限，在应用切后台以后也能获得定位更新，不过同时屏幕顶部会有一个 Blue Bar 提醒用户当前应用正在后台使用定位数据

### 参考文档

* [Location and Maps Programming Guide](https://developer.apple.com/library/ios/documentation/UserExperience/Conceptual/LocationAwarenessPG/GettingHeadings/GettingHeadings.html)
