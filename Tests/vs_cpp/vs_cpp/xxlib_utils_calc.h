#pragma once
namespace xxlib
{

// �õ��պ�С�� n ������ �������ڴ����
int GetPrime( int n );

// calc lead zero ���� 2 ��������λͷ�ϵ� 0 �ĸ���
int Clz( size_t x );
int Popcnt( size_t x );


// ���ظպô��� x �� 2^n ��ֵ�����ڴ����
// ��� x ���� 2^n ϵ �򷵻�ԭֵ
size_t Round2n( size_t len );

// ��һ������ 2 �ļ��η�
size_t Calc2n( size_t len );

}
