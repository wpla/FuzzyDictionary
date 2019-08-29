#include <core/precompiled.h>

#include <QTime>

#include "DictionaryDefines.h"
#include "Private.h"
#include "DictException.h"
#include "BitDistance.h"
#include "Builder.h"

namespace Distiller
{

namespace DictionaryImpl
{

void Builder::addGram(const QString& gram, quint32 key)
{
	d_.gramHash_.insert(gram, key);
}

void Builder::processLine(const QString& line)
{
	if (static_cast<uint>(d_.encodedEntries_.size()) == KEYTYPE_MAX)
		throw Exception(Exception::TooManyEntries);
	
	QString encodedLine = d_.encode(line);
	quint32 encodedLineSize = encodedLine.size();
	if (encodedLineSize == 0) return;
	if (encodedEntries_.contains(encodedLine)) return;
	encodedEntries_.insert(encodedLine);
	
	d_.encodedEntries_.append(encodedLine);
	d_.entries_.append(line);
	d_.bitencodedEntries_.append(BitDistance::bitPattern(encodedLine.toAscii().constData()));
	Q_ASSERT(d_.encodedEntries_.size() == 
		d_.entries_.size());
	
	int key = d_.encodedEntries_.size() - 1;
	
	if (encodedLineSize < d_.minGramSize())
		// Too small for our dictionary.
		return;
	else if (encodedLineSize <= d_.gramSize()) {
		// Take the whole encoded entry as a gram.
		addGram(encodedLine, key);
	}
	else {
		// Build all n-grams.
		for (quint32 i = 0; 
			 i < encodedLineSize - d_.minGramSize() + 1;
			 i++)
		{
			QString gram = encodedLine.mid(i, d_.gramSize());
			addGram(gram, key);
		}
	}
}

bool Builder::buildFrom(const QString& dictFile)
{
	d_.clear();
	d_.dictFilename_ = dictFile;
	QFile file(dictFile);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return false;
	QTextStream in(&file);
	in.setCodec("UTF-8");
#ifdef _DEBUG	
	quint32 lineCounter = 0;
#endif
	while(!in.atEnd()) {
		QString line = in.readLine();
		processLine(line);
#ifdef _DEBUG
		lineCounter += 1;
		if (lineCounter % 10000 == 0)
			qDebug("lines processed: %d", lineCounter);
#endif
	}
	d_.gramHash_.reCountAllNodes();
	return true;
}

bool Builder::buildFrom(const QStringList& stringList)
{
	using std::cout;
	using std::endl;
	
	d_.clear();
	QTime timer;
	uint inserttime = 0;
	for(int i = 0; i < stringList.size(); i++) {
		if (i % 10000 == 0)
			cout << "\r" << (int)((i * 100) / stringList.size()) << "% inserted.       ";
		timer.restart();
		processLine(stringList[i]);
		inserttime += timer.elapsed();
	}
	cout << endl;
	d_.gramHash_.reCountAllNodes();
	return true;
}

} // namespace DictionaryImpl

} // namespace Distiller

