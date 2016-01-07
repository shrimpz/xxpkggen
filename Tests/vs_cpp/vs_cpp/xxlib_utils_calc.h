#pragma once
namespace xxlib
{

// 得到刚好小于 n 的质数 主用于内存分配
int GetPrime( int n );

// calc lead zero （数 2 进制数高位头上的 0 的个数
int Clz( size_t x );
int Popcnt( size_t x );


// 返回刚好大于 x 的 2^n 的值用于内存分配
// 如果 x 本身 2^n 系 则返回原值
size_t Round2n( size_t len );

// 算一个数是 2 的几次方
size_t Calc2n( size_t len );

}
