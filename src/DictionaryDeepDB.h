#ifndef DISTILLER_DICTIONARYIMPL_DICTIONARYDEEPDB_H
#define DISTILLER_DICTIONARYIMPL_DICTIONARYDEEPDB_H

#pragma once

#include <QString>
#include <QIODevice>
#include <QFile>
#include <QDataStream>

#include "DictionaryDefines.h"
#include "GramNode.h"
#include "AbstractDB.h"
#include "Private.h"

namespace Distiller
{

namespace DictionaryImpl
{

/**
 * Manages loading from and writing the whole Dictionary to disk.
 * DictionaryDB does not implement lazy loading of the Containers.
 * Instead the whole dictionary is loaded at once (and thus slower).
 */
template <typename ThreadPolicy = NoThreadPolicy>
class DictionaryDeepDB :
	public AbstractDB<typename GramNode<ThreadPolicy>::Container>
{

	// Default filename extension for deep saving the Dictionary.
	static const QString fullDBExtension_;
	
	QFile* file_;
	
	QDataStream* stream_;
	
	bool open(const QString& dbname, QIODevice::OpenMode mode);
	
	void close();
	
	void saveFileFormat();
	
	bool checkFileFormat();
	
	void saveMembers(const Private& d);
	
	void loadMembers(Private& d);
	
public:

	static const quint16 magicByte_ = 0xFFE2;

	static const quint16 version_ = 0x0001;

	DictionaryDeepDB();
	
	~DictionaryDeepDB();
	
	virtual void load(const typename GramNode<ThreadPolicy>::Container* keylist)
		{ }
	
	virtual bool load(Private& d);
	
	virtual bool save(const Private& d);
	
};

template <typename ThreadPolicy>
const QString DictionaryDeepDB<ThreadPolicy>::fullDBExtension_ = ".fulldb";

template <typename ThreadPolicy>
DictionaryDeepDB<ThreadPolicy>::DictionaryDeepDB() :
	file_(0),
	stream_(0)
{ }

template <typename ThreadPolicy>
DictionaryDeepDB<ThreadPolicy>::~DictionaryDeepDB()
{
	close();
}

template <typename ThreadPolicy>
bool DictionaryDeepDB<ThreadPolicy>::open(const QString& dbname,
                                          QIODevice::OpenMode mode)
{
	close();
	file_ = new QFile(dbname + fullDBExtension_);
	if (file_->open(mode) == false) {
		delete file_;
		file_ = 0;
		return false;
	}
	try {
		stream_ = new QDataStream(file_);
	} catch (...) {
		file_->close();
		delete file_;
		file_ = 0;
	}
	return true;
}

template <typename ThreadPolicy>
void DictionaryDeepDB<ThreadPolicy>::close()
{
	if (stream_ != 0) {
		delete stream_;
		stream_ = 0;
	}
	if (file_ != 0) {
		file_->close();
		delete file_;
		file_ = 0;
	}
}

template <typename ThreadPolicy>
void DictionaryDeepDB<ThreadPolicy>::saveFileFormat()
{
	Q_ASSERT(stream_ != 0);
	Q_ASSERT(file_ != 0);

	// Write file format and version.
	*stream_ << magicByte_;
	*stream_ << version_;
}
	
template <typename ThreadPolicy>
bool DictionaryDeepDB<ThreadPolicy>::checkFileFormat()
{
	Q_ASSERT(stream_ != 0);
	Q_ASSERT(file_ != 0);

	// Check file format and version.
	quint16 magic_byte;
	quint16 version;
	
	*stream_ >> magic_byte >> version;
	if (magic_byte != magicByte_) return false;
	if (version != version_) return false;
	return true;
}

template <typename ThreadPolicy>
void DictionaryDeepDB<ThreadPolicy>::saveMembers(const Private& d)
{
	Q_ASSERT(stream_ != 0);
	Q_ASSERT(file_ != 0);

	// Write data to stream.
	*stream_ << d.gramSize_;
	*stream_ << d.encodedEntries_;
	*stream_ << d.entries_;
	*stream_ << d.bitencodedEntries_;
	*stream_ << d.gramHash_;
}

template <typename ThreadPolicy>
void DictionaryDeepDB<ThreadPolicy>::loadMembers(Private &d)
{
	Q_ASSERT(stream_ != 0);
	Q_ASSERT(file_ != 0);

	// Load members.
	*stream_ >> d.gramSize_;
	IF_PROFILER(d.profiler.timer.start());
	*stream_ >> d.encodedEntries_;
	*stream_ >> d.entries_;
	*stream_ >> d.bitencodedEntries_;
	IF_PROFILER(d.profiler.loadstringarray_time =
		d.profiler.timer.elapsed());
	*stream_ >> d.gramHash_;
}

template <typename ThreadPolicy>
bool DictionaryDeepDB<ThreadPolicy>::load(Private& d)
{
	if (open(d.dictFilename_, QIODevice::ReadOnly) == false)
		return false;
	if (checkFileFormat() == false)
		return false;
	loadMembers(d);
	close();
	return true;
}

template <typename ThreadPolicy>
bool DictionaryDeepDB<ThreadPolicy>::save(const Private& d)
{
	if (open(QIODevice::WriteOnly) == false)
		return false;
	saveFileFormat();
	saveMembers(d);
	close();
	return true;
}

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

