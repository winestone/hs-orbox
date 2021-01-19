#include "CDebug.h"

CDebug::CDebug () {
    file = NULL;
    IndentText = NULL;
    _Indent = 0;
}

void CDebug::Init () {
    file = fopen("cdebug.txt", "w");
    IndentText = new char[5];
    strcpy(IndentText, "    ");
}

void CDebug::SetIndentText (char* Text) {
    delete[] IndentText;
    IndentText = new char[strlen(Text) + 1];
    strcpy(IndentText, Text);
}

void CDebug::IncIndent (int Amount) {
    _Indent += Amount;
    if (_Indent < 0) {
        _Indent = 0;
    }
}

void CDebug::SetIndent (int Amount) {
    if (Amount >= 0) {
        _Indent = Amount;
    }
}

void CDebug::PInd () {
    for (int i = 0;i < _Indent;i++) {
        fprintf(file, IndentText);
    }
}

void CDebug::PIndS () {
    PInd();
    IncIndent(1);
}

void CDebug::PIndE () {
    IncIndent(-1);
    PInd();
}

void CDebug::Cleanup () {
    delete[] IndentText;
    IndentText = NULL;
    fclose(file);
}
