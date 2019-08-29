#include <core/precompiled.h>

#include "DictException.h"

namespace Distiller
{

namespace DictionaryImpl
{

Exception::Exception(Type type) : 
	type_(type)
{ }

Exception::~Exception()
{ }

Exception::Type Exception::type()
{
	return type_;
}

const char* Exception::toString()
{
	switch(type_) {
		case OutOfIds : return "Out of Ids.";
		case TooManyEntries : return "Too many entries.";
		default: return "No description available.";
	}
}

} // namespace DictionaryImpl

} // namespace Distiller

