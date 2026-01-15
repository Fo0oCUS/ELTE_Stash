#include <iostream>
#include <vector>
#include "selforgl.h"
#include <algorithm>
#include <list>
#include <string>
#include "selforgl.h"

struct is_good_language
{

  bool operator()( const std::string& s ) const
  {
    return "C++" == s || "C" == s;
  }

};

const int max = 1000;

struct impossible
{

  bool operator()( int i ) const
  {
    return i > max;
  }

};

int main()
{
  int your_mark = 1;
  /* 2-es */
  std::list<int> lm;
  for( int i = 0; i < max; ++i )
  {
    lm.push_back( i );
  }
  std::list<int> li;
  li.push_back( 4 );
  li.push_back( 3 );
  li.push_back( 5 );
  li.push_back( 2 );
  std::list<std::string> ls;
  ls.push_back( "C++" );
  ls.push_back( "Haskell" );
  ls.push_back( "Python" );

  self_organizing_list<int>* p = new move_to_front<int>( li );
  self_organizing_list<int>* q = new move_to_front<int>( lm );
  self_organizing_list<std::string>* s = new move_to_front<std::string>( ls );

  if( !p->contains( max ) && p->contains( 5 ) && 5 == li.front() &&
      !q->contains( max ) && q->contains( max / 2 ) && max / 2 == lm.front() &&
      1 == std::count( lm.begin(), lm.end(), 0 ) && !s->contains( "ABAP" ) &&
      s->contains( "C++" ) && "C++" == ls.front() )
  {
    your_mark = li.back();
  }
  delete p;
  delete q;
  delete s;
  
  /* 3-as */
  p = new frequency_list<int>( li );
  p->contains( 5 );
  p->contains( 5 );
  p->contains( 5 );
  p->contains( 2 );
  p->contains( 3 );
  p->contains( 2 );
  q = new frequency_list<int>( lm );
  q->contains( max - 1 );
  q->contains( 3 );
  q->contains( 3 );
  q->contains( 3 );
  q->contains( max - 1 );
  s = new frequency_list<std::string>( ls );
  s->contains( "Haskell" );
  s->contains( "C++" );
  s->contains( "C++" );

  std::vector<int> v( li.begin(), li.end() );

  if( q->contains( max / 3 ) && !p->contains( max / 3 ) && "C++" == ls.front() &&
      1 == std::count( ls.begin(), ls.end(), "Python" ) && 5 == v[ 0 ] &&
      2 == v[ 1 ] && 3 == v[ 2 ] && 4 == v[ 3 ] )
  {
    your_mark = lm.front();
  }
  delete q;
  delete p;
  delete s;
  
  /* 4-es */
  std::swap( ls.front(), ls.back() );
  move_to_front<std::string> fs( ls );
  move_to_front<int> fi( lm );

  if( fs.contains_if( is_good_language() ) && "C++" == ls.front() &&
      !fi.contains_if( impossible() ) )
  {
    your_mark += fs.contains( "Python" );
  }
  
  /* 5-os */
  std::swap( ls.front(), ls.back() );
  frequency_list<std::string> afs( ls );
  afs.contains( "C++" );
  afs.contains_if( is_good_language() );
  afs.contains( "Python" );
  frequency_list<int> afi( lm );

  if( "C++" == ls.front() && afs.contains_if( is_good_language() ) &&
      !afi.contains_if( impossible() ) )
  {
    your_mark += afi.contains( 0 );
  }
  
  std::cout << "Your mark is " << your_mark;
  std::endl( std::cout );
}