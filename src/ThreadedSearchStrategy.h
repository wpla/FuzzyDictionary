#ifndef DISTILLER_DICTIONARYIMPL_THREADEDSEARCHSTRATEGY_H
#define DISTILLER_DICTIONARYIMPL_THREADEDSEARCHSTRATEGY_H

#pragma once

#include <QReadWriteLock>

#include "KeyDistTuple.h"
#include "SharedThreadData.h"
#include "SearchStrategyBase.h"

namespace Distiller
{

namespace DictionaryImpl
{

class SearchThread;

class ThreadedSearchStrategy : public SearchStrategyBase
{

	static const int numberOfThreads_ = 3;
	
	SharedThreadData threadData_;
	
	QList<SearchThread*> threads_;
	
	QReadWriteLock lock_;
	
	QReadWriteLock debugInfoLock_;
	
	void startThreads();
	
	void waitForThreadsToFinish();
	
	KeyDistTuple executeThreads();
	
	void prepareSearch();
	
	KeyDistTuple searchBestKey(const QString& gram,
							   Dictionary::DebugInfo* debugInfo = 0);
							   
public:

	ThreadedSearchStrategy(Private& d);
	
	~ThreadedSearchStrategy();
	
	QString search(const QString& needle);
	
	QString search(const QString& needle, Dictionary::DebugInfo* debugInfo);
	
	friend class SearchThread;

};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

