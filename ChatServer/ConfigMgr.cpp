#include "ConfigMgr.h"

ConfigMgr::ConfigMgr() {
	boost::filesystem::path path = boost::filesystem::current_path();
	boost::filesystem::path config_path = path / "config.ini";

	std::cout << "config path is " << config_path << std::endl;

	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	for (const auto& section_pair : pt) {
		const ::std::string& section_name = section_pair.first;
		const boost::property_tree::ptree& section = section_pair.second;
		std::map<std::string, std::string> section_config;
		for (const auto& key_pair : section) {
			const std::string& key = key_pair.first;
			const std::string& value = key_pair.second.get_value<std::string>();
			section_config[key] = value;
		}

		SectionInfo section_info;
		section_info._section_datas = section_config;
		_config_map[section_name] = section_info;
	}

	// 输出所有的section和key-value对  
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto& key_value_pair : section_config._section_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}

}