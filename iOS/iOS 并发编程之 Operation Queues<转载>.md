---
layout: post
title: "iOS 并发编程之 Operation Queues<转载>"
date: 2015-07-29 22:06:33 +0800
comments: true
categories:
keywords: iOS 的并发编程模型, iOS, 并发编程模型, 并发编程, Operation Queues, Operation Queue, Operation, Queues, Queue, GCD
---

现如今移动设备也早已经进入了多核心 `CPU` 时代，并且随着时间的推移，`CPU` 的核心数只会增加不会减少。而作为软件开发者，我们需要做的就是尽可能地提高应用的并发性，来充分利用这些多核心 `CPU` 的性能。在 iOS 开发中，我们主要可以通过 Operation Queues、Dispatch Queues 和 Dispatch Sources 来提高应用的并发性。本文将主要介绍 Operation Queues 的相关知识，另外两个属于 Grand Central Dispatch（以下正文简称 `GCD` ）的范畴，将会在后续的文章中进行介绍。

由于本文涉及的内容较多，所以建议读者先提前了解一下本文的目录结构，以便对本文有一个宏观的认识：

- 基本概念
    + 术语
    + 串行 vs. 并发
    + 同步 vs. 异步
    + 队列 vs. 线程
- iOS 的并发编程模型
- Operation Queues vs. Grand Central Dispatch (GCD)
- 关于 Operation 对象
    + 并发 vs. 非并发 Operation
    + 创建 NSInvocationOperation 对象
    + 创建 NSBlockOperation 对象
- 自定义 Operation 对象
    + 执行主任务
    + 响应取消事件
    + 配置并发执行的 Operation
    + 维护 KVO 通知
- 定制 Operation 对象的执行行为
    + 配置依赖关系
    + 修改 Operation 在队列中的优先级
    + 修改 Operation 执行任务线程的优先级
    + 设置 Completion Block
- 执行 Operation 对象
    + 添加 Operation 到 Operation Queue 中
    + 手动执行 Operation
    + 取消 Operation
    + 等待 Operation 执行完成
    + 暂停和恢复 Operation Queue
- 总结

## 基本概念

在正式开始介绍 Operation Queues 的相关知识前，我想先介绍几个在 iOS 并发编程中非常容易混淆的基本概念，以帮助读者更好地理解本文。**注**，本文中的 Operation Queues 指的是 NSOperation 和 NSOperationQueue 的统称。

### 术语

首先，我们先来了解一下在 iOS 并发编程中非常重要的三个术语，这是我们理解 iOS 并发编程的基础：

