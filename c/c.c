#include <stdio.h>
#include <malloc.h>

#define MAX_WORD_LENGTH 255

//BUG: "..." at the end of a word is not stripped when counting the word.
/*
Discussion:
A very fast method of creating a concordance, in close to O(n) time where 
  n is the number of bytes in the document to be examined.

The document is read, and the words are split, accounting for acronyms
 (words with periods in the middle) being words.  Each word is added
 to the trie if it does not exist, and the count is increased.

Each trie node also contains a linked list containing a list of sentances
where each word appears.
*/
const char * filename = "../input/test2.txt";

//A LinkedList Node used for storing the list of sentences that 
//contain the word.  Doublely-Linked allows us to traverse the list
//in reverse order -- so that the list appears in increasing order
//without needing to be sorted.
struct ll_node
{
  int sentance_number;
  struct ll_node * next;
//  struct ll_node * prev;
};

//A trie will be used to hold the word list.
struct trie_node{
  int counter;
  struct ll_node * first;
  struct ll_node * last;
  struct trie_node* children[255];
};

//This will hold the root trie node.  It is not to be edited once
//initialized.
struct trie_node * root;

/*
  Utility function to help printing the trie. 

  Depth is used to deal with adjusting word.
*/
void print_trie( struct trie_node * node, char * word, int depth )
{
  int i;
  if( node->counter > 0 )
  {
    printf("%.*s%s\t\t{%d", depth, word,
      (depth > 7)?"":"\t", node->counter );
    //read the linked list
    printf("}\n");
  }
  
  for( i = 0; i < 255; ++i )
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
  char * word = malloc( MAX_WORD_LENGTH );
  int word_length = 0;

  int sentance_counter = 1;
  int queue_increase_sentance_counter = 0;

  char sentance_ending_symbols[] = {'.', '?', '!'};
  char sep[] = {' ', ',', '\n', '\r', '(', ')', '[', ']', '{', '}', ':', ';'};
  char other_trim_symbols[] = {'-', '=', '\'', '\"'};

  /*
    To speed up the C implementation of the sep, we will convert it to 
    a map, so we can do an O(1) lookup.  This would take up more memory
    in a unicode space, but would still work.
  */
  char sep_map[255];
  char sentance_ending_map[255];
  char all_trim_symbols_map[255];
  
  //temporary variables
  int i = 0;
  struct trie_node * ct;

  //clear the arrays.
  for(i = 0; i < 255; ++i) 
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

  do{
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
    //printf("%c", c);
  }while (c != EOF );

  //Now print out the contents of the trie.
  //We will reuse the word buffer.
  word = "---------------";
  print_trie( root, word, 0 );
  printf("Done.\n");
  free( word );
}
