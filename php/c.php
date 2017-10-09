<?php
$w = file_get_contents('..\input\const.txt');
$out_table = array();
$length = strlen($w);

//$k represents the index of the first letter of the word.
$k = 0;

//Counts the number of sentances.  If a period is found outside of a word,
//it is incremented.
$sentance_counter = 1;

//The queue contains a count of the number of periods in a word.
//If there is more than one period in a word, we'll assume it's an
//abbreviation and not the end of a sentance.
$queue_increase_sentance_counter = 0;

//symbols that can end a sentance.  Finding one will queue that the
//sentance counter be increased upon reaching the end of the word.
$sentance_ending_symbols = ['.', '?', '!'];

//Punctuation symbols that can separate words.
$sep = [' ', ',', "\n", "\r", '(', ')', '[', ']', '{', '}', ':', ';'];

//Other symbols that need to be removed from a word when found at the
//end, but neither split words nor sentances.
$other_trim_symbols = ['-', '=', "'", '"'];

//The length of the word. Is a variable for use during the loop to
//strip any punctuation from the ends of the word.
$word_length = 0;
//Main checking loop.
for( $i = 0; $i < $length; ++$i )
{
  if( in_array( $w[$i], $sentance_ending_symbols ) )
  {
    ++$queue_increase_sentance_counter;
  }

  //This is not an else if to make the period counting work properly.
  if( in_array( $w[$i], $sep ) )
  { 
    $word_length = $i - $k;
    $word_start = $k; //editable copy of $k for shortening the word.
    //Don't remove periods from abreviations with multiple periods.
    if( $word_length > 0 && $queue_increase_sentance_counter <= 1 )
    {
      while( in_array( $w[ $word_start + $word_length - 1 ], $sentance_ending_symbols )
        || in_array( $w[ $word_start + $word_length - 1 ], $sep )
        || in_array( $w[ $word_start + $word_length - 1 ], $other_trim_symbols ))
      {
        --$word_length;        
      }

      //and do the same from the front
      while( in_array( $w[ $word_start ], $sentance_ending_symbols )
        || in_array( $w[ $word_start ], $sep )
        || in_array( $w[ $word_start ], $other_trim_symbols ))
      {
        ++$word_start;
        --$word_length;        
      }
    }

    //Add the word to the array.
    if( $word_length > 0 )
    {
      //Store the word for readability.
      $word = substr( $w, $word_start, $word_length );
      
      //In PHP having this if statement isn't necessary due to 
      // the automatic typing, however it's here for clarity and
      // ease of conversion to C.
      if( !isset( $out_table[ $word ] ) )
      {
        $out_table[ $word ][ 'count' ] = 1;
        $out_table[ $word ][ 'sent' ][] = $sentance_counter;
      }
      else
      {
        ++$out_table[ $word ][ 'count' ];
        $out_table[ $word ][ 'sent' ][] = $sentance_counter;
      }

      //echo '"', $word, "\"\n";
    }
    
    
    //Increment the sentance if needed.
    if( $queue_increase_sentance_counter == 1 )
    {
      ++$sentance_counter;
    }

    //restore the word counting state.
    $queue_increase_sentance_counter = 0;
    $k = $i+1;
  }

  //make the letter lowercase if it's a letter.
  //in C this will be quicker by using if( $w[$i] >= 'A' && $w[$i] <= 'Z' )...
  $w[$i] = strtolower($w[$i]);
}

//Now print everything.
$sorted_keys = array_keys( $out_table );
asort( $sorted_keys );
foreach( $sorted_keys as $sk )
{
  echo $sk, "\t\t\t{", $out_table[$sk]['count'], ':';
  $comma_bool = false;
  foreach( $out_table[$sk]['sent'] as $s )
  {
    if( $comma_bool )
    {
      echo ',';
    }
    else
    {
      $comma_bool = true;
    }
    echo $s;
  }
  echo "}\n";
}
?>
