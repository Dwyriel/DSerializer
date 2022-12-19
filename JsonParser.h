#ifndef JSONPARSER_JSONPARSER_H
#define JSONPARSER_JSONPARSER_H

#include <stdexcept>
#include <map>
#include <vector>
#include <string>
#include <utility>
#include <libs/DVariant/DVariant.h>

class JObject {
    std::string objectName;
    std::map<std::string, DVariant> items;
    std::map<std::string, JObject> objects;
    std::map<std::string, std::vector<DVariant>> vectorOfItems;

public:
    JObject() noexcept;

    explicit JObject(std::string name) noexcept;

    void SetObjectName(std::string name) noexcept;

    const std::string &GetObjectName() noexcept;

    void SetItem(const std::string &name, DVariant item) noexcept;

    DVariant &GetItem(const std::string &name) noexcept;

    void SetObject(JObject object) noexcept;

    JObject &GetObject(const std::string &name) noexcept;

    void SetVector(const std::string &name, std::vector<DVariant> vector) noexcept;

    std::vector<DVariant> &GetVector(const std::string &name) noexcept;

    DVariant &operator[](const std::string &name);
};

#endif //JSONPARSER_JSONPARSER_H
