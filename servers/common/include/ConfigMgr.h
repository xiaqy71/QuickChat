#include <unordered_map>
struct SectionInfo
{
    SectionInfo()  = default;
    ~SectionInfo() = default;

    SectionInfo(const SectionInfo& rhs) = default;
    SectionInfo& operator=(const SectionInfo& rhs) = default;
    std::string& operator[](const std::string& key);

    std::unordered_map<std::string, std::string> section_datas_;
};

class ConfigMgr
{
public:
    ~ConfigMgr()                    = default;
    ConfigMgr(const ConfigMgr& src) = default;
    ConfigMgr&        operator=(const ConfigMgr& rhs) = default;
    SectionInfo       operator[](const std::string& section);
    static ConfigMgr& Inst();

private:
    ConfigMgr();
    std::unordered_map<std::string, SectionInfo> config_map_;
};