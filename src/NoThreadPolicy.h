#ifndef DISTILLER_DICTIONARYIMPL_NOTHREADPOLICY_H
#define DISTILLER_DICTIONARYIMPL_NOTHREADPOLICY_H

#pragma once

#include "AbstractThreadPolicy.h"

namespace Distiller
{

namespace DictionaryImpl
{

class NoThreadPolicy : public AbstractThreadPolicy
{

protected:

	virtual void lockForRead() const
		{ }
		
	virtual void lockForWrite() const
		{ }
		
	virtual void unlock() const
		{ }

public:

	NoThreadPolicy();
	
	virtual ~NoThreadPolicy();

};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

