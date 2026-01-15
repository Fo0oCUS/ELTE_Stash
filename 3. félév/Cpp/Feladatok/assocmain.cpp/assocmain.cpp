#include <iostream>
#include "assoc.h"
#include <deque>
#include <string>
#include <list>
#include <vector>
#include "assoc.h"
#include <complex>
#include <set>
#include <functional>

struct same_length
{

  bool operator()( const std::string& lhs,
                   const std::string& rhs ) const
  {
    return lhs.size() == rhs.size();
  }

};

const int max = 1000;

int main()
{
  int your_mark = 1;
  /* 2-es */
  std::list<int> s;
  s.push_back( 1 );
  s.push_back( 8 );
  s.push_back( 7 );

  association<std::string, std::list<int>, int> sa( s );
  sa.associate( "!!!", s.begin() );
  s.front() = 2;

  std::vector<double> v;
  v.reserve( 8 );
  v.push_back( 0.5 );
  v.push_back( 0.811 );
  v.push_back( 1.5 );
  v.push_back( 0.33 );
  v.push_back( 0.568 );
  association<std::complex<int>, std::vector<double>, double> av( v );
  const std::complex<int> i( 0, 1 );
  av.associate( i, v.begin() + 1 );

  std::deque<int> d;
  for( int i = 0; i < max; ++i )
  {
    d.push_back( i );
  }
  association<std::string, std::deque<int>, int> da( d );
  da.associate( ":O", d.begin() + max / 2 );
  da.associate( ";)", d.begin() + 1 );
  da.associate( ":/", d.begin() + 2 );

  if ( 0.77 < av.find( i ) && av.find( i ) < 1.1 && max / 2 == da.find( ":O" ) &&
       2 == da.find( ":/" ) && 1 == da.find( ";)" ) )
  {
    your_mark = sa.find( "!!!" );
  }
 
  /* 3-as */
  const std::complex<int> p( 1, 1 );
  av[ p ] = 0.473;

  da[ "WTF?" ] = 2 * max;

  if ( 6 == v.size() && 0.4 < av[ p ] && 0.5 > av[ p ] && 2 == sa[ "!!!" ] &&
       2 * max == da[ "WTF?" ] && !sa.has( "Unknown" ) && !da.has( ":(" ) )
  {
    your_mark = s.size();
  }
 
  /* 4-es */
  std::list<int> langs;
  association<std::string, std::list<int> > al( langs );
  al[ "C++" ] = 4;
  al[ "Fortran" ] = 2;
  al[ "C" ] = 5;

  std::list<int> m;
  association<std::string, std::list<int>, int, same_length> am( m );
  am[ ":(" ] = max;

  if ( al.has( "C" ) && !al.has( "Brainfuck" ) && 3 == langs.size() &&
       am.has( ":D" ) && max == am[ "?!" ] && 1 == m.size() && !am.has( ":" ) )
  {
    your_mark = al[ "C++" ];
  }
 
  /* 5-os */
  std::set<int> se;
  se.insert( 6 );
  se.insert( 5 );

  association<std::string, std::set<int> > ase( se );
  ase.associate( "<3", se.begin() );
  association_common<std::string, std::set<int> >* ptr = &ase;
  ptr = 0;
  delete ptr;

  std::set<int, std::greater<int> > sei;
  sei.insert( 2 );
  sei.insert( 4 );

  association<std::string, std::set<int, std::greater<int> > > asei( sei );
  asei.associate( ":|", sei.begin() );


  if( ase.has( "<3" ) && asei.has( ":|" ) && 4 == asei[ ":|" ] )
  {
    your_mark = ase[ "<3" ];
  }
 
  std::cout << "Your mark is " << your_mark;
  std::endl( std::cout );
}