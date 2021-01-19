#ifndef _CDEBUG_H_
    #define _CDEBUG_H_

#include <cstdio>
#include <cstring>

//The nextwindow_t enum
enum class nextwindow_t {
    SPLASHSCREEN,
    MAINMENU,
    INSTRUCTIONS,
    GAME,
    HIGHSCORE,
    EXIT
};

//C debugging class
class CDebug {
    public:
        //Constructor
        CDebug();
    
    public:
        //File pointer for printing to
        FILE* file;
    
    private:
        //Text to indent with
        char* IndentText;
        //Indent level
        int _Indent;
    
    public:
        //Initialises class
        void Init();
        
        //Set's indent text
        void SetIndentText(char* Text);
        //Changes indent by a relative amount
        void IncIndent(int Amount);
        //Sets indent
        void SetIndent(int Amount);
        
        void PInd();    //Print indent
        void PIndS();   //Print indent and increment level
        void PIndE();   //Decrement level and print indent
        
        //Cleans up class
        void Cleanup();
};

#endif
