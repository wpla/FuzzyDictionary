#include <core/precompiled.h>

#include "DictionaryDefines.h"
#include "KeyDistTuple.h"

namespace Distiller
{

namespace DictionaryImpl
{

const KeyType KeyDistTuple::invalidKey = KEYTYPE_MAX;

const DistType KeyDistTuple::invalidDistance = DISTTYPE_MAX;

KeyDistTuple::KeyDistTuple() :
	QPair<KeyType, DistType>(invalidKey, invalidDistance)
{ }

KeyDistTuple::~KeyDistTuple()
{ }

} // namespace DictionaryImpl

} // namespace Distiller

