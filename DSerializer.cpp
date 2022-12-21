#include "DSerializer.h"
#include <stdexcept>
#include <utility>
#include <fstream>

const char CURLY_BRACKET_START = '{', CURLY_BRACKET_END = '}', SQUARE_BRACKET_START = '[', SQUARE_BRACKET_END = ']', QUOTATION_MARKS = '\"', COLON = ':', SEMICOLON = ';', EQUAL = '=', COMMA = ',', NEW_LINE = '\n', TAB = '\t', SPACE = ' ';

//<!-- DObject --!>

DSerializer::DObject::DObject() noexcept = default;

DSerializer::DObject::DObject(std::string name) noexcept: objectName(std::move(name)) {}

void DSerializer::DObject::SetObjectName(std::string name) {
    if (name.empty())
        throw std::invalid_argument("Name can't be empty");
    if (name.find(SPACE) != std::string::npos || name.find(TAB) != std::string::npos || name.find(NEW_LINE) != std::string::npos)
        throw std::invalid_argument("Name contains invalid character (Space, new line or tab)");
    objectName = std::move(name);
}

const std::string &DSerializer::DObject::GetObjectName() const noexcept {
    return objectName;
}

void DSerializer::DObject::SetItem(const std::string &name, DVariant item) {
    checkName(name);
    names.insert(std::pair<std::string, char>(name, 0));
    items[name] = std::move(item);
}

DVariant &DSerializer::DObject::GetItem(const std::string &name) {
    auto iterator = items.find(name);
    if (iterator == items.end())
        SetItem(name, std::move(DVariant()));
    return items[name];
}

void DSerializer::DObject::SetObject(DObject dObject) {
    checkName(dObject.objectName);
    names.insert(std::pair<std::string, char>(dObject.objectName, 0));
    objects.insert(std::pair<std::string, DObject>(dObject.objectName, std::move(dObject)));
}

DSerializer::DObject &DSerializer::DObject::GetObject(const std::string &name) {
    auto iterator = objects.find(name);
    if (iterator == objects.end())
        SetObject(std::move(DObject(name)));
    return objects[name];
}

void DSerializer::DObject::SetVector(const std::string &name, std::vector<DVariant> vector) {
    checkName(name);
    names.insert(std::pair<std::string, char>(name, 0));
    vectorOfItems[name] = std::move(vector);
}

void DSerializer::DObject::SetVector(const std::string &name, std::vector<DObject> vector) {
    checkName(name);
    names.insert(std::pair<std::string, char>(name, 0));
    vectorOfObjects[name] = std::move(vector);
}

std::vector<DVariant> &DSerializer::DObject::GetVectorOfItems(const std::string &name) {
    auto iterator = vectorOfItems.find(name);
    if (iterator == vectorOfItems.end())
        SetVector(name, std::move(std::vector<DVariant>()));
    return vectorOfItems[name];
}

DSerializer::DObjVector &DSerializer::DObject::GetVectorOfObjects(const std::string &name) {
    auto iterator = vectorOfObjects.find(name);
    if (iterator == vectorOfObjects.end())
        SetVector(name, std::move(std::vector<DObject>()));
    return vectorOfObjects[name];
}

DVariant &DSerializer::DObject::operator[](const std::string &name) {
    return GetItem(name);
}

void DSerializer::DObject::checkName(const std::string &name) {
    if (name.empty())
        throw std::invalid_argument("Name can't be empty");
    if (name.find(SPACE) != std::string::npos || name.find(TAB) != std::string::npos || name.find(NEW_LINE) != std::string::npos)
        throw std::invalid_argument("Name contains invalid character (Space, new line or tab)");
    if (names.find(name) != names.end())
        throw std::invalid_argument("Two entities can't have the same name: \"" + name + "\"");
}

//<!-- DDocument --!>

DSerializer::DDocument::DDocument() noexcept: _mainObj("rootObj") {};

DSerializer::DDocument::DDocument(std::filesystem::path file) noexcept: _file(std::move(file)), _mainObj("rootObj") {}

DSerializer::DDocument::DDocument(DObject dObject) noexcept: _mainObj(std::move(dObject)) {}

DSerializer::DDocument::DDocument(std::filesystem::path file, DObject dObject) noexcept: _file(std::move(file)), _mainObj(std::move(dObject)) {}

void DSerializer::DDocument::SetObject(DObject dObject) noexcept {
    _mainObj = std::move(dObject);
}

DSerializer::DObject &DSerializer::DDocument::GetObject() noexcept {
    return _mainObj;
}

void DSerializer::DDocument::SetFile(std::filesystem::path file) noexcept {
    _file = std::move(file);
}

const std::filesystem::path &DSerializer::DDocument::GetFile() const noexcept {
    return _file;
}

