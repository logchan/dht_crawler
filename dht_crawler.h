#define BOOST_ASIO_SEPARATE_COMPILATION

#ifndef _DHT_CRAWLER_H
#define _DHT_CRAWLER_H

#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"
#include "libtorrent/alert_types.hpp"
#include "libtorrent/settings.hpp"
#include "libtorrent/session_settings.hpp"
#include <vector>
#include <map>

class dht_crawler
{
public:

	dht_crawler(std::string result_file, int session_num, int start_port, int total_intervals);
	void create_sessions();
	void print_settings(std::ostream&) const;
	void run();

	// session settings
	int upload_rate_limit = 200000;
	int download_rate_limit = 200000;
	int active_downloads = 30;
	int alert_queue_size = 4000;
	int dht_announce_interval = 60;
	int torrent_upload_limit = 20000;
	int torrent_download_limit = 20000;
	int auto_manage_startup = 30;
	int auto_manage_interval = 15;

	// crawler settings
	int start_port = 32900;
	int session_num = 50;
	int total_intervals = 60;
	int writing_interval = 60;
	std::string result_file;
	std::vector< std::pair<std::string, int> > trackers;

private:

	void handle_alerts(libtorrent::session*, std::deque<libtorrent::alert*>*);
	void add_magnet(std::string link);
	bool write_result_file();

	// storage
	int current_meta_count = 0;
	std::vector<libtorrent::session*> sessions;
	std::vector<std::string> info_hash_from_getpeers;
	std::map<std::string, int> meta;
};

#endif // _DHT_CRAWLER_H