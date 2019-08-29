#include <core/precompiled.h>

#include "DictionaryDefines.h"
#include "DebugInfo.h"
#include "Private.h"
#include "SearchThread.h"
#include "SearchInfo.h"
#include "ThreadedSearchStrategy.h"

namespace Distiller
{

namespace DictionaryImpl
{

ThreadedSearchStrategy::ThreadedSearchStrategy(Private& d) : 
	SearchStrategyBase(d),
	threadData_(),
	threads_(),
	lock_()
{
	int count = numberOfThreads_;
	
	try {
		while (count-- > 0) {
			SearchThread *thread = new SearchThread(*this, threadData_);
			threads_.append(thread);
		}
	}
	catch (...) {
		qDeleteAll(threads_);
		throw;
	}
}

ThreadedSearchStrategy::~ThreadedSearchStrategy()
{
	qDeleteAll(threads_);
}

void ThreadedSearchStrategy::startThreads()
{
	QList<SearchThread*>::iterator thread;
	for (thread = threads_.begin(); thread != threads_.end(); thread++)
		(*thread)->start();
}

void ThreadedSearchStrategy::waitForThreadsToFinish()
{
	QList<SearchThread*>::iterator thread;
	for (thread = threads_.begin(); thread != threads_.end(); thread++)
		(*thread)->wait();
}

KeyDistTuple ThreadedSearchStrategy::executeThreads()
{
	startThreads();
	waitForThreadsToFinish();
	return threadData_.bestMatchFromQueue();
}

QString ThreadedSearchStrategy::search(const QString& needle)
{
	return search(needle, 0);
}

KeyDistTuple ThreadedSearchStrategy::searchBestKey(
	const QString& gram,
	Dictionary::DebugInfo* debugInfo
)
{
	QReadLocker locker(&lock_);
	
	KeyDistTuple rv;
	KeyDistTuple match;
	
	if (d_.gramHash_.contains(gram) == false)
		return rv;
		
	for(Private::Value::const_iterator i = 
		d_.gramHash_[gram].constBegin();
	    i != d_.gramHash_[gram].constEnd(); i++)
	{
		match = (*i)->find(searchInfo_);
		if (match < rv)
			rv = match;
		if (match.distance() == 0)
			break;
	}
	
#ifdef DICTIONARY_WITH_DEBUGINFO
	if (debugInfo) {
		QWriteLocker locker(&debugInfoLock_);
		Dictionary::DebugInfo::GramInfo gramInfo;
		gramInfo.gram = gram;
		gramInfo.entries = d_.gramHash_[gram].valueCount();
		gramInfo.editdistance = rv.second;
		if (rv.keyIsValid())
			gramInfo.bestMatch = d_.entries_.toQString(rv.key());
		threadData_.debugInfo_->grams.append(gramInfo);
	}
#endif

	return rv;
}

void ThreadedSearchStrategy::prepareSearch()
{
	int restLen = encNeedleSize_;
	// Reset threadData.
	threadData_.clear();
	threadData_.setMaxTypos(maxTypos_);
	threadData_.setNeedle(encodedNeedle_);
	// Fill threadData_.gramQueue with grams.
	for (int i = 0; i < gramCount_; i++) {
		QString gram = encodedNeedle_.mid(i * gramJump_, gramLen_);
		restLen -= gramJump_;
		if (i == (gramCount_ - 1) && restLen > 0)
			gram = encodedNeedle_.mid(i * gramJump_, 
				d_.gramHash_.maxGramSize());
		threadData_.enqueueGram(gram);
	}
}

QString ThreadedSearchStrategy::search(const QString& needle,
									   Dictionary::DebugInfo* debugInfo)
{
	IF_PROFILER(d_.profiler.queryTimer.restart());

#ifdef DICTIONARY_WITH_DEBUGINFO
	threadData_.debugInfo_ = debugInfo;
#endif
	
	calculate(needle);
	
	if (encNeedleSize_ == 0)
		return QString();
	
	prepareSearch();
	
	KeyDistTuple bestMatch = executeThreads();
	
	IF_PROFILER(d_.profiler.querytime += d_.profiler.queryTimer.elapsed());
	IF_PROFILER(d_.profiler.queries++);
	IF_PROFILER(d_.profiler.queriesPerSec =
		(((double)d_.profiler.queries * 1000.0)/d_.profiler.querytime));
	
#ifdef DICTIONARY_WITH_DEBUGINFO
	if (debugInfo) {
		debugInfo->encQuery = encodedNeedle_;
		debugInfo->maxTypos = maxTypos_;
		if (bestMatch.keyIsValid()) {
			debugInfo->result = d_.entries_.toQString(bestMatch.key());
			debugInfo->editdistance = bestMatch.distance();
		}
	}
#endif

	// bestMatch holds the best KeyDistTuple.
	if (bestMatch.keyIsValid())
		return d_.entries_.toQString(bestMatch.key());
	return QString();
}

} // namespace DictionaryImpl

} // namespace Distiller

