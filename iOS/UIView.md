---
layout: post
title: 如何正确地写好一个界面
category: UI
tags: [UI, iOS, UIView, UIViewController, MVC, MVP, MVVM]
---


<br/>


写界面可以说是每位移动应用开发者的基本功，也是一位合格移动应用开发者绕不过去的坎。但就如不是每一位开发者都能够成为合格的开发者一样，本人在不同的团队中发现，甚少有人能够编写出合格的UI代码；而非常奇怪的是，在很多的开发者论坛上看到我们移动开发者更多关注于某个控件或者是动画，但却很少看到深入剖析UI机制，指导UI开发的文章。

由于界面涉及到的方面实在过于广泛，本文不可能事无巨细，一一道来，所以本文先立足于点，深入剖析iOS UI系统中不被重视却非常重要的机制，帮助本文读者对iOS的UI系统有整体了解；进而以点带面，拓展到UI逻辑设计和架构设计模式的讨论；最后读文而有所思有所得，设计开发出高效、易用、流畅的UI模块。

本文章节如下：

1. 基础与本质：说明普遍意义上的UI系统的三大模块，让读者从整体上对UI系统有清楚的认识。
2. View：深入View的内部机制，View与Layer之间的关系，以及Offscreen Render；
3. ViewController：讲解ViewController在UI系统中所扮演的角色，以及UI架构设计中ViewController运用和实践；
4. MVC、MVP、MVVM：简单分析三种主流的架构设计模式及其异同，并简单提出了一些做架构设计意见和想法；
5. 总结。

各章节间没有必然的联系，读者可以选择感兴趣章节阅读。



<br/><br/>
#1. 基础与本质

终端App开发区别于后端开发最大的不同，就是终端开发很大部分的逻辑是为用户提供界面以供人机交互，即所谓的UI(User Interface)。所以所有的UI架构主要关注三大模块：界面布局管理，渲染及动画、事件响应；

<br/>
###1.1 布局管理
即在规定的坐标系统上，按照一定的层级顺序位置大小排布在容器内。一个UI系统必然有个基于坐标的布局管理系统，不管是Windows、Sysbian，还是Andorid、iOS。好的布局管理机制直接影响界面逻辑实现的难易程度；

我们现在日常接触到的App的UI坐标系统都是二维的，我们现在玩的3D游戏，受限于二维的展示屏幕，所以实质上只是三维在二维上的映射投影。我们一直在往更高的维度发展：全息影像、Hololens等等。在此可以设想下，未来我们构建界面的布局管理很可能就是基于真实三维坐标。


<br/>
###1.2 动画及渲染

UI之所以叫User Interface，就是因为UI通过视觉上的展示，为用户提供信息。这些信息的展示需要通过一系列复杂的计算，最后操作液晶体展示在显示屏上，这一系列过程就是渲染和动画；

下图就是应用界面渲染到展示的流程：

![Render](/assets/images/2015-12/CoreAnimationPipline.jpg)

*引自[WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)*

这里不展开来讲，推荐没看过的同学都认真观看，能够很好的理解渲染流程和界面优化；

推荐资料：

* [WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)
* Objc.io的文章：[#Objc Issue 3:Views - Getting Pixels onto the Screen](https://www.objc.io/issues/3-views/moving-pixels-onto-the-screen/)


<br/>
###1.3 事件响应

UI除了展示信息之外，还需要接收并响应用户的点击、手势、摇晃等事件，经过一系列操作后更新展示信息，展示给用户；正确及时地响应用户的操作并给予反馈，是良好用户体验的保证。为何Android设备普遍给人的感觉比iOS设备要卡，其中一个主要的原因是iOS系统将响应用户事件放在主线程的最高优先级。

<br/>
###1.4 UI系统架构
从整体理解了上述三个方面，你会对UI架构有系统认识。iOS中的UI系统架构如下：

![iOS UI Arch](/assets/images/2015-12/UI_Arch.jpg)

*引自[WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)*


<br/><br/>
#2.  View

UIView是UIKit中最基本控件，就如同NSObject基本上是Cocoa库内所有类的基类一样，UIView也是UIKit中所有界面控件的基类。只要你愿意，你甚至只用UIView就可以搭建你的App（不过iOS9做了约束，必须设置keyWindow的rootViewControler）。

一般来说，熟练掌握常用的UIView子类控件（如UIButton, UIImageView, UILabel等）就足以应付90%的界面编码需要。但想要编写出高效、优美的界面代码，还需要更深入的了解。既然要深入，本文假设你对UIView已经有了初步的了解，至少使用写过几个完整的页面；基于此设定下，本文讨论聚焦于以下几点：

1) UIView 与 CALayer：讨论UIView背后的CALayer，了解CALayer与UIView的关系及渲染流程；

