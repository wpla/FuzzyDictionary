#ifndef DISTILLER_DICTIONARYIMPL_KEYLIST_H
#define DISTILLER_DICTIONARYIMPL_KEYLIST_H

#pragma once

#include <QSharedPointer>
#include <QReadWriteLock>

#include <tagdistiller/EditDistance.h>
#include <tagdistiller/SimpleString.h>

#include "Dictionary.h"
#include "DebugInfo.h"
#include "ThreadPolicy.h"
#include "NoThreadPolicy.h"
#include "AbstractDB.h"
#include "DictException.h"
#include "BitDistance.h"
#include "SearchInfo.h"
#include "KeyDistTuple.h"

namespace Distiller
{

class StringArray;

namespace DictionaryImpl
{

template<typename ContainerType>
class AbstractDB;

/**
 * KeyList is a list of keys whichs points to entries in 
 * a string list.
 *
 * Example: Let's consider this string list:
 *
 *      01 "Hello"
 *      02 "World"
 *      03 "Earth"
 *      04 "Sun"
 *      05 "Moon"
 *
 * A possible KeyList could be [1, 3, 4].
 *
 * KeyList offers methods to serialize and deserialize itself
 * to a QDataStream and a method to find the best match for
 * a (imperfect) search string.
 *
 * KeyList implements lazy loading and thread policies.
 */
template<typename ThreadPolicy = NoThreadPolicy>
class KeyList : protected ThreadPolicy
{

public:

	typedef quint32 IdType;
	
	static const IdType IDTYPE_MAX = 0xFFFFFFFF;
	
	static const IdType UndefinedId = 0;
	
	typedef QVector<KeyType>::iterator iterator;
	
	typedef QVector<KeyType>::const_iterator const_iterator;
	
	typedef QSharedPointer<KeyList<ThreadPolicy> > Ptr;

	typedef QVector<Ptr> ListOfPtrToContainer;

private:

	/**
	 * Each KeyList has a unique Id. We use a static counter
	 * to generate new Ids.
	 */
	static IdType idCounter_;
	
	/**
	 * Since we implement lazy loading all the members
	 * of const KeyList must be mutable.
	 */

	/**
	 * Points to a instance of AbstractDB.
	 * We don't own the DB.
	 */
	mutable AbstractDB<KeyList<ThreadPolicy> >* db_;
	
	/**
	 * The Id of this KeyList.
	 */
	mutable IdType id_;
	
	/**
	 * Is true if the KeyList is completely loaded.
	 */
	mutable bool loaded_;
	
	/**
	 * The list itself.
	 */
	mutable QVector<KeyType> list_;
	
	/**
	 * The size of the list.
	 */
	mutable int size_;
	
	/**
	 * Loads the KeyList from disc.
	 */
	void load() const;
	
public:

	KeyList(bool loaded = false);
	
	KeyList(quint16 id);
	
	/**
	 * Generates a new id.
	 * Throws Exception(Exception::OutOfIds) if we're out of Ids.
	 */
	static IdType newId();
	
	/**
	 * Returns the Id of this KeyList.
	 */
	IdType id() const;
		
	/**
	 * Returns true if the KeyList is fully loaded.
	 */
	bool isLoaded() const;
		
	/**
	 * Finds the best approximate match for a 
	 * needle. You must also provide the string list
	 * where the keys refer to.
	 */
	KeyDistTuple find(const SearchInfo& searchInfo);
			   
	/**
	 * Append a key to the list.
	 */
	void append(KeyType v);
		
	/**
	 * Returns the number of keys in the list.
	 */
	int size() const;
		
	/**
	 * Saves the whole KeyList in a QDataStream.
	 */
	QDataStream& saveDeep(QDataStream& out) const;
	
	friend QDataStream& operator << (QDataStream& out,
		const KeyList<ThreadPolicy>& keyList)
		{ return keyList.saveDeep(out); }

	/**
	 * Load the whole KeyList from a QDataStream.
	 */
	QDataStream& loadDeep(QDataStream& in) const;
									 
	friend QDataStream& operator >> (QDataStream& in, 
		const KeyList<ThreadPolicy>& keyList)
                { return keyList.loadDeep(in); }

