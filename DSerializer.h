#ifndef JSONPARSER_JSONPARSER_H
#define JSONPARSER_JSONPARSER_H

#include <map>
#include <vector>
#include <string>
#include <filesystem>
#include <libs/DVariant/DVariant.h>

class DObject {
    friend class DDocument;

    std::string objectName;
    std::map<std::string, char> names;
    std::map<std::string, DVariant> items;
    std::map<std::string, DObject> objects;
    std::map<std::string, std::vector<DVariant>> vectorOfItems;
    std::map<std::string, std::vector<DObject>> vectorOfObjects;

public:
    DObject() noexcept;

    explicit DObject(std::string name) noexcept;

    void SetObjectName(std::string name) noexcept;

    [[nodiscard]] const std::string &GetObjectName() const noexcept;

    void SetItem(const std::string &name, DVariant item);

    DVariant &GetItem(const std::string &name);

    void SetObject(DObject object);

    DObject &GetObject(const std::string &name);

    void SetVector(const std::string &name, std::vector<DVariant> vector);

    void SetVector(const std::string &name, std::vector<DObject> vector);

    std::vector<DVariant> &GetVectorOfItems(const std::string &name);

    std::vector<DObject> &GetVectorOfObjects(const std::string &name);

    DVariant &operator[](const std::string &name);
};

class DDocument {
    DObject _mainObj;
    std::filesystem::path _file;

    void checkFile();

    void checkObject();

    void checkInnerObject(const DObject &dObject);

public:
    DDocument() noexcept;

    explicit DDocument(std::filesystem::path file) noexcept;

    explicit DDocument(DObject dObject) noexcept;

    explicit DDocument(std::filesystem::path file, DObject dObject) noexcept;

    void SetObject(DObject dObject) noexcept;

    DObject &GetObject() noexcept;

    void SetFile(std::filesystem::path file) noexcept;

    [[nodiscard]] const std::filesystem::path &GetFile() const noexcept;

    bool Save();

    DObject &Load();
};

#endif //JSONPARSER_JSONPARSER_H
