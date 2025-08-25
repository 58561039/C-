### 第一章 绪论 省略

### 第二章 C++程序的组成部分

1.程序入口

int main(int argc, char* argv[])

argc是参数的数量

argv是命令行的参数数组，argv[0]默认是函数名字，例如./test



2.namespace命名空间

namespace的作用是在大型项目中，多人协同工作中，防止出现命名冲突。

例如程序员A和B，写了相同的函数或者类，程序不知道调用哪个，而实现声明命名空间即可避免这个问题。



3.流stream

流可以抽象理解为数据传输通道

流的来源和去向可以是内存，文件，控制台，网络

后续进行进一步学习





### 第三章 使用变量和常量

1.常量变量

变量，作用域内有效

常量，程序运行过程中不可改变

const定义的常量

const和constexpr都可以定义常量

但是const定义的常量可以在编译时不赋值，在运行时赋值一次。也就是说const定义的常量在运行时只可以赋值一次，不可修改，可以读取多次。

constexpr定义的常量必须在编译时就确定值，不可以在运行时赋值。相比于const更严格。



2.内存

C++程序有五个主要的存储区

代码区，数据区，BSS区，堆，栈



代码区：存放程序的机器指令

数据区：存放已经初始化的全局变量和静态变量

BSS区：存放未初始化或者初始化为0的全局变量和静态变量

堆：用来动态分配的内存

栈：存放函数的局部变量、函数参数、返回地址



3.static定义变量和函数

static定义的变量存放在数据区或者BSS区

在函数内部定义静态变量会使得该变量只会初始化一次(程序执行到这里的时候)，并且该变量的生命周期是从程序开始到程序结束，并且限制变量的适用范围只在该函数

static定义的全局变量会使得该变量无法被此项目中的其他文件访问，只能在该文件下被访问。



static定义的函数会使得该函数只能在该文件中被访问，在此项目中的其他文件无法被访问

static定义的类成员函数不会仅限该文件(public)，而且不需要this指针，可以在没有对象的情况下访问。



### 第四章 数组和字符串

1.

char定义的数组默认最后一个元素是\0，定义char数组时，后面要加上\0，中间不能加，否则会导致垃圾内存的出现。

总之建议用string，不建议用C语言风格的strlen，strcpy等函数。



### 第五章 使用表达式、语句和运算符

略



### 第六章 控制程序流程

略



### 第七章 使用函数组织代码

1.内联函数

使用inline定义函数，通常是建议编译器在编译过程中直接在调用的地方展开函数，一般函数较为简单且非递归，为了减少调用和返回，从而提高性能。



### 第八章 指针和引用

1.

```C++
int x=5;//整数
int *p=&x;//一级指针
int **q=&p;//二级指针
```

2.

内存泄漏，new的内存没有delete。或者说malloc的内存没有free

3.

malloc/free    new/delete

malloc和free是C语言，返回的指针默认是void*，实际使用需要强制类型转换

new和delete是C++语言，当动态分配的内存存放的是对象时，释放内存时会自动调用析构函数，free不会自动调用

malloc分配失败返回NULL，new分配失败返回std::bad_alloc异常

4.

const和指针

```C++
int const* /const int* //不可以通过指针修改内容

int *const           //指向的位置不可变

const int *const     //指向的位置不可变，也无法通过指针修改内容
```



### 第九章 类和对象

1.复制构造函数，浅拷贝/深拷贝

如果类的成员有动态分配内存的成员，必须单独写复制构造函数，为了防止出现浅拷贝的问题。如果使用默认的复制构造函数，会出现新对象中动态分配内存的指针指向原对象中的指针指向的内存，而当其中一个对象调用析构函数释放内存之后，另一个对象中的指针也无法使用，导致程序不安全。

所以针对这种情况需要单独写复制构造函数，将动态分配内存的成员进行复制，在新对象中也动态分配内存并将其中的内容也复制。

浅拷贝就是只复制值，包括指针也是。

深拷贝不仅复制值，指针会复制内存，这样两个指针指向不同的内存，互不影响。

