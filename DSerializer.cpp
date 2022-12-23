#include "DSerializer.h"
#include <stdexcept>
#include <utility>
#include <fstream>
#include <limits>

const char CURLY_BRACKET_START = '{', CURLY_BRACKET_END = '}', SQUARE_BRACKET_START = '[', SQUARE_BRACKET_END = ']', QUOTATION_MARKS = '\"', COLON = ':', SEMICOLON = ';', EQUAL = '=', DOT = '.', COMMA = ',', INVERSE_SLASH = '\\', NEW_LINE = '\n', TAB = '\t', SPACE = ' ', TRUE_STARTING_CHAR = 't', FALSE_STARTING_CHAR = 'f';
const char possibleItemStartChar[] = {TRUE_STARTING_CHAR, FALSE_STARTING_CHAR, QUOTATION_MARKS, '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};
const char possibleNumberStartChar[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};
const auto TRUE_STRING = "true", FALSE_STRING = "false";
const std::vector<char> entityEndChars = {COMMA, CURLY_BRACKET_END, SQUARE_BRACKET_END};

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

#include <iostream>

DSerializer::DObject &DSerializer::DDocument::Load() {
    checkFile();
    std::string fileContents;
    putFileContentsIntoString(fileContents);
    removeNewLinesTabsAndSpaces(fileContents);
    size_t currIndex = 0;
    throwParseErrorIf(fileContents[currIndex++] != CURLY_BRACKET_START);
    if (fileContents[currIndex] == CURLY_BRACKET_END)
        return (_mainObj = DObject());
    readObject(fileContents, _mainObj, currIndex);

    std::cout << fileContents << std::endl;
    return _mainObj;
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
    char currChar = 0, prevChar = 0;
    for (size_t index = 0; index < string.size() - 1; index++) {
        prevChar = currChar;
        currChar = string[index];
        if (currChar == QUOTATION_MARKS && prevChar != INVERSE_SLASH) {
            shouldContinue = !shouldContinue;
            continue;
        }
        if (shouldContinue)
            continue;
        if (currChar == NEW_LINE) {
            string.erase(index--, 1);
            continue;
        }
        if (currChar == SPACE) {
            string.erase(index--, 1);
            continue;
        }
        if (currChar == TAB) {
            string.erase(index--, 1);
            continue;
        }
    }
    string.shrink_to_fit();
}

void DSerializer::DDocument::readEntireString(std::string &string, std::string &outputString, size_t &index) {
    throwParseErrorIf(string.find('"', index + 1) == std::string::npos);
    char currChar = 0, prevChar;
    while (true) {
        prevChar = currChar;
        currChar = string[++index];
        throwParseErrorIf(currChar == EOF);
        if (currChar == QUOTATION_MARKS && prevChar != INVERSE_SLASH)
            break;
        outputString += currChar;
    }
    index++;
}

void DSerializer::DDocument::readUntilChar(std::string &string, std::string &outputString, size_t &index, const std::vector<char> &characters) {
    size_t smallestPosOfChar = std::numeric_limits<long long>::max();
    char character = 0;
    for (auto c: characters) {
        size_t posOfChar = string.find(c, index);
        if (posOfChar == std::string::npos)
            continue;
        if (posOfChar < smallestPosOfChar) {
            smallestPosOfChar = posOfChar;
            character = c;
        }
    }
    throwParseErrorIf(smallestPosOfChar == std::numeric_limits<long long>::max());
    outputString.reserve((smallestPosOfChar - index) + 1);
    while (string[index] != character)
        outputString += string[index++];
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

DSerializer::DDocument::TypeOfVector DSerializer::DDocument::checkTypeOfVector(char character) {
    if (character == CURLY_BRACKET_START)
        return TypeOfVector::Object;
    for (auto c: possibleItemStartChar)
        if (character == c)
            return TypeOfVector::Item;
    return TypeOfVector::None;
}

void DSerializer::DDocument::readObject(std::string &string, DSerializer::DObject &dObject, size_t &index) {
    if (string[index] == CURLY_BRACKET_END) {
        index++;
        return;
    }
    do {
        if (string[index] == COMMA) index++;
        std::string currItemName;
        throwParseErrorIf(string[index] != QUOTATION_MARKS);
        readEntireString(string, currItemName, index);
        throwParseErrorIf(string[index] != COLON);
        TypeOfEntity type = checkTypeOfEntity(string[++index]);
        switch (type) {
            case TypeOfEntity::String:
                readString(string, dObject, currItemName, index);
                break;
            case TypeOfEntity::Number:
                readNumber(string, dObject, currItemName, index);
                break;
            case TypeOfEntity::Boolean:
                readBoolean(string, dObject, currItemName, index);
                break;
            case TypeOfEntity::Object:
                readObject(string, dObject.GetObject(currItemName), ++index);
                break;
            case TypeOfEntity::Vector:
                readVector(string, dObject, currItemName, ++index);
                break;
            case TypeOfEntity::None:
                break;
        }
    } while (string[index++] != CURLY_BRACKET_END);
}

void DSerializer::DDocument::readString(std::string &string, DObject &dObject, std::string &itemName, size_t &index) {
    std::string content;
    readEntireString(string, content, index);
    dObject.SetItem(itemName, content);
}

void DSerializer::DDocument::readNumber(std::string &string, DSerializer::DObject &dObject, std::string &itemName, size_t &index) {
    std::string content;
    readUntilChar(string, content, index, entityEndChars);
    if (content.find(DOT) != std::string::npos) {
        dObject.SetItem(itemName, std::stod(content));
        return;
    }
    dObject.SetItem(itemName, std::stoll(content));
}

void DSerializer::DDocument::readBoolean(std::string &string, DSerializer::DObject &dObject, std::string &itemName, size_t &index) {
    std::string content;
    readUntilChar(string, content, index, entityEndChars);
    throwParseErrorIf(content != TRUE_STRING && content != FALSE_STRING);
    dObject.SetItem(itemName, content == TRUE_STRING);
}

void DSerializer::DDocument::readVector(std::string &string, DSerializer::DObject &dObject, std::string &itemName, size_t &index) {
    if (string[index] == SQUARE_BRACKET_END)
        return;
    TypeOfVector vecType = checkTypeOfVector(string[index]);
    switch (vecType) {
        case TypeOfVector::Item:
            readItensOfVector(string, dObject, itemName, index);
            break;
        case TypeOfVector::Object:
            readObjectsOfVector(string, dObject, itemName, index);
            break;
        case TypeOfVector::None:
            break;
    }
}

void DSerializer::DDocument::readItensOfVector(std::string &string, DSerializer::DObject &dObject, std::string &itemName, size_t &index) {
    do {
        if (string[index] == COMMA) index++;
        TypeOfEntity type = checkTypeOfEntity(string[index]);
        std::string content;
        switch (type) {
            case TypeOfEntity::String:
                readEntireString(string, content, index);
                dObject.GetVectorOfItems(itemName).emplace_back(content);
                break;
            case TypeOfEntity::Number:
                readUntilChar(string, content, index, entityEndChars);
                if (content.find(DOT) != std::string::npos) {
                    dObject.GetVectorOfItems(itemName).emplace_back(std::stod(content));
                    break;
                }
                dObject.GetVectorOfItems(itemName).emplace_back(std::stoll(content));
                break;
            case TypeOfEntity::Boolean:
                readUntilChar(string, content, index, entityEndChars);
                throwParseErrorIf(content != TRUE_STRING && content != FALSE_STRING);
                dObject.GetVectorOfItems(itemName).emplace_back(content == TRUE_STRING);
                break;
            default:
                throwParseErrorIf(true);
        }
    } while (string[index++] != SQUARE_BRACKET_END);
}

void DSerializer::DDocument::readObjectsOfVector(std::string &string, DSerializer::DObject &dObject, std::string &itemName, size_t &index) {
    do {
        if (string[index] == COMMA) index++;
        auto c = string[index];
        throwParseErrorIf(string[index++] != CURLY_BRACKET_START);
        DObject dObj;
        readObject(string, dObj, index);
        dObject.GetVectorOfObjects(itemName).emplace_back(std::move(dObj));
    } while (string[index++] != SQUARE_BRACKET_END);
}


