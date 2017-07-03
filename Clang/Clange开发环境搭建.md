#### 一、下载Clang和LLVM源码并安装

执行以下命令：
```
cd /opt
sudo mkdir llvm
cd llvm
export LLVM_HOME=`pwd`

git clone -b release_39 https://github.com/llvm-mirror/llvm.git llvm
git clone -b release_39 https://github.com/llvm-mirror/clang.git llvm/tools/clang
git clone -b release_39 https://github.com/llvm-mirror/clang-tools-extra.git llvm/tools/clang/tools/extra
git clone -b release_39 https://github.com/llvm-mirror/compiler-rt.git llvm/projects/compiler-rt
 
mkdir llvm_build
cd llvm_build
cmake ../llvm -DCMAKE_BUILD_TYPE:STRING=Release
make -j`sysctl -n hw.logicalcpu`
```

如果到这里都顺利的话，Clang和LLVM都已经成功安装了。但是要想在Xcode中使用我们自己编译的Clang，还需要破解Xcode。

#### 二、破解Xcode

##### 下载破解文件

首先下载 [XcodeHacking](./XcodeHacking.zip)文件，解压后需要做两点修改：

* 将HackXcodeing中的ExecPath中的根路径更改为你的LLVM根路径。

* 将HackedClang.xcspec文件中的NO = ( “-Wno-receiver-is-weak” ); 更改为NO = ();

##### 将破解文件放置到Xcode包中

HackXcodeing源文件作者署名是Apple，但是网上查不到原出处，Apple官方应该没有摆明支持这个东西，但这和线上App没关系，可以放心用。下一步，加入到Xcode的插件文件夹下。

```
sudo mv HackedClang.xcplugin `xcode-select -print-path`/../PlugIns/Xcode3Core.ideplugin/Contents/SharedSupport/Developer/Library/Xcode/Plug-ins

sudo mv HackedBuildSystem.xcspec `xcode-select -print-path`/Platforms/iPhoneSimulator.platform/Developer/Library/Xcode/Specifications
```

需要注意的是Xcode升级后需要重新破解。

##### 将编译器选择成Clang LLVM Trunk

在Compiler for C/C++/Objective-C中选择Clang LLVM Trunk：

![](https://github.com/wangzz/Blog/blob/master/image/Clang/xcode_clangplugin_compiler.png)

##### 修改工程配置

要想在Xcode工程中使用我们自己编译的Clang插件，需要在在OTHER_CFLAGS中加入dylib的路径：

```
-Xclang 
-load 
-Xclang 
/放llvm和clang源码的根路径/llvm/toy_clang_plugin/build/lib/Debug/ToyClangPlugin.dylib 
-Xclang 
-add-plugin 
-Xclang 
"插件名称"
```

![](https://github.com/wangzz/Blog/blob/master/image/Clang/xcode_clangplugin_flags.png)

顺利的话到这里工程就能运行起来了。

三、参考文档

* [Clang Plugin--Sherlock](http://blog.mrriddler.com/2017/02/24/Clang%E6%8F%92%E4%BB%B6-Sherlock/?utm_source=tuicool&utm_medium=referral)