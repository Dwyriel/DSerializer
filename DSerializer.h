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
        enum class TypeOfEntity : char {
            String, Number, Boolean, Object, Vector, None
        };

        enum class TypeOfVector : char {
            Item, Object, None
        };

        DObject _mainObj;
        std::filesystem::path _file;

        void checkFile();

        void checkObject(const DObject &dObject);

        void addTabs(std::ofstream &stream, int tabNumber);

        void serializeItems(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeObject(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeObjectWithoutName(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeObjectCommonAttributes(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeEntityOfVector(std::ofstream &stream, DVarVector &vector, int tabNumber);

        void serializeEntityOfVector(std::ofstream &stream, DObjVector &vector, int tabNumber);

        void serializeSubObjects(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeVectorOfItems(std::ofstream &stream, DObject &dObject, int tabNumber);

        void serializeVectorOfObjects(std::ofstream &stream, DObject &dObject, int tabNumber);

        inline void throwParseErrorIf(bool condition);

        void putFileContentsIntoString(std::string &string);

        void removeNewLinesTabsAndSpaces(std::string &string);

        /**
         * @param string the string containing the name
         * @param outputString the string that will receive the name
         * @param index The index of the first quotation mark(")
         */
        void readEntireString(std::string &string, std::string &outputString, size_t &index);

        void readUntilChar(std::string &string, std::string &outputString, size_t &index, const std::vector<char> &characters);

        TypeOfEntity checkTypeOfEntity(char character);

        TypeOfVector checkTypeOfVector(char character);

        void readObject(std::string &string, DObject &dObject, size_t &index);

        void readString(std::string &string, DObject &dObject, std::string &itemName, size_t &index);

        void readNumber(std::string &string, DObject &dObject, std::string &itemName, size_t &index);

        void readBoolean(std::string &string, DObject &dObject, std::string &itemName, size_t &index);

        void readVector(std::string &string, DObject &dObject, std::string &itemName, size_t &index);

        void readItensOfVector(std::string &string, DObject &dObject, std::string &itemName, size_t &index);

        void readObjectsOfVector(std::string &string, DObject &dObject, std::string &itemName, size_t &index);

    public:
        DDocument() noexcept;

        explicit DDocument(std::filesystem::path file) noexcept;

        explicit DDocument(DObject dObject) noexcept;

        explicit DDocument(std::filesystem::path file, DObject dObject) noexcept;

        void SetObject(DObject dObject) noexcept;

        DObject &GetObject() noexcept;

        void SetFile(std::filesystem::path file) noexcept;

        [[nodiscard]] const std::filesystem::path &GetFile() const noexcept;

        void Save();

        DObject &Load();
    };
}

#endif //JSONPARSER_JSONPARSER_H
