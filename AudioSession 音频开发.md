AVAudioSession 音频开发

1、申请录音权限
Until the user grants your app permission to record, your app can record only silence.

```
- (void)requestRecordPermission:(PermissionBlock)response NS_AVAILABLE_IOS(7_0) __TVOS_PROHIBITED;
```

如果是app不是首次调用该方法，response block 会被同步调用，立即返回结果；
如果是app首次调用该方法，response block 不会被同步调用，知道用户点了录音权限弹框上面的允许/不允许按钮才返回。

2、启动 AVAudioSession

```
- (BOOL)setActive:(BOOL)active error:(NSError **)outError;
- (BOOL)setActive:(BOOL)active withFlags:(NSInteger)flags error:(NSError **)outError NS_DEPRECATED_IOS(4_0, 6_0);
- (BOOL)setActive:(BOOL)active withOptions:(AVAudioSessionSetActiveOptions)options error:(NSError **)outError NS_AVAILABLE_IOS(6_0);
```

启动方法调用后必须要判断是否启动成功，启动不成功的情况经常存在，例如一个前台的app正在播放，你的 app 正在后台想要启动 AudioSession 那就会返回失败；如果另外一个活动中的 Audio Session 的优先级高于你所定义的（比如说有电话打进来）或者有一个允许混音的 Audio Session，那么激活也可能会失败。如果当前的音频相关对象正在运行的话，取消 Audio Session 有可能会失败。

一般情况下我们在启动和停止 AudioSession 调用第一个方法就可以了。但如果你正在做一个即时语音通讯 app 的话（类似于微信、易信）就需要注意在 deactive AudioSession 的时候需要使用第二个方法，inFlags 参数传入kAudioSessionSetActiveFlag_NotifyOthersOnDeactivation（AVAudioSession给options参数传入AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation）。当你的 app deactive 自己的 AudioSession 时系统会通知上一个被打断播放app打断结束（就是上面说到的打断回调），如果你的app在deactive时传入了 NotifyOthersOnDeactivation 参数，那么其他 app 在接到打断结束回调时会多得到一个参数 kAudioSessionInterruptionType_ShouldResume 否则就是ShouldNotResume（AVAudioSessionInterruptionOptionShouldResume），根据参数的值可以决定是否继续播放。

如果你的应用同时启动了多个播放或录音，如果 setActive:NO 则会把应用的整个音频 audio session 都关掉。也就是说只要应用里还有一个使用 audio session 的地方，就不应该调用 setActive:NO，否则会有报错：

```
AVAudioSession.mm:692: -[AVAudioSession setActive:withOptions:error:]: Deactivating an audio session that has running I/O. All I/O should be stopped or paused prior to deactivating the audio session.
```

3、AVAudioSessionCategoryOptions

```
enum {  
   AVAudioSessionCategoryOptionMixWithOthers  = 1, // 混合其他已激活的 Session 的音频。使用这个选项激活 Session 的话，会打断其他 app 正在播放的声音。如果不使用的话，则会打断其他不允许混合的 session  
   AVAudioSessionCategoryOptionDuckOthers  = 2, // 在本 Session 播放时避开其他 session 的音频  
   AVAudioSessionCategoryOptionAllowBluetooth  = 4, // 允许蓝牙作为输入  
   AVAudioSessionCategoryOptionDefaultToSpeaker  = 8 // 接入到系统默认的扬声器  
};  
typedef NSUInteger  AVAudioSessionCategoryOptions;  
```

前两个个 Options 都需要在 AVAudioSessionCategoryPlayback 或者 AVAudioSessionPlayAndRecord 下才能使用，第三个在 AVAudioSessionPlayAndRecord 或者 AVAudioSessionRecord，最后一个需在 AVAudioSessionPlayAndRecord 下使用


4、设置 Category

```
- (BOOL)setCategory:(NSString *)category error:(NSError **)outError 
- (BOOL)setCategory:(NSString *)category withOptions:(AVAudioSessionCategoryOptions)options error:(NSError **)outError 
```

一般会在激活之前设置好 Category 和 mode。但是也可以在已激活的 audio session 中设置，不过会在发生 route change 之后才会发生改变。

Category 类型：