2) Offscreen Render：阐述什么是Offscreen Render（离屏渲染），以及一些避免离屏渲染的方法；

3) UIResponser：讨论UIView和UIViewController的父类UIResponser，分析iOS设备上的事件响应链；

4) 设计与实践：结合本人开发实践经验，说明在UIView应用中好的设计实践规则；

参考：[View Programming Guide for iOS](https://developer.apple.com/library/ios/documentation/WindowsViews/Conceptual/ViewPG_iPhoneOS/WindowsandViews/WindowsandViews.html)


<br/>
##2.1 UIView 与 CALayer

我们应该都知道每个UIView都包含了一个CALayer，就算你没直接看过CALayer，应该也使用过。比如给一个View切个圆角：```view.layer.cornerRadius = 5.0f;```；加个边框：```view.layer.borderWidth = 1.0f; view.layer.borderColor = [UIColor darkGrayColor].CGColor;```，这里使用的layer就是CALayer。

CALayer是QuartzCore库内的类，是iOS上最基本的绘制单元；而UIView只是CALyer之上的封装，更准确的来说，UIView是CALyer的简版封装，加上事件处理的集合类。事件处理我们下一节再讨论，这里的简版封装如何理解，为什么不直接使用CALayer？

首先，如上一段所述，CALayer是最基本的绘制单元，每一个UIView都有一个CALayer的变量(```public var layer: CALayer { get }```)，UIView的渲染实质就是这个layer的渲染。我们可以看看<QuartzCore/CALayer>的类定义，里面有很多属性（变量）及方法在<UIKit/UIView>中可以找到几乎一模一样的对应；如属性变量```frame```、```hidden```，方法```public func convertPoint(p: CGPoint, fromLayer l: CALayer?) -> CGPoint```等；但也有更多的属性方法是UIView所没有的，这里就一一列举了。我们可以看到UIView其实是把常用的接口（属性和方法）暴露出来了，让UIView更为易用。

其次，我们知道iOS平台的[Cocoa Touch](https://en.wikipedia.org/wiki/Cocoa_Touch) 是源于OS X平台的[Cocoa](https://en.wikipedia.org/wiki/Cocoa_(API))，是在Cocoa的基础上添加了适用于移动手机设备的手势识别、动画等特性；但从底层实现上来说，Cocoa Touch与Cocoa共用一套底层的库，其中就包括了QuartCore.framework；但QuartCore.framework一开始就是为OS X设计的，所以其中有部分特性是不适合做移动设备开发的，比如最重要的坐标系统。因此，我们也就不难理解为何UIView/NSView在CALayer上做了一层封装。

以上，是UIView于CALayer的主要的关系。

<br/>
##2.2 Offscreen Render

当你尚在懵懂未知的开发初期，在写UIScrollView及其子类（UITableView、UICollectionView）时，一定会遇到滚动不流畅，经常卡顿的情况；你认真研究代码，发现你逻辑代码都放到了异步线程，主线程做的都是渲染界面的活，为什么会卡顿？然后你想老手寻求帮助，老手会让你去掉圆角、半透明和阴影之类，App又重回丝般顺滑；你不知道为什么，问老手，他可能会很详细跟你解释一通，然后你一知半解地点点头，脑中一片茫然；较好的情况，也许你依稀记得这么一个词：离屏渲染（Offscreen Render）。那到底什么是Offscreen Render？为什么Offscreen Render会导致卡顿？

在第一章的1.2节中有提到渲染的流程图，我们再更深入点，先看看最基本的渲染通道流程：


<br/>
![iOS UI Arch](/assets/images/2015-12/BaseRender.jpg)

*引自[WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)*

```
注：iOS的GPU渲染机制是Tile-Based的，而Tile-Based GPU也是现在移动设备的主流；
```

我们再来看看需要Offscreen Render的渲染通道流程：


<br/>
![iOS UI Arch](/assets/images/2015-12/MaskingRender.jpg)

*引自[WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)*

一般情况下，OpenGL会将应用提交到Render Server的动画直接渲染显示（基本的Tile-Based渲染流程），但对于一些复杂的图像动画的渲染并不能直接渲染叠加显示，而是需要根据Command Buffer分通道进行渲染之后再组合，这一组合过程中，就有些渲染通道是不会直接显示的；对比基本渲染通道流程和Masking渲染通道流程图，我们可以看到到Masking渲染需要更多渲染通道和合并的步骤；而这些没有直接显示在屏幕的上的通道（如上图的 Pass 1 和 Pass 2）就是Offscreen Rendering Pass。

Offscreen Render为什么卡顿，从上图我们就可以知道，Offscreen Render需要更多的渲染通道，而且不同的渲染通道间切换需要耗费一定的时间，这个时间内GPU会闲置，当通道达到一定数量，对性能也会有较大的影响；

那哪些情况会Offscreen Render呢？

```
1) drawRect
2) layer.shouldRasterize = true;
3) 有mask或者是阴影(layer.masksToBounds, layer.shadow*)；
4）Text（UILabel, CATextLayer, Core Text, etc）
...
```
注：layer.cornerRadius，layer.borderWidth，layer.borderColor并不会Offscreen Render，因为这些不需要加入Mask。

还有更多与Offscreen Render以及动画图形优化相关的知识，请认真观看WWDC。

参考：

* [WWDC2011 #121 Understanding UIKit Rendering](https://developer.apple.com/videos/play/wwdc2011-121/)
* [WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)



<br/>
##2.3 设计与实践

以上几节，对View在开发过程中经常遇到，但并不容易深入理解的概念进行了讨论。接下来，我想脱离View的具体概念，谈谈本人在View设计和开发中的一些实践经验；



<br/>
###2.3.1 精简扁平的View层次结构

复杂的View层次结果不仅会影响渲染效率，而且也会造成代码的臃肿，会造成不可预料的问题并且难以定位；怎么样维护一个精简扁平的View层次结构呢？原则如下：

1) 尽量使用系统原生的控件；

如实现一个icon跟title上下布局的按钮，很多人习惯是使用一个view包含了一个UIButton和一个UILabel。实际上更为推荐的方式是调整UIButon的contentInset/titleInset/imageInset三个参数来达到这个效果，非常简单，并且title有UIButton上的展示方式和特性，如可以设置高亮颜色等；

又比如一个有着复杂一点布局结构的滚动界面，有些开发者会觉得使用UITableView/UICollectionView实现会比较复杂，有些效果可能没办法达到，就用他们的基类UIScrollView来实现，自己造了一大套的轮子，代码可能也变得非常复杂；实际上根据我的经验，通过重写或者是内部属性的调整是完全可以使用UITableView/UICollectionView来达到这个效果，毕竟UITableView/UICollectionView是UIScrollView的子类，功能不会减少，而会更加强大，并且我们还能利用已有的data source和delegate机制，实现设计上的解耦。

其他常见的还有UINavigationBar、UITabBar、UIToolBar等等；

<br/>
2) 合理添加/删除动态View；

有些View是动态的，就是偶尔显示，偶尔隐藏。这类View有两种处理方式：增删，或者显示/隐藏。没有标准的答案，个人更推荐增删的处理方式，即在有需要的时候添加到对应的ContainerView上，在不需要的时候将其删除。这样即可以与懒加载结合在一起，而且也能避免两个动态View的相互影响，比如TableFooterView，或者是错误加载View。但这并不是唯一的方式，假如这个动态View所在的View层级比较简单，并且需要动画进行动态展示，则使用显示/隐藏也是不错的处理方式。


<br/>
###2.3.2 通用控件；

每一个程序员都可以建立自己的代码库，同理，每一位移动开发程序员都可以建立自己的通用控件代码库。这个库内的控件，可以是你自己写的，也可以是优秀的第三方开源控件。建立控件库，除了能够避免重新造轮子，大大提高我们的开发效率，还有更为重要的一点：在运用、改造、重构中掌握接口设计解耦，甚至是架构的知识和经验。

每个App的UI设计、交互、布局和配色往往千差万别，但总脱离不出移动App这一范畴，也就决定了在某些通用的控件交互上会保持一致性，以让用户依据自己在移动应用上的使用经验就能轻松快速上手使用，这就是App的移动性。所以通用控件的适用场景往往是很“通用”的。比如下拉刷新、加载更多、Tab Bar、提示Tips、加载错误重新加载等等。在新的App或者功能模块上运用这些控件时，你就会思考怎么让控件更加通用，即不影响旧的逻辑，又能够适用新的需求，这对于做界面的架构设计是非常好的锻炼。

<br/>
###2.3.3  合理运用VC在替代View组合复杂界面；

在界面开发过程中，我们常常会遇到复杂的界面，比如多页界面、多种布局方式展示多业务的首页等，但由于很大部分开发者已经对“一屏就是一个VC”这一初学者的习惯奉为教条，写出一个庞然大View，再加上复杂的逻辑代码，这一块的代码很可能就演变成了谁都不敢动的禁区。一个VC可以管理多个VC，所以合理的使用VC来替代View进行复杂界面组合，不仅能够将复杂界面切分成更小的粒度，逻辑代码也同步合理划分，便于维护和重构；而依托VC的机制，还能View和数据的动态加载管理。

下一章中关于轻VC的讨论是这一节知识的拓展。




<br/><br/>
#3.  ViewController

上一节关于View的章节已讨论了iOS界面机制，这一节则主要是来谈谈在写界面过程中的设计问题和基本规范；

ViewController在iOS只是一个非常重要的概念，它是我们在开发界面时最常打交道的模块，其在一个App中所扮演的角色，[View Controller Programming Guide for iOS](https://developer.apple.com/library/prerelease/ios/featuredarticles/ViewControllerPGforiPhoneOS/index.html#//apple_ref/doc/uid/TP40007457-CH2-SW1) 中有清晰准确的描述：

1) View Management：管理View；

2) Data Marshalling：管理数据；

