#include "katherine/KatherineConfigParser.h"
#include <iostream>
#include <katherine/katherine.h>
#include <yaml-cpp/yaml.h>

#ifdef WIN32
static inline FILE* FOPEN_MACRO(const char* path, const char* mode)
{
	FILE* file;
	fopen_s(&file, path, mode);
	return file;
}
 #define FOPEN(path, mode) FOPEN_MACRO(path, mode)
#else
 #define FOPEN(path, mode) fopen(path, mode)
#endif

KatherineConfigParser::KatherineConfigParser(const char * configFile)
{
	yml = new YAML::Node();
	*yml = YAML::LoadFile(configFile);
	configure_yaml(config, *yml);
	buffer_md_items_size = KATHERINE_MD_SIZE * ((*yml)["buffer_md_items"].as<std::size_t>());
	buffer_pixel_items_size = sizeof(katherine_px_f_toa_tot_t) * (*yml)["buffer_px_items"].as<std::size_t>();
}

KatherineConfigParser::~KatherineConfigParser()
{
	delete yml;
}

int KatherineConfigParser::getFrameCount()
{
	return frame_count;
}

katherine_config_t * KatherineConfigParser::getConfig()
{
	return &config;
}

size_t KatherineConfigParser::getBufferMD_Size()
{
	return buffer_md_items_size;
}

size_t KatherineConfigParser::getBufferPixel_Size()
{
	return buffer_pixel_items_size;
}

void KatherineConfigParser::configure_yaml(katherine_config_t & config, const YAML::Node & yml)
{
	// For now, these three constants are hard-coded.
	config.seq_readout_start = true;
	config.fast_vco_enabled = true;
	config.acq_mode = ACQUISITION_MODE_TOA_TOT;

	config.bias_id = yml["bias_id"].as<unsigned char>();
	config.acq_time = yml["acq_time"].as<double>() * 1e9;
	config.no_frames = yml["no_frames"].as<int>();
	config.bias = yml["bias"].as<float>();

	frame_count = config.no_frames;

	config.delayed_start = yml["delayed_start"].as<bool>();

	auto yml_start = yml["start_trigger"].as<YAML::Node>();
	config.start_trigger.use_falling_edge = yml_start["use_falling_edge"].as<bool>();
	config.start_trigger.channel = yml_start["channel"].as<char>();
	config.start_trigger.enabled = yml_start["enabled"].as<bool>();

	auto yml_end = yml["end_trigger"].as<YAML::Node>();
	config.stop_trigger.use_falling_edge = yml_end["use_falling_edge"].as<bool>();
	config.stop_trigger.channel = yml_end["channel"].as<char>();
	config.stop_trigger.enabled = yml_end["enabled"].as<bool>();

	config.gray_enable = yml["gray_enable"].as<bool>();
	config.polarity_holes = yml["polarity_holes"].as<bool>();

	switch (yml["phase"].as<int>()) {
	case 1: config.phase = PHASE_1; break;
	case 2: config.phase = PHASE_2; break;
	case 4: config.phase = PHASE_4; break;
	case 8: config.phase = PHASE_8; break;
	case 16: config.phase = PHASE_16; break;
	default:
		std::cerr << "Invalid phase." << std::endl;
		return;
	}

	switch (yml["freq"].as<int>()) {
	case 40: config.freq = FREQ_40; break;
	case 80: config.freq = FREQ_80; break;
	case 160: config.freq = FREQ_160; break;
	default:
		std::cerr << "Invalid frequency." << std::endl;
		exit(1);
	}

	auto yml_dac = yml["dacs"].as<YAML::Node>();
	config.dacs.named.Ibias_Preamp_ON = yml_dac["Ibias_Preamp_ON"].as<uint16_t>();
	config.dacs.named.Ibias_Preamp_OFF = yml_dac["Ibias_Preamp_OFF"].as<uint16_t>();
	config.dacs.named.VPReamp_NCAS = yml_dac["VPReamp_NCAS"].as<uint16_t>();
	config.dacs.named.Ibias_Ikrum = yml_dac["Ibias_Ikrum"].as<uint16_t>();
	config.dacs.named.Vfbk = yml_dac["Vfbk"].as<uint16_t>();
	config.dacs.named.Vthreshold_fine = yml_dac["Vthreshold_fine"].as<uint16_t>();
	config.dacs.named.Vthreshold_coarse = yml_dac["Vthreshold_coarse"].as<uint16_t>();
	config.dacs.named.Ibias_DiscS1_ON = yml_dac["Ibias_DiscS1_ON"].as<uint16_t>();
	config.dacs.named.Ibias_DiscS1_OFF = yml_dac["Ibias_DiscS1_OFF"].as<uint16_t>();
	config.dacs.named.Ibias_DiscS2_ON = yml_dac["Ibias_DiscS2_ON"].as<uint16_t>();
	config.dacs.named.Ibias_DiscS2_OFF = yml_dac["Ibias_DiscS2_OFF"].as<uint16_t>();
	config.dacs.named.Ibias_PixelDAC = yml_dac["Ibias_PixelDAC"].as<uint16_t>();
	config.dacs.named.Ibias_TPbufferIn = yml_dac["Ibias_TPbufferIn"].as<uint16_t>();
	config.dacs.named.Ibias_TPbufferOut = yml_dac["Ibias_TPbufferOut"].as<uint16_t>();
	config.dacs.named.VTP_coarse = yml_dac["VTP_coarse"].as<uint16_t>();
	config.dacs.named.VTP_fine = yml_dac["VTP_fine"].as<uint16_t>();
	config.dacs.named.Ibias_CP_PLL = yml_dac["Ibias_CP_PLL"].as<uint16_t>();
	config.dacs.named.PLL_Vcntrl = yml_dac["PLL_Vcntrl"].as<uint16_t>();

	{
		int res;
		auto bmc_path = yml["pixel_config"].as<std::string>();
		FILE* f = FOPEN(bmc_path.c_str(), "r");
		if (f != NULL)
		{
			char* buffer = new char[65536];
			fread(buffer, 1, 65536, f);

			katherine_bmc_init(&config.pixel_config);
			res = katherine_bmc_load(&config.pixel_config, buffer);

			if (res) {
				std::cerr << "Could not load BMC pixel configuration." << std::endl;
				exit(1);
			}

			delete[] buffer;
			fclose(f);
		}
	}
}
