原文链接：
http://www.cnblogs.com/breezemist/p/4422319.html

做项目时发现，在一个界面上的2个button竟然可以同时点击，依次push进去了2个 controller！我就产生了疑问，一个view的multipleTouchEnabled属性默认是false啊，那怎么会可以同时点击这个view上的2个子view呢？原来是对multipleTouchEnabled属性理解不对！下面看看对这个属性的理解。

multipleTouchEnabled的官方解释如下：

When set to true, the receiver receives all touches associated with a multi-touch sequence. When set to false, the receiver receives only the first touch event in a multi-touch sequence. The default value of this property is false.

Other views in the same window can still receive touch events when this property is false. If you want this view to handle multi-touch events exclusively, set the values of both this property and the exclusiveTouch property to true.
 

 

 

exclusiveTouch的官方解释如下：

Setting this property to true causes the receiver to block the delivery of touch events to other views in the same window. The default value of this property is false.

注意：touch events的发送，是以window为一个组，而不是某个在window下的子view。

看到这里其实就可以得出结论了，如果你不想让2个button同时点击，只需要把它们的exclusiveTouch都设定为YES，就行了！multipleTouchEnabled没这个作用，我用错了属性！
 

 

参考的官方文档  Event Handling Guide，这个文档需要仔细阅读！

一个父view的multipleTouchEnabled属性不会影响它的子view的multipleTouchEnabled属性，也就是说，在一个multipleTouchEnabled = false的父view里，仍然可以有接受多个手指点击消息的子view。

 

再来看这个函数touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent)，这里有2个参数，官方说明如下：

The set object. The passed-in NSSet contains all touches that are new or have changed in the phase represented by the method, such as UITouchPhaseBegan for the touchesBegan:withEvent: method.
The event object. The passed-in UIEvent object contains all of the touches for a given multitouch sequence.
The multipleTouchEnabled property is set to NO by default, which means that a view receives only the first touch in a multitouch sequence. When this property is disabled, you can retrieve a touch object by calling the anyObject method on the set object because there is only one object in the set.
下面是另一部分解释

The set of touches is a set (NSSet) of UITouch objects, representing new or changed touches for that phase. For example, when a touch transitions from the Began phase to the Moved phase, the app calls the touchesMoved:withEvent: method. The set of touches passed in to the touchesMoved:withEvent: method will now include this touch and all other touches in the Moved phase. The other parameter is an event (UIEvent object) that includes all touch objects for the event. This differs from the set of touches because some of the touch objects in the event may not have changed since the previous event message.
这里提到了一个名词 a multitouch sequence，一个sequence指从第一个手指按下到最后一个手指抬起，也可以通过以下的话理解：

iOS recognizes touches as part of a multitouch sequence. During a multitouch sequence, the app sends a series of event messages to the target responder. 
 上面的意思就是ios把所有的touch事件都作为 a multitouch sequence 的一部分，在a multitouch sequence生命周期内，不断发送touch event到target。

Each of these touch methods correspond to a touch phase: Began, Moved, Ended, and Canceled. When there are new or changed touches for a given phase, the app object calls one of these methods. Each method takes two parameters: a set of touches and an event.
 

所谓的multipleTouchEnabled = false 指的是这个view 只接收一个multitouch sequence中的第一个点在自己上面的手指的event信息，其余的手指event变化信息，都不会被这个view理会，所以对于一个multipleTouchEnabled = false的view来说，touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) ，第一个参数中永远只有一个touch对象，就是那个手指的event信息，而后边的event则会包含整个multitouch sequence 中的点击信息。

 

因此，可以看出multipleTouchEnabled这个属性不是为了控制这个view下的整个 tree 的点击控制的。这就导致了即使在一个multipleTouchEnabled=false的view上，放置了多个button，可以用多个手指同时点击这些button！

我们看一个例子，代码如下

复制代码
class TouchView: UIView {
    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {
        var touchSet = event.allTouches()
        println("touchesBegan...touches set count is \(touches.count)")
        println("UIEvent touch count is \(touchSet!.count)")
    }

}

class ViewController: UIViewController {

    @IBOutlet weak var view1: UIView!

    @IBOutlet weak var view2: UIView!

    override func viewDidLoad() {

        super.viewDidLoad()

        self.view.multipleTouchEnabled = false

        view1.multipleTouchEnabled = true

        view2.multipleTouchEnabled = false

    }

    override func touchesBegan(touches: Set<NSObject>, withEvent event: UIEvent) {

        println("self view touchesBegan.....")

    }

}

复制代码
界面截图如下



 

 

黄色是self.view  蓝色和红色是2个touchview，我们的点击方式如下

1.点击蓝色view，不放开

2.点击蓝色view，不放开

3.点击红色view，不放开

4.点击红色view，不放开

5.点击蓝色view，不放开

控制台输出如下：

复制代码
touchesBegan...touches set count is 1
UIEvent touch count is 1
touchesBegan...touches set count is 1
UIEvent touch count is 2
touchesBegan...touches set count is 1
UIEvent touch count is 3
touchesBegan...touches set count is 1
UIEvent touch count is 5
复制代码
这里的第三条是红色view第一次点击的输出,第二次的红色view点击输出没有，因为它的view2.multipleTouchEnabled = false

 

再来另一个测试：

1.点击黄色，不放开

2.点击蓝色，不放

3.点击红色，不放

输出如下：

self view touchesBegan.....
touchesBegan...touches set count is 1
UIEvent touch count is 2
touchesBegan...touches set count is 1
UIEvent touch count is 3
另外说一点，如果这里我们没有对红色和蓝色的类重写 touchesBegan 方法，那么系统默认会把消息交给 self.view 也就是黄色view处理。
