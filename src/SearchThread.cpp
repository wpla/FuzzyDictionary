#include <core/precompiled.h>

#include "DictionaryDefines.h"
#include "ThreadedSearchStrategy.h"
#include "KeyDistTuple.h"
#include "SharedThreadData.h"
#include "SearchThread.h"

namespace Distiller
{

namespace DictionaryImpl
{

SearchThread::SearchThread(ThreadedSearchStrategy& d,
	SharedThreadData& data) :
	QThread(),
	d_(d),
	data_(data)
{ }

void SearchThread::run()
{
	KeyDistTuple bestMatch;
	KeyDistTuple match;
	QString gram;
		
	while ((gram = data_.nextGram()) != QString())
	{
		if (data_.bestMatchFound() == true)
			// Best match already found by another Thread.
			return;
		match = d_.searchBestKey(gram, data_.debugInfo_);
		if (data_.bestMatchFound() == true)
			// Best match already found by another Thread.
			return;
		if (match < bestMatch)
			bestMatch = match;
		if (match.distance() == 0) {
			data_.setBestMatchFound(true);
			data_.clearGramQueue();
			break;
		}
	}
	if (bestMatch.keyIsValid())
		data_.enqueueKeyDistTuple(bestMatch);
}

} // namespace DictionaryImpl

} // namespace Distiller

