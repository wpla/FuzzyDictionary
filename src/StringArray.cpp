#include <core/precompiled.h>

#include <tagdistiller/SimpleString.h>

#include "StringArray.h"

namespace Distiller
{

class StringArray::Private {

public:

	/// Reference counter.
	unsigned int refcnt_;

	/// Pointer to memory area that holds all the strings.
	QChar* data_;

    /// Number of QChars that have been allocated.
	unsigned int dataSize_;

	/// Number of QChars that have already been used.
	unsigned int filledSize_;

	/// Pointer to memory area that holds the offset of each string in data_.
	unsigned int* pos_;	

    /// Number of QChars that have been allocated.
	unsigned int posSize_;

	/// Pointer to memory area that holds the size of each string in data_.
	unsigned int* strSize_;	

    /// Number of QChars that have been allocated for strSize_.
	unsigned int strSizeSize_;

	/// Number of strins.
	unsigned int size_;
	
	Private();
	
	~Private();
	
	void clear();
	
	Private* clone();
	
private:

	Private(const Private& other);
	
	Private& operator = (const Private& other);
	
};

StringArray::Private::Private() :
	refcnt_(1),
	data_(0),
	dataSize_(0),
	filledSize_(0),
	pos_(0),
	posSize_(0),
    strSize_(0),
	strSizeSize_(0),
    size_(0)
{
}

StringArray::Private::~Private()
{
	Q_ASSERT(refcnt_ <= 1);
	clear();
}

void StringArray::Private::clear()
{
	if (refcnt_ > 1)
		return;
	if (dataSize_ > 0)
		free(data_);
	if (posSize_ > 0)
		free(pos_);
	if (strSizeSize_ > 0)
		free(strSize_);
	data_ = 0;
	pos_ = 0;
	strSize_ = 0;
	filledSize_ = 0;
	size_ = 0;
	dataSize_ = 0;
	posSize_ = 0;
	strSizeSize_ = 0;
}

StringArray::Private* StringArray::Private::clone()
{
	StringArray::Private* rv = 0;
	
	try {
		rv = new StringArray::Private();
	} catch(...) {
		return 0;
	}
	
	rv->data_ = static_cast<QChar*>(malloc(dataSize_ * sizeof(QChar)));
	if (rv->data_ == 0) {
		delete rv;
		return 0;
	}
	memcpy(rv->data_, data_, filledSize_ * sizeof(QChar));
	
	rv->pos_ = static_cast<unsigned int*>(malloc(posSize_ * sizeof(unsigned int)));
	if (rv->pos_ == 0) {
		free(rv->data_);
		delete rv;
		return 0;
	}
	memcpy(rv->pos_, pos_, posSize_ * sizeof(unsigned int));
	
	rv->strSize_ = static_cast<unsigned int*>(
		malloc(strSizeSize_ * sizeof(unsigned int))
	);
	if (rv->strSize_ == 0) {
		free(rv->pos_);
		free(rv->data_);
		delete rv;
		return 0;
	}
	memcpy(rv->strSize_, strSize_, strSizeSize_ * sizeof(unsigned int));
	
	rv->size_ = size_;
	rv->filledSize_ = filledSize_;
	rv->dataSize_ = dataSize_;
	rv->posSize_ = posSize_;
	rv->strSizeSize_ = strSizeSize_;
	
	// Note: rv->refcnt_ == 1
	
	return rv;
}

StringArray::StringArray() :
	d_(new Private) 
{ }

StringArray::StringArray(const StringArray& other)
{
	d_ = other.d_;
	d_->refcnt_++;
}

/**
 * Note: This method is safe against self assignment.
 */
StringArray& StringArray::operator = (const StringArray& other)
{
	Private* new_d = other.d_;
	Private* old_d = d_;
	d_ = new_d;
	d_->refcnt_++;
	old_d->refcnt_--;
	if (old_d->refcnt_ == 0)
		delete old_d;
	return *this;
}

StringArray::~StringArray()
{
	if (d_->refcnt_ > 1)
		d_->refcnt_--;
	else
		delete d_;
}

/**
 * Makes a copy of the private data.
 */
void StringArray::detach()
{
	if (d_->refcnt_ > 1) {
		Private* new_d = d_->clone();
		if (new_d != 0) {
			d_->refcnt_--;   
			d_ = new_d;     // Never throws.
		}
		else
			throw std::bad_alloc();
	}
}

void StringArray::expandMemBlock(
	void*& data, 
	size_t elementSize,
	unsigned int& size, 
	unsigned int initialSize,
	double growthFactor
)
{
	unsigned int new_size;
	size_t new_allocatedSize;
	if (size == 0)
		new_size = initialSize;
	else
		new_size = size * growthFactor;
	new_allocatedSize = elementSize * new_size;
	void* new_data;
	new_data = realloc(data, new_allocatedSize);
	if (new_data == 0)
		throw std::bad_alloc();
	data = new_data;
	size = new_size;
}

void StringArray::expandData()
{
	void *tmp_data = d_->data_;
	expandMemBlock(tmp_data, sizeof(QChar), d_->dataSize_, 1000);
	d_->data_ = static_cast<QChar*>(tmp_data);
}

void StringArray::expandPos()
{
	void *tmp_data = d_->pos_;
	expandMemBlock(tmp_data, sizeof(unsigned int), d_->posSize_, 100);
	d_->pos_ = static_cast<unsigned int*>(tmp_data);
}

void StringArray::expandStrSize()
{
	void *tmp_data = d_->strSize_;
	expandMemBlock(tmp_data, sizeof(unsigned int), d_->strSizeSize_, 100);
	d_->strSize_ = static_cast<unsigned int*>(tmp_data);
}

void StringArray::append(const QString& str)
{
	detach();
	unsigned int strSize = str.size();
	if (d_->size_ == d_->posSize_)
		expandPos();
	if (d_->size_ == d_->strSizeSize_)
		expandStrSize();
	if (d_->filledSize_ + strSize + 1 >= d_->dataSize_)
		expandData();
	memcpy(d_->data_ + d_->filledSize_, str.unicode(),
		sizeof(QChar) * strSize);	
	d_->pos_[d_->size_] = d_->filledSize_;
	d_->strSize_[d_->size_] = strSize;
	d_->filledSize_ += strSize;
	// Terminate string with '\0'
	d_->data_[d_->filledSize_++] = QChar(0);
	d_->size_ += 1;
}

void StringArray::clear()
{
	if (d_->refcnt_ > 1) {
		d_->refcnt_--;
		d_ = new Private;
	}
	else
		d_->clear();
}

void operator >> (QDataStream& in, StringArray& strArray)
{
	quint32 size;
	quint32 filledSize;
	QChar* tmp_data = 0;
	unsigned int* tmp_pos = 0;
	unsigned int* tmp_strSize = 0;

	in >> size;
	in >> filledSize;
	
	tmp_data = static_cast<QChar*>(malloc(sizeof(QChar) * filledSize));
	if (tmp_data == 0)
		throw std::bad_alloc();
		
	tmp_pos = static_cast<unsigned int*>(
		malloc(sizeof(unsigned int) * size)
	);
	if (tmp_pos == 0) {
		free(tmp_data);
		throw std::bad_alloc();
	}
	
	tmp_strSize = static_cast<unsigned int*>(
		malloc(sizeof(unsigned int) * size)
	);
	if (tmp_strSize == 0) {
		free(tmp_data);
		free(tmp_pos);
		throw std::bad_alloc();
	}
	
	strArray.clear();
	
	strArray.d_->data_ = tmp_data;
	strArray.d_->pos_ = tmp_pos;
	strArray.d_->strSize_ = tmp_strSize;
	strArray.d_->size_ = size;
	strArray.d_->filledSize_ = filledSize;
	strArray.d_->dataSize_ = filledSize;
	strArray.d_->posSize_ = size;
	strArray.d_->strSizeSize_ = size;

	in.readRawData(reinterpret_cast<char*>(strArray.d_->pos_), 
		sizeof(unsigned int) * size);
	in.readRawData(reinterpret_cast<char*>(strArray.d_->strSize_),
		sizeof(unsigned int) * size);
	in.readRawData(reinterpret_cast<char*>(strArray.d_->data_),
		sizeof(QChar) * filledSize);
}

void operator << (QDataStream& out, const StringArray& strArray)
{
	out << (quint32)strArray.d_->size_;
	out << (quint32)strArray.d_->filledSize_;
	
	out.writeRawData(reinterpret_cast<const char*>(strArray.d_->pos_),
		sizeof(unsigned int) * strArray.d_->size_);
	out.writeRawData(reinterpret_cast<const char*>(strArray.d_->strSize_),
		sizeof(unsigned int) * strArray.d_->size_);
	out.writeRawData(reinterpret_cast<const char*>(strArray.d_->data_),
		sizeof(QChar) * strArray.d_->filledSize_);
}

int StringArray::size() const
{
	return d_->size_;
}

int StringArray::sizeOf(unsigned int pos) const
{
	Q_ASSERT(pos < d_->size_);
	return d_->strSize_[pos];
}

bool StringArray::isEqual(const QString& str, unsigned int pos) const
{
	Q_ASSERT(pos < d_->size_);
	if ((uint)str.size() != d_->strSize_[pos])
		return false;
	return (memcmp(d_->data_ + d_->pos_[pos], str.unicode(), 
		sizeof(QChar) * d_->strSize_[pos]) == 0);
}

QString StringArray::toQString(unsigned int pos) const
{
	Q_ASSERT(pos < d_->size_);
	return QString(d_->data_ + d_->pos_[pos],
		static_cast<int>(d_->strSize_[pos]));
}

SimpleString StringArray::toSimpleString(unsigned int pos) const
{
	Q_ASSERT(pos < d_->size_);
	return SimpleString(d_->data_ + d_->pos_[pos],
		d_->strSize_[pos]);
}

const QChar* StringArray::unicode(unsigned int pos) const
{
	Q_ASSERT(pos < d_->size_);
	return d_->data_ + d_->pos_[pos];
}

} // namespace Distiller

