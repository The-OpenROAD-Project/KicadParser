#ifndef KICADPCB_PARSER_H
#define KICADPCB_PARSER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include "tree.h"

class kicadParser
{

public:
    kicadParser(std::string);
    ~kicadParser();
    bool parseKicadPcb(tree *);
    bool readUntil(std::istream &, char);
    void readWhitespace(std::istream &);
    std::string readNodeName(std::istream &);
    tree readString(std::istream &);
    tree readQuotedString(std::istream &);
    tree readTree(std::istream &);
    void printTree(const tree &, int);
    void printKicadPcb(const tree &, int);

private:
    std::string fileName_;
};

#endif