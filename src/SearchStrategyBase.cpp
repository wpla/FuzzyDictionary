#include <core/precompiled.h>

#include "DictionaryDefines.h"
#include "Private.h"
#include "DebugInfo.h"
#include "BitDistance.h"
#include "SearchStrategyBase.h"

namespace Distiller
{

namespace DictionaryImpl
{

SearchStrategyBase::SearchStrategyBase(Private& d) :
	d_(d),
	encodedNeedle_(),
	encNeedleSize_(0),
	maxTypos_(0),
	gramJump_(0),
	gramLen_(0),
	gramCount_(0),
	searchInfo_()
{ }

SearchStrategyBase::~SearchStrategyBase()
{ }

void SearchStrategyBase::calculate(const QString& needle)
{

	// Encoded needle.
	encodedNeedle_ = d_.encode(needle);
	
	// Bit encoded needle.
	bitencodedNeedle_ = BitDistance::bitPattern(encodedNeedle_.toAscii().constData());

	// Lenth of encoded needle.
	encNeedleSize_ = encodedNeedle_.size();
	
	if (encNeedleSize_ == 0)
		return;
	
	// Maximum number of typos for needle.
	maxTypos_ = d_.calcMaxTypos(needle);
	
	// Number of characters to jump forward for next gram.
	gramJump_ = encNeedleSize_ / (maxTypos_ + 1);
	
	// Length of grams.
	gramLen_ = qMin<quint16>(gramJump_, d_.maxGramSize());
	
	// Number of grams.
	gramCount_ = encNeedleSize_ / gramLen_;

	// Set infos for searching.	
	searchInfo_.setNeedle(encodedNeedle_);
	searchInfo_.setBitencodedNeedle(bitencodedNeedle_);
	searchInfo_.setWordlist(d_.encodedEntries_);
	searchInfo_.setBitpatternList(d_.bitencodedEntries_);
	searchInfo_.setMaxTypos(maxTypos_);

}

} // namespace DictionaryImpl

} // namespace Distiller

