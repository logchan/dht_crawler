#include "dht_crawler.h"
#include "unistd.h"
#include <fstream>

dht_crawler::dht_crawler(std::string result_file, int session_num, int start_port, int total_intervals)
{
	this->total_intervals = total_intervals;
	this->session_num = session_num;
	this->start_port = start_port;
	this->result_file = result_file;

	this->trackers.push_back(std::make_pair("router.bittorrent.com", 6881));
	this->trackers.push_back(std::make_pair("router.utorrent.com", 6881));
	this->trackers.push_back(std::make_pair("router.bitcomet.com", 6881));
	this->trackers.push_back(std::make_pair("dht.transmissionbt.com", 6881));
}

void dht_crawler::print_settings(std::ostream& os) const
{
	std::cout << "\tupload_rate_limit: " << this->upload_rate_limit << std::endl;
	std::cout << "\tdownload_rate_limit: " << this->download_rate_limit << std::endl;
	std::cout << "\tactive_downloads: " << this->active_downloads << std::endl;
	std::cout << "\talert_queue_size: " << this->alert_queue_size << std::endl;
	std::cout << "\tdht_announce_interval: " << this->dht_announce_interval << std::endl;
	std::cout << "\ttorrent_upload_limit: " << this->torrent_upload_limit << std::endl;
	std::cout << "\ttorrent_download_limit: " << this->torrent_download_limit << std::endl;
	std::cout << "\tauto_manage_startup: " << this->auto_manage_startup << std::endl;
	std::cout << "\tauto_manage_interval: " << this->auto_manage_interval << std::endl;
	std::cout << "\tstart_port: " << this->start_port << std::endl;
	std::cout << "\tsession_num: " << this->session_num << std::endl;
	std::cout << "\ttotal_intervals: " << this->total_intervals << std::endl;
	std::cout << "\twriting_interval: " << this->writing_interval << std::endl;
	std::cout << "\tresult_file: " << this->result_file << std::endl;
	std::cout << "\ttrackers:" << std::endl;
	for (auto iter = this->trackers.cbegin(); iter != this->trackers.cend(); ++iter)
	{
		std::cout << "\t\t" << iter->first << ":" << iter->second << std::endl;
	}
}

void dht_crawler::run()
{
	std::cout << "Starting running with" << std::endl;
	this->print_settings(std::cout);

	int intervals = 0;
	while (intervals < total_intervals)
	{
		for (unsigned int i = 0; i < this->sessions.size(); ++i)
		{
			sessions[i]->post_torrent_updates();
			auto alerts = new std::deque<libtorrent::alert*>;
			sessions[i]->pop_alerts(alerts);
			this->handle_alerts(sessions[i], alerts);

			delete alerts;
		}
		++intervals;

		if (intervals % writing_interval == 0)
		{
			std::cout << "interval " << intervals << " done, writing result file...";
			if (this->write_result_file())
			{
				std::cout << " success." << std::endl;
			}
			else
			{
				std::cout << " failed." << std::endl;
			}
			std::cout << "meta status: " << std::endl;
			std::cout << "\tcount: " << meta.size() << std::endl;
		}

		sleep(1);
	}

	std::cout << "Stopping running, writing result file..." << std::endl;
	if (this->write_result_file())
	{
		std::cout << " success." << std::endl;
	}
	else
	{
		std::cout << " failed." << std::endl;
	}

	for (unsigned int i = 0; i < this->sessions.size(); ++i)
	{
		auto torrents = sessions[i]->get_torrents();
		for (unsigned int j = 0; j < torrents.size(); ++j)
		{
			sessions[i]->remove_torrent(torrents[j]);
		}
		delete sessions[i];
	}
}

void dht_crawler::handle_alerts(libtorrent::session* psession, std::deque<libtorrent::alert*>* palerts)
{
	while (palerts->size() > 0)
	{
		auto palert = palerts->front();
		palerts->pop_front();
		std::string info_hash;

		switch (palert->type())
		{
		case libtorrent::add_torrent_alert::alert_type:
		{
			auto p1 = (libtorrent::add_torrent_alert*) palert;
			p1->handle.set_upload_limit(this->torrent_upload_limit);
			p1->handle.set_download_limit(this->torrent_download_limit);
			break;
		}
		case libtorrent::dht_announce_alert::alert_type:
		{
			auto p2 = (libtorrent::dht_announce_alert*) palert;
			info_hash = p2->info_hash.to_string();
			// update meta
			// c++ map: if meta[info_hash] does not exist,
			// it will be inserted and initialized to zero
			if (meta[info_hash] > 0)
			{
				++meta[info_hash];
			}
			else
			{
				meta[info_hash] = 1;
				++this->current_meta_count;
			}
			break;
		}
		case libtorrent::dht_get_peers_alert::alert_type:
		{
			auto p3 = (libtorrent::dht_get_peers_alert*) palert;
			info_hash = p3->info_hash.to_string();
			if (meta[info_hash] > 0)
			{
				++meta[info_hash];
			}
			else
			{
				this->info_hash_from_getpeers.push_back(info_hash);
				meta[info_hash] = 1;
				++this->current_meta_count;
			}
			break;
		}
		default:
			break;
		}

		delete palert;
	}
}

void dht_crawler::create_sessions()
{
	int& start_port = this->start_port;

	for (int i = 0; i < this->session_num; ++i)
	{
		auto psession = new libtorrent::session;
		psession->set_alert_mask(libtorrent::alert::category_t::all_categories);
		psession->listen_on(std::make_pair(start_port + i, start_port + i));

		for (int j = 0; j < this->trackers.size(); ++j)
		{
			psession->add_dht_router(trackers[j]);
		}

		libtorrent::session_settings settings = psession->settings();
		settings.upload_rate_limit = this->upload_rate_limit;
		settings.download_rate_limit = this->download_rate_limit;
		settings.active_downloads = this->active_downloads;
		settings.auto_manage_interval = this->auto_manage_interval;
		settings.auto_manage_startup = this->auto_manage_startup;
		settings.dht_announce_interval = this->dht_announce_interval;
		settings.alert_queue_size = this->alert_queue_size;
		psession->set_settings(settings);
		this->sessions.push_back(psession);
	}
}

void dht_crawler::add_magnet(std::string link)
{

}

bool dht_crawler::write_result_file()
{
	std::fstream fs;
	fs.open(this->result_file, std::fstream::out | std::fstream::trunc);

	if (fs.is_open())
	{
		for (auto iter = this->meta.begin(); iter != this->meta.end(); ++iter)
		{
			const std::string& s = iter->first;
			for (unsigned int i = 0; i < s.size(); ++i)
			{
				fs << std::hex;
				unsigned char c = s[i];
				if (c < 16)
				{
					fs << '0' << (unsigned int)c;
				}
				else
				{
					fs << (unsigned int)c;
				}
			}
			fs << std::dec;
			fs << '\t' << iter->second << std::endl;
		}
		fs.close();
		return true;
	}
	else
	{
		return false;
	}
}
