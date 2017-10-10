Concordance Code Demo

Demonstration code that, given an input file of text in English, 
generates a concordance (a list of all words, the number of times they
appear, and which sentences the word appears in).

The final implementation is in C, (c.c).  A prototype implementation
was created in PHP to test the basic functionality of the algorithm,
although the PHP version takes advantage of the fact that all PHP arrays
are hash tables and does not implement the trie.

The C implementation implements a Trie data structure.  The trie allows
for O(m) accesses and adds, where m is the length of the word being
looked up.  Algorithmically, a trie may be slightly faster than a hash
table depending on the hashing algorithm used, and how collisions in a 
hash table are dealt with, however on modern processors, the trie may 
actually be slightly slower due to it being unfriendly to the CPU cache,
requiring a large amount of cache fills from RAM.  

I felt that implementing the trie provided a better programming
demonstration, as it shows implementation of a specific data structure
to meet a particular problem.  Each node of the trie also contains a 
linked list used to store the list of sentences where a word appears.

The trie nodes also contain a pointer to the last linked list node to
allow for O(1) additions to the list, while still allowing the list to
be traversed from the front to allow the sentence numbers to be printed
in order.

Compilation instructions - c:
The code is in a single file and has no external dependencies beyond the
C Standard IO, and Malloc.  It has been tested with GCC and Fabrice 
Bellards Tiny C Compiler on Windows and OpenBSD.  To compile with GCC:

gcc c.c

Other discussion:
There are three lists of characters that are searched for when reading
the text - sentence ending characters ('.', '?', and '!' in English), 
characters that can separate words (white space, parentheses, commas, 
colons, etc.), and other symbols that may be attached to words, but 
need to be removed to allow comparison to other words (dashes, quotes).

These symbols are stored in three "maps" -- arrays where the index 
corresponds to a relevant ASCII character, and the value corresponds to
whether that character is in the set, to allow for O(1) array lookups at
the expense of about 768 bytes of memory (there are three maps). 

The sentence ending characters are not used to split words.  A count of
the number of sentence ending characters in each word is counted.  If
there is exactly one found, it is assumed that this symbol was intended
to separate sentences.  If there are multiple such symbols found, it is
assumed that the word is an acronym, and the periods are not used to 
separate sentences.  No attempt is made to determine if an acronym is
at the end of a sentence, as this would have been unnecessarily 
complicated.  This issue also arises if a word ends in an ellipsis - 
an ellipsis does not necessarily need to appear at the end of a sentence,
but they occasionally do appear at the end of sentences.

The buffer for a word has a default length of 255, although this can be
changed to any length without an issue.  A word that exceeds the length
of the buffer will not be counted in the trie.  

The runtime of the algorithm to build the trie and linked list should be
roughly O(n * m) time, where n is the length of the document in characters,
and m is the average length of a word.  The m comes from inserting the 
words into the trie.  Since the average length of a word is derived by 
counting the number of characters and dividing by the number of words, 
this comes out close to O(2n).  Some characters will not be added to the
trie, and additional operations are done to remove undesirable characters
at the ends of words (e.g. dashes). 

Printing the concordance at the end is O(n * m), where n is the number
of nodes in the trie, which in worst case, is equal to the length of all
unique words combined (assuming there is no overlap at the beginning of 
the words).  Given the fact that there are relatively few unique words,
and many words have the same prefix, this will usually be even smaller.

During the printing time, there will be an O(n) (total) operation, where n
is the number of actual words in the document, to print all of the 
sentence numbers from the linked list nodes, since there is a linked list
node for every actual word that appears in the document.

In total, this comes out to a linear time complexity.
