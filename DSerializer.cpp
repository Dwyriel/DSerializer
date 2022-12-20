#include "DSerializer.h"

//<!-- DObject --!>

DObject::DObject() noexcept = default;

DObject::DObject(std::string name) noexcept: objectName(std::move(name)) {}

void DObject::SetObjectName(std::string name) noexcept {
    objectName = std::move(name);
}

const std::string &DObject::GetObjectName() noexcept {
    return objectName;
}

void DObject::SetItem(const std::string &name, DVariant item) noexcept {
    items[name] = std::move(item);
}

DVariant &DObject::GetItem(const std::string &name) noexcept {
    auto iterator = items.find(name);
    if (iterator == items.end())
        SetItem(name, std::move(DVariant()));
    return items[name];
}

void DObject::SetObject(DObject object) noexcept {
    objects.insert(std::pair<std::string, DObject>(object.objectName, std::move(object)));
}

DObject &DObject::GetObject(const std::string &name) noexcept {
    auto iterator = objects.find(name);
    if (iterator == objects.end())
        SetObject(std::move(DObject(name)));
    return objects[name];
}

void DObject::SetVector(const std::string &name, std::vector<DVariant> vector) noexcept {
    vectorOfItems[name] = std::move(vector);
}

void DObject::SetVector(const std::string &name, std::vector<DObject> vector) noexcept {
    vectorOfObjects[name] = std::move(vector);
}

std::vector<DVariant> &DObject::GetVectorOfItems(const std::string &name) noexcept {
    auto iterator = vectorOfItems.find(name);
    if (iterator == vectorOfItems.end())
        SetVector(name, std::move(std::vector<DVariant>()));
    return vectorOfItems[name];
}

std::vector<DObject> &DObject::GetVectorOfObjects(const std::string &name) noexcept {
    auto iterator = vectorOfObjects.find(name);
    if (iterator == vectorOfObjects.end())
        SetVector(name, std::move(std::vector<DObject>()));
    return vectorOfObjects[name];
}

DVariant &DObject::operator[](const std::string &name) {
    return GetItem(name);
}

//<!-- DDocument --!>

DDocument::DDocument() noexcept = default;

DDocument::DDocument(std::string file) noexcept: _file(std::move(file)) {}

DDocument::DDocument(DObject dObject) noexcept: _mainObj(std::move(dObject)) {}

DDocument::DDocument(std::string file, DObject dObject) noexcept: _file(std::move(file)), _mainObj(std::move(dObject)) {}

void DDocument::SetObject(DObject dObject) noexcept {
    _mainObj = std::move(dObject);
}

DObject &DDocument::GetObject() noexcept {
    return _mainObj;
}

void DDocument::SetFile(std::string file) noexcept {
    _file = std::move(file);
}

const std::string &DDocument::GetFile() noexcept {
    return _file;
}

bool DDocument::Save() {
    checkFile();
    checkObject();
    return true;
}

DObject &DDocument::Load() {
    checkFile();
    return _mainObj;
}

void DDocument::checkFile() {
    if (_file.empty())
        throw std::invalid_argument("No file passed");
    if(!std::filesystem::exists(_file))
        return;
    if (std::filesystem::is_directory(_file))
        throw std::invalid_argument("Passed file is a directory");
}

void DDocument::checkObject() {

}

