#include <core/precompiled.h>

#include "Profiler.h"

namespace Distiller
{

namespace DictionaryImpl
{

Profiler::Profiler() : 
	new_time(0),
	createnode_time(0), 
	insertnode_time(0),
	recount_time(0),
	loadstringarray_time(0),
	loadkeylistpos_time(0),
	queries(0),
	querytime(0),
	queriesPerSec(0)
{ }

void Profiler::reset()
{
	new_time = 0;
	createnode_time = 0;
	insertnode_time = 0;
	recount_time = 0;
	loadstringarray_time = 0;
	loadkeylistpos_time = 0;
	queries = 0;
	querytime = 0;
	queriesPerSec = 0;
}

} // namespace DictionaryImpl

} // namespace Distiller