3) User Interactions：响应用户交互；

4) Resource Management：管理资源；

5) Adaptivity：适配不同的屏幕尺寸空间的变化；

可以看到，ViewController有太多的事情要做，这也就导致了ViewController非常容易变得代码膨胀、逻辑混乱等问题；依照个人经验，一个ViewController类的有效代码超过500行，这个ViewController就会变得难以维护，但实际上在开发过程中，往往会遇到上1K行，甚至2~3K行的ViewController类；当一个ViewController类达到2~3K行，就意味着其他开发者接手这个模块来修改东西，已经无法通过滚动来定位代码，只能通过搜索；

所以，在进行界面开发时，ViewController需要特别注意模块设计，将不同的模块按照逻辑进行一定的拆分，即解耦，又防止ViewController模块的代码膨胀。这就是轻VC的理念；


<br/>
##3.1 轻VC

轻VC是前两年非常火的名词，现在似乎已经成为了一种业界规范或者是惯例。同上所述，一个VC的类，如果有效代码超过了500行，则表示这个类看是变得臃肿而难以维护；到达800行，只能通过搜索来定位代码时，重构已势在必行；

关于轻VC，objc.io的开篇第一章[#Issue 1 : Lighter View Controllers](https://www.objc.io/issues/1-view-controllers/)，足见这一理念的重要性。掌握轻VC的理念基本上是一个iOS开发者从初级迈向高级必备技能。[#Issue 1 : Lighter View Controllers](https://www.objc.io/issues/1-view-controllers/) 文中介绍了构建轻VC几种常见的方式：

1) 将数据源等复杂接口从VC中剥离；

