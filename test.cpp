/*
 * @Author: your name
 * @Date: 2021-07-18 01:28:38
 * @LastEditTime: 2021-07-19 21:36:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit#
 * @FilePath: \tagged_ptr\main.cpp
 */
#include"tagged_observer_ptr.hpp"

#include<iostream>

using namespace tagged_ptr;

int main(void)
{
    int a=123;
    observer_ptr<int,unsigned short> ptr(&a,65535);
    std::cout<<"size of ptr:"<<sizeof(ptr)<<'\n';
    std::cout<<*ptr<<' '<<ptr.info()<<'\n';
    return 0;
}
