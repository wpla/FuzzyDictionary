#ifndef DISTILLER_SIMPLESTRING_H
#define DISTILLER_SIMPLESTRING_H

#pragma once

namespace Distiller
{

/**
 * Helper class.
 *
 * Usually points to a string in StringArray.
 * A SimpleString can't be modified.
 */
class SimpleString {

	/// Pointer to '\0' terminated array of const QChars.	
	const QChar* cptr_;
	
	/// Size of the string.
	unsigned int size_;
	
public:

	/**
	 * Constructs a SimpleString from a pointer to a '\0' terminated const QChars
	 * and a size.
	 *
	 * SimpleString doesn't allocate any memory nor copies the array.
	 */
	SimpleString(const QChar* str, unsigned int size) :
		cptr_(str), size_(size) { }

	/**
	 * Copyconstructs a SimpleString from another SimpleString.
	 *
	 * Doesn't copy the data.
	 */		
	SimpleString(const SimpleString& other) :
		cptr_(other.cptr_), size_(other.size_) { }
		
	/**
	 * Constructs SimpleString from QString explicitly.
	 *
	 * SimpleString does not copy the data, it points to the data
	 * of the QString.
	 *
	 * SimpleString is valid as long the QString it is constructed
	 * from isn't modified.
	 */
	explicit SimpleString(const QString& other) :
		cptr_(other.unicode()), size_(other.size()) { }
		
	inline int size() const
		{ return size_; }
		
	inline QString toQString() const
		{ return QString(cptr_, size_); }
		
	inline const QChar* unicode() const
		{ return cptr_; }
		
	inline const QChar operator[] (unsigned int pos) const
		{ Q_ASSERT(pos < size_); return cptr_[pos]; }
		
	bool operator == (const SimpleString& rhs) const;
	
	bool startsWith(const SimpleString& other) const;
	
	bool contains(const SimpleString& other) const;
	
	/**
	 * Reduces the length of the string to n, leaving
	 * just the left end.
	 *
	 * Does nothing if n > size().
	 */
	void left(unsigned int n);
	
	/**
	 * Reduces the length of the string to n, leaving
	 * just the right end.
	 *
	 * Does nothing if n > size().
	 */
	void right(unsigned int n);
	
	/**
	 * Trims away n characters from the left end.
	 * Equal to str.right(str.size() - n).
	 *
	 * Does nothing if n = 0 or n > size().
	 */
	void trimLeft(unsigned int n);
	
	/**
	 * Trims away n characters from the right end.
	 * Equal to str.left(str.size() - n).
	 *
	 * Does nothing if n = 0 or n > size().
	 */
	void trimRight(unsigned int n);
	
	/**
	 * Removes a common prefix of the instance
	 * and the parameter str.
	 *
	 * This method modifies str.
	 * Does nothing if n < 0 or n > size().
	 */
	void removeCommonPrefix(SimpleString& str);
	
	/**
	 * Removes a common suffix of the instance
	 * and the parameter str.
	 *
	 * This method modifies str.
	 * Does nothing if n < 0 or n > size().
	 */
	void removeCommonSuffix(SimpleString& str);
		
private:

	bool startsWith(const QChar* cptr, const SimpleString& other) const;

	SimpleString();
};

} // namespace Distiller

#endif 

