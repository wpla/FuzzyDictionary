#ifndef DISTILLER_DICTIONARYIMPL_ABSTRACTTHREADPOLICY_H
#define DISTILLER_DICTIONARYIMPL_ABSTRACTTHREADPOLICY_H

#pragma once

namespace Distiller
{

namespace DictionaryImpl
{

class AbstractThreadPolicy
{

protected:

	virtual void lockForRead() const = 0;
	
	virtual void lockForWrite() const = 0;
	
	virtual void unlock() const = 0;

public:

	AbstractThreadPolicy()
		{ }
	
	virtual ~AbstractThreadPolicy()
		{ }

};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

