#ifndef LINE_EDIT
#define LINE_EDIT

#include <list>
#include <algorithm>

template <typename Cont, typename CharT = typename Cont::value_type>
class line_editor{
    std::list<CharT> line;
    typename std::list<CharT>::iterator cursor;
    bool isInsert;

public:
    line_editor() : cursor(line.end(), isInsert(false)){

    }

    void press(CharT c){
        if(isInsert && cursor != line.end()){
            *cursor = c;
        }
        else{
            cursor = line.insert(cursor, c);
        }
        cursor++;
    }

    void home(){
        cursor = line.begin();
    }

    Cont enter(){
        Cont ret(line.begin(), line.end());
        line.clear();
        cursor = line.end();
        return ret;
    }

    void insert(){
        isInsert = !isInsert;
    }

    void backspace(){
        cursor--;
        del();
    }

    void del(){
        cursor = line.erase(cursor);
    }

    template <class T, class U>
    void swap(line_editor<T, U>& le){
        std::swap(line, le.line);
        std::swap(cursor, le.cursor);
        std::swap(isInsert, le.isInsert);
    }

    template <class T, class U>
    friend class line_editor;
};

#endif