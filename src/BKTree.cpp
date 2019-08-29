#include <core/precompiled.h>

#include <tagdistiller/EditDistance.h>

#include "BKTree.h"

namespace Distiller
{

class BKTree::Node
{

public:

	typedef QHash<uint, Node*> NodeHash;

	uint key_;	
	
	NodeHash leafes_;
	
	Node();
	
	~Node();
	
	inline bool hasLeaf(uint distance) const
	{ return leafes_.contains(distance); }
	
	inline Node* getLeaf(uint distance)
	{ return leafes_.value(distance); }
	
	inline bool hasLeafes() const
	{ return !leafes_.isEmpty(); }
	
	inline void makeNewLeaf(uint distance, uint key)
	{
		BKTree::Node* newNode = new(BKTree::Node);
		newNode->key_ = key;
		leafes_.insert(distance, newNode);
	}
	
	friend QDataStream& operator >> (QDataStream& in, BKTree::Node& n);

	friend QDataStream& operator << (QDataStream& out, const BKTree::Node& n);
};

BKTree::Node::Node() :
	key_(invalidKey)
{ }

BKTree::Node::~Node()
{
	qDeleteAll(leafes_);
	leafes_.clear();
}

QDataStream& operator >> (QDataStream& in, BKTree::Node& n)
{
	in >> n.key_;
	quint32 size;
	in >> size;
	while (size-- > 0) {
		BKTree::Node *pn = new(BKTree::Node);
		quint32 distance;
		in >> distance;
		in >> *pn;
		n.leafes_.insert(distance, pn);
	}
	return in;
}

QDataStream& operator << (QDataStream& out, const BKTree::Node& n)
{
	out << n.key_;
	out << (quint32)n.leafes_.size();
	BKTree::Node::NodeHash::const_iterator i;
	for (i = n.leafes_.constBegin(); i != n.leafes_.constEnd(); i++) {
		 out << (quint32)(i.key());
		 out << *i.value();
	}
	return out;
}

class BKTree::Private
{
	
public:

	mutable uint distance_time_;
	
	mutable uint createnode_time_;
	
	mutable QTime timer_;

	BKTree::Node root_;
	
	int size_;

	/**
	 * Reference to wordlist.
	 *
	 * The BKTree itself doesn't store strings, just
	 * uint keys to the position of strings in a wordlist;
	 *
	 * Doesn't own or modify the wordlist.
	 */
	const QStringList& wordlist_;
	
	Private(const QStringList& wordlist);
	
	~Private();
	
	uint getDistance(const BKTree::Node* node, uint key) const;
	
	uint getDistance(const BKTree::Node* node, const QString& pattern) const;
	
	uint getDistance(const BKTree::Node* node, const QString& pattern, 
		uint maxTypos) const;

	void insert(uint key);
	
	Match find(BKTree::Node* node, 
		const QString& pattern, uint maxTypos) const;
		
};

BKTree::Private::Private(const QStringList& wordlist) :
        distance_time_(0),
        createnode_time_(0),
        timer_(),
        root_(),
        wordlist_(wordlist)
{ }

BKTree::Private::~Private()
{ }

uint BKTree::Private::getDistance(const BKTree::Node* node, uint key) const
{
	Q_ASSERT(node != 0);
	Q_ASSERT(node->key_ != BKTree::invalidKey);
	Q_ASSERT(key != BKTree::invalidKey);
	return EditDistance::calc(
		wordlist_[node->key_],
		wordlist_[key],
		EditDistance::ExactMatch
	);
}

uint BKTree::Private::getDistance(const BKTree::Node* node, 
	const QString& pattern) const
{
	Q_ASSERT(node != 0);
	Q_ASSERT(node->key_ != BKTree::invalidKey);
	return EditDistance::calc(
		wordlist_[node->key_],
		pattern,
		EditDistance::ExactMatch
	);
}

uint BKTree::Private::getDistance(const BKTree::Node* node, 
	const QString& pattern, uint maxTypos) const
{
	Q_ASSERT(node != 0);
	Q_ASSERT(node->key_ != BKTree::invalidKey);
	return EditDistance::calc(
		wordlist_[node->key_],
		pattern,
		maxTypos,
		EditDistance::ExactMatch
	);
}

void BKTree::Private::insert(uint key)
{
	Q_ASSERT(key != BKTree::invalidKey);
	BKTree::Node* currentNode = &root_;
	if (currentNode->key_ == BKTree::invalidKey) {
		currentNode->key_ = key;
		return;
	}
	timer_.restart();
	uint distance = getDistance(currentNode, key);
	while (currentNode->hasLeaf(distance)) {
		currentNode = currentNode->getLeaf(distance);
		distance = getDistance(currentNode, key);
	}
	distance_time_ += timer_.elapsed();
	timer_.restart();
	currentNode->makeNewLeaf(distance, key);
	createnode_time_ += timer_.elapsed();
	size_ += 1;
}

BKTree::Match BKTree::Private::find(BKTree::Node* node,
	const QString& pattern, uint maxTypos) const
{
	Match rv;
	uint distance = 0;
	if (node->hasLeafes() == false) {
		// We're at a leaf. 
		distance = getDistance(node, pattern, maxTypos);
		if (distance <= maxTypos)
			return Match(node->key_, distance);
	} else {
		distance = getDistance(node, pattern);
		int end = distance + maxTypos;
		int d = 0;
		int i = distance;
		while (true) {
			if (node->hasLeaf(i)) {
				// Find best match in subnode.
				Match result;
				result = find(node->getLeaf(i), pattern, maxTypos);
				if (result.second == 0)
					// Minimum found.
					return result;
				if (result.second <= maxTypos && result.second < rv.second)
					// Better match found.
					rv = result;
			}
			
			// Calc next leaf.
			// Start from * and then jump left to right outwards.
			// Example: [7|5|3|1|*|2|4|6|8]
			
			if (d == end)
				break;
			if (d < 0)
				d = -1 * d;
			else 
				d = (d + 1) * -1;
			i = distance + d;
		}
		distance = getDistance(node, pattern, maxTypos);
		if (distance <= maxTypos && distance < rv.second)
			// Current node fits better.
			rv = Match(node->key_, distance);
	}
	return rv;
}

const uint BKTree::invalidKey = UINT_MAX;

const uint BKTree::invalidDistance = UINT_MAX;

BKTree::BKTree(const QStringList& wordlist) :
	d_(new Private(wordlist))
{ }

BKTree::~BKTree()
{
	delete d_;
}

void BKTree::insert(uint key)
{
	d_->insert(key);
}

BKTree::Match BKTree::find(const QString& pattern, uint maxTypos) const
{	
	return d_->find(&d_->root_, pattern, maxTypos);
}

int BKTree::size() const
{
	return d_->size_;
}

uint BKTree::getDistanceTime() const
{
	return d_->distance_time_;
}

uint BKTree::getCreateNodeTime() const
{
	return d_->createnode_time_;
}


} // namespace Distiller

