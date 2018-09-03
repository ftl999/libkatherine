#pragma once

#include <katherine/katherine.h>

namespace YAML
{
	class Node;	
}

class KatherineConfigParser
{
public:
	KatherineConfigParser(const char* configFile);
	~KatherineConfigParser();
	int getFrameCount();
	katherine_config_t* getConfig();
	size_t getBufferMD_Size();
	size_t getBufferPixel_Size();

protected:
	void configure_yaml(katherine_config_t& config, const YAML::Node& yml);

private:
	int frame_count; 
	YAML::Node* yml;
	katherine_config_t config;
	size_t buffer_md_items_size;
	size_t buffer_pixel_items_size;
};
