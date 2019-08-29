#ifndef DISTILLER_DICTIONARYIMPL_PROFILER_H
#define DISTILLER_DICTIONARYIMPL_PROFILER_H

#pragma once

#include <QTime>

namespace Distiller
{

namespace DictionaryImpl
{

class Profiler
{

public:

	QTime timer;
	
	QTime queryTimer;
	
	uint new_time;
	
	uint createnode_time;
	
	uint insertnode_time;
	
	uint recount_time;
	
	uint loadstringarray_time;
	
	uint loadkeylistpos_time;
	
	uint queries;
	
	uint querytime;
	
	double queriesPerSec;
	
	Profiler();
	
	void reset();
	
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

