#include <iostream>
#include "DSerializer.h"

int main() {
    DSerializer::DDocument ddoc("dw.txt");
    auto &obj = ddoc.GetObject();
    obj.SetItem("First_Item", "Hi");
    obj["Sec"] = 25000;
    obj.GetItem("Third") = false;
    obj.GetItem("Forth") = (double) 52558541858823;
    obj["stringus"] = "stringy";
    obj["stringus"] = 25600;
    obj["stringus"] = -23.52312;
    obj["stringus"] = false;
    obj["stringus"] = "Back to string";
    obj.SetItem("Second_Item", 25);
    obj.SetItem("Third_Item", 36.6);
    obj.SetItem("Fourth_Item", false);
    {
        DSerializer::DObject obj2("First_Object");
        obj2.SetItem("string", "hi");
        obj2.SetItem("bool", true);
        obj2.SetItem("double", 29.234312);
        obj.SetObject(obj2);
    }
    for (int i = 0; i < 10; i++) {//THIS IS DUMB, TOO MANY ALLOCATIONS FOR NO REASON, I KNOW, IT'S JUST FOR TESTING
        DSerializer::DObjVector dObjvector{DSerializer::DObject("one"), DSerializer::DObject("two"), DSerializer::DObject("three")};
        obj.SetVector("Veccy" + std::to_string(i), dObjvector);
    }
    obj.GetVectorOfObjects("Veccy4").at(1).SetItem("Number_of_the_Beast", 666);
    for (int i = 0; i < 5; i++) {
        std::vector<DVariant> dVarVector{76, "one", 2, false, 25, .9997, "Haru"};
        obj.SetVector("Vec" + std::to_string(i), dVarVector);
    }
    obj.GetVectorOfItems("Vec2").emplace_back(333);
    ddoc.Save();
}