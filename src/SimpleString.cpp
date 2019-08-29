#include <core/precompiled.h>

#include "SimpleString.h"

namespace Distiller
{

bool SimpleString::operator ==(const SimpleString& rhs) const
{
	const QChar* ptr1 = cptr_;
	const QChar* ptr2 = rhs.unicode();
	
	while (*ptr1 == *ptr2 && *ptr1 != QChar(0) && *ptr2 != QChar(0)) {
		ptr1++;
		ptr2++;
	}
	
	return (*ptr1 == QChar(0) && *ptr2 == QChar(0));
}

bool SimpleString::startsWith(const SimpleString& other) const
{
	const QChar* ptr1 = cptr_;
	const QChar* ptr2 = other.unicode();
	
	while (*ptr1 == *ptr2 && *ptr1 != QChar(0) && *ptr2 != QChar(0)) {
		ptr1++;
		ptr2++;
	}
	
	return *ptr2 == QChar(0);
}

bool SimpleString::startsWith(const QChar* cptr, const SimpleString& other) const
{
	const QChar* ptr1 = cptr;
	const QChar* ptr2 = other.unicode();
	
	while (*ptr1 == *ptr2 && *ptr1 != QChar(0) && *ptr2 != QChar(0)) {
		ptr1++;
		ptr2++;
	}
	
	return *ptr2 == QChar(0);
}

bool SimpleString::contains(const SimpleString& other) const
{
	// TODO Implementing the naive approach, replace it with BM or KMP
	// sometime.
	
	const QChar *align1 = cptr_;
	bool rv = false;
	
	for (align1 = cptr_; *align1 != QChar(0); align1++) {
		rv = startsWith(align1, other);
		if (rv == true)
			break;
	}

	return rv;	
}

void SimpleString::left(unsigned int n)
{
	if (n > size_)
		return;
	size_ = n;
}

void SimpleString::right(unsigned int n)
{
	if (n > size_)
		return;
	cptr_ += size_ - n;
	size_ -= n;
}

void SimpleString::trimLeft(unsigned int n)
{
	if (n > size_)
		return;
	cptr_ += n;
	size_ -= n;
}

void SimpleString::trimRight(unsigned int n)
{
	if (n > size_)
		return;
	size_ -= n;
}

void SimpleString::removeCommonPrefix(SimpleString& str)
{
	unsigned int n = 0;
	const QChar *ptr1 = cptr_;
	const QChar *ptr2 = str.unicode();
	while (*ptr1 == *ptr2 && 
		   *ptr1 != QChar(0) &&
		   *ptr2 != QChar(0)) {
		ptr1++;
		ptr2++;
		n++;
	}
	Q_ASSERT(n <= size_);
	if (n > 0) {
		cptr_ = ptr1;
		size_ -= n;
		str.trimLeft(n);
	}
}

void SimpleString::removeCommonSuffix(SimpleString& str)
{
	unsigned int n = 0;
	const QChar *ptr1 = cptr_ + size_ - 1;
	const QChar *ptr2 = str.unicode() + str.size() - 1;
	const QChar *str_begin = str.unicode();
	while (*ptr1 == *ptr2 &&
	       ptr1 >= cptr_ &&
	       ptr2 >= str_begin) {
		ptr1--;
		ptr2--;
		n++;
	}
	Q_ASSERT(n <= size_);
	if (n > 0) {
		size_ -= n;
		str.trimRight(n);
	}
}

} // namespace Distiller

