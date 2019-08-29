#ifndef DISTILLER_DICTIONARYIMPL_BITDISTANCE_H
#define DISTILLER_DICTIONARYIMPL_BITDISTANCE_H

#pragma once

#include <QReadWriteLock>

namespace Distiller
{

namespace DictionaryImpl
{

typedef QVector<quint64> BitpatternList;

/**
 * BitDistance::minDistance() calculates the lower bound of the
 * edit distance between two strings.
 *
 * 1) Calculate a bit pattern for each string where the occourence
 *    of a character results in a 1 at a specific positition.
 *    Example:
 *                     abdefghjiklmnoqrstuvwxyz
 *         "beatles"   110100000010000011000000
 *         "bextles"   010100000010000011000100
 *
 * 2) XOR the bit patterns:
 *             XOR     100000000000000000000100
 *
 * 3) Calculate the number of 1s in the XOR result:
 *                   = 2
 *
 * 4) Note that the edit distance of the two strings must be
 *    at least the half of the result (1 in our example, which
 *    is the exact edit distance).
 *
 * Since this calculation gives a rough estimation of the edit
 * distance, it gives also a guaranteed lower bound of it and can
 * be calculated very fast, we use it before entering the
 * Ukkonen distance (Distiller::EditDistance).
 *
 * Note that the bit pattern of the entries of the dictionary 
 * can be calculated beforehand and also the bit pattern of the
 * needle had to be calculated only once. 
 */
class BitDistance
{

   static quint8 sparse_bitcount (quint64 n);
	
   static quint64 char2bit(const char c);
	
public:

	BitDistance();

	~BitDistance();

    static quint64 bitPattern(const char *string);
	
	static quint8 minDistance(const char* str1, const char* str2);
	
    static quint8 minDistance(const char* str, quint64 bit2);
	
    static quint8 minDistance(quint64 bit1, quint64 bit2);
};

} // namespace DictionaryImpl

} // namespace Distiller

#endif 

