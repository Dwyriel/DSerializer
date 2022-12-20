#ifndef JSONPARSER_JSONPARSER_H
#define JSONPARSER_JSONPARSER_H

#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <utility>
#include <libs/DVariant/DVariant.h>

class DObject {
    std::string objectName;
    std::map<std::string, DVariant> items;
    std::map<std::string, DObject> objects;
    std::map<std::string, std::vector<DVariant>> vectorOfItems;
    std::map<std::string, std::vector<DObject>> vectorOfObjects;

public:
    DObject() noexcept;

    explicit DObject(std::string name) noexcept;

    void SetObjectName(std::string name) noexcept;

    const std::string &GetObjectName() noexcept;

    void SetItem(const std::string &name, DVariant item) noexcept;

    DVariant &GetItem(const std::string &name) noexcept;

    void SetObject(DObject object) noexcept;

    DObject &GetObject(const std::string &name) noexcept;

    void SetVector(const std::string &name, std::vector<DVariant> vector) noexcept;

    void SetVector(const std::string &name, std::vector<DObject> vector) noexcept;

    std::vector<DVariant> &GetVectorOfItems(const std::string &name) noexcept;

    std::vector<DObject> &GetVectorOfObjects(const std::string &name) noexcept;

    DVariant &operator[](const std::string &name);
};

class DDocument {
    DObject _mainObj;
    std::filesystem::path _file;

    void checkFile();

    void checkObject();

public:
    DDocument() noexcept;

    explicit DDocument(std::string file) noexcept;

    explicit DDocument(DObject dObject) noexcept;

    explicit DDocument(std::string file, DObject dObject) noexcept;

    void SetObject(DObject dObject) noexcept;

    DObject &GetObject() noexcept;

    void SetFile(std::string file) noexcept;

    const std::string &GetFile() noexcept;

    bool Save();

    DObject &Load();
};

#endif //JSONPARSER_JSONPARSER_H
