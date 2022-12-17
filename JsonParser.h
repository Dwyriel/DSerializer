#ifndef JSONPARSER_JSONPARSER_H
#define JSONPARSER_JSONPARSER_H

#include <map>
#include <vector>
#include <string>
#include <variant>

class JObject{
    std::string objectName;
    std::map<std::string, std::string> strings;
    std::map<std::string, double> doubles;
    std::map<std::string, long long> ints;
    std::map<std::string, std::vector<std::string>> vectorStrings;
    std::map<std::string, std::vector<double>> vectorDoubles;
    std::map<std::string, std::vector<int>> vectorInts;

public:

    explicit JObject(std::string name);

    void SetString(const std::string &name, const std::string &string);

    std::string GetString(const std::string &name);

    void SetDouble(const std::string &name, const double &number);

    std::string GetDouble(const double &number);

    void SetInteger(const std::string &name, const long long &integer);

    std::string GetInteger(const std::string &name);

    void SetVectorString(const std::string name, const std::vector<std::string> vectorString);

    std::vector<std::string> GetVectorString(const std::string &name);

    void SetVectorDouble(const std::string name, const std::vector<double> vectorDouble);

    std::vector<double> GetVectorDouble(const std::string &name);

    void SetVectorInteger(const std::string name, const std::vector<long long> vectorInteger);

    std::vector<long long> GetVectorInteger(const std::string &name);
};

#endif //JSONPARSER_JSONPARSER_H
