#ifndef DISTILLER_BKTREE_H
#define DISTILLER_BKTREE_H

#pragma once

#include <QString>
#include <QStringList>
#include <QPair>

namespace Distiller
{

/**
 * Implementation of a Burkhard-Keller-Tree.
 */
class BKTree
{

	class Private;
	
	Private* d_;
	
public:
	
	static const uint invalidKey;
	
	static const uint invalidDistance;
	
	class Match : public QPair<uint, uint>
	{
	
	public:
	
        Match() :
            QPair<uint, uint>(invalidKey, invalidDistance)
		{ }
		
		Match(uint first, uint second) :
            QPair<uint, uint>(first, second)
		{ }
	};

	class Node;	

	BKTree(const QStringList& wordlist);
	
	virtual ~BKTree();
	
	void insert(uint key);
	
	Match find(const QString& pattern, uint maxTypos) const;
	
	int size() const;
	
	/**
	 * Used for profiling.
	 */
	uint getDistanceTime() const;
	
	uint getCreateNodeTime() const;
	
};

} // namespace Distiller

#endif 