2) 把业务逻辑代码抽象到Model层；

3) 将复杂View抽象成独立的类；

4) 使用VC的Containment的特点，将一个VC中逻辑分离的界面模块剥离成为多个子VC；

想要设计出合理而易于理解和维护的轻VC结构，需要掌握轻VC的知识并有一定实践经验。在以下情况下，可以考虑将一个VC设计或者重构成更多模块更多类的轻VC结构：

1) 如上所述，代码超过500行时；

2) VC内的View的数据源来自多个不同的地方；

3) VC内有多个复杂的View，需要展示数据实体类较为复杂；

总之，当你感觉你的VC已经变得臃肿，那么就可尝试轻VC的实践，实践才有收获。


<br/>
##3.2 VC的设计

相对于View关注于布局和展示，VC更关注设计和管理。本节以一个实例，来简单介绍在一个完整App中的VC设计。

先来看一个常见的UI结构设计例子：

<br/>
![iOS UI Arch](/assets/images/2015-12/CommonUIArchDesign.jpg)


这个图应该非常容易理解：最底部是一个侧滑抽屉控件，该抽屉包含了App内容展示的TabBarController和设置的VC；TabBarController的子Item VC包含了相应业务的List VC，点击List VC进入到详情View内；有些详情VC是使用WebViewController来进行内容的展示。非常简单，不是么？接下来说明该设计的洞见：

