### 1.

包含头文件thread

定义一个线程，选择该线程执行的函数，传参数

线程对象的join函数：主线程需要等待该线程执行结束之后才可以从join处继续执行。

joinable函数返回一个bool值，为1时，可以调用join和detach函数，否则不能调用

detach函数：将主线程与子线程分离，不管有没有执行完，都会继续向下执行。

```C++
#include<iostream>
#include<thread>
#include<string>
using namespace std;

void Output(string s){
    cout<<s<<endl;
    return;
}

int main(){
    thread t1(Output,"lzc");
    bool isJoin=t1.joinable();
    if(isJoin){
        t1.join();
    }
    cout<<"over"<<endl;
    return 0;
}
```

### 2.

thread t1(worker,a,b,c);

这种方式本质是将a，b，c拷贝一份，然后线程根据这些参数运行worker，但如果worker的参数是引用类型，worker中修改了参数，但主线程无法访问到。

因为thread不支持传引用，本质传参数是拷贝。

所以需要用到一个函数：std::ref()，该函数可以把一个参数包装成引用，从而避免thread拷贝参数的特点。

```C++
#include<iostream>
#include<thread>
using namespace std;
//write by lzc 

void worker(int& a){
    a++;
}

int main(){
    int a=1;
    thread t1(worker,std::ref(a));
    t1.join();
    cout<<a<<endl;//输出a=2
}
```

std::cref()传的是const的引用

### 3.与智能指针结合

如果需要参数的是对象，传递参数的方式是引用时，无法保证始终有效

因为使用了智能指针，不需要考虑delete，如果传的参数是引用，可能会因为局部变量的问题导致出错，如果传递的是普通指针，可能会导致指针指向的位置被释放，虽然地址有效，但指向的内容已经失效，所以智能指针是更好的选择。

```C++
class myclass{
    public:
        void foo(){
            cout<<"foo"<<endl;
        }
};

int main(){
    shared_ptr<myclass> a=make_shared<myclass>();//使用智能指针

    thread t1(&myclass::foo,a);

    t1.join();
}
```

### 4.防止数据竞争

使用互斥锁

一个线程对于一段代码加锁之后，其他线程无法访问这段代码（临界区）

下列代码是手动添加锁，释放锁

```C++
#include<iostream>
#include<thread>
#include<mutex>
using namespace std;

int a=0;
mutex mtx;

void worker(){
    for(int i=0;i<10000;i++){
        mtx.lock();
        a++;
        mtx.unlock();
    }
}


int main(){
    thread t1(worker);
    thread t2(worker);
    
    t1.join();
    t2.join();

    cout<<a<<endl;
}

```



可以使用RAII机制

lock_guard在调用构造函数时会自动上锁，当调用析构函数时会自动解锁

```C++
lock_guard<mutex> lock(mtx);//加锁，作用域内有效，出了作用域自动释放锁，不需要手动释放

//例如
for(int i=0;i<1000;i++){
    lock_guard<mutex> lock(mtx);//从这里上锁
    dosomething;
}//出了作用域立刻释放
```



### 5.死锁

互斥

不剥夺

请求和保持

循环等待

```C++
mutex m1,m2;

void worker1(){
    for(int i=0;i<50;i++){
        m1.lock();
        m2.lock();
        m1.unlock();
        m2.unlock();
    }
}

void worker2(){
    for(int i=0;i<50;i++){
        m2.lock();
        m1.lock();
        m1.unlock();
        m2.unlock();
    }
}

int main(){
    thread t1(worker1);
    thread t2(worker2);

    t1.join();
    t2.join();

    cout<<"over"<<endl;
}
```

该程序会发生死锁

进程1占用m1，等待m2

进程2占用m2，等待m1


