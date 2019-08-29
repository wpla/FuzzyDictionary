#ifndef DISTILLER_DICTIONARYIMPL_BUILDER_H
#define DISTILLER_DICTIONARYIMPL_BUILDER_H

#pragma once

#include "KeyDistTuple.h"

namespace Distiller
{

namespace DictionaryImpl
{

/**
 * Build the internal data structures of the dictionary
 * from an UTF-8 encoded text file where every line is 
 * a dictionary entry.
 */
class Builder
{

	Private& d_;
	
	QSet<QString> encodedEntries_;

	void addGram(const QString& gram, KeyType key);

	/**
	 * Inserts a line into the dictionary.
	 * Throws Exception(Exception::TooManyEntries) if the maximum numbers
	 * of entries is exceeded.
	 */
	void processLine(const QString& line);
	
public:

	Builder(Dictionary& dict) : d_(*dict.d_)
		{ }
		
	bool buildFrom(const QString& dictFile);
	
	bool buildFrom(const QStringList& stringList);
	
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