复制构造函数的参数必须按照引用传递，而且要用const

2.单例模式

指的是某些类只需要一个对象，例如日志系统

实现方式：

构造函数和静态对象放在private中

复制构造函数在private中delete

在public中写返回静态对象的函数

饿汉：类外声明

懒汉：类外声明为nullptr

懒汉模式是将对象存放在堆中，程序不会自动调用析构函数，需要显式调用，在写代码的时候调用析构函数，所以析构函数也需要写在public中。

饿汉模式是将对象存放在静态存储区中，可以将析构函数写在private中，即使这样，程序也会自动调用析构函数。

如果在栈中声明对象，会自动调用析构函数，析构函数必须是public。

如果只想在堆中声明对象，需要自己显式调用析构函数或者自己写一个销毁函数。因为堆的使用是由程序员自己决定的。

3.防止隐式转换，explicit，（单参数构造函数）

用explicit可以使得程序只能显式调用构造函数

```C++
class A {
public:
    explicit A(int x) { std::cout << "A构造函数: " << x << std::endl; }
};

void func(A a) {
    std::cout << "func called\n";
}

int main() {
    // func(10);  // ❌ 错误，不能隐式转换
    func(A(10));  // ✅ 显式调用
}

```

4.友元

声明以后可以访问private成员

```C++
class A{
    friend void submit();//外部函数
    friend class B;//类
    friend C::show();//类成员函数
};
```

5.union

共用体，结构的长度是最大成员的长度，同一时刻只有一个成员有效，只能访问一个成员



### 第十章 实现继承

1.继承方式与访问限制

|继承方式|基类 public|基类 protected|基类 private|
|-|-|-|-|
|**public**|public|protected|private|
|**protected**|protected|protected|private|
|**private**|private|private|private|


public保证派生类内部可以访问，外部可以访问

protect保证派生类内部可以访问，外部不可以访问

private保证派生类内部不可以访问，外部也不可以访问

2.子类与基类同名函数（重载）

有同名函数时

子类对象默认访问子类函数，要想访问基类同名函数必须加上作用域限定符

没有同名函数时

只要基类函数是public，子类对象就可以直接访问。

3.子类对象的初始化和析构

子类对象会先调用基类的构造函数，再调用子类的构造函数，默认从上到下

子类对象会先调用子类的析构函数，再调用基类的析构函数，默认从下到上

每个子类对象声明之后都会默认声明基类对象

4.切除

```C++
//基类base，子类derive
derive objder;
base objbase=objder;//显式
base objbase(objder);//隐式传参
```

这样的结果是基类对象只会复制基类，相当于新对象只保留原来的子类对象中的基类部分。

5.final

禁止继承



### 第十一章 多态

1.

概念 ：多态就是同一个接口，有多种实现

多态分为编译时多态，指的是在编译的时候就确定函数的调用，函数重载，运算符重载，模板

运行时多态，指的是在运行时才知道函数的调用，虚函数基类指针

2.

虚函数的意义是：将派生类对象视为基类对象，并调用派生类的实现

如果对象是存放在堆中，通过基类指针delete时，如果基类的析构函数不是虚函数，只会删除派生类对象的基类部分（只调用基类的析构函数），而不会删除派生类对象的派生类部分（不会调用派生类的析构函数），会导致内存泄漏。

3.

虚函数表和虚表指针

虚函数表是每个类中，只要有虚函数或者重写的虚函数，那这个类就会有这个表，以静态数组的形式存放在静态存储区。

虚表指针是每个有虚表的对象都会有的指针，在栈或者堆中。

每当创建一个有虚表类的对象时，初始化的时候会创建虚表指针。每次需要访问虚函数时，程序通过虚表指针找到虚函数表，再根据对应实现的入口地址访问。

虚表中的指针指向自身类中的实现。虚表指针指向自身类的虚表。

虚函数表是在编译的时候生成，虚表指针是在运行的时候生成。

4.

抽象基类和纯虚函数

抽象基类是至少有一个纯虚函数的类，不可被实例化，继承的子类如果没有实现该函数也不可实例化。

