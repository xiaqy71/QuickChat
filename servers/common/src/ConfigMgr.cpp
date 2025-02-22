#include "ConfigMgr.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

std::string& SectionInfo::operator[](const std::string& key)
{
    return section_datas_[key];
}

ConfigMgr::ConfigMgr()
{
    // 获取当前工作目录
    boost::filesystem::path current_path = boost::filesystem::current_path();
    // 构建config.ini文件的完整路径
    boost::filesystem::path config_path = current_path / "common.ini";
    std::cout << "Config path: " << config_path << std::endl;

    // 使用Boost.PropertyTree来读取ini文件
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(config_path.string(), pt);

    for (const auto& section_pair : pt) {
        const std::string&                 section_name = section_pair.first;
        const boost::property_tree::ptree& section_tree = section_pair.second;

        // 对于每个section，遍历其所有的key-value对
        std::unordered_map<std::string, std::string> section_config;
        for (const auto& key_value_pair : section_tree) {
            const std::string& key   = key_value_pair.first;
            const std::string& value = key_value_pair.second.get_value<std::string>();
            section_config[key]      = value;
        }
        SectionInfo sectionInfo;
        sectionInfo.section_datas_ = section_config;
        config_map_[section_name]  = sectionInfo;
    }

    for (const auto& section_entry : config_map_) {
        const std::string& section_name   = section_entry.first;
        SectionInfo        section_config = section_entry.second;
        std::cout << "[" << section_name << "]" << std::endl;
        for (const auto& key_value_pair : section_config.section_datas_) {
            std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
        }
    }
}

SectionInfo ConfigMgr::operator[](const std::string& section)
{
    return config_map_[section];
}

ConfigMgr& ConfigMgr::Inst()
{
    static ConfigMgr cfg_mgr;
    return cfg_mgr;
}
