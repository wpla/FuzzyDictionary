#ifndef DISTILLER_DICTIONARYIMPL_DICTIONARYDB_H
#define DISTILLER_DICTIONARYIMPL_DICTIONARYDB_H

#pragma once

#include <QString>
#include <QFile>
#include <QIODevice>
#include <QDataStream>
#include <QHash>

#include "DictionaryDefines.h"
#include "AbstractDB.h"
#include "GramNode.h"
#include "KeyList.h"
#include "Profiler.h"
#include "Private.h"

namespace Distiller
{

namespace DictionaryImpl
{

/**
 * Manages loading from and writing a Dictionary to disk.
 * DictionaryDB implements lazy loading of the Containers.
 */
template<typename ThreadPolicy = NoThreadPolicy>
class DictionaryDB : 
	public AbstractDB<typename GramNode<ThreadPolicy>::Container>,
	protected ThreadPolicy
{

	mutable QFile* dbfile_;

	mutable QDataStream* dbstream_;
	
	mutable QFile* containerFile_;
	
	QHash<typename GramNode<ThreadPolicy>::Container::IdType, 
		quint64> containerPos_;
	
	QString dbname_;
	
	QString containerName_;
	
	void deleteMembers() const;
	
	quint64 saveGramNode(quint64 pos, const GramNode<ThreadPolicy>& node);
	
	bool saveContainers(const Private& d);
	
	// Default filename extension for db file.
	static const QString dbExtension_;
	
	// Default filename extension for keylist file.
	static const QString containerExtension_;
	
public:

	static const quint16 magicByte_ = 0xFEEF;

	static const quint16 version_ = 0x0001;

	DictionaryDB();
	
	~DictionaryDB();
	
	bool open(const QString& dbname, QIODevice::OpenMode mode);
	
	bool reopen(QIODevice::OpenMode mode) const;
	
	/**
	 * Loads indiviual containers from disk.
	 */
	virtual void load(const typename GramNode<ThreadPolicy>::Container*
		container);
	
	virtual bool load(Private& d);
	
	virtual bool save(const Private& d);
	
	void close() const;
	
};

template<typename ThreadPolicy>
const QString DictionaryDB<ThreadPolicy>::dbExtension_ = ".idb";

template<typename ThreadPolicy>
const QString DictionaryDB<ThreadPolicy>::containerExtension_ = ".kdb";

template<typename ThreadPolicy>
DictionaryDB<ThreadPolicy>::DictionaryDB() : 
	AbstractDB<typename GramNode<ThreadPolicy>::Container>(),
	ThreadPolicy(),
	dbfile_(0),
	dbstream_(0),
	containerFile_(0),
	containerPos_(),
	dbname_(),
	containerName_()
{ }

template<typename ThreadPolicy>
DictionaryDB<ThreadPolicy>::~DictionaryDB()
{
	close();
}

template<typename ThreadPolicy>
void DictionaryDB<ThreadPolicy>::deleteMembers() const
{
	if (dbstream_ != 0) {
		delete dbstream_;
		dbstream_ = 0;
	}
	if (dbfile_ != 0) {
		dbfile_->close();
		delete dbfile_;
		dbfile_ = 0;
	}
	if (containerFile_ != 0) {
		containerFile_->close();
		delete containerFile_;
		containerFile_ = 0;
	}
}

template<typename ThreadPolicy>
bool DictionaryDB<ThreadPolicy>::open(const QString& dbname,
									  QIODevice::OpenMode mode)
{
	close();
	
	dbname_ = dbname + dbExtension_;
	containerName_ = dbname + containerExtension_;
	
	dbfile_ = new QFile(dbname_);
	containerFile_ = new QFile(containerName_);
	dbstream_ = new QDataStream(dbfile_);
	
	if (!dbfile_->open(mode))
		return false;
	if (!containerFile_->open(mode))
		return false;
	
	dbstream_ = new QDataStream(dbfile_);
	
	return true;
}

template<typename ThreadPolicy>
bool DictionaryDB<ThreadPolicy>::reopen(QIODevice::OpenMode mode) const
{
	close();
	
	dbfile_ = new QFile(dbname_);
	containerFile_ = new QFile(containerName_);
	dbstream_ = new QDataStream(dbfile_);
	
	if (!dbfile_->open(mode))
		return false;
	if (!containerFile_->open(mode))
		return false;
	
	dbstream_ = new QDataStream(dbfile_);
	
	return true;
}

template<typename ThreadPolicy>
void DictionaryDB<ThreadPolicy>::close() const
{
	deleteMembers();
}

template<typename ThreadPolicy>
bool DictionaryDB<ThreadPolicy>::load(Private& d)
{
	if (!open(d.dictFilename_, QIODevice::ReadOnly))
		return false;

	Q_ASSERT(dbfile_ != 0);
	Q_ASSERT(dbstream_ != 0);
	Q_ASSERT(dbfile_->isOpen());
	Q_ASSERT(dbfile_->isReadable());
	
	dbfile_->seek(0);
	
	// Check file format and version.
	quint16 magic_byte;
	quint16 version;
	
	*dbstream_ >> magic_byte >> version;
	if (magic_byte != magicByte_) return false;
	if (version != version_) return false;

	// Load members.
	*dbstream_ >> d.gramSize_;
	IF_PROFILER(d.profiler.timer.start());
	*dbstream_ >> d.encodedEntries_;
	*dbstream_ >> d.entries_;
	IF_PROFILER(d.profiler.loadstringarray_time = 
		d.profiler.timer.elapsed());
	*dbstream_ >> d.bitencodedEntries_;
	d.gramHash_.loadShallow(*dbstream_, this);
	IF_PROFILER(d.profiler.timer.start());
	*dbstream_ >> containerPos_;
	IF_PROFILER(d.profiler.loadkeylistpos_time = 
		d.profiler.timer.elapsed());
	
	close();
	return true;
}

template<typename ThreadPolicy>
quint64 DictionaryDB<ThreadPolicy>::saveGramNode(quint64 pos,
	const GramNode<ThreadPolicy>& node)
{
	typename GramNode<ThreadPolicy>::const_iterator j;
	for (j = node.constBegin();
		 j != node.end(); j++)
	{
		 const typename GramNode<ThreadPolicy>::Container& container = **j;
		 const typename GramNode<ThreadPolicy>::Container::IdType id =
			container.id();
		 QByteArray ar;
		 QDataStream stream(&ar, QIODevice::WriteOnly);
		 
		 containerPos_[id] = pos;           // Save Position
		 container.saveDeep(stream);
		 pos += containerFile_->write(ar);  // Write into File
	}
	return pos;
}

template<typename ThreadPolicy>
bool DictionaryDB<ThreadPolicy>::saveContainers(const Private& d)
{
	Q_ASSERT(containerFile_ != 0);
	Q_ASSERT(containerFile_->isOpen());
	Q_ASSERT(containerFile_->isWritable());
	
	containerFile_->seek(0);
	containerPos_.clear();
	
	const QHash<QString, GramNode<ThreadPolicy> >& distinctiveContainers = 
		d.gramHash_.distinctiveContainers_;
		
	quint64 pos = 0;
    typename QHash<QString, GramNode<ThreadPolicy> >::const_iterator i;
	for (i = distinctiveContainers.constBegin();
		 i != distinctiveContainers.end();
	 	 i++)
	{
		pos = saveGramNode(pos, *i);
	}
	return true;
}

template<typename ThreadPolicy>
bool DictionaryDB<ThreadPolicy>::save(const Private& d)
{
	if (!open(d.dictFilename_, QIODevice::WriteOnly))
		return false;

	Q_ASSERT(dbfile_ != 0);
	Q_ASSERT(dbstream_ != 0);
	Q_ASSERT(dbfile_->isOpen());
	Q_ASSERT(dbfile_->isWritable());
	
	// Save containers
	saveContainers(d);
	
	// Write file format and version.
	dbfile_->seek(0);
	*dbstream_ << magicByte_;
	*dbstream_ << version_;
	
	// Write data to stream.
	*dbstream_ << d.gramSize_;
	*dbstream_ << d.encodedEntries_;
	*dbstream_ << d.entries_;
	*dbstream_ << d.bitencodedEntries_;
	d.gramHash_.saveShallow(*dbstream_);
	*dbstream_ << containerPos_;
	
	close();
	return true;
}

template<typename ThreadPolicy>
void DictionaryDB<ThreadPolicy>::load(const typename
	GramNode<ThreadPolicy>::Container* container)
{
    ThreadPolicy::lockForWrite();
	reopen(QIODevice::ReadOnly);
	
	Q_ASSERT(containerFile_ != 0);
	Q_ASSERT(containerFile_->isOpen());
	Q_ASSERT(containerFile_->isReadable());

	QDataStream containerStream(containerFile_);
	containerFile_->seek(containerPos_[container->id()]);
	// All members of container are mutable so this is valid!
	container->loadDeep(containerStream);
	
	close();
    ThreadPolicy::unlock();
}

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

