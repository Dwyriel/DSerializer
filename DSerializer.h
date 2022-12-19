#ifndef JSONPARSER_JSONPARSER_H
#define JSONPARSER_JSONPARSER_H

#include <map>
#include <vector>
#include <string>
#include <utility>
#include <libs/DVariant/DVariant.h>

class DObject {
    std::string objectName;
    std::map<std::string, DVariant> items;
    std::map<std::string, DObject> objects;
    std::map<std::string, std::vector<DVariant>> vectorOfItems;

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

    std::vector<DVariant> &GetVector(const std::string &name) noexcept;

    DVariant &operator[](const std::string &name);
};

class DDocument {
    DObject _mainObj;
    std::string _file;

public:
    DDocument();

    explicit DDocument(std::string file);

    explicit DDocument(DObject jObject);

    explicit DDocument(std::string file, DObject jObject);

    bool Save();

    DObject &Load();
};

#endif //JSONPARSER_JSONPARSER_H
