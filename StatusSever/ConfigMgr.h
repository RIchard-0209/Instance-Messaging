#pragma once

#include "const.h"

struct SectionInfo {
	SectionInfo() {}
	~SectionInfo() { _section_datas.clear(); }

	SectionInfo(const SectionInfo& other) {
		_section_datas = other._section_datas;
	}

	SectionInfo operator=(const SectionInfo& other) {
		if (this == &other) {
			return *this;
		}
		_section_datas = other._section_datas;
		return *this;
	}

	std::map<std::string, std::string> _section_datas;
	std::string operator[] (const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end()) {
			return "";
		}

		return _section_datas[key];
	}
};


/* 
	因为在main函数中定义了这个，大部分都需要用到所以制作成单例的方式
*/
class ConfigMgr {
public:
	~ConfigMgr() {
		_config_map.clear();
	}

	SectionInfo operator[] (const std::string& key) {
		if (_config_map.find(key) == _config_map.end()) {
			return SectionInfo();
		}
		return _config_map[key];
	}

	static ConfigMgr& GetInstance() {
		static ConfigMgr cfg_mgr;
		return cfg_mgr;
	}

	ConfigMgr(const ConfigMgr& other) {
		_config_map = other._config_map;
	}

	ConfigMgr& operator=(const ConfigMgr& other) {
		if (this == &other) {
			return *this;
		}
		_config_map = other._config_map;
		return *this;
	}

private:
	std::map<std::string, SectionInfo> _config_map;

	ConfigMgr();
};

