#ifndef DISTILLER_DICTIONARYIMPL_ABSTRACTDB_H
#define DISTILLER_DICTIONARYIMPL_ABSTRACTDB_H

#pragma once

namespace Distiller
{

namespace DictionaryImpl
{

class Private;

/**
 * Serializing and deserializing the dictionary to and 
 * from a data stream is factored out into a seperate class.
 * 
 * Abstract base class for implementing lazy loading
 * the Containers.
 */
template<typename Container>
class AbstractDB
{
public:
	virtual void load(const Container* container) = 0;
	virtual bool load(Private& d) = 0;
	virtual bool save(const Private& d) = 0;
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

