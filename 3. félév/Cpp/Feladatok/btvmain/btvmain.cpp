#include <iostream>
#include <iterator>
#include <vector>
#include "btview.h"
#include <string>
#include <functional>
#include "btview.h"

const int max = 1000;

int main()
{
  int your_mark = 1;
  /* 2-es */
  std::vector<int> v;
  v.push_back( 1 );
  v.push_back( 9 );
  v.push_back( 8 );
  v.push_back( 7 );
  v.push_back( 5 );

  std::vector<int> m;
  for( int i = 0; i < max; ++i )
  {
    m.push_back( i );
  }

  std::vector<std::string> ls;
  ls.push_back( "Ada" );
  ls.push_back( "Python" );

  binary_tree_view<int> vv( v );
  binary_tree_view<int> mv( m );
  binary_tree_view<std::string> sv( ls );
  vv.root() = 14;
  mv.root() = max;
  sv.root() = "!!!";

  if( 14 == v[ 0 ] && max == m[ 0 ] && "!!!" == ls[ 0 ] && 9 == mv.height() )
  {
    your_mark = vv.height();
  }
 
  /* 3-as */
  vv.root().left().right() = 3;
  mv.root().right().right().left() = 4;
  mv.root().left().left().left() = 2;
  sv.root().left() = "Haskell";

  if( "Haskell" == ls[ 1 ] && 2 == m[ 7 ] && 4 == m[ 13 ] )
  {
    your_mark = v[ 4 ];
  }
 
  /* 4-es */
  sv.root().insert_right( "C" );
  sv.root().right() = "C++";
  sv.root().left().insert_right( "Kotlin" );

  vv.root().left().right().insert_left( 4 );

  if( "C++" == ls[ 2 ] && 5 == ls.size() && 0 == v[ 7 ] && 10 == v.size() &&
      3 == vv.height() && ls[ 3 ].empty() && "Kotlin" == ls[ 4 ] )
  {
     your_mark = v[ 9 ];
  }
 
  /* 5-os */
  vv.root().right().left() = 5;
  vv.root().right().right() = 2;
  vv.root().left().left().left() = 11;
  vv.root().left().left().right() = 6;
  vv.sort( std::greater<int>() );
  mv.sort();

  sv.root() = "Ada";
  sv.root().left().left() = "Java";
  sv.sort();

  if( "C++" == ls[ 1 ] && 512 == m[ 0 ] && 5 == ls.size() && 3 == v[ 2 ] )
  {
    your_mark = v[ 0 ];
  }
 
  std::cout << "Your mark is " << your_mark;
  std::endl( std::cout );
}