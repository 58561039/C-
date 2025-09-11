#include<iostream>
//write by lzc
//namespace的用法

/*
在企业级开发中，基本所有的内容都会放在namespace中，定义的所有类，定义的所有全局变量，都尽量放在namespace中
并且namespace还要分层，根据模块来分，互相隔离，防止命名冲突
*/


/*
例如，一个企业程序
一个项目是webserver，webserver包括网络（net）模块，日志（log）模块，核心(core)模块
*/

/*
namespace company{
    //本项目
    namespace webserver{
        namespace net{}
        namespace log{}
        namespace core{}
    }
}
*/

//在net.h或者net.cpp中
//常见的分层结构是公司->项目->模块
namespace company{
namespace webserver{
namespace net{

class HttpRequest{};//写HttpRequest类，或者写类函数的实现

}
}
}


//如果我想在一个程序中定义一个私有的函数，我不想别的文件可以访问该函数，可以使用匿名namespace

namespace{

void sort(){}//定义一个排序函数，没有定义namespace的名字，默认只有本文件可以访问调用

}

