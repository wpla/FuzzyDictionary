#ifndef DISTILLER_DICTIONARYIMPL_SEARCHTHREAD_H
#define DISTILLER_DICTIONARYIMPL_SEARCHTHREAD_H

#pragma once

namespace Distiller
{

namespace DictionaryImpl
{

class ThreadedSearchStrategy;

class SharedThreadData;

/**
 * When querying the dictionary we use a thread for finding the
 * best match for a given gram. Doing so we can parallelize the 
 * search by processing more than one gram at a time.
 */
class SearchThread : public QThread
{

	ThreadedSearchStrategy& d_;
	
	SharedThreadData& data_;
	
public:

	SearchThread(ThreadedSearchStrategy& d,
		SharedThreadData& data);
	
	void run();
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

