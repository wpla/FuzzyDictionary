#ifndef DISTILLER_DICTIONARYIMPL_SHAREDTHREADDATA_H
#define DISTILLER_DICTIONARYIMPL_SHAREDTHREADDATA_H

#pragma once

#include <QReadWriteLock>

#include "DebugInfo.h"

namespace Distiller
{

namespace DictionaryImpl
{

class KeyDistTuple;

/**
 * Stores the data that is shared among SearchThreads
 * and manages accesss to it's members through Mutexes.
 */
class SharedThreadData
{

	QQueue<QString> gramQueue_;
	
	QReadWriteLock gramQueueLock_;
	
	QQueue<KeyDistTuple> matchQueue_;
	
	QReadWriteLock matchQueueLock_;
	
	QString needle_;
	
	QReadWriteLock needleLock_;
	
	quint8 maxTypos_;
	
	QReadWriteLock maxTyposLock_;
	
	bool bestMatchFound_;
	
	QReadWriteLock bestMatchLock_;
	
public:

	Dictionary::DebugInfo* debugInfo_;
	
	SharedThreadData();
		
	void clear();
	
	void clearGramQueue();
	
	void setNeedle(const QString& needle);
	
	QString needle();
	
	void setBestMatchFound(bool val);
	
	bool bestMatchFound();
	
	void setMaxTypos(quint8 maxTypos);
	
	quint8 maxTypos();
	
	bool nomoreGrams();
		
	bool nomoreKeyDistTuples();
		
	void enqueueGram(const QString& gram);
	
	void enqueueKeyDistTuple(const KeyDistTuple& tuple);
	
	QString dequeueGram();
	
	QString nextGram();
	
	KeyDistTuple dequeueKeyDistTuple();
	
	KeyDistTuple bestMatchFromQueue();
	
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

