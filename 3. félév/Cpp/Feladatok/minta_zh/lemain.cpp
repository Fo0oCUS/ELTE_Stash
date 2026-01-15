#include "lineedit.h"
#include <algorithm>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>
const int max = 1000;
int main() {
int your_mark = 1;
// 2-es
line_editor<std::vector<int>, int> lev;
for (int i = 0; i < max; ++i) {
lev.press(i);
lev.home();
lev.press(i);
}
std::vector<int> v = lev.enter();
line_editor<std::list<double>, double> lel;
lel.press(4.8);
lel.home();
lel.press(1.1);
std::list<double> c = lel.enter();
line_editor<std::string, char> les;
les.press('W');
les.press('o');
les.press('r');
les.press('l');
les.press('d');
les.home();
les.press('H');
les.press('e');
les.press('l');
les.press('l');
les.press('o');
std::string s = les.enter();
if ("HelloWorld" == s && "" == les.enter() && 2.2 > *(c.begin()) &&
2 * max == v.size() && max - 1 == v[0]) {
your_mark = c.size();
}
// 3-as
les.press('H');
les.press('e');
les.press('l');
les.press('l');
les.press('o');
les.home();
les.insert();
les.press('H');
les.press('a');
s = les.enter();
for (int i = 0; i < max; ++i) {
lev.press(2);
}
lev.home();
lev.insert();
for (int i = 0; i < max; ++i) {
lev.press(1);
}
v = lev.enter();
lel.press(7.9);
lel.press(1.2);
lel.home();
lel.insert();
lel.press(1.5);
lel.insert();
lel.press(3.7);
c = lel.enter();
if (1.7 > c.front() && 1.3 < c.front() && v[max / 2] == v[max / 5] &&
1.4 > c.back() && 1U * max == v.size() && "Hallo" == s &&
1 == v[max / 4]) {
your_mark = c.size();
}
// 4-es
line_editor<std::vector<int>> llev;
llev.press(3.3);
llev.press(1.1);
llev.home();
llev.del();
std::vector<int> lv = llev.enter();
line_editor<std::string> lles;
lles.press('J');
lles.press('a');
lles.press('v');
lles.press('a');
lles.backspace();
lles.backspace();
std::string f = lles.enter();
if ("Ja" == f && lv[0] < 1.3) {
your_mark += lv.size();
}
// 5-os
line_editor<std::list<int>> lle;
lle.press(3);
lle.home();
lle.insert();
llev.press(8);
llev.press(2);
llev.home();
lle.swap(llev);
lle.press(1);
std::list<int> cl = lle.enter();
v = llev.enter();
if (1 == cl.front() && cl.size() > v.size() && 3 == v[0]) {
your_mark += v.size();
}
std::cout << "Your mark is " << your_mark;
std::endl(std::cout);
}
