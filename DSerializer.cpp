#include "DSerializer.h"
#include <stdexcept>
#include <utility>
#include <fstream>

const char CURLY_BRACKET_START = '{', CURLY_BRACKET_END = '}', SQUARE_BRACKET_START = '[', SQUARE_BRACKET_END = ']', QUOTATION_MARKS = '\"', COLON = ':', SEMICOLON = ';', EQUAL = '=', COMMA = ',', NEW_LINE = '\n', TAB = '\t', SPACE = ' ', TRUE_STARTING_CHAR = 't', FALSE_STARTING_CHAR = 'f';
const char possibleEntityStartChar[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.', 't', 'f', CURLY_BRACKET_START, SQUARE_BRACKET_START, QUOTATION_MARKS};
const char possibleNumberStartChar[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};

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
    serializeObjectWithoutName(outStream, _mainObj, 0);
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

void DSerializer::DDocument::addTabs(std::ofstream &stream, int tabNumber) {
    for (int i = 0; i < tabNumber; i++)
        stream << TAB;
}

void DSerializer::DDocument::serializeItems(std::ofstream &stream, DObject &dObject, int tabNumber) {
    for (auto it = dObject.items.begin(); it != dObject.items.end();) {
        addTabs(stream, tabNumber);
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SPACE;
        switch (it->second.GetType()) {
            case DVariant::Type::String:
                stream << QUOTATION_MARKS << it->second.AsString() << QUOTATION_MARKS;
                break;
            case DVariant::Type::Boolean:
                stream << (it->second.AsBool() ? "true" : "false");
                break;
            case DVariant::Type::Integer:
                stream << it->second.AsInteger();
                break;
            case DVariant::Type::Double:
                stream << it->second.AsDouble();
                break;
        }
        if (++it != dObject.items.end() || !dObject.objects.empty() || !dObject.vectorOfItems.empty() || !dObject.vectorOfObjects.empty())
            stream << COMMA << NEW_LINE;
    }
}

void DSerializer::DDocument::serializeObject(std::ofstream &stream, DSerializer::DObject &dObject, int tabNumber) {
    if (dObject.GetObjectName().empty())
        throw std::invalid_argument("DObject needs to have a name");
    addTabs(stream, tabNumber);
    stream << QUOTATION_MARKS << dObject.objectName << QUOTATION_MARKS << COLON << SPACE << CURLY_BRACKET_START << NEW_LINE;
    serializeObjectCommonAttributes(stream, dObject, tabNumber + 1);
    stream << NEW_LINE;
    addTabs(stream, tabNumber);
    stream << CURLY_BRACKET_END;
}

void DSerializer::DDocument::serializeObjectWithoutName(std::ofstream &stream, DSerializer::DObject dObject, int tabNumber) {
    addTabs(stream, tabNumber);
    stream << CURLY_BRACKET_START << NEW_LINE;
    serializeObjectCommonAttributes(stream, dObject, tabNumber + 1);
    stream << NEW_LINE;
    addTabs(stream, tabNumber);
    stream << CURLY_BRACKET_END;
}

void DSerializer::DDocument::serializeObjectCommonAttributes(std::ofstream &stream, DSerializer::DObject dObject, int tabNumber) {
    serializeItems(stream, dObject, tabNumber);
    for (auto it = dObject.objects.begin(); it != dObject.objects.end();) {
        serializeObject(stream, it->second, tabNumber);
        if (++it != dObject.objects.end() || !dObject.vectorOfItems.empty() || !dObject.vectorOfObjects.empty())
            stream << COMMA << NEW_LINE;
    }
    for (auto it = dObject.vectorOfItems.begin(); it != dObject.vectorOfItems.end();) {
        addTabs(stream, tabNumber);
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SPACE << SQUARE_BRACKET_START << NEW_LINE;
        serializeVector(stream, it->second, tabNumber + 1);
        stream << NEW_LINE;
        addTabs(stream, tabNumber);
        stream << SQUARE_BRACKET_END;
        if (++it != dObject.vectorOfItems.end() || !dObject.vectorOfObjects.empty())
            stream << COMMA << NEW_LINE;
    }
    for (auto it = dObject.vectorOfObjects.begin(); it != dObject.vectorOfObjects.end();) {
        addTabs(stream, tabNumber);
        stream << QUOTATION_MARKS << it->first << QUOTATION_MARKS << COLON << SPACE << SQUARE_BRACKET_START << NEW_LINE;
        serializeVector(stream, it->second, tabNumber + 1);
        stream << NEW_LINE;
        addTabs(stream, tabNumber);
        stream << SQUARE_BRACKET_END;
        if (++it != dObject.vectorOfObjects.end())
            stream << COMMA << NEW_LINE;
    }
}

void DSerializer::DDocument::serializeVector(std::ofstream &stream, DSerializer::DVarVector &vector, int tabNumber) {
    for (auto it = vector.begin(); it != vector.end();) {
        addTabs(stream, tabNumber);
        switch (it->GetType()) {
            case DVariant::Type::String:
                stream << QUOTATION_MARKS << it->AsString() << QUOTATION_MARKS;
                break;
            case DVariant::Type::Boolean:
                stream << (it->AsBool() ? "true" : "false");
                break;
            case DVariant::Type::Integer:
                stream << it->AsInteger();
                break;
            case DVariant::Type::Double:
                stream << it->AsDouble();
                break;
        }
        if (++it != vector.end())
            stream << COMMA << NEW_LINE;
    }
}

void DSerializer::DDocument::serializeVector(std::ofstream &stream, DSerializer::DObjVector &vector, int tabNumber) {
    for (auto it = vector.begin(); it != vector.end();) {
        serializeObjectWithoutName(stream, *it, tabNumber);
        if (++it != vector.end())
            stream << COMMA << NEW_LINE;
    }
}

void DSerializer::DDocument::throwParseErrorIf(bool condition) {
    if (condition)
        throw std::runtime_error("Error parsing file: " + _file.string());
}

void DSerializer::DDocument::putFileContentsIntoString(std::string &string) {
    if (!std::filesystem::exists(_file))
        throw std::runtime_error("File " + _file.string() + " does not exist");
    auto fileSize = std::filesystem::file_size(_file);
    std::ifstream inputStream(_file);
    if (!inputStream.is_open())
        throw std::runtime_error("Couldn't open file: " + _file.string());
    string.reserve(fileSize + 1);
    while (!inputStream.eof())
        string += (char) inputStream.get();
}

void DSerializer::DDocument::removeNewLinesTabsAndSpaces(std::string &string) {
    bool shouldContinue = false;
    for(size_t index = 0; index < string.size() - 1; index++){
        char c = string[index];
        if(c == QUOTATION_MARKS) {
            shouldContinue = !shouldContinue;
            continue;
        }
        if(shouldContinue)
            continue;
        if(c == NEW_LINE) {
            string.erase(index--, 1);
            continue;
        }
        if(c == SPACE) {
            string.erase(index--, 1);
            continue;
        }
        if(c == TAB) {
            string.erase(index--, 1);
            continue;
        }
    }
    string.shrink_to_fit();
}

void DSerializer::DDocument::readEntityName(std::string &string, std::string &outputName, size_t &currIndex) {
    throwParseErrorIf(string.find('"', currIndex+1) == std::string::npos);
    while (string[++currIndex] != QUOTATION_MARKS) {
        throwParseErrorIf(string[currIndex] == EOF);
        outputName += string[currIndex];
    }
}

DSerializer::DDocument::TypeOfEntity DSerializer::DDocument::checkTypeOfEntity(char character) {
    if (character == CURLY_BRACKET_START)
        return TypeOfEntity::Object;
    if (character == SQUARE_BRACKET_START)
        return TypeOfEntity::Vector;
    if (character == QUOTATION_MARKS)
        return TypeOfEntity::String;
    if (character == TRUE_STARTING_CHAR || character == FALSE_STARTING_CHAR)
        return TypeOfEntity::Boolean;
    for (auto c: possibleNumberStartChar)
        if (character == c)
            return TypeOfEntity::Number;
    return TypeOfEntity::None;
}
