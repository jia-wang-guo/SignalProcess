

[TOC]

> 注：下面图片中的代码为了查看函数调用情况，加了很多打印语句，没有打印语句的代码查看第三节

# 一、cuVector内存结构

![image-20200718163707302](.md_inage/image-20200718163707302.png)

```c++
typedef UtilVec<cuFloatComplex> cuVector;
```

cuVector是变量类型为cuFloatComplex的GPU复数向量。对于cuVector的对象，其内存模型如上图所示。主要依赖于类模板UtilVec，下面主要介绍以下UtilVec。UtilVec类主要利用了RAII特性，仿照std::shared_ptr内存模型，对GPU的地址进行包装，并增加了引用计数。重写拷贝构造函数和拷贝赋值运算符，防止对GPU地址double free。



## (1) 构造函数

<img src=".md_inage/image-20200718163814995.png" alt="image-20200718163814995" style="zoom:67%;" />

## (2) 拷贝构造函数重写

<img src=".md_inage/image-20200718163849974.png" alt="image-20200718163849974" style="zoom:67%;" />

## （3）拷贝赋值运算符



![](.md_inage/image-20200718163910486.png)

## (4) 析构函数

![image-20200718164018243](.md_inage/image-20200718164018243.png)

# 二、可靠性测试

## 测试1

下面一段代码，在发生函数调用的时候，会两次调用构造函数和析构函数，如果不改写拷贝构造函数，就会申请一次释放三次，导致程序崩溃，利用改写后的拷贝构造函数，申请一次内存，释放一次内存。保证了程序的安全。

<img src=".md_inage/image-20200718164111495.png" alt="image-20200718164111495" style="zoom:67%;" />

<img src=".md_inage/image-20200718164137587.png" alt="image-20200718164137587" style="zoom:67%;" />

使用GPU内存分析工具nvprof分析CUDA函数调用情况：

![image-20200718164200208](.md_inage/image-20200718164200208.png)

可以看到cudaMalloc和cudaFree均只被调用了一次。

## 测试2

<img src=".md_inage/image-20200718164223933.png" alt="image-20200718164223933" style="zoom: 67%;" />

一共构造了三个GPU数组，但是他们指向同一块内存

![image-20200718164247782](.md_inage/image-20200718164247782.png)

# 三、代码

[Utility.h](./Utility/Utility.h)

[cuvec_test.cu](./Utility/cuvec_test.cu)