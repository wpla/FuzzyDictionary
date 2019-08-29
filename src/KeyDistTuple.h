#ifndef DISTILLER_DICTIONARYIMPL_KEYDISTTUPLE_H
#define DISTILLER_DICTIONARYIMPL_KEYDISTTUPLE_H

#pragma once

namespace Distiller
{

namespace DictionaryImpl
{

typedef quint32 KeyType;

static const KeyType KEYTYPE_MAX = 0xFFFFFFFF;

typedef quint8 DistType;

static const DistType DISTTYPE_MAX = 0xFF;

/**
 * Stores a (key, distance) tuple.
 *
 * Provides comparison operators, to compare the
 * distance of two tuples.
 */
class KeyDistTuple : public QPair<KeyType, DistType>
{

public:

	static const KeyType invalidKey;

	static const DistType invalidDistance;

	KeyDistTuple();
	
	~KeyDistTuple();
	
	inline KeyType key() const
		{ return first; }
		
	inline void setKey(KeyType key)
		{ first = key; }
		
	inline DistType distance() const
		{ return second; }
	
	inline void setDistance(DistType dist)
		{ second = dist; }
		
	inline void set(KeyType key, DistType dist)
		{ first = key; second = dist; }
		
	inline bool keyIsValid() const 
		{ return first != invalidKey; }
		
	inline bool distValid() const
		{ return second != invalidDistance; }
	
	bool operator ==(const KeyDistTuple& rhs) const
		{ return second == rhs.second; }
		
	bool operator !=(const KeyDistTuple& rhs) const
		{ return second != rhs.second; }
		
	bool operator < (const KeyDistTuple& rhs) const
		{ return second < rhs.second; }
		
	bool operator <= (const KeyDistTuple& rhs) const
		{ return second <= rhs.second; }
		
	bool operator > (const KeyDistTuple& rhs) const
		{ return second > rhs.second; }
		
	bool operator >= (const KeyDistTuple& rhs) const
		{ return second > rhs.second; }
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

