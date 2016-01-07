#include "xxlib.h"
namespace xxlib
{
ScopeGuard::~ScopeGuard()
{
	if( func ) func();
}

void ScopeGuard::RunAndCancel()
{
	if( func )
	{
		func();
		func = nullptr;
	}
}

void ScopeGuard::Run()
{
	if( func )
	{
		func();
	}
}

void ScopeGuard::Cancel()
{
	func = nullptr;
}

}