	/**
	 * Saves just the id and the size to QDataStream.
	 */
	void saveShallow(QDataStream& out) const;
	
	/**
	 * Load just the id and the size from QDataStream.
	 */
	void loadShallow(QDataStream& in,
		AbstractDB<KeyList<ThreadPolicy> >* db);
	
    template<typename>
	friend class DictionaryDB;
};

template<typename ThreadPolicy>
typename KeyList<ThreadPolicy>::IdType KeyList<ThreadPolicy>::idCounter_ = 0;

template<typename ThreadPolicy>
KeyList<ThreadPolicy>::KeyList(bool loaded) : 
	ThreadPolicy(),
	db_(0),
	id_(newId()),
	loaded_(loaded),
	list_(),
	size_(0)
{ }

template<typename ThreadPolicy>
KeyList<ThreadPolicy>::KeyList(quint16 id) :
	id_(id),
	db_(0),
	loaded_(false),
	list_(),
	size_(0)
{
	if (id >= idCounter_)
		idCounter_ = id;
}

template<typename ThreadPolicy>
typename KeyList<ThreadPolicy>::IdType
KeyList<ThreadPolicy>::newId()
{	
	if (idCounter_ == IDTYPE_MAX)
		// We're out of IDs.
		throw Exception(Exception::OutOfIds);
	return ++idCounter_; 
}

template<typename ThreadPolicy>
typename KeyList<ThreadPolicy>::IdType
KeyList<ThreadPolicy>::id() const
{
	return id_;
}
	
template<typename ThreadPolicy>
bool KeyList<ThreadPolicy>::isLoaded() const
{
	bool rv;
    ThreadPolicy::lockForRead();
	rv = loaded_;
    ThreadPolicy::unlock();
	return rv;
}
	
template<typename ThreadPolicy>
void KeyList<ThreadPolicy>::append(KeyType v)
{	
	list_.append(v);
	size_++; 
}
	
template<typename ThreadPolicy>
int KeyList<ThreadPolicy>::size() const
{
	return (isLoaded())? list_.size() : size_;
}
	
template<typename ThreadPolicy>
QDataStream& KeyList<ThreadPolicy>::saveDeep(QDataStream& out) const
{
	out << id_;
	out << (quint32)list_.size();
	out << list_;
	return out;
}

/**
 * All members are mutable so this is valid.
 */
template<typename ThreadPolicy>
QDataStream& KeyList<ThreadPolicy>::loadDeep(QDataStream& in) const
{
	in >> id_;
	quint32 size;
	in >> size;
	size_ = size;
	in >> list_;
	loaded_ = true;
	return in;
}

template<typename ThreadPolicy>
void KeyList<ThreadPolicy>::saveShallow(QDataStream& out) const
{
	out << (quint32)id_;
	out << (quint32)size_;
}

template<typename ThreadPolicy>
void KeyList<ThreadPolicy>::loadShallow(QDataStream& in,
	AbstractDB<KeyList<ThreadPolicy> >* db)
{
	Q_ASSERT(db != 0);
	quint32 id;
	quint32 size;
	in >> id;
	in >> size;
    id_ = (typename KeyList::IdType)id;
	size_ = size;
	loaded_ = false;
	db_ = db;
}

template<typename ThreadPolicy>
void KeyList<ThreadPolicy>::load() const
{
	if (isLoaded() == true) return;
    ThreadPolicy::lockForWrite();
	Q_ASSERT(db_ != 0);
	db_->load(this);
	loaded_ = true;
    ThreadPolicy::unlock();
}

template<typename ThreadPolicy>
KeyDistTuple KeyList<ThreadPolicy>::find(const SearchInfo& searchInfo)
{
	load();
    ThreadPolicy::lockForRead();

	KeyDistTuple rv;	
	for (const_iterator i = list_.constBegin(); 
		 i != list_.constEnd(); i++)
	{
		KeyType key = *i;
		DistType dist = searchInfo.calcDistance(key);
		if (dist < rv.distance())
			rv.set(key, dist);
		if (dist == 0)
			break;
	}
    ThreadPolicy::unlock();
	return rv;
}

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

