#include <core/precompiled.h>

#include "DictionaryDefines.h"
#include "DebugInfo.h"
#include "KeyDistTuple.h"
#include "Private.h"
#include "SearchInfo.h"
#include "SimpleSearchStrategy.h"

namespace Distiller
{

namespace DictionaryImpl
{

SimpleSearchStrategy::SimpleSearchStrategy(Private& d) :
	SearchStrategyBase(d)
{ }

SimpleSearchStrategy::~SimpleSearchStrategy()
{ }

QString SimpleSearchStrategy::search(const QString& needle)
{
	return search(needle, 0);
}

KeyDistTuple SimpleSearchStrategy::searchBestKey(const QString& gram,
										 Dictionary::DebugInfo* debugInfo)
{
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
		Dictionary::DebugInfo::GramInfo gramInfo;
		gramInfo.gram = gram;
		gramInfo.entries = d_.gramHash_[gram].valueCount();
		gramInfo.editdistance = rv.second;
		if (rv.keyIsValid())
			gramInfo.bestMatch = d_.entries_.toQString(rv.key());
		debugInfo->grams.append(gramInfo);
	}
#endif

	return rv;
}

QString SimpleSearchStrategy::search(const QString& needle, Dictionary::DebugInfo* debugInfo)
{
	IF_PROFILER(d_.profiler.queryTimer.restart());

	calculate(needle);
	
	if (encNeedleSize_ == 0)
		return QString();

	int restLen = encNeedleSize_;
	KeyDistTuple bestMatch;
	KeyDistTuple tmpMatch;

	for (int i = 0; i < gramCount_; i++) {
		QString gram = encodedNeedle_.mid(i * gramJump_, gramLen_);
		restLen -= gramJump_;
		if (i == (gramCount_ - 1) && restLen > 0)
			gram = encodedNeedle_.mid(i * gramJump_, 
				d_.gramHash_.maxGramSize()
			);
		tmpMatch = searchBestKey(gram, debugInfo);
		if (tmpMatch < bestMatch)
			bestMatch = tmpMatch;
		if (tmpMatch.distance() == 0)
			// Optimum found.
			break;
	}
	
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

