#include <iostream>
#include "DSerializer.h"

int main() {
    DSerializer::DDocument ddoc("test.json");
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
    {
        DSerializer::DObject obj2("Second_Object");
        obj2.SetItem("string", "hi");
        obj2.SetItem("bool", true);
        obj2.SetItem("double", 29.234312);
        obj.SetObject(obj2);
    }
    {
        DSerializer::DObject obj2("Cool_Object");
        obj2.SetItem("boolean", true);
        obj2.SetItem("bool", true);
        obj2.SetItem("integer", false);
        obj.SetObject(obj2);
    }
    for (int i = 0; i < 5; i++) {
        std::vector<DVariant> dVarVector{76, "one", 2, false, 25, .9997, "Haru"};
        obj.SetVector("ArrayItems" + std::to_string(i), dVarVector);
    }
    obj.GetVectorOfItems("Vec2").emplace_back(333);
    obj.GetVectorOfObjects("Veccy4").emplace_back("Itenus");
    obj.GetVectorOfObjects("Veccy4").at(0).SetItem("Number_of_the_Beast", 666);
    obj.GetVectorOfObjects("Veccy4").at(0).SetObject(DSerializer::DObject("Objectus"));
    obj.GetVectorOfObjects("Veccy4").at(0).GetObject("Objectus").SetItem("Doublely_So", 2);
    obj.GetVectorOfObjects("Veccy4").at(0).GetObject("Objectus").GetObject("Objectify").SetItem("Str", "I like cute things :)");
    for (int i = 0; i < 10; i++) {//THIS IS DUMB, TOO MANY ALLOCATIONS FOR NO REASON, I KNOW, IT'S JUST FOR TESTING
        DSerializer::DObjVector dObjvector{DSerializer::DObject(), DSerializer::DObject(), DSerializer::DObject()};
        dObjvector.at(0).SetItem("One_Item", 1);
        dObjvector.at(1).SetItem("To_Rule", 2);
        dObjvector.at(2).SetItem("Them_All", 3);
        obj.SetVector("ArrayObjs" + std::to_string(i), dObjvector);
    }
    ddoc.Save();
}