1) **Root ViewController**，是整个App内Window的根VC，这是一个生命周期与App相同的VC，即Window的RootViewController是唯一且一直存在的，需要切换场景则使用这个Root VC控制子VC切换来实现（常见于场景：需要进行强登录，即登录之后才能使用的App，登录成功后从登录界面切换到主界面，则登录VC和主界面VC都应该是Root VC的子VC，受Root VC的控制来进行切换）。这个RootViewController建议是一个UINavigationController，以此保证足够扩展性，并提供更为丰富的界面交互选择。这个Root VC的生命周期与App一致，这样一些突发的灵活分支界面可以很好的展示在Root VC上，如全局的Loading提示、OpenURL的分支调整等；

2) **Main ViewController**：主界面，是主要业务展示界面的根界面。该VC与RootVC功能上会很容易重合在一起，但需要注意的是，该VC并非一直存在，但切换到一些特定分支时，该VC会从Root VC上remove掉，比如前面所说的强登录App，登录界面与主界面就会需要进行切换。另外，该VC隔离了主要业务展示界面的VC与Root VC，便于App整体界面风格的改版和重构。比如现在上图展示的是一个侧滑抽屉+TabBar的组合，那到下个版本改版把侧滑抽屉去掉，那么只需要使用TabBar替换DrawerMenu VC在Main VC中的位置即可，而不会影响到RootVC中其他分支展示出来的界面（如Push等）。

