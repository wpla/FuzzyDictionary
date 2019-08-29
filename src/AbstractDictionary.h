#ifndef DISTILLER_ABSTRACTDICTIONARY_H
#define DISTILLER_ABSTRACTDICTIONARY_H

#pragma once

class QString;

namespace Distiller {

class AbstractDictionary
{

public:

	AbstractDictionary()
		{ }
		
	virtual ~AbstractDictionary()
		{ }
	
	/**
	 * Finds the match to a given (incorrect) pattern.
	 * Returns QString() if nothing is found.
	 */
	virtual QString find(const QString& needle) const = 0;
	
	virtual bool load(const QString& dictionary) = 0;
	
	virtual bool save() = 0;
	
};

} // namespace Distiller

#endif
