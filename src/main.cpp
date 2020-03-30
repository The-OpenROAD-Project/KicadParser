#include "kicadPcbDataBase.h"

int main(int argc, char *argv[])
{

    std::string designName = argv[1];
    auto db = kicadPcbDataBase{designName};
    db.printNodes();
    cout << endl;
    std::vector<instance> &instances = db.getInstances();
    for (auto &inst : instances) {
        int layer = inst.getLayer();
        if (layer == 31) {
        	int l = 0;
			inst.setLayer(l);
        } else {
        	int l = 31;
        	inst.setLayer(l);
        }
    }
    cout << endl;
    db.printNodes();
    db.printKiCad();
    //db.printFile();
    //db.printSegment();

    return 0;
}
