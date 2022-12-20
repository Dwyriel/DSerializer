#include <iostream>
#include "DSerializer.h"

int main() {

    DDocument ddoc("/home/dwyriel/Projects/DSerializer/cmake-build-debug/dw.txt");
    ddoc.Save();
    return 0;
    DObject obj{"First Obj"};
    obj.SetItem("First Item", {"Hi"});
    obj["Sec"] = 25000;
    obj.GetItem("Third") = false;
    obj.GetItem("Forth") = (double) 52558541858823;
    obj["stringus"] = "stringy";
    obj["stringus"] = 25600;
    obj["stringus"] = -23.52312;
    obj["stringus"] = false;
    obj["stringus"] = "Back to string";
    obj.SetItem("Second Item", 25);
    obj.SetItem("Third Item", 36.6);
    obj.SetItem("Fourth Item", false);
    {
        DObject obj2("First Object");
        obj2.SetItem("string", {"hi"});
        obj2.SetItem("bool", true);
        obj2.SetItem("double", 29.234312);
        obj.SetObject(obj2);
    }
    for (int i = 0; i < 1000; i++) {
        std::vector<DObject> dObjvector{DObject("one"), DObject("two"), DObject("three")};
        obj.SetVector("Veccy" + std::to_string(i), dObjvector);
    }
}