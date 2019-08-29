#include <core/precompiled.h>

#include <limits.h>

#include <tagdistiller/SimpleString.h>

#include "EditDistance.h"

namespace Distiller
{

int EditDistance::levenshtein_counter = 0;
int EditDistance::ukkonen_counter = 0;

int EditDistance::calc(const SimpleString& pattern,
					   const SimpleString& text, 
					   uint maxTypos,
					   MatchType matchType)
{
	return Ukkonen(pattern, text, maxTypos, matchType);	
}

int EditDistance::calc(const QString& pattern, 
					   const QString& text, 
					   uint maxTypos,
					   MatchType matchType)
{
	return Ukkonen(SimpleString(pattern), SimpleString(text), maxTypos, matchType);	
}

int EditDistance::calc(const SimpleString& pattern, 
					   const SimpleString& text, 
					   MatchType matchType)
{
	return Levenshtein(pattern, text, matchType);
}

int EditDistance::calc(const QString& pattern, 
					   const QString& text, 
					   MatchType matchType)
{
	return Levenshtein(SimpleString(pattern), SimpleString(text), matchType);
}

int EditDistance::Levenshtein(const SimpleString& pattern, 
							  const SimpleString& text, 
							  MatchType matchType)
{
	if (matchType == SubstringMatch)
		return Levenshtein_substring(pattern, text);
	return Levenshtein_exact(pattern, text);
}

int EditDistance::Levenshtein(const QString& pattern, 
							  const QString& text, 
							  MatchType matchType)
{
	if (matchType == SubstringMatch)
		return Levenshtein_substring(SimpleString(pattern), SimpleString(text));
	return Levenshtein_exact(SimpleString(pattern), SimpleString(text));
}

int EditDistance::Ukkonen(const SimpleString& pattern, 
						  const SimpleString& text, 
						  uint maxTypos,
						  MatchType matchType)
{
	if (matchType == SubstringMatch)
		return Ukkonen_substring(pattern, text, maxTypos);
	return Ukkonen_exact(pattern, text, maxTypos);
}

int EditDistance::Ukkonen(const QString& pattern,
						  const QString& text,
						  uint maxTypos,
						  MatchType matchType)
{
	if (matchType == SubstringMatch)
		return Ukkonen_substring(SimpleString(pattern), 
			SimpleString(text), maxTypos);
	return Ukkonen_exact(SimpleString(pattern), SimpleString(text), maxTypos);
}

int EditDistance::Levenshtein_exact(const SimpleString& pattern, 
									const SimpleString& text)
{
	quint16 m = pattern.size();
	quint16 n = text.size();
	
	// Trivial case.
	if (pattern == text) return 0;
	
	QVector<quint8> V1(m + 1);
	QVector<quint8> V2(m + 1);
	V2.fill(0);
	
	QVector<quint8>* D1 = &V1;
	QVector<quint8>* D2 = &V2;
	
	// Initialize D1
	for (quint8 i = 0; i < m + 1; i++)  {
		(*D1)[i] = i;
	}
	
	for(quint16 j = 1; j < n + 1; j++) {
		(*D2)[0] = j;
		for (int i = 1; i < m + 1; i++) {
			if (pattern[i - 1] == text[j - 1])
				(*D2)[i] = (*D1)[i-1];
			else {
				(*D2)[i] = qMin(qMin(
								(*D1)[i-1] + 1,
								(*D2)[i-1] + 1),
								(*D1)[i]   + 1);
			}
#ifdef _DEBUG			
			levenshtein_counter++;
#endif
		}
		qSwap(D1, D2);
		D2->fill(0);
	}
	return (*D1)[m];
}


int EditDistance::Levenshtein_substring(const SimpleString& pattern, 
										const SimpleString& text)
{
	quint16 m = pattern.size();
	quint16 n = text.size();
	int rv = INT_MAX;
	
	// Trivial case.
	if (pattern == text || text.contains(pattern)) return 0;
	
	QVector<quint8> V1(m + 1);
	QVector<quint8> V2(m + 1);
	V2.fill(0);
	
	QVector<quint8>* D1 = &V1;
	QVector<quint8>* D2 = &V2;
	
	// Initialize D1
	for (quint8 i = 0; i < m + 1; i++)  {
		(*D1)[i] = i;
	}
	
	int j = 0;
	int i = 0;
	for(j = 1; j < n + 1; j++) {
		for (i = 1; i < m + 1; i++) {
			if (pattern[i - 1] == text[j - 1])
				(*D2)[i] = (*D1)[i-1];
			else {
				(*D2)[i] = qMin(qMin(
								(*D1)[i-1] + 1,
								(*D2)[i-1] + 1),
								(*D1)[i]   + 1);
			}
#ifdef _DEBUG
			levenshtein_counter++;
#endif
		}
		if ((*D2)[i-1] < rv)
			rv = (*D2)[i-1];
		if (rv == 0)
			return 0;
		qSwap(D1, D2);
		D2->fill(0);
	}
	return rv;
}

/**
 * Optimized version of the Levenshtein distance.
 *
 * See docs/Levenshtein Distance - Optimization.pdf for infos
 * about the optimization.
 */
int EditDistance::Ukkonen_exact(const SimpleString& aPattern, 
								const SimpleString& aText,
								uint maxTypos)
{
	SimpleString pattern(aPattern);
	SimpleString text(aText);
	
	/* removeCommon... modifies text as well. */
	pattern.removeCommonPrefix(text);
	pattern.removeCommonSuffix(text);

	uint m = pattern.size();
	uint n = text.size();
	
	// Trivial cases.
	Q_ASSERT(maxTypos < UINT_MAX);
	if (m == 0 && n == 0)
		return 0;
	if (m == 0 && n > 0)
		return n;
	if (n == 0 && m > 0)
		return m;
	if ((uint)qAbs<int>(n - m) > maxTypos)
		return maxTypos + 1;
	
	QVector<quint8> V1(m + 1);
	QVector<quint8> V2(m + 1);
	V2.fill(0);
	
	QVector<quint8>* D1 = &V1;
	QVector<quint8>* D2 = &V2;
	
	// Inititialize D1.
	for (quint8 i = 0; i < m + 1; i++)  {
		(*D1)[i] = i;
	}
	
	uint i = 0;
	uint j = 0;
	for(j = 1; j < n + 1; j++) {
		(*D2)[0] = j;
		for (i = 1; i < m + 1; i++) {
			if (pattern[i - 1] == text[j - 1])
				(*D2)[i] = (*D1)[i-1];
			else {
				(*D2)[i] = qMin(qMin(
								(*D1)[i-1] + 1,
								(*D2)[i-1] + 1),
								(*D1)[i]   + 1);
			}

			// Optimization
			
			bool upper_area = false;
			bool lower_area = false;
			
			if (n > m) {
				// Landscape.
				if (i + n - m < j)
					upper_area = true;
				else if (i + n - m > j)
					lower_area = true;
			} else {
				// Portrait or Square.
				if (j + m - n > i)
					upper_area = true;
				else if (j + m - n < i)
					lower_area = true;
			}

			if (lower_area) {
				if ((*D2)[i] > (maxTypos + n - j - m + i)) 
					return maxTypos + 1;
			}
			else if (upper_area) {
				if ((*D2)[i] > (maxTypos - n + j + m - i)) 
					return maxTypos + 1;
			}
			else {
				// We're at the diagonal.
				if ((*D2)[i] > maxTypos)
					return maxTypos + 1;
			}
#ifdef _DEBUG
			ukkonen_counter++;
#endif
		}
		qSwap(D1, D2);
		D2->fill(0);
	}
	return (*D1)[i - 1];
}

/**
 * Calculates the minimum edit distance of a pattern
 * in text. Pattern can be a substring of text.
 *
 * Example: 
 * pattern:    "bach"
 * text:    "refbauchcfrte"
 * result:  1
 *
 * The parameter maxTypos specifies the maximum edit distance
 * between pattern and text. If no match below maxTypos is 
 * found, the return value is greater than maxTypos. Use it like
 * this:
 * 
 *     if (Ukkonen_substring(pattern, text, 4) > 4)
 *         // too many errors
 *
 * maxTypos is a marginal condition which we can use to apply
 * some optimzations. The matrix D of the edit distances has some
 * useful properties that we can exploit for optimizing the
 * function (see [1]).
 *
 * Property 1:  D_{i+1, j+1} >= D_{i,j}
 *              Each diagonal is monotonously increasing.
 *
 * Property 2:  abs(D_{i,j} - D_{i, j+1}) <= 1
 *              abs(D_{i,j} - D_{i+1, j}) <= 1
 *              The absolute difference between two cells is
 *              at most 1 (given that the edit costs are
 *              1 for each operation).
 *
 * I've identified four cases, depending on the current
 * position (i, j).
 *
 * First, we have to calculate at which area of D we are.
 *
 * Example (n > m):
 *
 *     i = 0 ... m rows
 *     j = 0 ... n columns
 * 
 *         0 --------------------- n
 *
 *     0   L L L L L L D U U U U U U
 *     |   L L L L L L L D U U U U U
 *     |   L L L L L L L L D U U U U
 *     |   L L L L L L L L L D U U U
 *     |   L L L L L L L L L L D U U
 *     |   L L L L L L L L L L L D U
 *     m   L L L L L L L L L L L L D
 *
 *     U ... upper area
 *     D ... diagonal
 *     L ... lower area
 *
 * For every position D[i][j] we can calculate the area code.
 *
 * If the matrix D is landscape orientated (n > m):
 *
 *     i + n - m < j => upper area
 *     i + n - m > j => lower area
 *              else => diagonal
 *
 * If the matrix D is portrait orientated or squared (m >= n):
 *
 *     j + m - n > i => upper area
 *     j + m - n < i => lower area
 *              else => diagonal
 *
 * All optimizations are based on the following idea: If we already
 * have a best edit distance at position (i, j) and we can assume that
 * given the current value D[i][j] no better edit distance is possible,
 * we're aborting the inner loop and return the already computed best
 * edit distance.
 *
 * Example:
 *
 * Let's look at this matrix:
 *
 *      0 ----j---- n
 *    0 . . . . . . .
 *    | . . . . . . .
 *    i . . . X . . .
 *    | . . . . . . .
 *    m 3 2 1 . . Y .
 *          
 *
 * We're at position (i = 2, j = 3), marked with X. The best edit
 * distance at this position is 1 (computed at the previous column).
 * 
 * If X is equal or greater than 4 we can stop the computation and 
 * leave the function. Why?
 * 
 * Because of property 1 we know that Y is also equal or greater than
 * 4. And property 2 gives us the certainty, that it's impossible to
 * get a better value than 1 at the bottom row, given that Y is equal
 * or greater than 4.
 * 
 * Let's look at the best case, given X = 4:
 *
 *      0 ----j---- n
 *    0 . . . . . . .
 *    | . . . . . . .
 *    i . . . 4 . . .
 *    | . . . . 4 . .
 *    m 3 2 1 2 3 4 3
 *
 * We clearly see, that theres no better result than 1, so we can stop
 * now.
 *
 * Let's deduce a formular for this:
 *
 *    if D[i][j] = X we know that
 *       D[n][j + m - i] (= Y) is >= X because of property 1
 *
 *    if rv is our best match (1 in the example matrix)
 *    and D[i][j] > rv + m - i we can stop the computation.
 *
 * This is an illustration how the optimization works. There are four
 * cases we have to distinguish. All other cases are based on the same
 * idea (see docs/Levenshtein Distance - Optimization.pdf).
 *
 * References
 *
 * [1] Ukkonen, E. (1985) Finding Approximate Patterns in Strings.
 *     Journal of Algorithms 6, 132--137.
 */
int EditDistance::Ukkonen_substring(const SimpleString& aPattern, 
									const SimpleString& aText,
									uint maxTypos)
{
	SimpleString pattern(aPattern);
	SimpleString text(aText);
	
	uint m = pattern.size();
	uint n = text.size();
	int rv = maxTypos;
	bool found = false;
	
	Q_ASSERT(maxTypos < UINT_MAX);

	// Trivial cases.
	if (pattern == text || 
		text.contains(pattern))
		return 0;
	
	QVector<quint8> V1(m + 1);
	QVector<quint8> V2(m + 1);
	V2.fill(0);
	
	QVector<quint8>* D1 = &V1;
	QVector<quint8>* D2 = &V2;
	
	// Initialize D1
	for (quint8 i = 0; i < m + 1; i++)  {
		(*D1)[i] = i;
	}
	
	uint j = 0;
	uint i = 0;
	for (j = 1; j < n + 1; j++) {
		for (i = 1; i < m + 1; i++) {
			if (pattern[i - 1] == text[j - 1])
				(*D2)[i] = (*D1)[i-1];
			else {
				(*D2)[i] = qMin(qMin(
								(*D1)[i-1] + 1,
								(*D2)[i-1] + 1),
								(*D1)[i]   + 1);
			}
			
			// Optimization
			
			bool upper_area = false;
			bool lower_area = false;
			
			if (n > m) {
				// Landscape.
				if (i + n - m < j)
					upper_area = true;
				else if (i + n - m > j)
					lower_area = true;
			} else {
				// Potrait or Square.
				if (j + m - n > i)
					upper_area = true;
				else if (j + m - n < i)
					lower_area = true;
			}
			
			if (lower_area) {
				if (m - i > n - j - m + i) {
					if ((*D2)[i] > rv + m - i)
						// Case #1
						return found? rv : maxTypos + 1;
				} else {
					if ((*D2)[i] > rv + n - j - m + i)
						// Case #2
						return found? rv : maxTypos + 1;
				}
			} else if (upper_area) {
				if ((*D2)[i] > rv + m - i)
					// Case #3
					return found? rv : maxTypos + 1;
			} else {
				// We're at the diagonal.
				if ((*D2)[i] > rv + n - j)
					// Case #4
					return found? rv : maxTypos + 1;
			}
#ifdef _DEBUG
			ukkonen_counter++;
#endif			
		}
		if ((*D2)[i-1] <= rv) {
			rv = (*D2)[i-1];
			found = true;
		}
		if (rv == 0)
			return 0;
		qSwap(D1, D2);
		D2->fill(0);
	}
	return found? rv : maxTypos + 1;
}

void EditDistance::resetLevenshteinCounter()
{
	levenshtein_counter = 0;
}

void EditDistance::resetUkkonenCounter()
{
	ukkonen_counter = 0;
}

int EditDistance::levenshteinCounter()
{
	return levenshtein_counter;
}

int EditDistance::ukkonenCounter()
{
	return ukkonen_counter;
}

} // namespace Distiller

