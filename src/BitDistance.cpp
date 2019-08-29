#include <core/precompiled.h>

#include <QReadWriteLock>

#include "BitDistance.h"

namespace Distiller
{

namespace DictionaryImpl
{

BitDistance::BitDistance()
{ }

BitDistance::~BitDistance()
{ }

quint8 BitDistance::sparse_bitcount(quint64 n)
{
   quint8 count = 0;
   while (n) {
      count++;
      n &= (n - 1);
   }
   return count;
}

/*

  00000000011111111112222222222333333333344444444445555555555666
  12345678901234567890123456789012345678901234567890123456789012
  abcdefghijklmnopqrstuvwxyz 0123456789
  
*/

quint64 BitDistance::char2bit(const char c)
{
	switch (c) {
                case 'a' : return 0x0000000001u;
                case 'b' : return 0x0000000002u;
                case 'c' : return 0x0000000004u;
                case 'd' : return 0x0000000008u;
                case 'e' : return 0x0000000010u;
                case 'f' : return 0x0000000020u;
                case 'g' : return 0x0000000040u;
                case 'h' : return 0x0000000080u;
                case 'i' : return 0x0000000100u;
                case 'j' : return 0x0000000200u;
                case 'k' : return 0x0000000400u;
                case 'l' : return 0x0000000800u;
                case 'm' : return 0x0000001000u;
                case 'n' : return 0x0000002000u;
                case 'o' : return 0x0000004000u;
                case 'p' : return 0x0000008000u;
                case 'q' : return 0x0000010000u;
                case 'r' : return 0x0000020000u;
                case 's' : return 0x0000040000u;
                case 't' : return 0x0000080000u;
                case 'u' : return 0x0000100000u;
                case 'v' : return 0x0000200000u;
                case 'w' : return 0x0000400000u;
                case 'x' : return 0x0000800000u;
                case 'y' : return 0x0001000000u;
                case 'z' : return 0x0002000000u;
                case ' ' : return 0x0004000000u;
                case '0' : return 0x0008000000u;
                case '1' : return 0x0010000000u;
                case '2' : return 0x0020000000u;
                case '3' : return 0x0040000000u;
                case '4' : return 0x0080000000u;
                case '5' : return Q_UINT64_C(0x0100000000);
                case '6' : return Q_UINT64_C(0x0200000000);
                case '7' : return Q_UINT64_C(0x0400000000);
                case '8' : return Q_UINT64_C(0x0800000000);
                case '9' : return Q_UINT64_C(0x1000000000);
	}
	return 0;
}

quint64 BitDistance::bitPattern(const char *string)
{
    quint64 rv = 0;
	const char *pc = string;
	while (*pc)
		rv |= char2bit(*pc++);
	return rv;
}

quint8 BitDistance::minDistance(const char* str1, const char* str2)
{
        quint64 bit1 = bitPattern(str1);
        quint64 bit2 = bitPattern(str2);
        quint64 rv = bit1 ^ bit2;
        return sparse_bitcount(rv) / 2;
}

quint8 BitDistance::minDistance(const char* str1, quint64 bit2)
{
        quint64 bit1 = bitPattern(str1);
        quint64 rv = bit1 ^ bit2;
        return sparse_bitcount(rv) / 2;
}

quint8 BitDistance::minDistance(quint64 bit1, quint64 bit2)
{
        quint64 rv = bit1 ^ bit2;
        return sparse_bitcount(rv) / 2;
}

} // namespace DictionaryImpl

} // namespace Distiller