bool DSerializer::DDocument::Save() {
    checkFile();
    checkObject(_mainObj);
    std::ofstream outStream(_file, std::ios::trunc);
    outStream << CURLY_BRACKET_START;
    serializeObject(outStream, _mainObj);
    outStream << CURLY_BRACKET_END;
    return true;
}

DSerializer::DObject &DSerializer::DDocument::Load() {
    checkFile();
    return _mainObj;
}

void DSerializer::DDocument::checkFile() {
    if (_file.empty())
        throw std::invalid_argument("No file passed");
    if (!std::filesystem::exists(_file))
        return;
    if (std::filesystem::is_directory(_file))
        throw std::invalid_argument("Passed file is a directory");
}

void DSerializer::DDocument::checkObject(const DObject &dObject) {
    for (const auto &[stringKey, innerDObject]: dObject.objects)
        checkObject(innerDObject);
    for (const auto &[stringKey, vectorOfDObjects]: dObject.vectorOfObjects)
        for (const auto &innerDObject: vectorOfDObjects)
            checkObject(innerDObject);
}

void DSerializer::DDocument::serializeItems(std::ofstream &stream, DObject& dObject) {
    for (auto it = dObject.items.begin(); it != dObject.items.end();) {
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << QUOTATION_MARKS << it->second.GetString() << QUOTATION_MARKS;
        if (++it != dObject.items.end() || !dObject.objects.empty() || !dObject.vectorOfItems.empty() || !dObject.vectorOfObjects.empty())
            stream << COMMA;
    }
}

void DSerializer::DDocument::serializeObject(std::ofstream &stream, DSerializer::DObject &dObject) {
    if (dObject.GetObjectName().empty())
        throw std::invalid_argument("DObject needs to have a name");
    stream << QUOTATION_MARKS << dObject.objectName << QUOTATION_MARKS << COLON << CURLY_BRACKET_START;
    serializeItems(stream, dObject);
    for (auto it = dObject.objects.begin(); it != dObject.objects.end();) {
        serializeObject(stream, it->second);
        if (++it != dObject.objects.end() || !dObject.vectorOfItems.empty() || !dObject.vectorOfObjects.empty())
            stream << COMMA;
    }
    for (auto it = dObject.vectorOfItems.begin(); it != dObject.vectorOfItems.end();) {
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SQUARE_BRACKET_START;
        serializeVector(stream, it->second);
        stream << SQUARE_BRACKET_END;
        if (++it != dObject.vectorOfItems.end() || !dObject.vectorOfObjects.empty())
            stream << COMMA;
    }
    for (auto it = dObject.vectorOfObjects.begin(); it != dObject.vectorOfObjects.end();) {
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SQUARE_BRACKET_START;
        serializeVector(stream, it->second);
        stream << SQUARE_BRACKET_END;
        if (++it != dObject.vectorOfObjects.end())
            stream << COMMA;
    }
    stream << CURLY_BRACKET_END;
}

void DSerializer::DDocument::serializeObjectOfVector(std::ofstream &stream, DSerializer::DObject dObject) {
    stream << CURLY_BRACKET_START;
    serializeItems(stream, dObject);
    for (auto it = dObject.objects.begin(); it != dObject.objects.end();) {
        serializeObject(stream, it->second);
        if (++it != dObject.objects.end() || !dObject.vectorOfItems.empty() || !dObject.vectorOfObjects.empty())
            stream << COMMA;
    }
    for (auto it = dObject.vectorOfItems.begin(); it != dObject.vectorOfItems.end();) {
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SQUARE_BRACKET_START;
        serializeVector(stream, it->second);
        stream << SQUARE_BRACKET_END;
        if (++it != dObject.vectorOfItems.end() || !dObject.vectorOfObjects.empty())
            stream << COMMA;
    }
    for (auto it = dObject.vectorOfObjects.begin(); it != dObject.vectorOfObjects.end();) {
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SQUARE_BRACKET_START;
        serializeVector(stream, it->second);
        stream << SQUARE_BRACKET_END;
        if (++it != dObject.vectorOfObjects.end()) continue;
        stream << COMMA;
    }
    stream << CURLY_BRACKET_END;
}

void DSerializer::DDocument::serializeVector(std::ofstream &stream, DSerializer::DVarVector &vector) {
    for(auto it = vector.begin(); it != vector.end();){
        stream << QUOTATION_MARKS << it->GetString() << QUOTATION_MARKS;
        if(++it != vector.end())
            stream << COMMA;
    }
}

void DSerializer::DDocument::serializeVector(std::ofstream &stream, DSerializer::DObjVector &vector) {
    for(auto it = vector.begin(); it != vector.end();){
        serializeObjectOfVector(stream, *it);
        if(++it != vector.end())
            stream << COMMA;
    }
}
