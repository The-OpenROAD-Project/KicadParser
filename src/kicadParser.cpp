#include "kicadParser.h"

kicadParser::kicadParser(std::string fileName) : fileName_(fileName)
{
    /*std::cerr << "Parse Kicad Pcb database..." << std::endl;
    if (!parseKicadPcb())
    {
        std::cerr << "ERROR: Parsering Kicad Pcb database failed." << std::endl;
        assert(false);
    }*/
}

kicadParser::~kicadParser()
{
}

bool kicadParser::parseKicadPcb(Tree *tree)
{
    std::ifstream file;
    file.open(fileName_, std::ifstream::in);
    if (!file.is_open())
        return false;
    std::istream &in = file;

    //create tree from input
    *tree = readTree(in);
    //printTree(tree, 0);
    file.close();
    return true;
}

//read input till given byte appears
bool kicadParser::readUntil(std::istream &in, char c)
{
    char input;
    while (in.get(input))
    {
        if (input == c)
            return false;
    }
    return true;
}

//read whitespace
void kicadParser::readWhitespace(std::istream &in)
{
    for (;;)
    {
        auto b = in.peek();
        if (b != '\t' && b != '\n' && b != '\r' && b != ' ')
            break;
        char c;
        in.get(c);
    }
}

std::string kicadParser::readNodeName(std::istream &in)
{
    std::string s;
    for (;;)
    {
        auto b = in.peek();
        if (b == '\t' || b == '\n' || b == '\r' || b == ' ' || b == ')')
            break;
        char c;
        in.get(c);
        s.push_back(c);
    }
    return s;
}

Tree kicadParser::readString(std::istream &in)
{
    std::string s;
    for (;;)
    {
        auto b = in.peek();
        if (b == '\t' || b == '\n' || b == '\r' || b == ')' || b == ' ')
            break;
        char c;
        in.get(c);
        s.push_back(c);
    }
    return Tree{s, {}};
}

Tree kicadParser::readQuotedString(std::istream &in)
{
    std::string s;
    s.push_back('"');
    auto a = in.peek();
    for (;;)
    {
        auto b = in.peek();
        if (b == '"' && a != '\\')
            break;
        char c;
        in.get(c);
        s.push_back(c);
        a = b;
    }
    s.push_back('"');
    return Tree{s, {}};
}

Tree kicadParser::readTree(std::istream &in)
{
    readUntil(in, '(');
    readWhitespace(in);
    auto t = Tree{readNodeName(in), {}};
    for (;;)
    {
        readWhitespace(in);
        auto b = in.peek();
        char c;
        if (b == EOF)
        {
            break;
        }

        if (b == ')')
        {
            in.get(c);
            break;
        }
        if (b == '(')
        {
            t.m_branches.push_back(readTree(in));
            continue;
        }
        if (b == '"')
        {
            in.get(c);
            t.m_branches.push_back(readQuotedString(in));
            in.get(c);
            continue;
        }
        t.m_branches.push_back(readString(in));
    }
    return t;
}

void kicadParser::printTree(const Tree &t, int indent)
{
    if (!t.m_value.empty())
    {
        for (auto i = 1; i < indent; ++i)
            std::cout << "  ";
        std::cout << t.m_value << "\n";
    }
    for (auto &ct : t.m_branches)
    {
        printTree(ct, indent + 1);
    }
}

void kicadParser::printKicadPcb(const Tree &t, int indent)
{
    if (!t.m_value.empty())
    {
        for (auto i = 1; i < indent; ++i)
            std::cout << " ";
        if (!t.m_branches.empty())
            std::cout << " ( ";
        std::cout << t.m_value << " ";
        
    }

    if (!t.m_branches.empty())
    {
        for (auto &ct : t.m_branches)
        {
            printKicadPcb(ct, indent);
        }
        std::cout << ")" << std::endl;
    }
}

void kicadParser::writeKicadPcb(const Tree &t)
{
    std::ofstream file;
    file.open(fileName_);
    if (!t.m_value.empty())
    {
        for (auto i = 1; i < 0; ++i)
            file << " ";
        if (!t.m_branches.empty())
            file << " ( ";
        file << t.m_value << " ";   
    }

    if (!t.m_branches.empty())
    {
        for (auto &ct : t.m_branches)
        {
            writeKicadPcb(ct, 0, file);
        }
        file << ")" << std::endl;
    }

    file.close();

}
void kicadParser::writeKicadPcb(const Tree &t, int indent, std::ofstream &file)
{
    
    if (!t.m_value.empty())
    {
        for (auto i = 1; i < indent; ++i)
            file << " ";
        if (!t.m_branches.empty())
            file << " ( ";
        file << t.m_value << " ";
        
    }

    if (!t.m_branches.empty())
    {
        for (auto &ct : t.m_branches)
        {
            writeKicadPcb(ct, indent, file);
        }
        file << ")" << std::endl;
    }
}