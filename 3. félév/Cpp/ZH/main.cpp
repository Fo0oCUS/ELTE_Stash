#include <iostream>
#include <numeric>
#include <list>
#include "selforgl.h"
#include <algorithm>
#include <string>
#include "selforgl.h"

const int max = 1000;

struct is_even
{

  bool operator()( int i ) const
  {
    return 0 == i % 2;
  }

};

struct greater_max
{

  bool operator()( int i ) const
  {
    return i > max;
  }

};

struct is_good_language
{

  bool operator()( const std::string& s ) const
  {
    return "C" == s || "C++" == s;
  }

};


int main()
{
  int your_mark = 1;
  /* 2-es */
  std::list<int> c;
  c.push_back( 7 );
  c.push_back( 2 );
  c.push_back( 9 );
  c.push_back( 1 );

  std::list<int> d;
  for( int i = 0; i < max; ++i )
  {
    d.push_back( i );
  }

  std::list<std::string> langs;
  langs.push_back( "C" );
  langs.push_back( "Rust" );
  langs.push_back( "C++" );
  langs.push_back( "Erlang" );

  self_organizing_list<int>* p = new transpose<int>( c );
  self_organizing_list<int>* q = new transpose<int>( d );
  self_organizing_list<std::string>* r = new transpose<std::string>( langs );

  if( !q->contains( max ) && q->contains( 1 ) && 1 == d.front() &&
      r->contains( "Erlang" ) && "Erlang" != langs.back() && p->contains( 1 ) &&
      9 == c.back() && !p->contains( max / 2 ) &&
      1 == std::count( langs.begin(), langs.end(), "Rust" ) )
  {
    your_mark = langs.back().size() - langs.front().size();
  }
  delete p;
  delete q;
  delete r;
 
  /* 3-as */
  p = new move_to_back<int>( c );
  q = new move_to_back<int>( d );
  r = new move_to_back<std::string>( langs );
  if( r->contains( "C" ) && "C" == langs.back() && r->contains( "C++" ) )
  {
    your_mark = langs.back().size();
  }
  delete p;
  delete q;
  delete r;
 
  /* 4-es */
  move_to_back<int> mc( c );
  move_to_back<int> md( d );
  move_to_back<std::string> ms( langs );

  if( !md.contains_if( greater_max() ) && ms.contains_if( is_good_language() ) &&
      mc.contains_if( is_even() ) && 2 == c.back() )
  {
    your_mark = langs.size();
  }
 
  /* 5-os */
  transpose<int> tc( c );
  transpose<int> td( d );
  transpose<std::string> ts( langs );

  if( !td.contains_if( greater_max() ) && ts.contains_if( is_good_language() ) &&
      3 < langs.back().size() && tc.contains_if( is_even() ) &&
      19 == std::accumulate( c.begin(), c.end(), 0 ) )
  {
    your_mark = c.size() + ( c.back() % 2 );
  }
 
  std::cout << "Your mark is " << your_mark << std::endl;
}