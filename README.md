dht_crawler
===========

a dht crawler using libtorrent

##How to compile
~~Well, currently all params are hard coded. I'm just testing whether it works fine. Basically, do NOT use it yet.~~

Installing the dependencies:

```shell
apt-get install autoconf libtool libssl-dev libboost-all-dev
apt-get install libtorrent-rasterbar-dev
```
Then you can make use of my *Makefile*.

Note: The output directory of the Makefile is set to **HOME**, and you may want to change it.

A general way of compiling the code using libtorrent:

```shell
g++ source.cpp -ltorrent-rasterbar -lboost_system -o output
```
##How to use
```shell
./dht_crawler
./dht_crawler config.json
```
If config file is given, it tries to parse it (and exit if failed). Otherwise, it uses the default values.

A config file should be a *json* file like this:

```json
{
	"start_port": "31321",
	"result_file": "dht_result.txt",
	"trackers":
	{
		"some.tracker.address": 1234,
		"some.tracker2.address": 5678
	}
}
```
For a list of accepted configurations, run it without a config file, and check the printed list of settings.

The *trackers* section shall contain address-port pairs. The format is given above.

The *result_file* contains the hash of torrents. The format of each line is
```
SOME_HEX_NUMBER_OF_HASH HEAT
```
The two values are seperated by a tab (```\t```).
##Working on
Save and resume states

Show and save statistics

##Special thanks
#####This project is inspired by the python version of DHTCrawler written by blueskyz: https://github.com/blueskyz/DHTCrawler
