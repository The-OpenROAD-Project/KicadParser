#ifndef KICAD_PCB_KICADPCB_PARSER_H
#define KICAD_PCB_KICADPCB_PARSER_H

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
    bool parseKicadPcb(Tree *);
    bool readUntil(std::istream &, char);
    void readWhitespace(std::istream &);
    std::string readNodeName(std::istream &);
    Tree readString(std::istream &);
    Tree readQuotedString(std::istream &);
    Tree readTree(std::istream &);
    void printTree(const Tree &, int);
    void printKicadPcb(const Tree &, int);
    void writeKicadPcb(const Tree &, int, std::ofstream &);
    void writeKicadPcb(const Tree &);

private:
    std::string fileName_;
};

#endif