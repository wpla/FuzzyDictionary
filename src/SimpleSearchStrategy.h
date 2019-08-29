#ifndef DISTILLER_DICTIONARYIMPL_SIMPLESEARCHSTRATEGY_H
#define DISTILLER_DICTIONARYIMPL_SIMPLESEARCHSTRATEGY_H

#pragma once

#include "SearchStrategyBase.h"

namespace Distiller
{

namespace DictionaryImpl
{

class KeyDistTuple;

class SimpleSearchStrategy : public SearchStrategyBase
{
	
	KeyDistTuple searchBestKey(const QString& gram,
							   Dictionary::DebugInfo* debugInfo);

public:

	SimpleSearchStrategy(Private& d);
	
	~SimpleSearchStrategy();
	
	QString search(const QString& needle);
	
	QString search(const QString& needle,
				   Dictionary::DebugInfo* debugInfo);

};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

