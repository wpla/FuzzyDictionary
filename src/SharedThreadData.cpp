#include <core/precompiled.h>

#include "DictionaryDefines.h"
#include "KeyDistTuple.h"
#include "SharedThreadData.h"

namespace Distiller
{

namespace DictionaryImpl
{

SharedThreadData::SharedThreadData() :
	maxTypos_(0),
	bestMatchFound_(false),
	debugInfo_(0)
{ }
	
void SharedThreadData::clear()
{
	setNeedle("");
	setMaxTypos(0);
	QWriteLocker locker1(&gramQueueLock_);
	gramQueue_.clear();
	QWriteLocker locker2(&matchQueueLock_);
	matchQueue_.clear();
	QWriteLocker locker3(&bestMatchLock_);
	bestMatchFound_ = false;
	debugInfo_ = 0;
}

void SharedThreadData::clearGramQueue()
{
	QWriteLocker locker(&gramQueueLock_);
	gramQueue_.clear();
}

void SharedThreadData::setNeedle(const QString& needle)
{
	QWriteLocker locker(&needleLock_);
	needle_ = needle;
}

QString SharedThreadData::needle()
{
	QReadLocker locker(&needleLock_);
	return needle_;
}

void SharedThreadData::setBestMatchFound(bool val)
{
	QWriteLocker locker(&bestMatchLock_);
	bestMatchFound_ = val;
}

bool SharedThreadData::bestMatchFound()
{
	QReadLocker locker(&bestMatchLock_);
	return bestMatchFound_;
}

void SharedThreadData::setMaxTypos(quint8 maxTypos)
{
	QWriteLocker locker(&maxTyposLock_);
	maxTypos_ = maxTypos;
}

quint8 SharedThreadData::maxTypos()
{
	QReadLocker locker(&maxTyposLock_);
	return maxTypos_;
}
	
bool SharedThreadData::nomoreGrams()
{
	QReadLocker locker(&gramQueueLock_);
	return gramQueue_.isEmpty();
}
	
bool SharedThreadData::nomoreKeyDistTuples()
{
	QReadLocker locker(&matchQueueLock_);
	return matchQueue_.isEmpty();
}
	
void SharedThreadData::enqueueGram(const QString& gram)
{
	QWriteLocker locker(&gramQueueLock_);
	gramQueue_.enqueue(gram);
}

void SharedThreadData::enqueueKeyDistTuple(const KeyDistTuple& tuple)
{
	QWriteLocker locker(&matchQueueLock_);
	matchQueue_.enqueue(tuple);
}

QString SharedThreadData::dequeueGram()
{
	QWriteLocker locker(&gramQueueLock_);
	return gramQueue_.dequeue();
}

QString SharedThreadData::nextGram()
{
	QWriteLocker locker(&gramQueueLock_);
	if (gramQueue_.isEmpty())
		return QString();
	return gramQueue_.dequeue();
}

KeyDistTuple SharedThreadData::dequeueKeyDistTuple()
{
	QWriteLocker locker(&matchQueueLock_);
	return matchQueue_.dequeue();
}

KeyDistTuple SharedThreadData::bestMatchFromQueue()
{
	QWriteLocker locker(&matchQueueLock_);
	KeyDistTuple bestMatch;
	KeyDistTuple tmpMatch;
	while (matchQueue_.isEmpty() == false) {
		tmpMatch = matchQueue_.dequeue();
		if (tmpMatch < bestMatch)
			bestMatch = tmpMatch;
		if (tmpMatch.distance() == 0)
			// Optimal match found.
			break;
	}
	matchQueue_.clear();
	return bestMatch;
}

} // namespace DictionaryImpl

} // namespace Distiller

