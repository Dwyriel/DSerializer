#ifndef JSONPARSER_JSONPARSER_H
#define JSONPARSER_JSONPARSER_H

#include <map>
#include <vector>
#include <filesystem>
#include <libs/DVariant/DVariant.h>

namespace DSerializer {
    class DObject {
        friend class DDocument;

        std::string objectName;
        std::map<std::string, char> names;
        std::map<std::string, DVariant> items;
        std::map<std::string, DObject> objects;
        std::map<std::string, std::vector<DVariant>> vectorOfItems;
        std::map<std::string, std::vector<DObject>> vectorOfObjects;

        void checkName(const std::string &name);

    public:
        DObject() noexcept;

        explicit DObject(std::string name) noexcept;

        void SetObjectName(std::string name);

        [[nodiscard]] const std::string &GetObjectName() const noexcept;

        void SetItem(const std::string &name, DVariant item);

        DVariant &GetItem(const std::string &name);

        void SetObject(DObject dObject);

        DObject &GetObject(const std::string &name);

        void SetVector(const std::string &name, std::vector<DVariant> vector);

        void SetVector(const std::string &name, std::vector<DObject> vector);

        std::vector<DVariant> &GetVectorOfItems(const std::string &name);

        std::vector<DObject> &GetVectorOfObjects(const std::string &name);

        DVariant &operator[](const std::string &name);
    };

    using DObjVector = std::vector<DObject>;

    using DVarVector = std::vector<DVariant>;

    class DDocument {
        DObject _mainObj;
        std::filesystem::path _file;

        void checkFile();

        void checkObject(const DObject &dObject);

        void addTabs(std::ofstream &stream, int tabNumber);

        void serializeItems(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeObject(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeObjectOfVector(std::ofstream &stream, DObject dObject, int tabNumber);

        void serializeVector(std::ofstream &stream, DVarVector &vector, int tabNumber);

        void serializeVector(std::ofstream &stream, DObjVector &vector, int tabNumber);

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
}

#endif //JSONPARSER_JSONPARSER_H
