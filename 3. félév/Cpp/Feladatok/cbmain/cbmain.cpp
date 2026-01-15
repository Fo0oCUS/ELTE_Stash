#include <iostream>
#include "circbuf.h"
#include <string>
#include <vector>
#include "circbuf.h"
#include <algorithm>

const int max = 1000;

int main()
{
  int your_mark = 1;
  /* 2-es */
  char s[ 4 ];
  circular_buffer<char> scb( s, sizeof( s ) / sizeof( s[ 0 ] ) );
  scb.insert( 'H' );
  scb.insert( 'e' );
  scb.insert( 'l' );
  scb.insert( 'l' );
  scb.insert( 'o' );

  int a[ 6 ];
  circular_buffer<int> acb( a, sizeof( a ) / sizeof( a[ 0 ] ) );
  for( int i = 0; i < max; ++i )
  {
    acb.insert( i );
}

std::string ls[ 4 ];
circular_buffer<std::string> lcb( ls, sizeof( ls ) / sizeof( ls[ 0 ] ) );
lcb.insert( "C++" );
std::cout << lcb.size() << std::endl;
  if( 'e' == s[ 1 ]  && 'H' == s[ 0 ] && 4 == scb.size() && 6 == acb.size() &&
      "C++" == ls[ 0 ] && 1 == lcb.size() && 5 == a[ 5 ] && 0 == a[ 0 ] &&
      0 == std::count( s, s + sizeof( s ) / sizeof( s[ 0 ] ), 'o' ) )
  {
    your_mark = std::count( s, s + sizeof( s ) / sizeof( s[ 0 ] ), 'l' );
  }
  
  /* 3-as */
  scb.read();
  scb.insert( 'o' );

  for( int i = 0; i < max; ++i )
  {
    acb.read();
    acb.insert( i );
  }

  if( 0 == std::count( s, s + sizeof( s ) / sizeof( s[ 0 ] ), 'H' ) &&
      'o' == s[ 0 ] && 'e' == scb.read() && 'l' == scb.read() &&
      2 == scb.size() && "C++" == lcb.read() && 0 == lcb.size() &&
      max - 1 == a[ 3 ] && max - 5 == a[ 5 ] && 4 == scb.capacity() )
  {
    scb.insert( '!' );
    your_mark = scb.size();
  }
 
  /* 4-es */
  int x[ 16 ];
  int y[ 8 ];
  circular_buffer<int> xcb( x );
  circular_buffer<int> ycb( y );
  xcb.insert( 2 );
  ycb.insert( 4 );

  std::vector<std::string> v( 4 );
  circular_buffer<std::string> vcb( v );
  vcb.insert( ":-)" );

  if( 16 == xcb.capacity() && 8 == ycb.capacity() && 1 == xcb.size() &&
      2 == x[ 0 ] && 1 == vcb.size() && 4 == vcb.capacity() && ":-)" == v[ 0 ] )
  {
    your_mark = ycb.read();
  }
 
  /* 5-os */
  int iax[ 2 ];
  iax[ 0 ] = iax[ 1 ] = 0;
  circular_buffer<bool> b( iax );
  b.insert( true );
  b.insert( false );
  b.insert( true );
  b.insert( false );
  int iay[] = { 0, 0, 0 };
  circular_buffer<bool> c( iay );
  for( int i = 0; i < max; ++i )
  {
    c.insert( true );
  }

  if( b.read() && !b.read() && b.read() && !b.read() && b.size() == 0 &&
      3 == std::count( iay, iay + sizeof( iay ) / sizeof( iay[ 0 ] ), -1 ) &&
      96 == c.size() && b.capacity() != c.capacity() )
  {
    your_mark = iax[ 0 ];
  }
 
  std::cout << "Your mark is " << your_mark << std::endl;
}