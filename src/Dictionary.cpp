#include <core/precompiled.h>

#include <iostream>

#include "DictionaryDefines.h"
#include "Profiler.h"
#include "Private.h"
#include "Builder.h"
#include "DictionaryDB.h"
#include "Dictionary.h"

namespace Distiller {

Dictionary::Dictionary() :
	d_(new DictionaryImpl::Private)
{ }

Dictionary::~Dictionary()
{
	delete d_;
}

QString Dictionary::find(const QString& needle) const
{
	return d_->find(needle);
}

QString Dictionary::findVerbose(const QString& needle, DebugInfo* debugInfo) const
{
#ifdef DICTIONARY_WITH_DEBUGINFO
	return d_->find(needle, debugInfo);
#else
	return d_->find(needle);
#endif
}

bool Dictionary::load(const QString& dictionary)
{
	d_->dictFilename_ = dictionary;
	if (d_->load() == false)
		build(dictionary);
	return true;
}

bool Dictionary::build(const QString& dictionary)
{
	d_->dictFilename_ = dictionary;
	DictionaryImpl::Builder builder(*this);
	return builder.buildFrom(dictionary);
}

bool Dictionary::save()
{
	return d_->save();
}

void Dictionary::clear()
{
	d_->clear();
}

QString Dictionary::encode(const QString& text) const
{
	return d_->encode(text);
}

uint Dictionary::calcMaxTypos(const QString &text) const
{
	return d_->calcMaxTypos(text);
}

const DictionaryImpl::Profiler Dictionary::profiler() const
{
#ifdef DICTIONARY_WITH_PROFILER
	return d_->profiler;
#else
	return DictionaryImpl::Profiler();
#endif
}

void Dictionary::resetProfiler() const
{
#ifdef DICTIONARY_WITH_PROFILER
	d_->profiler.reset();
#endif
}

} // namespace Distiller
