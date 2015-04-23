## hit-test流程

* 流程

hit-test使用的是逆序递归遍历法：
>
 It implements it by searching the view hierarchy using reverse pre-order depth-first traversal algorithm.
>

比如window上先后添加了A、B、C三个view：
![traversal](http://smnh.me/images/hit-test-depth-first-traversal.png)

那么屏幕上任何一点发生触摸事件时都会按如下顺序遍历：

** C  如果在C上继续递归遍历C的subview，不在C上则遍历B
** B  如果在B上继续递归遍历B的subview，不在B上则遍历A
** A  如果在A上继续递归遍历A的subview，不在A上则遍历A的上一级

直到找到一个包含点击区域的最上层view。

* 关于多次调用hit-test

** iOS6 及之前系统会重复调用三次hit-test流程；
** iOS7 及之后系统会重复调用两次hit-test流程。

调用那么多次的原因不明。

* 关于UITabbarController

点击包含UITabBar、UIToolBar等控件上方40像素区域内时，多次hit-test的最后一次中point值会发生随机变化，point的Y值会被增大若干不等的像素。

原因未知。


参考链接： [Hit-Testing in iOS](http://smnh.me/hit-testing-in-ios/)
