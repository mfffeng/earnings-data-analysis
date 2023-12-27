#include <map>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

class csvParser {
    public:
        csvParser(string filename);
        ~csvParser();
        void parse();
        vector<vector<string>> getParsedCsv();
        void splitGroups();
        map<string, vector<vector<string>>> getGroups();
        unordered_map<string, string> getRawCsv();

    private:
        string filename;
        bool isParsed = false;
        vector<vector<string>> parsedCsv;
        unordered_map<string, string> rawCsv;
        map<string, vector<vector<string>>> groups;
};