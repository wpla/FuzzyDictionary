#ifndef DISTILLER_DICTIONARYIMPL_GRAMNODE_H
#define DISTILLER_DICTIONARYIMPL_GRAMNODE_H

#pragma once

#include "DictionaryDefines.h"
#include "ThreadPolicy.h"
#include "KeyList.h"
#include "Profiler.h"

namespace Distiller
{

namespace DictionaryImpl
{

/**
 * Represents a GramNode.
 *
 * A GramNode is a list of pointers to Containers
 * (usually KeyLists).
 *
 * \note GramNode doesn't take care of valueCount itself.
 * After inserting into or deleting element of the Containers
 * you have to call reCount() to recount the valueCounter
 * member.
 */
template<typename ThreadPolicy = NoThreadPolicy>
class GramNode : 
	protected KeyList<ThreadPolicy>::ListOfPtrToContainer
{

public:

	typedef KeyList<ThreadPolicy> Container;
	
	typedef typename Container::Ptr PtrToContainer;
	
private:

	typedef typename Container::ListOfPtrToContainer Super;

	/**
	 * Counts the number of elements in the 
	 * Containers of the list. Let's say there are three
	 * Containers in the List, which have 3, 5 and 7 items
	 * each. Then valueCount_ equals 15.
	 *
	 * Note that valueCount_ is only correct if the
	 * pointers point to distinctive Containers!
	 */
	quint32 valueCount_;
	
public:

	typedef typename Super::iterator iterator;
	
	typedef typename Super::const_iterator const_iterator;

	IF_PROFILER(mutable Profiler *profiler);

	GramNode();
	
	/**
	 * Appends a PtrToContainer to the list.
	 *
	 * \note You have to call reCount() after calling
	 * this method.
	 */
	void append(const PtrToContainer& p);
	
	/**
	 * Inserts a KeyDistTuple at the first Container.
	 *
	 * \note You have to call reCount() after calling
	 * this method.
	 */
	void append(KeyType key);
		
	quint32 valueCount() const;
			
	/**
	 * Recalculate valueCount.
	 */
	quint32 reCount();
	
	const_iterator constBegin() const;
	
	const_iterator begin() const;
	
	const_iterator constEnd() const;	
	
	const_iterator end() const;
	
	iterator begin();
	
	iterator end();
		
	QDataStream& saveDeep(QDataStream& out) const;
	
    friend QDataStream& operator << (QDataStream& out,
                                     const GramNode<ThreadPolicy>& node)
		{ return node.saveDeep(out); }

	QDataStream& loadDeep(QDataStream& in);
									 
    friend QDataStream& operator >> (QDataStream& in,
                                     GramNode<ThreadPolicy>& node)
        { return node.loadDeep(in); }

	void saveShallow(QDataStream& out) const;
	
	void loadShallow(QDataStream& out, AbstractDB<Container>* db);							
};

template<typename ThreadPolicy>
GramNode<ThreadPolicy>::GramNode() :
	valueCount_(0),
#ifdef DICTIONARY_WITH_PROFILER
	profiler(0)
#endif
{ }

template<typename ThreadPolicy>
void GramNode<ThreadPolicy>::append(const PtrToContainer& p)
{
	Super::append(p);
}

template<typename ThreadPolicy>
void GramNode<ThreadPolicy>::append(KeyType key)
{
    Q_ASSERT(Super::size() > 0);
	(*this)[0]->append(key);
}
	
template<typename ThreadPolicy>
quint32 GramNode<ThreadPolicy>::valueCount() const
{	
	return valueCount_;
}

template<typename ThreadPolicy>
quint32 GramNode<ThreadPolicy>::reCount()
{
	valueCount_ = 0;
	for (iterator i = begin(); i != end(); i++)
		valueCount_ += (*i)->size();
	return valueCount_;
}

template<typename ThreadPolicy>
void GramNode<ThreadPolicy>::saveShallow(QDataStream& out) const
{
    out << (quint32)Super::size();
	for(const_iterator i = constBegin(); i != constEnd(); i++)
		(*i)->saveShallow(out);
}

template<typename ThreadPolicy>
void GramNode<ThreadPolicy>::loadShallow(QDataStream& in, 
	AbstractDB<typename GramNode<ThreadPolicy>::Container>* db)
{
	Q_ASSERT(db != 0);
	quint32 size = 0;
	in >> size;
	while (size-- > 0) {
		IF_PROFILER(profiler->timer.restart());
		PtrToContainer p(new Container);
		IF_PROFILER(profiler->new_time += profiler->timer.elapsed());
		IF_PROFILER(profiler->timer.restart());
		p->loadShallow(in, db);
		append(p);
		IF_PROFILER(profiler->createnode_time +=
			profiler->timer.elapsed());
	}
}

template<typename ThreadPolicy>
typename GramNode<ThreadPolicy>::const_iterator
GramNode<ThreadPolicy>::constBegin() const
{
	return Super::constBegin();
}
	
template<typename ThreadPolicy>
typename GramNode<ThreadPolicy>::const_iterator
GramNode<ThreadPolicy>::begin() const
{
	return Super::begin();
}
	
template<typename ThreadPolicy>
typename GramNode<ThreadPolicy>::const_iterator
GramNode<ThreadPolicy>::constEnd() const
{
	return Super::constEnd();
}
	
template<typename ThreadPolicy>
typename GramNode<ThreadPolicy>::const_iterator
GramNode<ThreadPolicy>::end() const
{
	return Super::end();
}
	
template<typename ThreadPolicy>
typename GramNode<ThreadPolicy>::iterator
GramNode<ThreadPolicy>::begin()
{
	return Super::begin();
}
	
template<typename ThreadPolicy>
typename GramNode<ThreadPolicy>::iterator
GramNode<ThreadPolicy>::end()
{
	return Super::end();
}

template<typename ThreadPolicy>
QDataStream& GramNode<ThreadPolicy>::saveDeep(QDataStream& out) const
{
	// No need to serialize valueCounter_.
    out << (quint32)Super::size();
    typename GramNode::const_iterator i;
    for (i = constBegin(); i != constEnd(); i++)
	{
        typename GramNode::PtrToContainer p = *i;
		p->saveDeep(out);
	}
	return out;
}

template<typename ThreadPolicy>
QDataStream& GramNode<ThreadPolicy>::loadDeep(QDataStream& in)
{
	quint32 size;
	in >> size;
	while (size-- > 0) {
		IF_PROFILER(profiler->timer.restart());
        typename GramNode::PtrToContainer p(new typename GramNode::Container);
		IF_PROFILER(profiler->new_time += profiler->timer.elapsed());
		IF_PROFILER(profiler->timer.restart());
		p->loadDeep(in);
		IF_PROFILER(profiler->createnode_time += profiler->timer.elapsed());
		append(p);
	}
	return in;
}

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

