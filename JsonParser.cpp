#include "JsonParser.h"

JObject::JObject() noexcept = default;

JObject::JObject(std::string name) noexcept: objectName(std::move(name)) {}

void JObject::SetObjectName(std::string name) noexcept {
    objectName = std::move(name);
}

const std::string &JObject::GetObjectName() noexcept {
    return objectName;
}

void JObject::SetItem(const std::string &name, DVariant item) noexcept {
    items[name] = std::move(item);
}

DVariant &JObject::GetItem(const std::string &name) noexcept {
    auto iterator = items.find(name);
    if (iterator == items.end())
        SetItem(name, std::move(DVariant()));
    return items[name];
}

void JObject::SetObject(JObject object) noexcept {
    objects.insert(std::pair<std::string, JObject>(object.objectName, std::move(object)));
}

JObject &JObject::GetObject(const std::string &name) noexcept {
    auto iterator = objects.find(name);
    if (iterator == objects.end())
        SetObject(std::move(JObject(name)));
    return objects[name];
}

void JObject::SetVector(const std::string &name, std::vector<DVariant> vector) noexcept {
    vectorOfItems[name] = std::move(vector);
}

std::vector<DVariant> &JObject::GetVector(const std::string &name) noexcept {
    auto iterator = vectorOfItems.find(name);
    if (iterator == vectorOfItems.end())
        SetVector(name, std::move(std::vector<DVariant>()));
    return vectorOfItems[name];
}

DVariant &JObject::operator[](const std::string &name) {
    return GetItem(name);
}
