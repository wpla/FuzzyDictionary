#ifndef DISTILLER_DICTIONARYIMPL_EXCEPTION_H
#define DISTILLER_DICTIONARYIMPL_EXCEPTION_H

#pragma once

namespace Distiller
{

namespace DictionaryImpl
{

class Exception
{

public:

	enum Type { OutOfIds, TooManyEntries };

private:

	Type type_;
	
public:

	Exception(Type type);
	
	~Exception();
	
	Type type();
	
	const char* toString();

};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

