#include <core/precompiled.h>

#include "ThreadPolicy.h"

namespace Distiller
{

namespace DictionaryImpl
{

ThreadPolicy::ThreadPolicy() :
	lock_()
{ }

ThreadPolicy::~ThreadPolicy()
{ }

void ThreadPolicy::lockForRead() const
{
	lock_.lockForRead();
}

void ThreadPolicy::lockForWrite() const
{
	lock_.lockForWrite();
}

void ThreadPolicy::unlock() const
{
	lock_.unlock();
}

} // namespace DictionaryImpl

} // namespace Distiller

