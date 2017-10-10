#include <stdio.h>
#include <malloc.h>

#define MAX_WORD_LENGTH 255
const char * filename = "../input/test2.txt";

//BUG: "..." at the end of a word is not stripped when counting the word.
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
  
  int i = 0;

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
      printf("$");
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
      printf("\"%.*s\"\n", word_length, word);
      word_length = 0;

      if( queue_increase_sentance_counter == 1 )
      {
        printf("NEW sentence\n");
      }

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

  printf("Done.\n");
  free( word );
}
