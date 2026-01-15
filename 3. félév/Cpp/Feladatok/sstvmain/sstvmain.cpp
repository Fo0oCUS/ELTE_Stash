#include <string>
#include <list>
#include <functional>
#include <iostream>
#include "sstview.h"
#include <set>
#include <vector>
#include <stack>
#include "sstview.h"

struct string_size_less
{

  bool operator()( const std::string& lhs,
                   const std::string& rhs ) const
  {
    return lhs.size() < rhs.size();
  }

};

const int max = 1000;

int main()
{
  int your_mark = 1;
  /* 2-es */
  std::set<int> s;
  s.insert( 8 );
  set_stack_view<int> sv( s );
  sv.push( 1 );
  sv.push( 3 );
  sv.push( 1 );

  
  
  std::set<std::string> n;
  n.insert( "Haskell" );
  set_stack_view<std::string> nv( n );
  nv.push( "C++" );
  nv.push( "C" );
  nv.push( "C++" );
  nv.push( "C++" );
  nv.push( "Ada" );
  
  std::set<int> a;
  {
      a.insert( 3 );
      set_stack_view<int> av( a );
      for( int i = 0; i < max; ++i )
      {
          av.push( 4 );
      }
  }
    
std::cout << a.count(3) << std::endl;
  if ( 3 == sv.top() && 2 == s.size() && 0 == s.count( 8 ) && 3 == n.size() &&
       "Ada" == nv.top() && 1 == n.count( "C++" ) && 0 == n.count( "Haskell" ) )
  {
    your_mark = a.count( 3 ) + a.size();
  }
 
  /* 3-as */
  std::set<std::string> ns;
  ns.insert( "C" );
  ns.insert( "C++" );
  nv += ns;
  nv.pop();

  std::set<int> mi;
  for( int i = 0; i < max; ++i )
  {
    mi.insert( i );
  }
  set_stack_view<int> av( a );
  av += mi;
  av.pop();

  sv += mi;
  for( int i = 0; i < max - 2; ++i )
  {
    sv.pop();
  }

  if( "C" == nv.top() && 2 == n.size() && 0 == s.count( 5 ) && 2 == s.size() &&
      max - 1 == a.size() && max - 2 == av.top() )
  {
    your_mark = sv.top();
  }
 
  /* 4-es */
  std::set<std::string, string_size_less> sstl;
  set_stack_view<std::string, string_size_less> vsl( sstl );
  vsl.push( "Python" );
  vsl.push( "Cobol" );
  vsl += ns;

  std::set<int, std::greater<int> > sg;
  for( int i = 0; i < max; ++i )
  {
    sg.insert( i );
  }
  std::set<int> sisg;
  set_stack_view<int> sisgv( sisg );
  sisgv += sg;

  if( 1 == sstl.count( ":-)" ) && 1 == sstl.count( "WTF???" ) &&
      max == sisg.size() && 1 == sisg.count( 1 ) )
  {
    your_mark = sstl.size();
  }
 
  /* 5-os */
  std::stack<std::string, std::list<std::string> > sta = vsl;

  std::set<int> si;
  set_stack_view<int> siv( si );
  std::list<int> c;
  c.push_back( 3 );
  c.push_back( 6 );
  siv.push( c.begin(), c.end() );
  std::stack<int, std::vector<int> > stvi = siv;

  int ar[] = { 7, 2, 5, 3, 5, 1 };
  std::set<int> ssi;
  set_stack_view<int> ssiv( ssi );
  ssiv.push( ar, ar + sizeof( ar ) / sizeof( ar[ 0 ] ) );

  if( 6 == stvi.top() && ( stvi.pop(), 3 == stvi.top() ) && 4 == sta.size() &&
      1 == ssi.count( 2 ) && 1 == ssiv.top() )
  {
    your_mark = ssi.size();
  }
 
  std::cout << "Your mark is " << your_mark << std::endl;
}