#ifndef DISTILLER_DICTIONARYIMPL_SEARCHINFO_H
#define DISTILLER_DICTIONARYIMPL_SEARCHINFO_H

#pragma once

class QString;

#include <tagdistiller/StringArray.h>

#include "BitDistance.h"

namespace Distiller
{

namespace DictionaryImpl
{

class SearchInfo
{

	QString needle_;
	
	StringArray wordlist_;
	
	quint64 bitencodedNeedle_;
	
	BitpatternList bitpatternList_;
	
	quint8 maxTypos_;
	
	// QReadWriteLock lock_;
	
public:

	SearchInfo();

	~SearchInfo();
	
	void setNeedle(const QString& needle)
		{ needle_ = needle; }
		
	void setWordlist(const StringArray& wordlist)
		{ wordlist_ = wordlist; }
		
	void setBitencodedNeedle(quint64 bitencodedNeedle)
		{ bitencodedNeedle_ = bitencodedNeedle; }
		
	void setBitpatternList(const BitpatternList& bitpatternList)
		{ bitpatternList_ = bitpatternList; }
		
	void setMaxTypos(quint8 maxTypos)
		{ maxTypos_ = maxTypos; }
	
	quint8 maxTypos() const
		{ return maxTypos_; }
		
	bool sizeDiffersTooMuch(uint key) const;
	
	bool editDistanceTooLarge(uint key) const;
		
	quint8 calcDistance(uint key) const;

};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