3) **TabBarItem ViewController**：作为TabBar Controller的子Item VC，通常会设计为NavigationController，用以管理各TabBarItem内的VC栈。
注：如果需要在Push进入二级界面（Detail VC）时隐藏TabBar，只需要设置二级VC的```hidesBottomBarWhenPushed = true```即可，如果想更加灵活的控制TabBar，例如进到三级页面的时候显示出TabBar（这个场景应该很少见），或者你的TabBar是自定义的，可以参考我写的一个开源控件[MZNavTab](https://github.com/jaminzzhang/MZNavTab)；

本节所示例的UI结构是一个非常通用的UI结构，市面上除游戏外60%以上的App都是类似的UI交互（统计来源于个人手机），假如你的UI交互与此类似而你的UI结构很混乱的话，不如尝试下这个UI结构设计。


<br/>
#4. MVC、MVP、MVVM

**MVC**：

<br/>
![iOS UI Arch](/assets/images/2015-12/MVC.jpg)


**MVP**：

<br/>
![iOS UI Arch](/assets/images/2015-12/MVP.jpg)


**MVVM**：

<br/>
![iOS UI Arch](/assets/images/2015-12/MVVM.jpg)


图注：
虚线箭头：表示两者之间是非强依赖关系。如MVC图，View与Model一般没有直接联系。

虚线矩形：表示该模块在对应架构设计中的隐性存在。即一般性架构中并没有这个角色，但立足于iOS这个平台，这又是不可或缺的一部分；

本文并不打算将MVC、MVP、MVVM这个几个通用架构设计模式的概念统统在这里叙述一遍，上面三个图基本上能够很明白地对比出三者之间的差异。也许与你在网上看到的不尽相同，这是因为以上三图更立足于iOS平台。

<br/>
##4.1 MVC

我们最初看到的MVC设计模式图可能是这样的：

<br/>
![iOS UI Arch](/assets/images/2015-12/MSDN_MVC.jpg)

*引自[MSDN#ASP.NET - Single-Page Applications: Build Modern, Responsive Web Apps with ASP.NET(https://msdn.microsoft.com/en-us/magazine/dn463786.aspx)*

而苹果官方给的MVC的设计模式图却是这样的：

<br/>
![iOS UI Arch](/assets/images/2015-12/Apple_MVC.jpg)


到底哪一副图才是真正的MVC？我的答案只能是：都是。

MVC从施乐帕克实验室提出至今，已经应用到各种应用开发领域中：Web App可以用MVC，iOS/Android/Windows客户端应用也用MVC，Web前端也在用MVC，等等；这些几乎涵盖了我们常见的开发领域，所以MVC其实已经超越了他原本最初的设计，基于所有涉及展示的应用都能套上MVC，只不过不同的平台在设计上略有差别。而MVP和MVVM，也不过是MVC的衍生变种，除这两者之外，还有我们没怎么见过的[HMVC](https://en.wikipedia.org/wiki/Hierarchical_model–view–controller)、[MVA](https://en.wikipedia.org/wiki/Model–view–adapter)等。

<br/>
##4.2 Model Layer

在讨论MVP和MVVM之前，我想先明确一个经常被误解的概念：Model。由于Model这个词太通用化，如数据Model，数据库Model，这就导致了Model这一概念理解差异化，简单的说，就是被玩坏。抛开其他，我们来看看常见的定义：

**[Wikipedia](https://en.wikipedia.org/wiki/Model–view–controller)的定义**：

```
The central component of MVC, the model, captures the behavior of the application in terms of its problem domain, independent of the user interface.[11] The model directly manages the data, logic and rules of the application.
```

**[MSDN](https://msdn.microsoft.com/en-us/library/ff649643.aspx)中的定义**：

```
Model. The model manages the behavior and data of the application domain, responds to requests for information about its state (usually from the view), and responds to instructions to change state (usually from the controller).

```

上面两个定义基本一致：Model，管理应用的行为和数据。

再来看看**Apple官方文档[Model-View-Controller](https://developer.apple.com/library/prerelease/watchos/documentation/General/Conceptual/DevPedia-CocoaCore/MVC.html#//apple_ref/doc/uid/TP40008195-CH32-SW1)的定义**：

```
Model Objects

Model objects encapsulate the data specific to an application and define the logic and computation that manipulate and process that data. For example, a model object might represent a character in a game or a contact in an address book. A model object can have to-one and to-many relationships with other model objects, and so sometimes the model layer of an application effectively is one or more object graphs. Much of the data that is part of the persistent state of the application (whether that persistent state is stored in files or databases) should reside in the model objects after the data is loaded into the application. Because model objects represent knowledge and expertise related to a specific problem domain, they can be reused in similar problem domains. Ideally, a model object should have no explicit connection to the view objects that present its data and allow users to edit that data—it should not be concerned with user-interface and presentation issues.

Communication: User actions in the view layer that create or modify data are communicated through a controller object and result in the creation or updating of a model object. When a model object changes (for example, new data is received over a network connection), it notifies a controller object, which updates the appropriate view objects.
```


虽然Apple的官方文档是定义Model **Objects**，但它的含义还是封装数据以及管理数据相关的逻辑计算；


所以这里需要明确的一个概念是：在MVC的设计模式中，Model是一个Layer，而不只是一个数据模型（Data Model）类。总体来说，Model Layer 包含了数据模型，以及管理这些数据相关的逻辑计算，如本地数据变化、数据缓存、从网络请求数据等业务逻辑。关于这个问题，还可以参考这篇文章：《[iOS应用架构谈 view层的组织和调用方案](http://casatwy.com/iosying-yong-jia-gou-tan-viewceng-de-zu-zhi-he-diao-yong-fang-an.html)》。但有一点需要说明：该文章更倾向于从Model Object上思考Model的定义，因为里面的关于Model的示例是从数据模型中扩展出业务接口；而本人则更倾向于从Model Layer来思考Model，即Model并不限于数据模型，可以是数据管理类（各种Manager）、请求队列管理等等。

<br/>
##4.3 MVP VS MVVM

上一节关于Model Layer中推荐的文章《[iOS应用架构谈 view层的组织和调用方案](http://casatwy.com/iosying-yong-jia-gou-tan-viewceng-de-zu-zhi-he-diao-yong-fang-an.html)》对MVC和MVVM都做了非常详细的讨论，是一篇非常不错的文章，推荐各位阅读，那么本节就来说说MVP，以及我为什么更倾向于选择MVP作为App架构设计中的设计框架。

回顾下在本章一开始祭出的MVP以及MVVM两张图，两者之间有什么不同？

MVVM的VM(View Model)到V(View)，比MVP的P(Presenter)到V(View)，多了数据绑定。也就是

[MVP](https://en.wikipedia.org/wiki/Model–view–presenter)：是MVC的变种，其中Model和View的定义与MVC的一致，不同点在于：MVC的Controller是管理一组Model与View之间交互逻辑，是一个管理者；而Presenter（展示者）则是Model于View之间的连接者，针对特定模块的View提供对应的格式化的Model数据，将View中的行为反馈到Model中。所以MVC中的Controller一般会管理一个或多个Model和一个或多个View，而Presenter则是 M-P-V 一对一，有更细的粒度和更好的解耦。

从MVP的定义，你会发现MVP与MVVM极其相似，Presenter与View Model扮演的角色基本没有差别，除了前面所说到绑定机制。但绑定机制既有很明显的强大优点——自动连接View和Model，也有很明显的缺点——更高的耦合度，更复杂的代码逻辑；但让人感叹命运无常的是：MVVM随着ReativeCocoa而在iOS平台炙手可热，而iOS平台上甚少有人提及的MVP，在Android平台却几乎成了标准(Android5.0引入了数据绑定支持，MVVM会在Android平台有新的发展)。

我为什么倾向于MVP？不过是相比于MVVM双向绑定的便利，我更希望我的App设计中有更强的灵活性和扩展性。没有完美的架构设计模式，只有适用于你的App业务场景和团队的设计模式。比如数据逻辑并不复杂、更注重视觉展示的应用，原始的MVC往往是最优解。所有的MVC衍生出的变种，无非是为了Solve The Problem。


<br/>
##4.4 架构设计模式应用

无论MVC、MVP还是MVVM，都是指导我们进行架构设计的模式，并非可以生搬硬套的；而且在实际的应用中，对于这些设计模式总会有不同的理解，并且需要根据项目需求进行必要的调整；更为重要的是在我们App的架构设计中，处理好Model-View-Controller之间的关系只是基础，最主要的挑战来自于复杂的业务逻辑和场景，这才是体现一个架构师能力所在。


唐巧前不久写的一篇文章《[被误解的MVC和被神化的MVVM](http://blog.devtang.com/blog/2015/11/02/mvc-and-mvvm/)》对MVC和MVVM的实践的讨论应该是体现了现在移动端主流架构思想，其中对```网络请求层、ViewModel 层、Service 层、Storage 层等其它类```的提取设计，才决定了一个App架构设计的优劣。


对于架构设计，我准备在下一篇文章，结合本人在iOS/Android两端的设计经验，做个深入的讨论，并给出自己的设计范例，供各位讨论参考。这里先抛出几个在架构设计中最常思考的点，作为下一篇文章的引子：

1) 架构是为了解耦，越松的耦合就代表越多的份层，但人的思维总是更愿意接受直线思维，怎么解决这个矛盾？

2) 在一个App中，统一（一致）的架构设计能够让逻辑代码更健壮，更有利于团队成员间的沟通和项目维护，但如何解决其和灵活性之间的矛盾？

3) 架构设计是否只包含逻辑分层？需要设计数据流和多线程么？

4) 设计模式中的几大原则；


<br/><br/>
##5 总结

以上四个章节，先从UI整体出发，到剖析UIView几点重要机制，接着讨论怎么用好VC这个UI中重要的管理角色，最后则漫谈了MVC/MVVM/MVP几个架构设计模式的异同和实践应用，想通过以点带面，让我们在关注了具体实现之后，能够脱离出来，从俯视下我们App开发更为整体核心的部分。




<br/><br/>
参考阅读：

* [WWDC2011 #121 Understanding UIKit Rendering](https://developer.apple.com/videos/play/wwdc2011-121/)
* [WWDC2014 #419 Advanced Graphics and Animations for iOS Apps](https://developer.apple.com/videos/play/wwdc2014-419/)
* [iOS Developer Library#View and Window Architecture](https://developer.apple.com/library/ios/documentation/WindowsViews/Conceptual/ViewPG_iPhoneOS/WindowsandViews/WindowsandViews.html)
* [iOS Developer Library#UIKit Framework](https://developer.apple.com/library/ios/documentation/UIKit/Reference/UIKit_Framework/)
* [iOS Developer Library#Getting Started with Graphics & Animation](https://developer.apple.com/library/prerelease/mac/referencelibrary/GettingStarted/GS_GraphicsImaging/_index.html)
* [#Objc 3-View](https://www.objc.io/issues/3-views/)
* [#Objc 1 : Lighter View Controllers](https://www.objc.io/issues/1-view-controllers/)


* [被误解的MVC和被神化的MVVM](http://blog.devtang.com/blog/2015/11/02/mvc-and-mvvm/)
* [iOS应用架构谈 view层的组织和调用方案](http://casatwy.com/iosying-yong-jia-gou-tan-viewceng-de-zu-zhi-he-diao-yong-fang-an.html)


