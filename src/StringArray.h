#ifndef DISTILLER_STRINGARRAY_H
#define DISTILLER_STRINGARRAY_H

#pragma once

namespace Distiller
{

class SimpleString;

/**
 * Implements a string array of adjacent QChars.
 *
 * The difference to QStringArray is that all strings are stored
 * in one memory block. Thus the block can be read or written at
 * once from or to a QDataStream.
 *
 * Loading a StringArray from a QDataStream is very fast, since
 * all strings are loaded in one read operation.
 *
 * Implements Copy-on-write.
 */
class StringArray
{
	class Private;
	
	Private* d_;

	void detach();
	
	void expandMemBlock(
		void*& data, 
		size_t elementSize,
		unsigned int& size, 
		unsigned int initialSize = 100,
		double growthFactor = 1.5
	);
	
	void expandData();

	void expandPos();

	void expandStrSize();

public:

	StringArray();
	
	StringArray(const StringArray& other);

	~StringArray();
	
	StringArray& operator = (const StringArray& other);
	
	/**
	 * Appends a string to the array.
	 */
	void append(const QString& str);

	/**
	 * Returns true if str is equal with string at position pos.
	 * pos must be smaller than size().
	 */
	bool isEqual(const QString& str, unsigned int pos) const;

	/**
	 * Returns a deep copied QString of the string at position pos.
	 * pos must be smaller than size().
	 */
	QString toQString(unsigned int pos) const;
	
	/**
	 * Returns a shallow copied SimpleString.
	 *
	 * Valid as long the StringArray is not modified or 
	 * deleted.
	 */
	SimpleString toSimpleString(unsigned int pos) const;

	/**
	 * Returns a '\0' terminated string of QChars of string at position pos.
	 * pos must be smaller than size().
	 */
	const QChar* unicode(unsigned int pos) const;
	
	/**
	 * Clears the StringArray and frees all the allocated memory.
	 */
	void clear();

	/**
	 * Number of strings in the array.
	 */
	int size() const;

	/**
	 * Size of string at position pos.
	 */
	int sizeOf(unsigned int pos) const;	

	/** 
	 * Loads StringArray from QDataStream.
	 *
	 * This operation is significantly faster than loading
	 * a QStringList or a QVector<QString> from a QDataStream
	 * because all strings are loaded in one read operation.
	 */
	friend void operator >> (QDataStream& in, StringArray& strArray);

	/**
	 * Saves StringArray to a QDataStream.
	 */
	friend void operator << (QDataStream& out, const StringArray& strArray);
};

} // namespace Distiller

#endif 

