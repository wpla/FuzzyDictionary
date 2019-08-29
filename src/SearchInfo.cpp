#include <core/precompiled.h>

#include <QString>

#include <tagdistiller/StringArray.h>
#include <tagdistiller/EditDistance.h>
#include <tagdistiller/SimpleString.h>

#include "KeyDistTuple.h"
#include "BitDistance.h"
#include "SearchInfo.h"

namespace Distiller
{

namespace DictionaryImpl
{

SearchInfo::SearchInfo()
{ }

SearchInfo::~SearchInfo()
{ }

bool SearchInfo::sizeDiffersTooMuch(uint key) const
{
	if (qAbs<int>(needle_.size() - 
		wordlist_.sizeOf(key)) > maxTypos_)
		return true;
	return false;
}

bool SearchInfo::editDistanceTooLarge(uint key) const
{
	/**
	 * We gonna estimate the lower bound of the edit
	 * distance via a fast bitwise algorithm.
	 */
	if (BitDistance::minDistance(bitencodedNeedle_, 
		bitpatternList_[key]) > maxTypos_)
		return true;
	return false;
}

quint8 SearchInfo::calcDistance(uint key) const
{
	Q_ASSERT(key < (uint)wordlist_.size());
	Q_ASSERT(key < (uint)bitpatternList_.size());
	
	DistType dist = KeyDistTuple::invalidDistance;
	if (sizeDiffersTooMuch(key) == true)
		return dist;
	if (editDistanceTooLarge(key) == true)
		return dist;
	dist = EditDistance::calc(
				SimpleString(needle_),
				wordlist_.toSimpleString(key),
				(int)maxTypos_,
				EditDistance::SubstringMatch
		   );
	if (dist <= maxTypos_)
		return dist;
	return KeyDistTuple::invalidDistance;
}

} // namespace DictionaryImpl

} // namespace Distiller

