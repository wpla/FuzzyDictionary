#ifndef DISTILLER_DICTIONARYIMPL_SEARCHSTRATEGYBASE_H
#define DISTILLER_DICTIONARYIMPL_SEARCHSTRATEGYBASE_H

#pragma once

#include "AbstractSearchStrategy.h"
#include "SearchInfo.h"
#include "Dictionary.h"

namespace Distiller
{

namespace DictionaryImpl
{

class SearchStrategyBase : public AbstractSearchStrategy
{

protected:

	/// Pointer to DictionaryImpl::Private.
	Private& d_;
	
	/// Encoded needle.
	QString encodedNeedle_;
	
	/// Bit encoded needle;
	quint64 bitencodedNeedle_;
	
	/// Length of encoded needle.
	int encNeedleSize_;

	/// Maximum number of allowed typos for needle.
	int maxTypos_;
	
	/// Number of characters to jump forward in needle for next gram.
	int gramJump_;
	
	/// Length of grams.
	int gramLen_;
	
	/// Number of grams.
	int gramCount_;
	
	/// Grouped data for searching.
	SearchInfo searchInfo_;

	void calculate(const QString& needle);

public:

	SearchStrategyBase(Private& d);
	
	virtual ~SearchStrategyBase();
	
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

