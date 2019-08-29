#ifndef DISTILLER_DICTIONARYIMPL_THREADPOLICY_H
#define DISTILLER_DICTIONARYIMPL_THREADPOLICY_H

#pragma once

#include <QReadWriteLock>

#include "AbstractThreadPolicy.h"

namespace Distiller
{

namespace DictionaryImpl
{

class ThreadPolicy : public AbstractThreadPolicy
{

protected:

	mutable QReadWriteLock lock_;
	
	virtual void lockForRead() const;
	
	virtual void lockForWrite() const;
	
	virtual void unlock() const;
	
public:

	ThreadPolicy();
	
	virtual ~ThreadPolicy();
	
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