纯虚函数：

```C++
virtual void doSomething()=0;
```

5.

构造函数不可能是虚函数，因为虚函数的运行必须根据虚表指针，但是虚表指针是在对象初始化之后才会有。

6.

析构函数推荐是虚函数，如果析构函数不是虚函数，那么子类对象调用基类的析构函数，只会删除基类的对象，不会删除子类的对象，造成内存泄漏。

7.

虚继承

```C++
class A{};
class B:public virtual A{};
class C:public virtual A{};
class D:public B,public C{};
```

用虚继承防止出现菱形继承。

声明D的对象后，程序会按照继承顺序，按照A,B,C,D的顺序生成对象。

虚基类表和虚基类偏移指针。

每个虚继承的类都会生成虚基类表，例如B和C

虚基类表是在编译时，编译器根据类的布局生成的表。每个含虚继承的类都有，例如B和C

虚基类偏移指针（VBPtr）是在运行时生成的指针。

含虚继承的子类的对象声明后，才会出现VBptr，在D对象中的B和C对象中。

8.

override能够强制编译器在编译的时候确定函数已经被覆盖。

### 第十二章 运算符类型与运算符重载

1.

单目运算符：一个操作数

双目运算符：两个操作数

2.

operator的所有用法

```C++
class myclass{
    public:
    myclass(int v){
        value=v;
    }
    void display(){
        cout<<"value is"<<value<<endl;
    }
    int operator<<(const myclass& obj){//operator第一种用法，运算符重载
        return value;
    }
    int operator+(const myclass& other) const{ // 双目运算符重载（成员函数，左操作数为当前对象）
        return value + other.value;
    }
    int operator()(const int& x)const{return value*x;}//operator第二种用法，直接对象函数化
    private:
        int value;
};


int main(){
    myclass obj1(10);myclass obj2(20);
    cout<<obj1+obj2<<endl;
    cout<<obj1(15)<<endl;
    return 0;
}
```

### 第十三章 类型转换运算符

1.static_cast

只会在编译时检查，而实际运行时可能会出错，例如基类指针转换成子类指针

基本类型的转换，例如int与double

子类指针转换成基类指针

void*指针转换为其他类型

2.dynamic_cast

只用于指针和引用，并且有继承关系的类指针之间的转换

例如有虚继承。会在运行时检查是否出错，如果转换失败，转换的指针会变成nullptr。转换引用的时候失败，如果用try catch，会检查到抛出std::bad_cast

3.reinterpret_cast

最没有约束的转换，直接将底层内存重新解释，但也是最危险的转换。

4.const_cast

去掉或者加上const/volatile。

### 第十四章 模板

1.

```C++
template <typename T>   // 或者 template <class T>
class MyClass {
private:
    T data;   // 使用模板参数作为成员变量类型
public:
    MyClass(T value) : data(value) {}
    void set(T value) { data = value; }
    T get() const { return data; }
};

```

可以声明多个模板

可以为某种类型设置一个特殊情况

```C++
template <typename T>   // 或者 template <class T>
class MyClass {
private:
    T data;   // 使用模板参数作为成员变量类型
public:
    MyClass(T value) : data(value) {}
    void set(T value) { data = value; }
    T get() const { return data; }
};


template <>   // 模板特化
class MyClass <char>{
private:
    T data;   
public:
    MyClass(T value) : data(value) {}
    void set(T value) { data = value+1; }
    T get() const { return data; }
};

```

也可以设置默认类型

```C++
template <typename T=int>   // 默认是int
class MyClass {
private:
    T data;   
public:
    MyClass(T value) : data(value) {}
    void set(T value) { data = value; }
    T get() const { return data; }
};


int main(){
    MyClass<> m(15);//没有显式指明模板类型，则使用默认的模板
}
```

2.

模板类和模板函数在编译时没有生成具体的机器指令，在编译时会生成抽象语法树，让编译器直到有这个模板类或者模板函数，并且在运行时声明具体类型之后再生成对应的机器指令。



