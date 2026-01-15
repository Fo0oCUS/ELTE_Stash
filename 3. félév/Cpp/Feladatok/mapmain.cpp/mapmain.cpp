#include <iostream>
#include <map>
#include "mapaccpart.h"
#include <string>
#include "mapaccpart.h"
#include <functional>

struct string_size_less
{

  bool operator()( const std::string& lhs,
                   const std::string& rhs ) const
  {
    return lhs.size() < rhs.size();
  }

};

struct track_length
{

  track_length( int m = 0, int s = 0 ): min( m ), sec( s ) { }

  int min;
  int sec;

};

track_length operator+( const track_length& lhs,
                        const track_length& rhs )
{
  track_length ret = lhs;
  ret.sec += rhs.sec;
  if( ret.sec >= 60 )
  {
    ret.sec -= 60;
    ++ret.min;
  }
  ret.min += rhs.min;
  return ret;
}

const int max = 1000;

int main()
{
  int your_mark = 1;
  /* 2-es */
  std::map<int, int> m;
  for( int i = 0; i < max; ++i )
  {
    m[ 2 * i ] = i;
  }

  std::map<std::string, track_length> pl;
  pl[ "1st track" ] = track_length( 5, 46 );
  pl[ "2nd track" ] = track_length( 4, 25 );
  pl[ "3rd track" ] = track_length( 4, 51 );
  pl[ "4th track" ] = track_length( 5, 22 );

  map_accumulate_partition<int, int> mp( m );
  mp.set_partition_key( 20 );
  mp.set_partition_key( 50 );
  mp.set_partition_key( 30 );

  map_accumulate_partition<std::string, track_length> plp( pl );
  plp.set_partition_key( "4th track" );
  plp.set_partition_key( "Unknown" );

  if( 10 == pl[ "2nd track" ].min && max / 2 == m[ max ] && max == m.size() &&
      4 == pl.size() && 6 == m[ 6 ] && 10 == m[ 8 ] && 5 == pl[ "4th track" ].min )
  {
    your_mark = pl[ "3rd track" ].sec;
  }
 
  /* 3-as */
  plp--;
  mp++;
  if( max == m.size() && 4 == pl[ "3rd track"].min && 11 == pl[ "2nd track" ].sec &&
      16 == m[ 32 ] )
  {
    your_mark = m[ 4 ];
  }
 
  /* 4-es */
  ++++mp;
  std::map<std::string, int, string_size_less> ls;
  ls[ "Haskell" ] = 5;
  ls[ "Brainfuck" ] = 6;
  ls[ "Kotlin" ] = 7;
  ls[ "C" ] = 1;
  ls[ "Lua" ] = 3;
  map_accumulate_partition<std::string, int, string_size_less> lsp( ls );
  lsp.set_partition_key( "Fortran" );
  --lsp;

  if( 5 == ls.size() && max == m.size() && 18 == m[ 36 ] )
  {
    your_mark = ls[ "C++" ];
  }
 
  /* 5-os */
  std::map<std::string, int, string_size_less> mls;
  mls[ "Ada" ] = 1;
  mls[ "Agda" ] = 3;
  mls[ "Delphi" ] = 8;
  mls[ "C#" ] = 5;
  mls[ "C" ] = 1;
  map_accumulate_partition<std::string,
                           int,
                           string_size_less,
                           std::multiplies<int> > mlsp( mls );
  mlsp.set_partition_key( "Pascal" );
  --mlsp;
  if( 5 == mls.size() )
  {
    your_mark = mls[ "C++" ];
  }
 
  std::cout << "Your mark is " << your_mark;
  std::endl( std::cout );
}