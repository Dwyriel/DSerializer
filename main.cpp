#include <iostream>
#include "JsonParser.h"

int main(){
    auto obj1Name = "First Item";
    JObject obj {"First Obj"};
    obj.SetItem(obj1Name, {"Hi"});
    std::cout << obj.GetItem(obj1Name).GetString();
    obj["Sec"] = 25000;
    obj.GetItem("Third") = false;
    obj.GetItem("Forth") = (double)52558541858823;
    obj["stringus"] = "Back to string";
    obj.SetItem("Second Item", 25);
    obj.SetItem("Third Item", 36.6);
    obj.SetItem("Fourth Item", false);
    JObject obj2("First Object");
    obj2.SetItem("string", {"hi"});
    obj2.SetItem("bool", true);
    obj2.SetItem("double", 29.234312);
    obj.SetObject(obj2);
}