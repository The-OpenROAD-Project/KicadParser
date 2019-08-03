#include "kicadPcbDataBase.h"



int main(int argc, char *argv[])
{

  std::string designName = argv[1];
  auto db = kicadPcbDataBase{designName};

  return 0;
}