```
/*  Use this category for background sounds such as rain, car engine noise, etc.  
 Mixes with other music. */
 
/* 用于非以语音为主的应用，使用这个category的应用会随着静音键和屏幕关闭而静音。并且不会中止其它应用播放声音，可以和其它自带应用如iPod，safari等同时播放声音。注意：该Category无法在后台播放声音。 */
AVF_EXPORT NSString *const AVAudioSessionCategoryAmbient;
	
/*  Use this category for background sounds.  Other music will stop playing. */
/* 类似于AVAudioSessionCategoryAmbient 不同之处在于它会中止其它应用播放声音。 这个category为默认category。该Category无法在后台播放声音。 */
AVF_EXPORT NSString *const AVAudioSessionCategorySoloAmbient;

/* Use this category for music tracks.*/
/* 用于以语音为主的应用，使用这个category的应用不会随着静音键和屏幕关闭而静音。可在后台播放声音。 */
AVF_EXPORT NSString *const AVAudioSessionCategoryPlayback;

/*  Use this category when recording audio. */
/* 用于需要录音的应用，设置该category后，除了来电铃声，闹钟或日历提醒之外的其它系统声音都不会被播放。该Category只提供单纯录音功能。 */
AVF_EXPORT NSString *const AVAudioSessionCategoryRecord;

/*  Use this category when recording and playing back audio. */
/* 用于既需要播放声音又需要录音的应用，语音聊天应用(如微信）应该使用这个category。该Category提供录音和播放功能。如果你的应用需要用到iPhone上的听筒，该category是你唯一的选择，在该Category下声音的默认出口为听筒（在没有外接设备的情况下）。 */
AVF_EXPORT NSString *const AVAudioSessionCategoryPlayAndRecord;

/*  Use this category when using a hardware codec or signal processor while
 not playing or recording audio. */
AVF_EXPORT NSString *const AVAudioSessionCategoryAudioProcessing __TVOS_PROHIBITED;

/*  Use this category to customize the usage of available audio accessories and built-in audio hardware.
 For example, this category provides an application with the ability to use an available USB output 
 and headphone output simultaneously for separate, distinct streams of audio data. Use of 
 this category by an application requires a more detailed knowledge of, and interaction with, 
 the capabilities of the available audio routes.  May be used for input, output, or both.
 Note that not all output types and output combinations are eligible for multi-route.  Input is limited
 to the last-in input port. Eligible inputs consist of the following:
	AVAudioSessionPortUSBAudio, AVAudioSessionPortHeadsetMic, and AVAudioSessionPortBuiltInMic.  
 Eligible outputs consist of the following: 
	AVAudioSessionPortUSBAudio, AVAudioSessionPortLineOut, AVAudioSessionPortHeadphones, AVAudioSessionPortHDMI, 
	and AVAudioSessionPortBuiltInSpeaker.  
 Note that AVAudioSessionPortBuiltInSpeaker is only allowed to be used when there are no other eligible 
 outputs connected.  */
AVF_EXPORT NSString *const AVAudioSessionCategoryMultiRoute NS_AVAILABLE_IOS(6_0);
```

5、mode

```
NSString *const  AVAudioSessionModeDefault;  
NSString *const  AVAudioSessionModeVoiceChat;  
NSString *const  AVAudioSessionModeGameChat;   
NSString *const  AVAudioSessionModeVideoRecording;  
NSString *const  AVAudioSessionModeMeasurement;  
NSString *const  AVAudioSessionModeMoviePlayback;  
NSString *const  AVAudioSessionModeVideoChat;
```

设置 mode:

```
- (BOOL)setMode:(NSString *)theMode error:(NSError **)outError  
```

6、audio route

audio route 属性有以下两个，一个是默认的听筒，另一个则是扬声器：

```
enum {  
   kAudioSessionOverrideAudioRoute_None    = 0,
   kAudioSessionOverrideAudioRoute_Speaker = 'spkr'
}; 
```
Audio Route的选择
当你的iPhone接有多个外接音频设备时（耳塞，蓝牙耳机等），AudioSession将遵循last-in wins的原则来选择外接设备，即声音将被导向最后接入的设备。

当没有接入任何音频设备时，一般情况下声音会默认从扬声器出来，但有一个例外的情况：在PlayAndRecord这个category下，听筒会成为默认的输出设备。如果你想要改变这个行为，可以提供MPVolumeView来让用户切换到扬声器，也可通过overrideOutputAudioPort方法来programmingly切换到扬声器，也可以修改category option为AVAudioSessionCategoryOptionDefaultToSpeaker。

参考文档：http://blog.csdn.net/xy5811/article/details/8563137

7、AVAudioSessionPortOverrideSpeaker 和 AVAudioSessionCategoryOptionDefaultToSpeaker 异同

相同点：

二者都只适用于 AVAudioSessionCategoryPlayAndRecord category。

不同点：

设置方法不同：
[session overrideOutputAudioPort:AVAudioSessionPortOverrideSpeaker error:&error];
[audioSession setCategory:AVAudioSessionCategoryPlayAndRecord withOptions:AVAudioSessionCategoryOptionDefaultToSpeaker error:&error];


AVAudioSessionPortOverrideSpeaker：
设置之后，如果此时插入耳机，在拔掉。播放的声音会从听筒输出，而不是回到扬声器。
这是一种临时将输出切换到扬声器的方式，任何 route 的切换或者 interruption 都将使音频输出变回 normal route(AVAudioSessionPortOverrideNone)。

AVAudioSessionCategoryOptionDefaultToSpeaker：
设置之后，始终输出到扬声器，而不是其他接收器，如果没有耳机。（简要的说，就是如果有个蓝牙音箱，哪怕接上都不会有声音输出到蓝牙音响，插上耳机，则会有声音输出到耳机。）

如果在某个场景下，希望强制从speaker输出声音，最好使用

AVAudioSessionCategoryPlayback, 而不是 AVAudioSessionCategoryPlayAndRecord ，因为后者默认从听筒输出。无外接设备的情况。
[session setCategory:AVAudioSessionCategoryPlayback error:&error];

需要使用扬声器和听筒切换场景，用 AVAudioSessionCategoryPlayAndRecord

参考文档：https://developer.apple.com/library/ios/qa/qa1754/_index.html