## 剩余内容总结为三章：STL容器和算法，智能指针，流

### 第十五章 STL容器和算法

#### 容器

|容器名|用途/特点|
|-|-|
|`vector`|动态数组，支持快速随机访问，尾部插入/删除快|
|`deque`|双端队列，支持头尾插入/删除快，随机访问较快|
|`list`|双向链表，插入/删除快，但不支持随机访问|
|`forward_list`|单向链表，占用空间少，插入/删除快|
|`array`|固定大小数组，支持随机访问，大小在编译期确定|
|`string`|字符串容器，动态长度，提供丰富的操作函数|
|`set`|有序集合，元素唯一，内部使用红黑树|
|`multiset`|有序集合，允许重复元素，红黑树实现|
|`map`|键值对集合，键唯一，按键排序|
|`multimap`|键值对集合，键可以重复，按键排序|
|`unordered_set`|无序集合，基于哈希表，查找快|
|`unordered_multiset`|无序集合，允许重复元素|
|`unordered_map`|无序键值对集合，基于哈希表，查找快|
|`unordered_multimap`|无序键值对集合，允许重复键|
|`stack`|栈（LIFO），适合后进先出场景|
|`queue`|队列（FIFO），适合先进先出场景|
|`priority_queue`|优先队列，自动按优先级排序元素|
|`bitset`|固定位集合，支持按位操作|


#### 算法

|算法函数名|用途/特点|
|-|-|
|`sort`|排序，可自定义比较函数|
|`stable_sort`|稳定排序，保留相等元素顺序|
|`partial_sort`|对前 n 个元素排序|
|`nth_element`|找第 n 大/小元素，部分排序|
|`binary_search`|二分查找，返回是否存在|
|`lower_bound`|有序序列中第一个不小于给定值的位置|
|`upper_bound`|有序序列中第一个大于给定值的位置|
|`equal_range`|返回元素等于某值的区间|
|`find`|查找第一个匹配元素|
|`find_if`|查找第一个满足条件的元素|
|`count`|统计某元素出现次数|
|`count_if`|统计满足条件的元素数量|
|`for_each`|对每个元素执行操作|
|`transform`|元素映射/转换|
|`copy`|拷贝序列|
|`copy_if`|拷贝满足条件的元素|
|`move`|移动元素到另一序列|
|`fill`|填充元素|
|`fill_n`|填充前 n 个元素|
|`remove`|移除元素（逻辑删除，需 erase）|
|`remove_if`|移除满足条件的元素（逻辑删除）|
|`unique`|移除连续重复元素（需 erase）|
|`reverse`|反转序列|
|`rotate`|旋转序列|
|`shuffle`|随机打乱序列|
|`merge`|合并有序序列|
|`includes`|判断一个有序序列是否包含另一个|
|`set_union`|集合并集（有序序列）|
|`set_intersection`|集合交集（有序序列）|
|`set_difference`|集合差集（有序序列）|
|`set_symmetric_difference`|集合对称差（有序序列）|
|`accumulate`|累加求和（）|
|`inner_product`|内积计算|
|`partial_sum`|前缀和|
|`adjacent_difference`|邻位差分|
|`min_element`|找最小元素|
|`max_element`|找最大元素|
|`minmax_element`|同时找最小和最大元素|
|`all_of`|判断所有元素是否满足条件|
|`any_of`|判断是否有元素满足条件|
|`none_of`|判断是否没有元素满足条件|


#### 迭代器

|类型|用途/特点|
|-|-|
|`iterator`|一般迭代器，用于遍历容器|
|`const_iterator`|常量迭代器，只读访问容器|
|`reverse_iterator`|反向迭代器，从尾到头遍历容器|
|`const_reverse_iterator`|反向只读迭代器|
|`istream_iterator`|输入流迭代器，从输入流读取数据|
|`ostream_iterator`|输出流迭代器，向输出流写入数据|


### 第十六章 智能指针

智能指针能够自动的管理内存，不需要手动delete，防止出现内存泄漏

智能指针分为三种

