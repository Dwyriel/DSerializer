#include "DSerializer.h"
#include <stdexcept>
#include <utility>
#include <fstream>

const char CURLY_BRACE_START = '{', CURLY_BRACE_END = '}', QUOTATION_MARKS = '\"', COMMA = ',', NEW_LINE = '\n', TAB = '\t', SPACE = ' ';
//<!-- DObject --!>

DObject::DObject() noexcept = default;

DObject::DObject(std::string name) noexcept: objectName(std::move(name)) {}

void DObject::SetObjectName(std::string name) {
    if (name.empty())
        throw std::invalid_argument("Name can't be empty");
    if (name.find(SPACE) != std::string::npos || name.find(TAB) != std::string::npos || name.find(NEW_LINE) != std::string::npos)
        throw std::invalid_argument("Name contains invalid character (Space, new line or tab)");
    objectName = std::move(name);
}

const std::string &DObject::GetObjectName() const noexcept {
    return objectName;
}

void DObject::SetItem(const std::string &name, DVariant item) {
    checkName(name);
    names.insert(std::pair<std::string, char>(name, 0));
    items[name] = std::move(item);
}

DVariant &DObject::GetItem(const std::string &name) {
    auto iterator = items.find(name);
    if (iterator == items.end())
        SetItem(name, std::move(DVariant()));
    return items[name];
}

void DObject::SetObject(DObject dObject) {
    checkName(dObject.objectName);
    names.insert(std::pair<std::string, char>(dObject.objectName, 0));
    objects.insert(std::pair<std::string, DObject>(dObject.objectName, std::move(dObject)));
}

DObject &DObject::GetObject(const std::string &name) {
    auto iterator = objects.find(name);
    if (iterator == objects.end())
        SetObject(std::move(DObject(name)));
    return objects[name];
}

void DObject::SetVector(const std::string &name, std::vector<DVariant> vector) {
    checkName(name);
    names.insert(std::pair<std::string, char>(name, 0));
    vectorOfItems[name] = std::move(vector);
}

void DObject::SetVector(const std::string &name, std::vector<DObject> vector) {
    checkName(name);
    names.insert(std::pair<std::string, char>(name, 0));
    vectorOfObjects[name] = std::move(vector);
}

std::vector<DVariant> &DObject::GetVectorOfItems(const std::string &name) {
    auto iterator = vectorOfItems.find(name);
    if (iterator == vectorOfItems.end())
        SetVector(name, std::move(std::vector<DVariant>()));
    return vectorOfItems[name];
}

std::vector<DObject> &DObject::GetVectorOfObjects(const std::string &name) {
    auto iterator = vectorOfObjects.find(name);
    if (iterator == vectorOfObjects.end())
        SetVector(name, std::move(std::vector<DObject>()));
    return vectorOfObjects[name];
}

DVariant &DObject::operator[](const std::string &name) {
    return GetItem(name);
}

void DObject::checkName(const std::string &name) {
    if (name.empty())
        throw std::invalid_argument("Name can't be empty");
    if (name.find(SPACE) != std::string::npos || name.find(TAB) != std::string::npos || name.find(NEW_LINE) != std::string::npos)
        throw std::invalid_argument("Name contains invalid character (Space, new line or tab)");
    if (names.find(name) != names.end())
        throw std::invalid_argument("Two entities can't have the same name: \"" + name + "\"");
}

//<!-- DDocument --!>

DDocument::DDocument() noexcept = default;

DDocument::DDocument(std::filesystem::path file) noexcept: _file(std::move(file)) {}

DDocument::DDocument(DObject dObject) noexcept: _mainObj(std::move(dObject)) {}

DDocument::DDocument(std::filesystem::path file, DObject dObject) noexcept: _file(std::move(file)), _mainObj(std::move(dObject)) {}

void DDocument::SetObject(DObject dObject) noexcept {
    _mainObj = std::move(dObject);
}

DObject &DDocument::GetObject() noexcept {
    return _mainObj;
}

void DDocument::SetFile(std::filesystem::path file) noexcept {
    _file = std::move(file);
}

const std::filesystem::path &DDocument::GetFile() const noexcept {
    return _file;
}

bool DDocument::Save() {
    checkFile();
    checkObject();
    std::ofstream outStream(_file, std::ios::trunc);
    outStream << CURLY_BRACE_START;
    //TODO stuff
    outStream << CURLY_BRACE_END;
    return true;
}

DObject &DDocument::Load() {
    checkFile();
    return _mainObj;
}

void DDocument::checkFile() {
    if (_file.empty())
        throw std::invalid_argument("No file passed");
    if (!std::filesystem::exists(_file))
        return;
    if (std::filesystem::is_directory(_file))
        throw std::invalid_argument("Passed file is a directory");
}

void DDocument::checkObject() {
    for (const auto &[stringKey, innerDObject]: _mainObj.objects)
        checkInnerObject(innerDObject);
    for (const auto &[stringKey, vectorOfDObjects]: _mainObj.vectorOfObjects)
        for (const auto &innerDObject: vectorOfDObjects)
            checkInnerObject(innerDObject);
}

void DDocument::checkInnerObject(const DObject &dObject) {
    if (dObject.GetObjectName().empty())
        throw std::invalid_argument("DObject needs to have a name");
    for (const auto &[stringKey, innerDObject]: dObject.objects)
        checkInnerObject(innerDObject);
    for (const auto &[stringKey, vectorOfDObjects]: dObject.vectorOfObjects)
        for (const auto &innerDObject: vectorOfDObjects)
            checkInnerObject(innerDObject);
}
