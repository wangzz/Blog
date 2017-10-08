## iOS疑难Crash及其解决办法

###### 1、NSInternalInconsistencyException(SIGABRT)

* 详细日志

```
Invalid update: invalid number of rows in section 1. The number of rows contained in an existing section after the update (4) must be equal to the number of rows contained in that section before the update (4), plus or minus the number of rows inserted or deleted from that section (0 inserted, 1 deleted) and plus or minus the number of rows moved into or out of that section (0 moved in, 0 moved out).
```

* 重现概率

无法重现

* 原因分析

在 `- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath` 方法里调用 `- (void)deleteRowsAtIndexPaths:(NSArray<NSIndexPath *> *)indexPaths withRowAnimation:(UITableViewRowAnimation)animation` 方法时需要用
`beginUpdates` 和 `endUpdates` 将数据源的删除操作和 `deleteRowsAtIndexPaths` 方法的调用包起来，并且数据源的删除一定要在 'deleteRowsAtIndexPaths' 方法之前。

* 参考文档
http://blog.csdn.net/lvxiangan/article/details/25161157
http://blog.csdn.net/ayuapp/article/details/17591183
http://blog.csdn.net/wengyupeng/article/details/44276293
https://stackoverflow.com/a/7301700/2293677









