# Implementation of a fuzzy dictionary

Implementation of a dictionary with approximate 
string lookup.

We use a q-Gram approach (see [1]) together with an optimized
version of an algorithm that checks if a pattern P is at most
k edit steps away from the dictionary entry X (see [2]).

## Looking up a pattern in the dictionary

If a pattern occurs in the text with k errors, and if we cut
the pattern in k + 1 pieces arbitrarily, then at least one of 
the pieces must be present in the occourence with no errors.

Example:

Let's say we have the pattern P="a.cd.fghi" and the original
text X=("abcdefghi") and we're allowing at most 2 errors in the
pattern. If we split the pattern in 3 (or more) parts, then at
leat 1 part of it must also occour unaltered in the original
text X.

In our example we're splitting the pattern into "a.c", "d.f",
and "ghi". Instantly we see, that "ghi" is part of the original
text while the other parts are not.

For a given pattern P we're allowing an error every n characters.
So k is calculated with formula (1)

    k = (length(P) / n) + 1                                 (1)

And P is splitted into substrings p[1]..p[k] with length 

    length(p[i]) = length(P) / k .                          (2)

The substrings are then trimmed to have at least minGramSize and 
at most maxGramSize characters. The default values are:

    minGramSize = 2
    maxGramSize = 4

Then we're looking up our dictionary and retrieve all entries for
the substrings. This will a return a lot of entries which are not
suitable but much less if we would had to check every entry of the
dictionary. We check if the edit distance between the found entry
and the pattern is at least k, which reduces the entries again.

In the last step we're returning the entry with the smallest distance
to the pattern.

## Building the dictionary

For every entry in our dictionary we calculate all q-grams and 
relate them to the position of the entry.

Example:

Let's say we have the entry "gammas" and it's the 15th entry.
Then we calculate all q-grams (of length maxGramSize) and relate then
to the position 15.

    "gamm" --> 15
    "amma" --> 15
    "mmas" --> 15

Since it's possible that the splits of our pattern are smaller than
maxGramSize, we're also storing the suffixes of the q-grams down to
minGramSize:

    "gam"  --> 15
    "ga"   --> 15
    "amm"  --> 15
    "am"   --> 15
    "mma"  --> 15
    "mm"   --> 15

So, all in all we have to save 8 grams for our entry.

We have to take into account that other entries of the dictionary 
could have the same q-grams, so after processing all entries our 
lookup-table could look like this:

    "gamm" --> 15, 20                                       (3)
    "amma" --> 15, 20, 3
    "mmas" --> 15, 6

And also the suffixes point to (at least!) those entries:

    "gam"  --> 15, 20
    "ga"   --> 15, 20, 9*
    "amm"  --> 15, 20, 3, 4*, 24*
    "am"   --> 15, 20, 3, 4*, 24*
    "mma"  --> 15, 6, 7*
    "mm"   --> 15, 6, 7*, 23*, 8*

All suffixes marked with a are also part of q-grams that are 
not listed in (3), such as "gast", "ammo", "mmat" or "mmic", that
are also part of our dictionary.

It would be a waste of space if all lists would be stored seperately
because they are highly redundant. Only the lists of the grams of
length maxGramSize are unique while the lists of the suffixes are 
concatenations of the lists of their longer counterparts.

This is our first optimization and it looks like this:

    GramHash   GramNodes        Containers

    "gamm" --> [#1]             #1: [15, 20]
    "amma" --> [#2]             #2: [15, 20, 3]
    "mmas" --> [#3]             #3: [15, 6]
    "gam"  --> [#1]             #4: [9]
    "ga"   --> [#1, #4]         #5: [4, 24]
    "amm"  --> [#2, #5]         #6: [7]
    "am"   --> [#2, #5]         #7: [23, 8]
    "mma"  --> [#3, #6]  
    "mm"   --> [#3, #6, #7]

We can do another optimization suggested by [1].  If we split our
pattern into k pieces, each maxGramSize long, but our text T is
longer that k maxGramSize characters we can choose our parts
with length(T) - k maxGramSize degrees of freedom.  We should
choose the splits to minimize the amount of words we have to check
later.

    GramHash   GramNodes        Containers

    "gamm" --> 2, [#1]          #1: [15, 20]
    "amma" --> 3, [#2]          #2: [15, 20, 3]
    "mmas" --> 2, [#3]          #3: [15, 6]
    "gam"  --> 2, [#1]          #4: [9]
    "ga"   --> 3, [#1, #4]      #5: [4, 24]
    "amm"  --> 5, [#2, #5]      #6: [7]
    "am"   --> 5, [#2, #5]      #7: [23, 8]
    "mma"  --> 4, [#3, #6]  
    "mm"   --> 5, [#3, #6, #7]

This what our data structure looks like in general:

GramHash stores a GramNode for each gram and its suffixes.

GramNode stores a list of pointers to Containers and a counter
variable to store the accumulated length of all Containers it
points to.

## Fine grain checks

After finding all entries we apply an optimized version of
Ukkonen's distance as described in [2] to find the best match.

## References

1) G. Navarro, R. Baeza-Yates: A Practical q-Gram Index for
   Text Retrieval Allowing Errors. 
2) E. Ukkonen (1985): Finding Approximate Patterns in Strings.
   Journal of Algorithms 6, 132--137.

