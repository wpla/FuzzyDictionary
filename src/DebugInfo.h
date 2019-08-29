#ifndef DISTILLER_DICTIONARYIMPL_DEBUGINFO_H
#define DISTILLER_DICTIONARYIMPL_DEBUGINFO_H

#pragma once

#include "Dictionary.h"

namespace Distiller
{

/**
 * Helper classes for debugging purposes.
 */
class Dictionary::DebugInfo
{

public:

	class GramInfo;
	
	QString original;
	
	QString query;

	QString encQuery;

	QString result;

	uint msecs;

	uint maxTypos;

	uint typos;

	uint editdistance;

	QList<GramInfo> grams;

	DebugInfo();

	friend std::ostream& operator << (std::ostream& out, const DebugInfo& rhs);
	
};

class Dictionary::DebugInfo::GramInfo
{

public:

	QString gram;

	uint entries;

	QString bestMatch;

	uint editdistance;

	GramInfo();

	friend std::ostream& operator << (std::ostream& out, const GramInfo& rhs);
};

} // namespace Distiller

#endif 