- 进程（process），指的是一个正在运行中的可执行文件。每一个进程都拥有独立的虚拟内存空间和系统资源，包括端口权限等，且至少包含一个主线程和任意数量的辅助线程。另外，当一个进程的主线程退出时，这个进程就结束了；
- 线程（thread），指的是一个独立的代码执行路径，也就是说线程是代码执行路径的最小分支。在 iOS 中，线程的底层实现是基于 [POSIX threads API](https://developer.apple.com/library/ios/documentation/Cocoa/Conceptual/Multithreading/Introduction/Introduction.html) 的，也就是我们常说的 pthreads ；
- 任务（task），指的是我们需要执行的工作，是一个抽象的概念，用通俗的话说，就是一段代码。

### 串行 vs. 并发

从本质上来说，串行和并发的主要区别在于允许同时执行的任务数量。串行，指的是一次只能执行一个任务，必须等一个任务执行完成后才能执行下一个任务；并发，则指的是允许多个任务同时执行。

### 同步 vs. 异步

同样的，同步和异步操作的主要区别在于是否等待操作执行完成，亦即是否阻塞当前线程。同步操作会等待操作执行完成后再继续执行接下来的代码，而异步操作则恰好相反，它会在调用后立即返回，不会等待操作的执行结果。

### 队列 vs. 线程

有一些对 iOS 并发编程模型不太了解的同学可能会对队列和线程产生混淆，不清楚它们之间的区别与联系，因此，我觉得非常有必要在这里简单地介绍一下。在 iOS 中，有两种不同类型的队列，分别是串行队列和并发队列。正如我们上面所说的，串行队列一次只能执行一个任务，而并发队列则可以允许多个任务同时执行。iOS 系统就是使用这些队列来进行任务调度的，它会根据调度任务的需要和系统当前的负载情况动态地创建和销毁线程，而不需要我们手动地管理。

## iOS 的并发编程模型

在其他许多语言中，为了提高应用的并发性，我们往往需要自行创建一个或多个额外的线程，并且手动地管理这些线程的生命周期，这本身就已经是一项非常具有挑战性的任务了。此外，对于一个应用来说，最优的线程个数会随着系统当前的负载和低层硬件的情况发生动态变化。因此，一个单独的应用想要实现一套正确的多线程解决方案就变成了一件几乎不可能完成的事情。而更糟糕的是，线程的同步机制大幅度地增加了应用的复杂性，并且还存在着不一定能够提高应用性能的风险。

然而，值得庆幸的是，在 iOS 中，苹果采用了一种比传统的基于线程的系统更加异步的方式来执行并发任务。与直接创建线程的方式不同，我们只需定义好要调度的任务，然后让系统帮我们去执行这些任务就可以了。我们可以完全不需要关心线程的创建与销毁、以及多线程之间的同步等问题，苹果已经在系统层面帮我们处理好了，并且比我们手动地管理这些线程要高效得多。

因此，我们应该要听从苹果的劝告，珍爱生命，远离线程。不过话又说回来，尽管队列是执行并发任务的首先方式，但是毕竟它们也不是什么万能的灵丹妙药。所以，在以下三种场景下，我们还是应该直接使用线程的：

- 用线程以外的其他任何方式都不能实现我们的特定任务；
- 必须实时执行一个任务。因为虽然队列会尽可能快地执行我们提交的任务，但是并不能保证实时性；
- 你需要对在后台执行的任务有更多的可预测行为。

## Operation Queues vs. Grand Central Dispatch (GCD)

简单来说，`GCD` 是苹果基于 `C` 语言开发的，一个用于多核编程的解决方案，主要用于优化应用程序以支持多核处理器以及其他对称多处理系统。而 Operation Queues 则是一个建立在 `GCD` 的基础之上的，面向对象的解决方案。它使用起来比 `GCD` 更加灵活，功能也更加强大。下面简单地介绍了 Operation Queues 和 `GCD` 各自的使用场景：

- Operation Queues ：相对 `GCD` 来说，使用 Operation Queues 会增加一点点额外的开销，但是我们却换来了非常强大的灵活性和功能，我们可以给 operation 之间添加依赖关系、取消一个正在执行的 operation 、暂停和恢复 operation queue 等；
- `GCD` ：则是一种更轻量级的，以 `FIFO` 的顺序执行并发任务的方式，使用 `GCD` 时我们并不关心任务的调度情况，而让系统帮我们自动处理。但是 `GCD` 的短板也是非常明显的，比如我们想要给任务之间添加依赖关系、取消或者暂停一个正在执行的任务时就会变得非常棘手。

## 关于 Operation 对象

在 iOS 开发中，我们可以使用 NSOperation 类来封装需要执行的任务，而一个 operation 对象（以下正文简称 operation ）指的就是 NSOperation 类的一个具体实例。NSOperation 本身是一个抽象类，不能直接实例化，因此，如果我们想要使用它来执行具体任务的话，就必须创建自己的子类或者使用系统预定义的两个子类，NSInvocationOperation 和 NSBlockOperation 。

**NSInvocationOperation** ：我们可以通过一个 `object` 和 `selector` 非常方便地创建一个 NSInvocationOperation ，这是一种非常动态和灵活的方式。假设我们已经有了一个现成的方法，这个方法中的代码正好就是我们需要执行的任务，那么我们就可以在不修改任何现有代码的情况下，通过方法所在的对象和这个现有方法直接创建一个 NSInvocationOperation 。

**NSBlockOperation** ：我们可以使用 NSBlockOperation 来并发执行一个或多个 block ，只有当一个 NSBlockOperation 所关联的所有 block 都执行完毕时，这个 NSBlockOperation 才算执行完成，有点类似于 `dispatch_group` 的概念。

另外，所有的 operation 都支持以下特性：

- 支持在 operation 之间建立依赖关系，只有当一个 operation 所依赖的所有 operation 都执行完成时，这个 operation 才能开始执行；
- 支持一个可选的 completion block ，这个 block 将会在 operation 的主任务执行完成时被调用；
- 支持通过 `KVO` 来观察 operation 执行状态的变化；
- 支持设置执行的优先级，从而影响 operation 之间的相对执行顺序；
- 支持取消操作，可以允许我们停止正在执行的 operation 。

### 并发 vs. 非并发 Operation

通常来说，我们都是通过将 operation 添加到一个 operation queue 的方式来执行 operation 的，然而这并不是必须的。我们也可以直接通过调用 `start` 方法来执行一个 operation ，但是这种方式并不能保证 operation 是异步执行的。NSOperation 类的 `isConcurrent` 方法的返回值标识了一个 operation 相对于调用它的 `start` 方法的线程来说是否是异步执行的。在默认情况下，isConcurrent 方法的返回值是 `NO` ，也就是说会阻塞调用它的 `start` 方法的线程。

如果我们想要自定义一个并发执行的 operation ，那么我们就必须要编写一些额外的代码来让这个 operation 异步执行。比如，为这个 operation 创建新的线程、调用系统的异步方法或者其他任何方式来确保 `start` 方法在开始执行任务后立即返回。

在绝大多数情况下，我们都不需要去实现一个并发的 operation 。如果我们一直是通过将 operation 添加到 operation queue 的方式来执行 operation 的话，我们就完全没有必要去实现一个并发的 operation 。因为，当我们将一个非并发的 operation 添加到 operation queue 后，operation queue 会自动为这个 operation 创建一个线程。因此，只有当我们需要手动地执行一个 operation ，又想让它异步执行时，我们才有必要去实现一个并发的 operation 。

### 创建 NSInvocationOperation 对象

正如上面提到的，NSInvocationOperation 是 NSOperation 类的一个子类，当一个 NSInvocationOperation 开始执行时，它会调用我们指定的 `object` 的 `selector` 方法。通过使用 NSInvocationOperation 类，我们可以避免为每一个任务都创建一个自定义的子类，特别是当我们在修改一个已经存在的应用，并且这个应用中已经有了我们需要执行的任务所对应的 `object` 和 `selector` 时非常有用。

下面的示例代码展示了如何通过 `object` 和 `selector` 创建一个 NSInvocationOperation 对象。**说明**，本文中的所有示例代码都可以在这里 [OperationQueues](https://github.com/leichunfeng/OperationQueues) 找到，每一个类都有与之对应的测试类，充当 `client` 的角色，建议你在看完一个小节的代码时，运行一下相应的测试用例，观察打印的结果，以加深理解。

``` objc
@implementation OQCreateInvocationOperation

- (NSInvocationOperation *)invocationOperationWithData:(id)data {
    return [[NSInvocationOperation alloc] initWithTarget:self selector:@selector(myTaskMethod1:) object:data];
}

- (void)myTaskMethod1:(id)data {
    NSLog(@"Start executing %@ with data: %@, mainThread: %@, currentThread: %@", NSStringFromSelector(_cmd), data, [NSThread mainThread], [NSThread currentThread]);
    sleep(3);
    NSLog(@"Finish executing %@", NSStringFromSelector(_cmd));
}

@end
```

另外，我们在前面也提到了，NSInvocationOperation 类的使用可以非常的动态和灵活，其中比较显著的一点就是我们可以根据上下文动态地调用 `object` 的不同 `selector` 。比如说，我们可以根据用户的输入动态地执行不同的 `selector` ：

``` objc
- (NSInvocationOperation *)invocationOperationWithData:(id)data userInput:(NSString *)userInput {
    NSInvocationOperation *invocationOperation = [self invocationOperationWithData:data];
    
    if (userInput.length == 0) {
        invocationOperation.invocation.selector = @selector(myTaskMethod2:);
    }
    
    return invocationOperation;
}

- (void)myTaskMethod2:(id)data {
    NSLog(@"Start executing %@ with data: %@, mainThread: %@, currentThread: %@", NSStringFromSelector(_cmd), data, [NSThread mainThread], [NSThread currentThread]);
    sleep(3);
    NSLog(@"Finish executing %@", NSStringFromSelector(_cmd));
}
```

### 创建 NSBlockOperation 对象

NSBlockOperation 是 NSOperation 类的另外一个系统预定义的子类，我们可以用它来封装一个或多个 `block` 。我们知道 `GCD` 主要就是用来进行 `block` 调度的，那为什么我们还需要 NSBlockOperation 类呢？一般来说，有以下两个场景我们会优先使用 NSBlockOperation 类：

- 当我们在应用中已经使用了 Operation Queues 且不想创建 Dispatch Queues 时，NSBlockOperation 类可以为我们的应用提供一个面向对象的封装；
- 我们需要用到 Dispatch Queues 不具备的功能时，比如需要设置 operation 之间的依赖关系、使用 `KVO` 观察 operation 的状态变化等。

下面的示例代码展示了创建一个 NSBlockOperation 对象的基本方法：

``` objc
@implementation OQCreateBlockOperation

- (NSBlockOperation *)blockOperation {
    NSBlockOperation *blockOperation = [NSBlockOperation blockOperationWithBlock:^{
        NSLog(@"Start executing block1, mainThread: %@, currentThread: %@", [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing block1");
    }];
    
    [blockOperation addExecutionBlock:^{
        NSLog(@"Start executing block2, mainThread: %@, currentThread: %@", [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing block2");
    }];
    
    [blockOperation addExecutionBlock:^{
        NSLog(@"Start executing block3, mainThread: %@, currentThread: %@", [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing block3");
    }];
    
    return blockOperation;
}

@end
```

## 自定义 Operation 对象

当系统预定义的两个子类 NSInvocationOperation 和 NSBlockOperation 不能很好的满足我们的需求时，我们可以自定义自己的 NSOperation 子类，添加我们想要的功能。目前，我们可以自定义非并发和并发两种不同类型的 NSOperation 子类，而自定义一个前者要比后者简单得多。

对于一个非并发的 operation ，我们需要做的就只是执行 `main` 方法中的任务以及能够正常响应取消事件就可以了，其它的复杂工作比如依赖配置、KVO 通知等 NSOperation 类都已经帮我们处理好了。而对于一个并发的 operation ，我们还需要重写 NSOperation 类中的一些现有方法。接下来，我们将会介绍如何自定义这两种不同类型的 NSOperation 子类。

### 执行主任务

从最低限度上来说，每一个 operation 都应该至少实现以下两个方法：

- 一个自定义的初始化方法；
- `main` 方法。

我们需要用一个自定义的初始化方法来将创建的 operation 置于一个已知的状态，并且重写 `main` 方法来执行我们的任务。当然，我们也可以实现一些其他的额外方法，比如实现 `NSCoding` 协议来允许我们归档和解档 operation 等。下面的示例代码展示了如何自定义一个简单的 operation ：

``` objc
@interface OQNonConcurrentOperation ()

@property (strong, nonatomic) id data;

@end

@implementation OQNonConcurrentOperation

- (id)initWithData:(id)data {
    self = [super init];
    if (self) {
        self.data = data;
    }
    return self;
}

///  不支持取消操作
- (void)main {
    @try {
        NSLog(@"Start executing %@ with data: %@, mainThread: %@, currentThread: %@", NSStringFromSelector(_cmd), self.data, [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing %@", NSStringFromSelector(_cmd));
    }
    @catch(NSException *exception) {
        NSLog(@"Exception: %@", exception);
    }
}

@end
```

### 响应取消事件

当一个 operation 开始执行后，它会一直执行它的任务直到完成或被取消为止。我们可以在任意时间点取消一个 operation ，甚至是在它还未开始执行之前。为了让我们自定义的 operation 能够支持取消事件，我们需要在代码中定期地检查 `isCancelled` 方法的返回值，一旦检查到这个方法返回 `YES` ，我们就需要立即停止执行接下来的任务。根据苹果官方的说法，`isCancelled` 方法本身是足够轻量的，所以就算是频繁地调用它也不会给系统带来太大的负担。

> The isCancelled method itself is very lightweight and can be called frequently without any significant performance penalty.

通常来说，当我们自定义一个 operation 类时，我们需要考虑在以下几个关键点检查 `isCancelled` 方法的返回值：

- 在真正开始执行任务之前；
- 至少在每次循环中检查一次，而如果一次循环的时间本身就比较长的话，则需要检查得更加频繁；
- 在任何相对来说比较容易中止 operation 的地方。

看到这里，我想你应该可以意识到一点，那就是尽管 operation 是支持取消操作的，但却并不是立即取消的，而是在你调用了 operation 的 `cancel` 方法之后的下一个 `isCancelled` 的检查点取消的。

``` objc
///  支持取消操作
- (void)main {
    @try {
        if (self.isCancelled) return;
        
        NSLog(@"Start executing %@ with data: %@, mainThread: %@, currentThread: %@", NSStringFromSelector(_cmd), self.data, [NSThread mainThread], [NSThread currentThread]);
        
        for (NSUInteger i = 0; i < 3; i++) {
            if (self.isCancelled) return;
            
            sleep(1);
            
            NSLog(@"Loop %@", @(i + 1));
        }

        NSLog(@"Finish executing %@", NSStringFromSelector(_cmd));
    }
    @catch(NSException *exception) {
        NSLog(@"Exception: %@", exception);
    }
}
```

### 配置并发执行的 Operation

在默认情况下，operation 是同步执行的，也就是说在调用它的 `start` 方法的线程中执行它们的任务。而在 operation 和 operation queue 结合使用时，operation queue 可以为非并发的 operation 提供线程，因此，大部分的 operation 仍然可以异步执行。但是，如果你想要手动地执行一个 operation ，又想这个 operation 能够异步执行的话，你需要做一些额外的配置来让你的 operation 支持并发执行。下面列举了一些你可能需要重写的方法：

- `start` ：必须的，所有并发执行的 operation 都必须要重写这个方法，替换掉 NSOperation 类中的默认实现。`start` 方法是一个 operation 的起点，我们可以在这里配置任务执行的线程或者一些其它的执行环境。另外，需要特别注意的是，在我们重写的 `start` 方法中一定不要调用父类的实现；
- `main` ：可选的，通常这个方法就是专门用来实现与该 operation 相关联的任务的。尽管我们可以直接在 `start` 方法中执行我们的任务，但是用 `main` 方法来实现我们的任务可以使设置代码和任务代码得到分离，从而使 operation 的结构更清晰；
- `isExecuting` 和 `isFinished` ：必须的，并发执行的 operation 需要负责配置它们的执行环境，并且向外界客户报告执行环境的状态。因此，一个并发执行的 operation 必须要维护一些状态信息，用来记录它的任务是否正在执行，是否已经完成执行等。此外，当这两个方法所代表的值发生变化时，我们需要生成相应的 `KVO` 通知，以便外界能够观察到这些状态的变化；
- `isConcurrent` ：必须的，这个方法的返回值用来标识一个 operation 是否是并发的 operation ，我们需要重写这个方法并返回 `YES` 。

下面我们将分三部分内容来介绍一下定义一个并发执行的 operation 所需的基本代码，主体部分的代码如下所示：

``` objc
@implementation OQConcurrentOperation

@synthesize executing = _executing;
@synthesize finished  = _finished;

- (id)init {
    self = [super init];
    if (self) {
        _executing = NO;
        _finished  = NO;
    }
    return self;
}

- (BOOL)isConcurrent {
    return YES;
}

- (BOOL)isExecuting {
    return _executing;
}

- (BOOL)isFinished {
    return _finished;
}

@end
```

这一部分的代码看上去比较简单，但是却需要我们用心地去理解它。首先，我们用 `@synthesize` 关键字手动合成了两个实例变量 `_executing` 和 `_finished` ，然后分别在重写的 `isExecuting` 和 `isFinished` 方法中返回了这两个实例变量。另外，我们通过查看 NSOperation 类的头文件可以发现，`executing` 和 `finished` 属性都被声明成了只读的 `readonly` 。所以我们在 NSOperation 子类中就没有办法直接通过 `setter` 方法来自动触发 `KVO` 通知，这也是为什么我们需要在接下来的代码中手动触发 `KVO` 通知的原因。

接下来是 `start` 方法的代码，在这个方法中，我们最需要关注的部分就是为 `main` 方法分离了一个新的线程，这是 operation 能够并发执行的关键所在。此外，在真正开始执行任务前，我们通过检查 `isCancelled` 方法的返回值来判断 operation 是否已经被 `cancel` ，如果是就直接返回了。

``` objc
- (void)start {
    if (self.isCancelled) {
        [self willChangeValueForKey:@"isFinished"];
        _finished = YES;
        [self didChangeValueForKey:@"isFinished"];
        
        return;
    }
    
    [self willChangeValueForKey:@"isExecuting"];
    
    [NSThread detachNewThreadSelector:@selector(main) toTarget:self withObject:nil];
    _executing = YES;
    
    [self didChangeValueForKey:@"isExecuting"];
}
```

最后，是真正执行任务的 `main` 方法，值得注意的是在任务执行完毕后，我们需要手动触动 `isExecuting` 和 `isFinished` 的 `KVO` 通知。

``` objc
- (void)main {
    @try {
        NSLog(@"Start executing %@, mainThread: %@, currentThread: %@", NSStringFromSelector(_cmd), [NSThread mainThread], [NSThread currentThread]);

        sleep(3);
        
        [self willChangeValueForKey:@"isExecuting"];
        _executing = NO;
        [self didChangeValueForKey:@"isExecuting"];

        [self willChangeValueForKey:@"isFinished"];
        _finished  = YES;
        [self didChangeValueForKey:@"isFinished"];

        NSLog(@"Finish executing %@", NSStringFromSelector(_cmd));
    }
    @catch (NSException *exception) {
        NSLog(@"Exception: %@", exception);
    }
}
```

**注意**，有一个非常重要的点需要引起我们的注意，那就是即使一个 operation 是被 `cancel` 掉了，我们仍然需要手动触发 `isFinished` 的 `KVO` 通知。因为当一个 operation 依赖其他 operation 时，它会观察所有其他 operation 的 `isFinished` 的值的变化，只有当它依赖的所有 operation 的 `isFinished` 的值为 `YES` 时，这个 operation 才能够开始执行。因此，如果一个我们自定义的 operation 被取消了但却没有手动触发 `isFinished` 的 `KVO` 通知的话，那么所有依赖它的 operation 都不会执行。

### 维护 KVO 通知

NSOperation 类的以下 `key paths` 支持 `KVO` 通知，我们可以通过观察这些 `key paths` 非常方便地监听到一个 operation 内部状态的变化：

- isCancelled
- isConcurrent
- isExecuting
- isFinished
- isReady
- dependencies
- queuePriority
- completionBlock

与重写 `main` 方法不同的是，如果我们重写了 `start` 方法或者对 NSOperation 类做了大量定制的话，我们需要保证自定义的 operation 在这些 `key paths` 上仍然支持 `KVO` 通知。比如，当我们重写了 `start` 方法时，我们需要特别关注的是 `isExecuting` 和 `isFinished` 这两个 `key paths` ，因为这两个 `key paths` 最可能受重写 `start` 方法的影响。

## 定制 Operation 对象的执行行为

我们可以在创建一个 operation 后，添加到 operation queue 前，对 operation 的一些执行行为进行定制。下面介绍的所有定制均适用于所有的 operation ，与是否是自定义的 NSOperation 子类或系统预定义的 NSOperation 子类无关。

### 配置依赖关系

通过配置依赖关系，我们可以让不同的 operation 串行执行，正如我们前面提到的，一个 operation 只有在它依赖的所有 operation 都执行完成后才能开始执行。配置 operation 的依赖关系主要涉及到 NSOperation 类中的以下两个方法：

``` objc
- (void)addDependency:(NSOperation *)op;
- (void)removeDependency:(NSOperation *)op;
```

顾名思义，第一个方法用于添加依赖，第二个方法则用于移除依赖。需要特别注意的是，用 `addDependency:` 方法添加的依赖关系是单向的，比如 `[A addDependency:B];` ，表示 A 依赖 B，B 并不依赖 A 。 

另外，这里的依赖关系并不局限于相同 operation queue 中的 operation 之间。其实，从上面两个配置依赖关系的方法是存在于 NSOperation 类中的，我们也可以看出来，operation 的依赖关系是它自己管理的，与它被添加到哪个 operation queue 无关。因此，我们完全可以给一些 operation 配置好依赖关系，然后将它们添加到不同的 operation queue 中。但是，有一点是需要我们特别注意的，就是不要在 operation 之间添加循环依赖，因为这样会导致这些 operation 都不会被执行。

**注意**，我们应该在手动执行一个 operation 或将它添加到 operation queue 前配置好依赖关系，因为在之后添加的依赖关系可能会失效。

### 修改 Operation 在队列中的优先级

对于被添加到 operation queue 中的 operation 来说，决定它们执行顺序的第一要素是它们的 `isReady` 状态，其次是它们在队列中的优先级。operation 的 `isReady` 状态取决于它的依赖关系，而在队列中的优先级则是 operation 本身的属性。默认情况下，所有新创建的 operation 的队列优先级都是 `normal` 的，但是我们可以根据需要通过 `setQueuePriority:` 方法来提高或降低 operation 的队列优先级。

需要注意的是，队列优先级只应用于相同 operation queue 中的 operation 之间，不同 operation queue 中的 operation 不受此影响。另外，我们也需要清楚 operation 的队列优先级和依赖关系之间的区别。operation 的队列优先级只决定当前所有 `isReady` 状态为 `YES` 的 operation 的执行顺序。比如，在一个 operation queue 中，有一个高优先级和一个低优先级的 operation ，并且它们的 `isReady` 状态都为 `YES` ，那么高优先级的 operation 将会优先执行。而如果这个高优先级的 operation 的 `isReady` 状态为 `NO` ，而低优先级的 operation 的 `isReady` 状态为 `YES` 的话，那么这个低优先级的 operation 反而会优先执行。

### 修改 Operation 执行任务线程的优先级

从 iOS 4.0 开始，我们可以修改 operation 的执行任务线程的优先级。虽然 iOS 系统中的线程策略是由 `kernel` 内核管理的，但是一般来说，高优先级的线程相对于低优先级的线程来说能够得到更多的运行机会。我们可以给 operation 的线程优先级指定一个从 `0.0` 到 `1.0` 的浮点数值，`0.0` 表示最低的优先级，`1.0` 表示最高的优先级，默认值为 `0.5` 。

**注意**，我们只能够在执行一个 operation 或将其添加到 operation queue 前，通过 operation 的 `setThreadPriority:` 方法来修改它的线程优先级。当 operation 开始执行时，NSOperation 类中默认的 `start` 方法会使用我们指定的值来修改当前线程的优先级。另外，我们指定的这个线程优先级只会影响 `main` 方法执行时所在线程的优先级。所有其它的代码，包括 operation 的 completion block 所在的线程会一直以默认的线程优先级执行。因此，当我们自定义一个并发的 operation 类时，我们也需要在 `start` 方法中根据指定的值自行修改线程的优先级。

### 设置 Completion Block

从 iOS 4.0 开始，一个 operation 可以在它的主任务执行完成时回调一个 completion block 。我们可以用 completion block 来执行一些主任务之外的工作，比如，我们可以用它来通知一些客户 operation 已经执行完毕，而并发的 operation 也可以用这个 block 来生成最终的 `KVO` 通知。如果需要设置一个 operation 的 completion block ，直接调用 NSOperation 类的 `setCompletionBlock:` 方法即可。

**注意**，当一个 operation 被取消时，它的 completion block 仍然会执行，所以我们需要在真正执行代码前检查一下 `isCancelled` 方法的返回值。另外，我们也没有办法保证 completion block 被回调时一定是在主线程，理论上它应该是与触发 `isFinished` 的 `KVO` 通知所在的线程一致的，所以如果有必要的话我们可以在 completion block 中使用 `GCD` 来保证从主线程更新 `UI` 。

## 执行 Operation 对象

最终，我们需要执行 operation 来调度与其关联的任务。目前，主要有两种方式来执行一个 operation ：

- 将 operation 添加到一个 operation queue 中，让 operation queue 来帮我们自动执行；
- 直接调用 `start` 方法手动执行 operation 。

### 添加 Operation 到 Operation Queue 中

就目前来说，将 operation 添加到 operation queue 中是最简单的执行 operation 的方式。另外，这里的 operation queue 指的就是 NSOperationQueue 类的一个具体实例。就技术上而言，我们可以在应用中创建任意数量的 operation queue ，但是 operation queue 的数量越多并不意味着我们就能同时执行越多的 operation 。因为同时并发的 operation 数量是由系统决定的，系统会根据当前可用的核心数以及负载情况动态地调整最大的并发 operation 数量。创建一个 operation queue 非常简单，跟创建其他普通对象没有任何区别：

``` objc
NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
```

创建好 operation queue 后，我们可以使用下面三个方法添加 operation 到 operation queue 中：

- `addOperation:` ，添加一个 operation 到 operation queue 中；
- `addOperations:waitUntilFinished:` ，添加一组 operation 到 operation queue 中；
- `addOperationWithBlock:` ，直接添加一个 block 到 operation queue 中，而不用创建一个 NSBlockOperation 对象。

在大多数情况下，一个 operation 被添加到 operation queue 后不久就会执行，但是也有很多原因会使 operation queue 延迟执行入队的 operation 。比如，我们前面提到了的，如果一个 operation 所依赖的其他 operation 还没有执行完成时，这个 operation 就不能开始执行；再比如说 operation queue 被暂停执行或者已经达到了它最大可并发的 operation 数。下面的示例代码展示了这三种方法的基本用法：

``` objc
@implementation OQUseOperationQueue

- (void)executeOperationUsingOperationQueue {
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    
    NSInvocationOperation *invocationOperation = [[NSInvocationOperation alloc] initWithTarget:self selector:@selector(taskMethod) object:nil];
    [operationQueue addOperation:invocationOperation];
    
    NSBlockOperation *blockOperation1 = [NSBlockOperation blockOperationWithBlock:^{
        NSLog(@"Start executing blockOperation1, mainThread: %@, currentThread: %@", [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing blockOperation1");
    }];
    
    NSBlockOperation *blockOperation2 = [NSBlockOperation blockOperationWithBlock:^{
        NSLog(@"Start executing blockOperation2, mainThread: %@, currentThread: %@", [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing blockOperation2");
    }];
    
    [operationQueue addOperations:@[ blockOperation1, blockOperation2 ] waitUntilFinished:NO];
    
    [operationQueue addOperationWithBlock:^{
        NSLog(@"Start executing block, mainThread: %@, currentThread: %@", [NSThread mainThread], [NSThread currentThread]);
        sleep(3);
        NSLog(@"Finish executing block");
    }];
    
    [operationQueue waitUntilAllOperationsAreFinished];
}

- (void)taskMethod {
    NSLog(@"Start executing %@, mainThread: %@, currentThread: %@", NSStringFromSelector(_cmd), [NSThread mainThread], [NSThread currentThread]);
    sleep(3);
    NSLog(@"Finish executing %@", NSStringFromSelector(_cmd));
}

@end
```

**注意**，在将一个 operation 添加到 operation queue 后就不要再修改这个 operation 了。因为 operation 被添加到 operation queue 后随时可能会执行，这个是由系统决定的，所以再修改它的依赖关系或者所包含的数据就很有可能会造成未知的影响。

尽管 NSOperationQueue 类是被设计成用来并发执行 operation 的，但是我们也可以强制一个 operation queue 一次只执行一个 operation 。我们可以通过 `setMaxConcurrentoperationCount:` 方法来设置一个 operation queue 最大可并发的 operation 数，因此将这个值设置成 1 就可以实现让 operation queue 一次只执行一个 operation 的目的。但是需要注意的是，虽然这样可以让 operation queue 一次只执行一个 operation ，但是 operation 的执行顺序还是一样会受其他因素影响的，比如 operation 的 `isReady` 状态、operation 的队列优先级等。因此，一个串行的 operation queue 与一个串行的 dispatch queue 还是有本质区别的，因为 dispatch queue 的执行顺序一直是 `FIFO` 的。如果 operation 的执行顺序对我们来说非常重要，那么我们就应该在将 operation 添加到 operation queue 之前就建立好它的依赖关系。

### 手动执行 Operation

尽管使用 operation queue 是执行一个 operation 最方便的方式，但是我们也可以不用 operation queue 而选择手动地执行一个 operation 。从原理上来说，手动执行一个 operation 也是非常简单的，只需要调用它的 `start` 方法就可以了。但是从严格意义上来说，在调用 `start` 方法真正开始执行一个 operation 前，我们应该要做一些防范性的判断，比如检查 operation 的 `isReady` 状态是否为 `YES` ，这个取决于它所依赖的 operation 是否已经执行完成；又比如检查 operation 的 `isCancelled` 状态是否为 `YES` ，如果是，那么我们就根本不需要再花费不必要的开销去启动它。

另外，我们应该一直通过 `start` 方法去手动执行一个 operation ，而不是 `main` 或其他的什么方法。因为默认的 `start` 方法会在真正开始执行任务前为我们做一些安全性的检查，比如检查 operation 是否已取消等。另外，正如我们前面说的，在默认的 `start` 方法中会生成一些必要的 `KVO` 通知，比如 `isExcuting` 和 `isFinished` ，而这些 `KVO` 通知正是 operation 能够正确处理好依赖关系的关键所在。

更进一步说，如果我们需要实现的是一个并发的 operation ，我们也应该在启动 operation 前检查一下它的 `isConcurrent` 状态。如果它的 `isConcurrent` 状态为 `NO` ，那么我们就需要考虑一下是否可以在当前线程同步执行这个 operation ，或者是先为这个 operation 创建一个单独的线程，以供它异步执行。

当然，如果你已经能够确定一个 operation 的可执行状态，那么你大可不必做这些略显啰嗦的防范性检查，直接调用 `start` 方法执行这个 operation 即可。下面的示例代码展示了手动执行一个 operation 的基本流程：

``` objc
@implementation OQManualExecuteOperation

- (BOOL)manualPerformOperation:(NSOperation *)operation {
    BOOL ranIt = NO;
    
    if (operation.isCancelled) {
        ranIt = YES;
    } else if (operation.isReady) {
        if (!operation.isConcurrent) {
            [operation start];
        } else {
            [NSThread detachNewThreadSelector:@selector(start) toTarget:operation withObject:nil];
        }
        ranIt = YES;
    }
    
    return ranIt;
}

@end
```

### 取消 Operation

从原则上来说，一旦一个 operation 被添加到 operation queue 后，这个 operation 的所有权就属于这个 operation queue 了，并且不能够被移除。唯一从 operation queue 中出队一个 operation 的方式就是调用它的 `cancel` 方法取消这个 operation ，或者直接调用 operation queue 的 `cancelAllOperations` 方法取消这个 operation queue 中所有的 operation 。另外，我们前面也提到了，当一个 operation 被取消后，这个 operation 的 `isFinished` 状态也会变成 `YES` ，这样处理的好处就是所有依赖它的 operation 能够接收到这个 `KVO` 通知，从而能够清除这个依赖关系正常执行。

### 等待 Operation 执行完成

一般来说，为了让我们的应用拥有最佳的性能，我们应该尽可能地异步执行所有的 operation ，从而让我们的应用在执行这些异步 operation 的同时还能够快速地响应用户事件。当然，我们也可以调用 NSOperation 类的 `waitUntilFinished` 方法来阻塞当前线程，直到这个 operation 执行完成。虽然这种方式可以让我们非常方便地处理 operation 的执行结果，但是却给我们的应用引入了更多的串行，限制了应用的并发性，从而降低了我们应用的响应性。

**注意**，我们应该要坚决避免在主线程中去同步等待一个 operation 的执行结果，阻塞的方式只应该用在辅助线程或其他 operation 中。因为阻塞主线程会大大地降低我们应用的响应性，带来非常差的用户体验。

除了等待一个单独的 operation 执行完成外，我们也可以通过调用 NSOperationQueue 的 `waitUntilAlloperationsAreFinished` 方法来等待 operation queue 中的所有 operation 执行完成。有一点需要特别注意的是，当我们在等待一个 operation queue 中的所有 operation 执行完成时，其他的线程仍然可以向这个 operation queue 中添加 operation ，从而延长我们的等待时间。

### 暂停和恢复 Operation Queue

如果我们想要暂停和恢复执行 operation queue 中的 operation ，可以通过调用 operation queue 的 `setSuspended:` 方法来实现这个目的。不过需要注意的是，暂停执行 operation queue 并不能使正在执行的 operation 暂停执行，而只是简单地暂停调度新的 operation 。另外，我们并不能单独地暂停执行一个 operation ，除非直接 `cancel` 掉。

## 总结

看到这里，我想你对 iOS 的并发编程模型已经有了一定的了解。正如文中所说的，我们应该尽可能地直接使用队列而不是线程，让系统去与线程打交道，而我们只需定义好要调度的任务就可以了。一般情况下，我们也完全不需要去自定义一个并发的 operation ，因为在与 operation queue 结合使用时，operation queue 会自动为非并发的 operation 创建一个线程。Operation Queues 是对 `GCD` 面向对象的封装，它可以高度定制化，对依赖关系、队列优先级和线程优先级等提供了很好的支持，是我们实现复杂任务调度时的不二之选。

## 参考链接

[https://developer.apple.com/library/ios/documentation/General/Conceptual/ConcurrencyProgrammingGuide/Introduction/Introduction.html](https://developer.apple.com/library/ios/documentation/General/Conceptual/ConcurrencyProgrammingGuide/Introduction/Introduction.html)
<br>
[http://www.raywenderlich.com/76341/use-nsoperation-nsoperationqueue-swift](http://www.raywenderlich.com/76341/use-nsoperation-nsoperationqueue-swift)
<br>
[http://blog.xcodev.com/archives/operation-queue-intro/](http://blog.xcodev.com/archives/operation-queue-intro/)

**版权声明**：我已将本文在微信公众平台的发表权「独家代理」给 iOS 开发（iOSDevTips）微信公众号。扫下方二维码即可关注「iOS 开发」：

![iOS 开发二维码](http://blog.devtang.com/images/weixin-qr.jpg)
