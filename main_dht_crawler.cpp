#include "dht_crawler.h"
#include <iostream>
#include <fstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace boost::property_tree;

template <class T>
void try_set_property_from_ptree(T& to_set, const string key, const ptree& pt)
{
	try
	{
		to_set = pt.get<T>(key);
	}
	catch (ptree_bad_path& e)
	{
		// don't have this property, ignore
		return;
	}
	catch (ptree_bad_data& e)
	{
		// failed translating data
		std::cout << "ignoring illegal value for " << key << endl;
	}
}

dht_crawler* create_crawler(string config_data)
{
	dht_crawler* crawler = new dht_crawler("result.txt", 20, 32900, 1800);

	if (config_data == "")
		return crawler;

	stringstream ss(config_data);
	ptree pt;

	try 
	{
		std::cout << config_data << endl;
		read_json(ss, pt);
	}
	catch (ptree_error& e)
	{
		std::cout << e.what() << endl;
		std::cout << "Failed parsing config file." << endl;
		exit(0);
	}

	try_set_property_from_ptree(crawler->upload_rate_limit, "upload_rate_limit", pt);
	try_set_property_from_ptree(crawler->download_rate_limit, "download_rate_limit", pt);
	try_set_property_from_ptree(crawler->active_downloads, "active_downloads", pt);
	try_set_property_from_ptree(crawler->alert_queue_size, "alert_queue_size", pt);
	try_set_property_from_ptree(crawler->dht_announce_interval, "dht_announce_interval", pt);
	try_set_property_from_ptree(crawler->torrent_upload_limit, "torrent_upload_limit", pt);
	try_set_property_from_ptree(crawler->torrent_download_limit, "torrent_download_limit", pt);
	try_set_property_from_ptree(crawler->auto_manage_startup, "auto_manage_startup", pt);
	try_set_property_from_ptree(crawler->auto_manage_interval, "auto_manage_interval", pt);
	try_set_property_from_ptree(crawler->start_port, "start_port", pt);
	try_set_property_from_ptree(crawler->session_num, "session_num", pt);
	try_set_property_from_ptree(crawler->total_intervals, "total_intervals", pt);
	try_set_property_from_ptree(crawler->writing_interval, "writing_interval", pt);
	try_set_property_from_ptree(crawler->result_file, "result_file", pt);

	return crawler;
}

int main(int argc, char** argv)
{
	string config_data("");

	// check if config file provided
	if (argc == 2)
	{
		fstream fs(argv[1], ios::in);
		if (fs.is_open())
		{
			std::cout << "Reading config file " << argv[1] << endl;
			
			stringstream ss;
			ss << fs.rdbuf();
			config_data = ss.str();

			fs.close();
		}
		else
		{
			std::cout << "Can not open config file " << argv[1] << endl;
			exit(0);
		}
	}

	std::cout << "creating crawler object" << endl;
	dht_crawler* crawler = create_crawler(config_data);

	if (crawler == NULL)
	{
		std::cout << "failed creating the crawler object" << endl;
		exit(0);
	}

	std::cout << "creating crawler sessions" << endl;
	crawler->create_sessions();

	std::cout << "running crawler" << endl;
	crawler->run();

	return 0;
}