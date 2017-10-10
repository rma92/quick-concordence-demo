#include <stdio.h>
#include <malloc.h>

#define MAX_WORD_LENGTH 980
#define CHARSET_SIZE 255

//BUG: "..." at the end of a word is not stripped when counting the word.
/*
Discussion:
A very fast method of creating a concordance, in close to O(n) time where 
 n is the number of characters in the document to be examined.  The actual
 time complexity is approximately O(n * m), where n is the number of
 characters in the document and m is the average length of each word.
 (There is some overhead for dealing with unusual punctuation.)

To increase the performance, checking for punctuation is done using arrays
 that map to every ASCII character, allowing for a check in O(1) time.

The document is read, and the words are split, accounting for acronyms
 (words with periods in the middle) being words.  If a word has not yet
 been seen, it is added to the trie.  Otherwise it is found in the trie,
 and the counter increased.  The current sentence number is also added 
 to a Linked List that is associated with each unique word in the trie.

This implementation expects an ASCII character set (up to 255 characters),
 however, it could fairly easily be modified to work with wide characters.
 More memory would be used, and startup would be slower, but the speed
 should remain similar, aside from additional time for memory allocation
 being required.

Known issues:
 An ellipsis does not count as an end of a sentence.  While an ellipsis
  does not necessarily need to be the end of a sentence, no method of
  attempting to determine whether an ellipsis is the end of a sentence
  or not has been implemented.  The same is true for acronyms that contain
  multiple periods. 
 An abbreviation that ends in a period will not include the period when
  it is added to the table.  This would only affect the count if the 
  abbreviation happens to match a word, and the period is required to 
  tell the difference.

*/
const char * filename = "../input/war_and_peace.txt";

//A LinkedList Node used for storing the list of sentences that 
//contain the word.  Doublely-Linked allows us to traverse the list
//in reverse order -- so that the list appears in increasing order
//without needing to be sorted.
struct ll_node
{
  int sentance_number;
  struct ll_node * next;
};

//A trie will be used to hold the word list.
struct trie_node{
  int counter;
  struct ll_node * first;
  struct ll_node * last;
  struct trie_node* children[ CHARSET_SIZE ];
};

//This will hold the root trie node.  It is not to be edited once
//initialized.
struct trie_node * root;

/*
  Recursive utility function to help printing the trie. 

  node is the trie_node to print from.
  word is a buffer that is used for printing the current word.
  Depth is used to deal with adjusting word.
*/
void print_trie( struct trie_node * node, char * word, int depth )
{
  int i;
  struct ll_node* lln;
  if( node->counter > 0 )
  {
    printf("%.*s%s\t\t{%d:", depth, word,
      (depth > 7)?"":"\t", node->counter );
    //read the linked list
    lln = node->first;
    do{
      printf("%s%d", (lln==node->first)?"":",", lln->sentance_number);
      lln = lln->next;
    }while( lln != NULL );
    printf("}\n");
  }
  
  for( i = 0; i < CHARSET_SIZE; ++i )
  {
    if( node->children[i] != NULL )
    {
      word[depth] = (char)i;
      print_trie( node->children[i], word, depth+1);
    }
  }
}
int main()
{
  //file pointer
  FILE *ifp;

  //the char we are reading right now.
  char c;
  unsigned char * word = malloc( MAX_WORD_LENGTH );
  int word_length = 0;

  int sentance_counter = 1;
  int queue_increase_sentance_counter = 0;

  char sentance_ending_symbols[] = {'.', '?', '!'};
  char sep[] = {' ', ',', '\n', '\r', '(', ')', '[', ']', '{', '}', ':', ';', EOF};
  char other_trim_symbols[] = {'-', '=', '\'', '\"'};

  /*
    To speed up the C implementation of the sep, we will convert it to 
    a map, so we can do an O(1) lookup.  This would take up more memory
    in a unicode space, but would still work.
  */
  char sep_map[CHARSET_SIZE];
  char sentance_ending_map[CHARSET_SIZE];
  char all_trim_symbols_map[CHARSET_SIZE];
  
  //temporary variables
  int i = 0;
  struct trie_node * ct;

  //clear the arrays.
  for(i = 0; i < CHARSET_SIZE; ++i) 
  {
    sep_map[i] = 0;
    sentance_ending_map[i] = 0;
    all_trim_symbols_map[i] = 0;
  }
  //now prepare the maps.
  for(i = 0; i < sizeof( sentance_ending_symbols ); ++i )
  {
    sentance_ending_map[ sentance_ending_symbols[i] ] = 1;
    all_trim_symbols_map[ sentance_ending_symbols[i] ] = 1;
  }
  for(i = 0; i < sizeof( sep); ++i )
  {
    sep_map[ sep[i] ] = 1;
    all_trim_symbols_map[ sep[i] ] = 1;
  }
  for(i = 0; i < sizeof( other_trim_symbols ); ++i )
  {
    all_trim_symbols_map[ other_trim_symbols[i] ] = 1;
  }

  //prepare the trie.
  root = (struct trie_node*) calloc( 1, sizeof( struct trie_node ) );

  //Open file
  ifp = fopen( filename, "r" );

  if( ifp == NULL )
  {
    fprintf(stderr, "Can't open file.");
  }

  while( c != EOF )
  {
  //do{
    c = fgetc( ifp );
    //Check for end of sentance.
    if( sentance_ending_map[ c ] )
    {
      ++queue_increase_sentance_counter;
    }

    //Check to see if we are at the end of a word.
    if( sep_map[ c ] )
    {
      //Remove any undesirable trailing characters.
      //But don't remove periods from abbreviations. 
      if( word_length > 0 && queue_increase_sentance_counter <= 1 )
      {
        while( all_trim_symbols_map[ word[word_length - 1] ] )
        {
          --word_length;
        }
      }
  
      //Add the word.
      if( word_length > 0 )
      {
        i = 0;
        ct = root;
        do{
          if( ct->children[ word[i] ] == NULL )
          {
            ct->children[ word[i] ] = 
              (struct trie_node*) calloc( 1, sizeof( struct trie_node ) );
          }
          ct = ct->children[ word[i] ];
          ++i;
        }while( i < word_length );
        ++( ct->counter );
        //Add an element to the LinkedList
        if( ct->last == NULL )
        {
          ct->last = calloc( 1, sizeof( struct ll_node ) );
          ct->first = ct->last;
        }
        else
        {
          ct->last->next = calloc( 1, sizeof( struct ll_node ) );
          ct->last = ct->last->next;
        }
        ct->last->sentance_number = sentance_counter;

        //printf("\"%.*s\"\n", word_length, word);
      }//if( word_length > 0 )
      if( queue_increase_sentance_counter == 1 )
      {
        ++sentance_counter;
      }
      word_length = 0;
      queue_increase_sentance_counter = 0;
    }
    else
    {
      /*
        Add the character to the word, but only
        -if the word will not have a buffer overflow
        -we're at the beginning of the word, and the character is a
         separator character.
      */
      if( word_length < MAX_WORD_LENGTH )
      {
        if( !(word_length == 0 && all_trim_symbols_map[c] ) )
        {
          if( c <= 'Z' && c >= 'A' )
          {
            word[ word_length ] = c | 32;
          }
          else
          {
            word[ word_length ] = c;
          }
          ++word_length;
        }
      }
      else
      {
        fprintf( stderr, "There is a word longer than the maximum word length.");
      }
    }
  }//end of main while loop

  //Now print out the contents of the trie, reusing the word buffer.
  print_trie( root, word, 0 );
  printf("Done.\n");
}
