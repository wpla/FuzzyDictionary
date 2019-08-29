#ifndef DISTILLER_DICTIONARYIMPL_GRAMHASH_H
#define DISTILLER_DICTIONARYIMPL_GRAMHASH_H

#pragma once

#include "GramNode.h"

namespace Distiller
{

namespace DictionaryImpl
{

template<typename ThreadPolicy>
class DictionaryDB;

/**
 * GramHash stores relations between grams (substrings of
 * a string) and list of keys. For every gram we get a list 
 * of keys, where the gram occours. We store this list of keys
 * in a Container, that can be a Vector of keys or a
 * Burkhard-Keller-Tree.
 * 
 * What makes this data structure a bit complicated is the
 * need to get the proper keys for all suffixes of the stores
 * grams down to a length of minGramSize as well.
 *
 * Example:
 *
 * The gram "foob" is part of the word "afoobar".
 * "afoobar" itself is a encoded string of the dictionary
 * entry "A Foo Bar". It is the 11th entry in our dictionary.
 *
 * So we want to store the relation "foob" --> 11.
 * 
 * Since we need to look up suffixes of the grams as well we
 * also have to store
 *
 *     "foo" --> 11
 *     "fo"  --> 11
 *
 * (Assuming that minGramSize equals 2).
 *
 * We don't need to store copies of the Container, which would be
 * a waste of space. We just save references to the Container in 
 * the suffixes. So our example would look like this:
 *
 *    GramNode    List of          Container 
 *                PtrToContainer     (usually of type KeyList)
 * -----------------------------------------------------------
 *
 *     "foob" --->[0xFE]
 *                   |
 *                   +-----------> [7, 10, 13, ...]
 *
 *     "fooc" --->[0xFD]
 *                   | 
 *                   +-----------> [8, 12, 4, 6, ...]
 *                 
 * "foo" is the suffix of both the grams "foob" and "fooc"
 * so we need a list of pointers to both Containers.
 *
 *     "foo" ---->[0xFE, 0xFD]
 *
 * The same applies to "fo", which is also a suffix of "foob"
 * and "fooc". 
 *
 *     "fo" ----->[0xFE, 0xFD] 
 */
template <typename ThreadPolicy = NoThreadPolicy>
class GramHash :
	protected QHash<QString, GramNode<ThreadPolicy> >
{

public:
	
	typedef GramNode<ThreadPolicy> Value;

private:

	typedef QHash<QString, Value> Super;

	/**
	 * distinctiveContainers_ is a helper member. We need it to
	 * correctly serialize and deserialize the GramHash.
	 *
	 * The hash itself stores GramNodes which points to Containers.
	 * Suffixes of Grams point to the same Containers. 
	 *
	 * To serialize and deserialize the GramHash in a QDataStream
	 * we're interested only in the distinctive Containers, so we have
	 * to save links to them also. Otherwise we had to compute
	 * which Containers had been serialized already, which is not as
	 * easy as just storing pointers to them.
	 */
	QHash<QString, Value> distinctiveContainers_;
	
	quint32 gramCount_;

	quint32 minGramSize_;

	quint32 maxGramSize_;

	/**
	 * Inserts a distinctive PtrToContainer.
	 *
	 * \note The Container is assumed to be distinctive,  
	 * the method doesn't check that. It just adds it to
	 * distinctiveContainers_.
	 */
	void insertDistinctivePtrToContainer(const QString& gram, 
		const typename Value::PtrToContainer& p);
		
	/**
	 * Inserts a given PtrToContainer for a gram and all its
	 * suffixes down to minGramSize.
	 *
	 * Constructs proper GramNodes where necessary.
	 *
	 * \note This method doesn't append p to distinctiveKeySets_.
	 */
	void insertPtrToContainer(const QString& gram,
							  const typename Value::PtrToContainer& p);
						 
	/**
	 * Inserts every PtrToContainer of node into the data
	 * structure.
	 *
	 * Node itself isn't inserted but the method iterates
	 * over all PtrToContainer and inserts them using
	 * insertPtrToContainer().
	 *
	 * \note Assumes that every PtrToContainer points to a 
	 * distinctive Container.
	 */
	void insertNode(const QString& gram,
					const Value& node);

public:

    typedef typename Super::iterator iterator;
	
    typedef typename Super::const_iterator const_iterator;

	IF_PROFILER(mutable Profiler* profiler);

	static const quint32 defaultMinGramSize = 2;

	static const quint32 defaultMaxGramSize = 4;

	GramHash(quint32 maxGramSize = defaultMaxGramSize,
			 quint32 minGramSize = defaultMinGramSize);

	/**
	 * Inserts a (key, distance) tuple for gram and all of it's 
	 * suffixes down to a length of minGramSize.
	 * 
	 * Constructs Containers and proper GramNodes.
	 *
	 * You have to recalculate all of the GramNode's valueCounter_
	 * afterwards by runnung reCountAllNodes().
	 */
	void insert(const QString& gram, KeyType key);
	
	quint32 minGramSize() const;
	
	quint32 maxGramSize() const;
		
	bool contains(const QString& gram) const;
		
	/**
	 * Recalculates the valueCounter_ of all nodes.
	 */
	void reCountAllNodes();

	QDataStream& loadDeep(QDataStream& in);

	friend QDataStream& operator >> (QDataStream& in, 
									 GramHash<ThreadPolicy>& node)
    { return node.loadDeep(in); }

	QDataStream& saveDeep(QDataStream& out) const;
	
	friend QDataStream& operator << (QDataStream& out, 
                                     const GramHash<ThreadPolicy>& hash)
    { return hash.saveDeep(out); }

	void loadShallow(QDataStream& in,
					 AbstractDB<typename Value::Container>* db);
	
	void saveShallow(QDataStream& out) const;
	
	const_iterator constBegin() const;
	
	const_iterator begin() const;
	
	const_iterator constEnd() const;	
	
	const_iterator end() const;
	
	const Value operator[] (const QString& gram) const;
	
	Value& operator[] (const QString& gram);
	
	iterator begin();
	
	iterator end();
	
	void clear();
		
    template<typename>
	friend class DictionaryDB;
};

template <typename ThreadPolicy>
GramHash<ThreadPolicy>::GramHash(quint32 maxGramSize,
								 quint32 minGramSize) : 
	QHash<QString, Value>(),
	distinctiveContainers_(),
	gramCount_(0),
	minGramSize_(minGramSize),
	maxGramSize_(maxGramSize)
#ifdef DICTIONARY_WITH_PROFILER
	, profiler(0)
#endif
{ }

template <typename ThreadPolicy>
quint32 GramHash<ThreadPolicy>::minGramSize() const
{
	return minGramSize_;
}

template <typename ThreadPolicy>
quint32 GramHash<ThreadPolicy>::maxGramSize() const
{
	return maxGramSize_;
}
	
template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::reCountAllNodes()
{
	for (iterator i = begin(); i != end(); i++)
		i->reCount();
	for (iterator j = distinctiveContainers_.begin(); 
                j != distinctiveContainers_.end(); j++)
		j->reCount();
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::insertDistinctivePtrToContainer(
	const QString& gram, 
	const typename Value::PtrToContainer& p
)
{
	Q_ASSERT((quint32)gram.size() >= minGramSize_ &&
			 (quint32)gram.size() <= maxGramSize_);
	Value node;
	node.append(p);
	distinctiveContainers_.insert(gram, node);
}

template <typename ThreadPolicy>
bool GramHash<ThreadPolicy>::contains(const QString& gram) const
{
	return Super::contains(gram);
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::insertPtrToContainer(const QString& gram,
	const typename Value::PtrToContainer& p)
{	
	Q_ASSERT((quint32)gram.size() >= minGramSize_ &&
			 (quint32)gram.size() <= maxGramSize_);
			 
	QString subgram = gram;
	quint32 size = gram.size();
	while (size >= minGramSize_) {
		if (contains(subgram) == false) {
			Value node;
			node.append(p);
			Super::insert(subgram, node);
			gramCount_ += 1;
		}
		else
			(*this)[subgram].append(p);
		size--;
		subgram = gram.left(size);
	}
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::insert(const QString& gram, 
									KeyType key)
{
	quint32 size = qMin<quint32>(maxGramSize_, gram.size());
	QString subgram = gram.left(size);
	if (contains(subgram)) {
		/**
		 * Node already there, just insert (key, distance) tuple.
		 *
		 * Since this insertion expands a Container directly,
		 * all valueCount_ members of GramNodes that point to this
		 * Container get out of sync!
		 *
		 * You have to run reCountAllNodes() to recalculate the 
		 * valueCount_ of all GramNodes!
		 */ 
		(*this)[subgram].append(key);
	}
	else {
        typename Value::PtrToContainer p(new typename Value::Container(true));
		p->append(key);
		// We have a newly created Container here, so
		// add it to distinctiveContainer_.
		insertDistinctivePtrToContainer(subgram, p);
		// Finally append PtrToContainer to list.
		insertPtrToContainer(subgram, p);
	}
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::insertNode(const QString& gram, 
										const Value& node)
{
    for (typename Value::const_iterator i = node.constBegin();
		 i != node.constEnd(); i++)
	{
		insertDistinctivePtrToContainer(gram, *i);
		insertPtrToContainer(gram, *i);
	}
}

template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::const_iterator
GramHash<ThreadPolicy>::constBegin() const
{
	return Super::constBegin();
}
	
template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::const_iterator
GramHash<ThreadPolicy>::begin() const
{
	return Super::begin();
}
	
template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::const_iterator
GramHash<ThreadPolicy>::constEnd() const
{
	return Super::constEnd();
}
	
template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::const_iterator
GramHash<ThreadPolicy>::end() const
{
	return Super::end();
}
	
template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::iterator
GramHash<ThreadPolicy>::begin()
{
	return Super::begin();
}
	
template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::iterator
GramHash<ThreadPolicy>::end()
{
	return Super::end();
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::clear()
{
	Super::clear();
}

template <typename ThreadPolicy>
const typename GramHash<ThreadPolicy>::Value 
GramHash<ThreadPolicy>::operator[] (const QString& gram) const
{
	return Super::operator[](gram);
}

template <typename ThreadPolicy>
typename GramHash<ThreadPolicy>::Value&
GramHash<ThreadPolicy>::operator[] (const QString& gram)
{
	return Super::operator[](gram);
}

template <typename ThreadPolicy>
QDataStream& GramHash<ThreadPolicy>::loadDeep(QDataStream& in)
{
	quint32 size;
	in >> size;
	
	while (size-- > 0) {
		QString gram;
		Value node;
		in >> gram;
		
		IF_PROFILER(node.profiler = profiler);
		node.loadDeep(in);
		
		IF_PROFILER(profiler->timer.restart());
		insertNode(gram, node);
		IF_PROFILER(profiler->insertnode_time += profiler->timer.elapsed());
	}
	IF_PROFILER(profiler->timer.restart());
	reCountAllNodes();
	IF_PROFILER(profiler->recount_time += profiler->timer.elapsed());
	
	return in;
}

template <typename ThreadPolicy>
QDataStream& GramHash<ThreadPolicy>::saveDeep(QDataStream& out) const
{
	out << (quint32)distinctiveContainers_.size();
	
    for (typename Value::const_iterator i =
		 distinctiveContainers_.constBegin();
		 i != distinctiveContainers_.constEnd(); i++)
	{
		out << i.key();
		i->saveDeep(out);
	}
	
	return out;
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::loadShallow(QDataStream& in,
	AbstractDB<typename Value::Container>* db)
{
	Q_ASSERT(db != 0);
	quint32 size;
	in >> size;

	while (size-- > 0) {
		QString gram;
		Value node;
		IF_PROFILER(node.profiler = profiler);
		in >> gram;
		
		node.loadShallow(in, db);
		
		IF_PROFILER(profiler->timer.restart());
		insertNode(gram, node);
		IF_PROFILER(profiler->insertnode_time += profiler->timer.elapsed());
	}
	IF_PROFILER(profiler->timer.restart());
	reCountAllNodes();
	IF_PROFILER(profiler->recount_time += profiler->timer.elapsed());
}

template <typename ThreadPolicy>
void GramHash<ThreadPolicy>::saveShallow(QDataStream& out) const
{
	out << (quint32)distinctiveContainers_.size();
	
	for (const_iterator i = distinctiveContainers_.constBegin();
		 i != distinctiveContainers_.constEnd(); i++)
	{
		out << i.key();
		i.value().saveShallow(out);
	}
}

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

