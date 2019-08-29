#ifndef DISTILLER_EDITDISTANCE_H
#define DISTILLER_EDITDISTANCE_H

#pragma once

namespace Distiller
{

class SimpleString;

class EditDistance
{

public:	

	enum MatchType {
		ExactMatch,
		SubstringMatch
	};

	/**
	 * Returns the edit distance between pattern and text.
	 * If matchType == SubstringMatch, the pattern can be surrounded
	 * by other text. Otherwise the pattern is assumed to be
	 * a misspelled version of the whole text.
	 *
	 */
	static int calc(const SimpleString& pattern,
					const SimpleString& text, 
					MatchType matchType = ExactMatch);

	static int calc(const QString& pattern, 
					const QString& text, 
					MatchType matchType = ExactMatch);

	/**
	 * Optimized version of calc.
	 *
	 * If there are more errors than maxTypos not the whole distance
	 * is computed. Instead the function returns a value greater than
	 * maxTypos and the computation is aborted. This is sufficient
	 * for most applications.
	 *
	 * if (calc(pattern, text, 5) > 5)
	 *     // too many errors, abort
	 *
	 * \return Returns the edit distance if it is at most maxTypos,
	 * otherwise it returns a value greater than maxTypos. 
	 */
	static int calc(const SimpleString& pattern,
					const SimpleString& text, 
					uint maxTypos,
					MatchType matchType = ExactMatch);

	static int calc(const QString& pattern, 
					const QString& text, 
					uint maxTypos,
					MatchType matchType = ExactMatch);
	/**
	 * Calculated the Levenshtein distance.
	 */
	static int Levenshtein(const SimpleString& pattern,
						   const SimpleString& text, 
						   MatchType matchType = ExactMatch);

	static int Levenshtein(const QString& pattern, 
						   const QString& text, 
						   MatchType matchType = ExactMatch);

	/**
	 * An optimized version of the Levenshtein distance.
	 *
	 * You can specifiy an upper bound for the editing operations.
	 *
	 * Returns the edit distance if it smaller or equal than
	 * maxTypos. Otherwise it returns a undefined value greater than
	 * mayTypos.
	 *
	 * Use it like this:
	 *
	 * if (Ukkonen(pattern, text, 5) > 5)
	 *     // too much errors
	 */
	static int Ukkonen(const SimpleString& pattern,
					   const SimpleString& text, 
					   uint maxTypos,
					   MatchType matchType = ExactMatch);

	static int Ukkonen(const QString& pattern, 
					   const QString& text, 
					   uint maxTypos,
					   MatchType matchType = ExactMatch);

	/**
	 * Returns the edit distance of pattern to text.
	 *
	 * Needs O(len(pattern)) space.
	 */
	static int Levenshtein_exact(const SimpleString& pattern,
								 const SimpleString& text);

	/**
	 * Returns the smallest edit distance of a substring
	 * of text to pattern.
	 *
	 * Example:
	 * text:    "GAAGCTAGGATCGA"
	 * pattern:     "CTGGGA"
	 * Returns: 1
	 *
	 * Needs O(len(pattern)) space.
	 */
	static int Levenshtein_substring(const SimpleString& pattern,
									 const SimpleString& text);

	/**
	 * An optimized version of the Levenshtein distance.
	 * Optimizied in both space and time consumption.
	 * 
	 * Needs O(len(pattern)) space.
	 */
	static int Ukkonen_exact(const SimpleString& aPattern, 
							 const SimpleString& aText,
							 uint maxTypos);
					   
	/**
	 * An optimized version of the Levenshtein distance.
	 * 
	 * Returns the smallest edit distance of a substring
	 * of text to pattern.
	 *
	 * Example:
	 * text:    "GAAGCTAGGATCGA"
	 * pattern:     "CTGGGA"
	 * Return value: 1
	 *
	 * text:    "GTCCCGTAAGAGA"
	 * pattern:       "TAAAAAC"
	 * maxTypos: 2
	 * Return value: > 2
	 *
	 * If it can be estimated that the edit distance is 
	 * greater than maxTypos, the function returns a value
	 * greater than maxTypos.
	 *
	 * Needs O(len(pattern)) space.
	 */
	static int Ukkonen_substring(const SimpleString& aPattern,
								 const SimpleString& aText, 
								 uint maxTypos);
								 
	static int Stettner_exact(const QString& pattern,
							  const QString& text,
							  uint maxTypos);

	static int Stettner_exact(const SimpleString& pattern,
							  const SimpleString& text,
							  uint maxTypos);
								 
	/**
	 * Used for performance testing.
	 */
	static void resetLevenshteinCounter();
	
	/**
	 * Used for performance testing.
	 */
	static void resetUkkonenCounter();
	
	/**
	 * Used for performance testing.
	 */
	static int levenshteinCounter();
	
	/**
	 * Used for performance testing.
	 */
	static int ukkonenCounter();
	
private:
	EditDistance();
	
	/**
	 * Used for performance testing.
	 */
	static int levenshtein_counter;

	/**
	 * Used for performance testing.
	 */
	static int ukkonen_counter;

}; // class EditDistance

} // namespace Distiller

#endif 

