/*
 * Scraper.h
 *
 *  Created on: Apr 24, 2014
 *      Author: max
 */

#ifndef SCRAPER_H_
#define SCRAPER_H_

#include <iostream>
#include <boost/regex.hpp>
#include <curl/curl.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <map>
#include <fstream>
#include "ScrapeRequest.h"
//#include <string>


/* =======================================================
				Assistant Functions for Curl
   ======================================================= */

// collects incoming data
struct MemoryStruct {
  char *memory;
  size_t size;
};


MemoryStruct getMemChunk();

// called by curl whenever new data becomes available
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);



/* =======================================================
				Assistant Structs for Scraper
   ======================================================= */
struct ScrapeResult
{
	std::string result;		// raw output
	std::string errorMsg;

	std::map<std::string,std::string> cookies;

	std::vector<std::map<std::string,std::string> > regexMatches;

	void clear() { result = ""; errorMsg = ""; cookies.clear(); regexMatches.clear(); }

};



bool fetchFile(ScrapeRequest & req, ScrapeResult & res);

bool fetchHTML(ScrapeRequest & req, ScrapeResult & res, CURL *);

void processRegex(ScrapeRequest & req, ScrapeResult & res);

/* =======================================================
					Main Scraper Class
   ======================================================= */

class Scraper {
public:
	Scraper();
	virtual ~Scraper();

	bool scrape(ScrapeRequest &, ScrapeResult &);

private:

	CURL * curl;


};

#endif /* SCRAPER_H_ */
