#include <core/precompiled.h>

#include <tagdistiller/EditDistance.h>
#include <tagdistiller/SimpleString.h>

#include "DictionaryDefines.h"
#include "DictionaryDB.h"
#include "DictionaryDeepDB.h"
#include "ThreadedSearchStrategy.h"
#include "SimpleSearchStrategy.h"
#include "DebugInfo.h"
#include "Private.h"

namespace Distiller
{

namespace DictionaryImpl
{

Private::Private(quint32 gramSize) : 
	db_(0),
	searchStrategy_(0),
#ifdef DICTIONARY_WITH_PROFILER
	profiler(),
#endif
	dictFilename_(),
	gramSize_(gramSize),
	encodedEntries_(),
	entries_(),
	gramHash_(gramSize)
{
	db_ = new DB;
	try {
		searchStrategy_ = new SearchStrategy(*this);
	}
	catch (...)
	{
		delete db_;
		throw;
	}
	IF_PROFILER(gramHash_.profiler = &profiler);
}

Private::~Private() 
{
	delete db_;
}

QString Private::encode(const QString& text) const
{
	QString rv;
	for (int i = 0; i < text.size(); i++) {
		QChar ch = text[i];
		if (ch.isLetterOrNumber() || ch.isSpace())
			rv += ch.toLower();
	}
	return rv;
}

uint Private::calcMaxTypos(const QString &text) const
{
	QString encText = encode(text);
	return qMin<uint>(gramHash_.maxGramSize(), 
		encText.size() / Dictionary::charsPerError_);
}

bool Private::save()
{
	return db_->save(*this);
}

bool Private::load()
{
	return db_->load(*this);
}

void Private::clear()
{
	dictFilename_.clear();
	encodedEntries_.clear();
	entries_.clear();
	gramHash_.clear();
}

QString Private::find(const QString& needle,
					  Dictionary::DebugInfo* debugInfo) const
{
	return searchStrategy_->search(needle, debugInfo);
}

} // namespace DictionaryImpl

} // namespace Distiller

