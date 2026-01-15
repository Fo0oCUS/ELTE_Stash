#include <iostream>
#include "dcircbuf.h"
#include <vector>
#include <string>
#include "dcircbuf.h"
#include <deque>
#include <algorithm>

const int max = 1000;

int main()
{
  int your_mark = 1;
  /* 2-es */
  int a[ 12 ];
  double_circular_buffer<int> cbi( a, sizeof( a ) / sizeof( a[ 0 ] ) );
  for( int i = 0; i < max; ++i )
  {
    cbi.insert_A( i );
    cbi.insert_B( 2 * i );
  }

  std::string v[ 4 ];
  double_circular_buffer<std::string> cbs( v, sizeof( v ) / sizeof( v[ 0 ] ) );
  cbs.insert_B( "Hello" );
  cbs.insert_B( "World" );
  cbs.insert_B( "Szia" );

  if( 3 == a[ 3 ] && 2 == a[ 7 ] &&
      0 == std::count( v, v + sizeof( v ) / sizeof( v[ 0 ] ), "Szia" ) )
  {
    your_mark = std::count( v[ 2 ].begin(), v[ 2 ].end(), 'l' );
  }
 
  /* 3-as */
  cbi.read_A();
  cbi.read_A();
  cbi.read_B();
  cbi.read_A();
  cbi.read_B();
  cbi.read_A();
  cbi.read_A();
  cbi.read_A();
  for( int i = 0; i < max; ++i )
  {
    cbi.insert_A( 3 * i );
  }

  if( 18 == a[ 6 ] && 6 == a[ 9 ] && "Hello" == cbs.read_B() &&
      "World" == cbs.read_B() && ( cbs.insert_B( "!!" ), "!!" == v[ 2 ] ) )
  {
    your_mark = a[ 1 ];
  }
 
  /* 4-es */
  std::vector<int> vec( 8 );
  double_circular_buffer<int> vb( vec );
  vb.insert_A( 7 );
  vb.insert_B( 3 );

  int x[ 6 ];
  int y[ 10 ];

  double_circular_buffer<int> bx( x );
  bx.insert_B( 6 );
  double_circular_buffer<int> by( y );
  by.insert_B( 5 );

  if( 3 == vec[ 4 ] && 6 == x[ 3 ] && 6 == bx.read_B() && 5 == y[ 5 ] )
  {
    your_mark = vb.read_A() - vb.read_B();
  }

  /* 5-os */
  int ia[] = { 4, 2, 7 };
  std::deque<int> d;
  d.push_back( 8 );
  vb.insert_A( ia, ia + sizeof( ia ) / sizeof( ia[ 0 ] ) );
  vb.insert_B( d.begin(), d.end() );

  by.insert_A( d.begin(), d.end() );
  by.insert_B( ia, ia + sizeof( ia ) / sizeof( ia[ 0 ] ) );

  if( 4 == vec[ 1 ] && 8 == vec[ 5 ] && 7 == y[ 8 ] && 8 == y[ 0 ] )
  {
    your_mark = vec[ 3 ] - vec[ 2 ];
  }
 
  std::cout << "Your mark is " << your_mark;
  std::endl( std::cout );
}