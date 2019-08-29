#ifndef DISTILLER_DICTIONARYIMPL_PRIVATE_H
#define DISTILLER_DICTIONARYIMPL_PRIVATE_H

#pragma once

#include <tagdistiller/StringArray.h>

#include "DictionaryDefines.h"
#include "GramHash.h"
#include "BitDistance.h"
#include "Profiler.h"

namespace Distiller
{

namespace DictionaryImpl
{

template<typename ContainerType>
class AbstractDB;

class AbstractSearchStrategy;

class SearchStrategyBase;

class ThreadedSearchStrategy;

class SimpleSearchStrategy;

template<typename ThreadPolicy>
class DictionaryDB;

template<typename ThreadPolicy>
class DictionaryDeepDB;

/**
 * Contains all private data structures of the 
 * dictionary.
 */
class Private
{
public:

	/**
	 * The dictionary offers a multi-threaded search algorithm implemented by
	 * class ThreadedSearchStrategy. To make this work the data structures as
	 * well as the DictionaryDB have to be thread-safe.
	 *
	 * Thread-safety can be mixed in via a template parameter by a policy class.
	 * There are two classes you can choose from:
	 *
	 *      ThreadPolicy 
	 *      NoThreadPolicy
	 *
	 * If you parametrize the data structures with the policy NoThreadPolicy you
	 * also have to choose the SimpleSearchStrategy, since the
	 * ThreadedSearchStrategy won't work with thread-unsafe data structures.
	 */
	
	typedef GramNode<ThreadPolicy> Value;
	
	typedef GramHash<ThreadPolicy> Hash;
	
	typedef DictionaryDB<ThreadPolicy> DB;
	
	/**
	 * Choose between ThreadedSearchStrategy or SimpleSearchStrategy.
	 * SimpleSearchStrategy will also work with thread-safe data structures
	 * but not the other way round.
	 */
	typedef ThreadedSearchStrategy SearchStrategy;

private:

    AbstractDB<Value::Container>* db_;
	
	mutable AbstractSearchStrategy* searchStrategy_;

	IF_PROFILER(mutable Profiler profiler);
	
	static const int defaultGramSize_ = 4;
	
    QString dictFilename_;
	
	quint32 gramSize_;

	/// A list of all encoded dictionary entries.
	StringArray encodedEntries_;

	/// A list of all dictionary entries.
	StringArray entries_;
	
	/// A list of the bit encoded entries.
	BitpatternList bitencodedEntries_;

	/// The hash of all grams.
	Hash gramHash_;
	
public:

	Private(quint32 gramSize = defaultGramSize_);

	~Private();
	
	inline quint32 gramSize() const
		{ return gramSize_; }
		
	inline quint32 minGramSize() const
		{ return gramHash_.minGramSize(); }
		
	inline quint32 maxGramSize() const
		{ return gramHash_.maxGramSize(); }
		
	bool save();
	
	bool load();
	
	void clear();

	/**
	 * Encodes a string.
	 *
	 * Transfroms the string to lowercase and
	 * delete remove every character other than 
	 * letter, digit and spaces.
	 *
	 * Example: "Louis Armstrong" --> "louis armstrong"
	 */
	QString encode(const QString& text) const;
	
	/**
	 * Returns the maximum of allowed errors for a string.
	 */
	uint calcMaxTypos(const QString& text) const;
	
	/**
	 * The match to a given pattern.
	 * Returns QString() if nothing is found.
	 */
	QString find(const QString& needle,
		         Dictionary::DebugInfo* debugInfo = NULL) const;
	
    friend class Distiller::Dictionary;
	
    friend class Distiller::DictionaryImpl::Builder;
	
	template<typename ThreadPolicy>
    friend class Distiller::DictionaryImpl::DictionaryDB;
	
	template<typename ThreadPolicy>
    friend class Distiller::DictionaryImpl::DictionaryDeepDB;
	
    friend class Distiller::DictionaryImpl::SearchStrategyBase;
	
    friend class Distiller::DictionaryImpl::SimpleSearchStrategy;
	
    friend class Distiller::DictionaryImpl::ThreadedSearchStrategy;
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

