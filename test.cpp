/*
 * @Author: your name
 * @Date: 2021-07-18 01:28:38
 * @LastEditTime: 2021-07-18 01:42:50
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit#
 * @FilePath: \bithack_ptr\main.cpp
 */
#include"bithack_observer_ptr.hpp"

#include<iostream>

using namespace bithack;

int main(void)
{
    int a=123;
    observer_ptr<int,unsigned short> ptr(&a,65535);
    std::cout<<"size of ptr:"<<sizeof(ptr)<<'\n';
    std::cout<<*ptr<<' '<<ptr.info()<<'\n';
    return 0;
}
