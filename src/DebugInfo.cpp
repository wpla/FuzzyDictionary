#include <core/precompiled.h>

#include "DebugInfo.h"

namespace Distiller
{

Dictionary::DebugInfo::DebugInfo() : 
	original(),
	query(),
	encQuery(),
	result(),
	msecs(0),
	maxTypos(0),
	typos(),
	editdistance(),
	grams()
{ }
	
Dictionary::DebugInfo::GramInfo::GramInfo() : 
	gram(),
	entries(),
	bestMatch(), 
	editdistance()
{ }


std::ostream& operator << (std::ostream& out, const Dictionary::DebugInfo::GramInfo& rhs)
{
	out << "****     gram: " << rhs.gram.toAscii().constData() << std::endl;
	out << "   valueCount: " << rhs.entries << std::endl;
	out << "      bestKey: " << rhs.bestMatch.toAscii().constData() << std::endl;
	out << " editdistance: " << rhs.editdistance << std::endl;
	return out;
}

std::ostream& operator << (std::ostream& out, const Dictionary::DebugInfo& rhs)
{
	out << "*********************************************" << std::endl;
	out << "ORIGINAL:       " << rhs.original.toAscii().constData() << std::endl;
	out << "QUERY:          " << rhs.query.toAscii().constData() << std::endl;
	out << "encoded Query:  " << rhs.encQuery.toAscii().constData() << std::endl;
	out << "inserted typos: " << rhs.typos << std::endl;
	out << "maxTypos:       " << rhs.maxTypos << std::endl;
	foreach(Dictionary::DebugInfo::GramInfo info, rhs.grams)
		out << info;
	out << "*********************************************" << std::endl;
	out << "ORIGINAL:       " << rhs.original.toAscii().constData() << std::endl;
	out << "QUERY:          " << rhs.query.toAscii().constData() << std::endl;
	out << "RESULT:         " << rhs.result.toAscii().constData() << std::endl;
	out << "editdistance:   " << rhs.editdistance << std::endl;
	out << "FOUND:          " << ((rhs.query == rhs.result)? "true" : "FALSE") << std::endl;
	
	// in one line for grepping
	out << "@@ ORIG:|" << rhs.original.toAscii().constData() << 
		   "| - ENCQUERY:|" << rhs.encQuery.toAscii().constData() << 
	       "| - QUERY:|" << rhs.query.toAscii().constData() << 
	       "| - RESULT:|" << rhs.result.toAscii().constData() << "|" << std::endl;
	
	out << "*********************************************" << std::endl;
	
	return out;
}

} // namespace Distiller

