## iOS定位相关问题

### 一、Heading vs. Course

Core Location提供了两种获取方向相关信息的方法：

* 装有磁力仪(magnetometer)的机器可以提供当前设备所指方向信息，又被称为heading。

* 装有GPS的机器可以提供当前设备的移动方向信息，又被称为course。

Heading和course是有区别的。Heading表示当前设备的实际指向相对于地理北极或地磁北极的角度，course表示当前设备移动的方向，而和设备的实际指向没有关系。我们的app可以根据使用场景来选择到底用哪个数据，比如在步行的时候heading可以更加真实的体现出用户的运行方向信息；而在驾车的时候course能更好的表示出用户的运行方向信息。




### 参考文档

* [Location and Maps Programming Guide](https://developer.apple.com/library/ios/documentation/UserExperience/Conceptual/LocationAwarenessPG/GettingHeadings/GettingHeadings.html)
