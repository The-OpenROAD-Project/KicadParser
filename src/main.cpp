#include "kicadPcbDataBase.h"

int main(int argc, char *argv[])
{

    std::string designName = argv[1];
    auto db = kicadPcbDataBase{designName};
    db.printNodes();
    db.printKiCad();
    //db.printFile();
    //db.printSegment();

    return 0;
}
