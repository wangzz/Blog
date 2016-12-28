## 不同view继承体系之间区域转换

例如一个视图控制器的view中有一个UITableView，UITableView的某个cell中有个UITextField，想要得到UITextField在view中的位置：

``` objective-c
CGRect rect = [self.view convertRect:textField.frame fromView:textField.superview];
//或者
CGRect rect = [textField.superview convertRect:textField.frame toView:self.view];
```
## 不同view继承体系之间点转换

#### 把一个点从一个坐标系转换到接收者的坐标系

``` objective-c
- (CGPoint)convertPoint:(CGPoint)point fromView:(UIView *)view
```

##### 参数

* point

一个视图中坐标系上的点

* view

一个视图包含了点和他自身坐标系。如果是图是nil，那么这个方法将尝试转换基于窗口的坐标系。否则视图和那个接收者必须属于同一个UIWindow对象。

#### 转换一个点从接收者坐标系到给定的视图坐标系

``` objective-c
- (CGPoint)convertPoint:(CGPoint)point toView:(UIView *)view
```

##### 参数

* point

一个在调用者坐标系中的点

* view

一个包含了需要被转换的点的视图。如果视图是nil，那么这个方法将会转换成基于窗口的坐标。否则视图和接收者都要属于同一个UIWindow对象。
返回值