共享智能指针shared_ptr

可以出现多个共享指针指向同一块内存，内部会有一个计数器，如果所有的指针放弃所有权或者离开作用域，就会自动调用该对象的析构函数，避免内存泄漏

```C++
int main(){
    shared_ptr<int> s=make_shared<int>(15);
    auto ss=s;
    *ss=18;
    cout<<*s<<endl;
    ss.reset();//该指针放弃所有权，引用计数减一
}
```

独占智能指针unique_ptr

```C++
    unique_ptr<int> iptr(new int(25));
    unique_ptr<string> sptr=make_unique<string> ("aabbcc");
    cout<<*iptr<<endl;
    unique_ptr<string> ssptr=std::move(sptr);//这里通过move将sptr从左值变成右值，将指针所有权转让
    cout<<*ssptr<<endl;
    //cout<<*sptr<<endl;
```

弱引用智能指针weak_ptr

weak_ptr 不能独立管理对象，它只是对 shared_ptr 的一个观察者。
不会增加引用计数，因此不会阻止对象销毁。
必须通过 lock() 转换为 shared_ptr 才能访问对象。
主要用于解决 循环引用 问题（两个 shared_ptr 互相持有，会导致内存无法释放）。

```C++
class A{
    public:
        shared_ptr<B> AA;
};

class B{
    public:
        shared_ptr<A> BB;
};


int main(){
    auto a=make_shared<A>();
    auto b=make_shared<B>();
    a->AA=b;
    b->BB=a;//这里出现了循环引用，导致共享指针a和b的引用计数都是2，导致无法自动释放，从而导致内存泄漏
}
```



### 第十七章 流

流分为输入流，输出流

头文件：

istream ostream iostream

ifstream ofstream fstream

istringstream ostringstream stringstream

```C++
void OutputDebugInfo(ostream& os,std::string const& DebugInfo){
    os<<DebugInfo<<endl;
}

int main(){
    fstream ifs("log.txt",std::ios::out);
    OutputDebugInfo(ifs,"程序开始运行");

    OutputDebugInfo(ifs,"马上要输出helloworld");
    cout<<"hello world"<<endl;//程序的任务是输出helloworld
    OutputDebugInfo(ifs,"输出了helloworld");
    OutputDebugInfo(ifs,"程序结束");

    OutputDebugInfo(cout,"程序结束");

    ifs.close();
}
```









## C++补充学习知识点

### 1.std::move与移动构造函数

std::move的作用是将一个左值变成一个右值。

如果用左值进行赋值操作，本质上是拷贝，将需要拷贝的内容复制一份。

如果需要拷贝的内容是一个临时变量，这时为了程序的性能，就无需拷贝，而是将临时变量直接赋值，此时内存没有发生改变，仅仅是把临时变量的资源的所有权发生了改变。如果赋值操作发生后，原临时变量有效，但处于未定义的状态，类似于int a; a刚定义没有设置初值。

```C++
string s="hello world";
vector<string> v;
v.push_back(s);
//本质是复制了一份string字符串，然后再用vector存放
v.push_back(std::move(s));
//本质是s字符串放弃了自己资源的所有权，资源直接被vector插入使用，相当于少拷贝一次
```

移动构造函数：

```C++
class myclass{
    private:
        int *t;
        int size;
        shared_ptr<int> sp;
    public:
        myclass(){
            t=nullptr;
            sp=nullptr;
            size=0;
        }
        myclass(myclass &&other) noexcept 
        :t(other.t),size(other.size),sp(other.sp)
        {//移动构造函数
            other.t=nullptr;
            other.sp=nullptr;//主要是将动态资源指针置空
            size=0;//重置即可，不一定非得是0，看需要
        }
};
```

一般类没有声明移动构造函数时会自动生成，

但是如果已经显式声明复制构造函数，重载复制构造函数运算符，重载移动构造函数运算符时，则不会自动生成移动构造函数，想用就要显式声明移动构造函数。

如果没有显式声明，也没有自动生成，遇到 std::move时会自动退化为执行复制构造函数。
