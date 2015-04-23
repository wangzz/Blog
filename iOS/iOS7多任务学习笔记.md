## iOS7多任务学习笔记

苹果在iOS7中对后台任务真是下足了功夫，带来了不少变化，体现在以下几点：
* 改变了后台任务的运行方式
* 增加了后台获取（Background Fetch）
* 增加了推送唤醒（静默推送，Silent Remote Notifications）
* 增加了后台传输（￼Background Transfer Service）

#### 一、后台任务执行方式

###### 1、iOS6（及之前）

在应用被切到后台之后，如果有后台任务的话，应用会有 10mins 左右的时间在后台执行该任务，直到超时以后才进入低功耗休眠状态，如下图：

![iOS6 Background Task](http://img.onevcat.com/2013/ios-multitask-ios6.png)


###### 2、iOS7（及之后）

应用被切到后台后，将会有 3mins 左右的时间执行后台任务，然后将处于suspend状态。在设备于特定时刻（比如检查邮件或者有来电等）被唤醒时，之前暂停的任务会被继续执行。

也就是说，iOS7会在应用进入后台后尽快让设备进入休眠状态，以节省电力；系统不会专门为第三方应用保持设备活动状态，只会在适当的时候重新唤醒任务。

![iOS7 Background Task](http://img.onevcat.com/2013/ios-multitask-ios7.png)

对于`Voip`、`位置服务`等后台长时应用，在应用被切到后台以后会处于suspend状态，当有socket消息或者位置更新时，会被唤醒重新执行任务。

#### 二、后台获取

很多应用都是在用户打开应用以后才进行网络请求，这就造成了每次启动应用都有一个等待时间，iOS7中的后台获取就是为了解决这个不足的：用户在打开app之前让应用有机会获取数据并刷新UI，这样在用户启动应用后立即显示最新内容，这是很好的用户体验。

###### 1、启用后台获取

通过Info.plist添加Background fetch选项

###### 2、设定时间间隔

可以通过以下方式设定系统进行后台获取的时间间隔：

```
[[UIApplication sharedApplication] setMinimumBackgroundFetchInterval:UIApplicationBackgroundFetchIntervalMinimum];
```
需要注意的是：

* 如果不设置该值，系统将使用默认值`UIApplicationBackgroundFetchIntervalNever`；
* 就算设置了值，并不是就一定会间隔设定的时间就进行一次后台获取，而是系统会根据设定的时间间隔值在方便的时候（比如检查邮件）进行一次后台获取；

我们需要根据自己应用的实际需要设定一个合理的下载时间间隔。

###### 3、实现相关方法

完成上述两步后需要在AppDelegate里实现下述方法：

```objective-c
//File: YourAppDelegate.m
-(void)application:(UIApplication *)application performFetchWithCompletionHandler:(void (^)(UIBackgroundFetchResult))completionHandler
{
    UINavigationController *navigationController = (UINavigationController*)self.window.rootViewController;

    id fetchViewController = navigationController.topViewController;
    if ([fetchViewController respondsToSelector:@selector(fetchDataResult:)]) {
        [fetchViewController fetchDataResult:^(NSError *error, NSArray *results){
            if (!error) {
                if (results.count != 0) {
                    //Update UI with results.
                    //Tell system all done.
                    completionHandler(UIBackgroundFetchResultNewData);
                } else {
                    completionHandler(UIBackgroundFetchResultNoData);
                }
            } else {
                completionHandler(UIBackgroundFetchResultFailed);
            }
        }];
    } else {
        completionHandler(UIBackgroundFetchResultFailed);
    }
}
```

系统会通过调用该方法实现后台获取功能。

开发者在该方法里要做的事就是完成下载工作、刷新UI，并通知系统获取结束，以便系统尽快回到休眠状态。

通常一次后台获取的执行时间小于一分钟。

通知系统结束的CompletionHandler会接收一个`UIBackgroundFetchResult`参数，该enum的定义如下：

```objective-c
typedef NS_ENUM(NSUInteger, UIBackgroundFetchResult) {
    UIBackgroundFetchResultNewData,
    UIBackgroundFetchResultNoData,
    UIBackgroundFetchResultFailed
} NS_ENUM_AVAILABLE_IOS(7_0);
```
分别表示获取到了新数据（此时系统将对现在的UI状态截图并更新App Switcher中你的应用的截屏），没有新数据，以及获取失败。

另一个比较神奇的地方是系统将追踪用户的使用习惯，并根据对每个应用的使用时刻给一个合理的fetch时间。比如系统将记录你在每天早上9点上班的电车上，中午12点半吃饭时，以及22点睡觉前会刷一下微博，只要这个习惯持续个三四天，系统便会将应用的后台获取时刻调节为9点，12点和22点前一点。这样在每次你打开应用都直接有最新内容的同时，也节省了电量和流量。

###### 4、后台获取调试方法

Xcode5提供了两种测试后台获取的方法：

* 从后天获取中启动应用

点击Xcode5的Product->Scheme->Edit Scheme(或者直接使用快捷键⌘<)。在编辑Scheme的窗口中点Duplicate Scheme按钮复制一个当前方案，然后在新Scheme的option中将Background Fetch打上勾。如下图所示：

![image](http://img.onevcat.com/2013/ios7-back-fetch-scheme.png)

从这个Scheme来运行应用的时候，应用将不会直接启动切入前台，而是调用后台获取部分代码并更新UI，这样再点击图标进入应用时，你应该可以看到最新的数据和更新好的UI了。



* 直接模拟一次后台获取

在app调试运行时，点击Xcode5的Debug菜单中的`Simulate Background Fetch`，即可模拟完成一次获取调用。

#### 三、推送唤醒

在iOS6和之前，用户收到推送消息通过解锁进入你的应用后，appDelegate中通过推送打开应用的回调将被调用，然后你再获取数据，进行显示。

在iOS7中这个行为发生了一些改变，我们有机会使设备在接收到远端推送后让系统唤醒设备和我们的后台应用，并先执行一段代码来准备数据和UI，然后再提示用户有推送。这时用户如果解锁设备进入应用后将不会再有任何加载过程，新的内容将直接得到呈现，这就解决了用户等待问题。

###### 1、启用推送唤醒

在Info.plist的`UIBackgroundModes`中加入`remote-notification`。

###### 2、更改推送payload

在iOS7中，要想使用推送来唤醒应用运行代码的话，需要在payload中加入`content-available`，并设置成1。

```
aps {  
     content-available: 1
     alert: {...}
   }
```

###### 3、实现相关方法

在AppDelegate中实现￼`-application:didReceiveRemoteNotification:fetchCompletionHandle:`即可，类似后台获取部分。

需要说明的是，Apple会限制推送频率。当频率超过一定限制后，带有content-available标志的推送将会被阻塞，以保证用户设备不被频繁唤醒。按照Apple的说法，这个频率在一小时内个位数次的推送的话不会有太大问题。


#### 四、后台传输

iOS6和之前，iOS应用在大块数据的下载这一块限制是比较多的：只有应用在前台时能保持下载（用户按Home键切到后台或者是等到设备自动休眠都可能中止下载），在后台只有很短的最多十分钟时间可以保持网络连接。如果想要完成一个较大数据的下载，用户将不得不打开你的app并且基本无所事事。

iOS7引入了后台传输的相关方式，用来保证应用退出后数据下载或者上传能继续进行。这种传输是由iOS系统进行管理的，没有时间限制，也不要求应用运行在前台。

想要实现后台传输，就必须使用iOS7的新的网络连接的类，`NSURLSession`，这是iOS7中引入用以替代陈旧的`NSURLConnection`的类。

通过`NSURLSession`开始后台下载后，需要在AppDelegate中实现`-application:handleEventsForBackgroundURLSession:completionHandler:`方法，以刷新UI及通知系统传输结束。

需要说明的是：

* `NSURLSession`会首先将下载的内容写到缓存里，等待下载结束时会调用`- (void)URLSession:(NSURLSession *)session downloadTask:(NSURLSessionDownloadTask *)downloadTask didFinishDownloadingToURL:(NSURL *)location;`,这是我们需要将下载完成的文件移到指定的位置；

* 如果下载过程中应用意外退出，会在下次实例化`NSURLSession`对象的时候调用`- (void)URLSession:(NSURLSession *)session task:(NSURLSessionTask *)task didCompleteWithError:(NSError *)error;`方法，以便取出上次下载的文件继续下载，以实现断点续传，而不是从头再来。


#### 五、参考文档

* [WWDC 2013 Session笔记 - iOS7中的多任务](http://onevcat.com/2013/08/ios7-background-multitask/)
