#ifndef DISTILLER_DICTIONARYIMPL_ABSTRACTSEARCHSTRATEGY_H
#define DISTILLER_DICTIONARYIMPL_ABSTRACTSEARCHSTRATEGY_H

#pragma once

#include "DebugInfo.h"

namespace Distiller
{

namespace DictionaryImpl
{

class AbstractSearchStrategy
{

public:

	AbstractSearchStrategy()
		{ }

	virtual ~AbstractSearchStrategy()
		{ }

	virtual QString search(const QString& needle) = 0;

	virtual QString search(const QString& needle,
		Dictionary::DebugInfo* debugInfo) = 0;
	
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

