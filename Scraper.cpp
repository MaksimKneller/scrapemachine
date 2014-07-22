/*
 * Scraper.cpp
 *
 *  Created on: Apr 24, 2014
 *      Author: max
 */

#include "Scraper.h"
#include <map>


size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  mem->memory = (char *)realloc(mem->memory, mem->size + realsize + 1);
  if (mem->memory == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    exit(EXIT_FAILURE);
  }

  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

MemoryStruct getMemChunk()
{
	struct MemoryStruct chunk;
	chunk.memory = (char *)malloc(1);  	/* will be grown as needed by the realloc above */
	chunk.size = 0;    					/* no data at this point */

	return chunk;
}

Scraper::Scraper() {

	curl = curl_easy_init();

}

bool Scraper::scrape(ScrapeRequest & req, ScrapeResult & res)
{

	bool scrapeResult(0);

	if(req.connParams.url.substr(0,5) == "file:")
	{
		scrapeResult = (fetchFile(req,res)) ? 1 : 0;
	}
	else if(req.connParams.url.substr(0,5) == "http:")
	{
		scrapeResult = (fetchHTML(req, res, curl));
	}
	else if(req.connParams.url.substr(0,6) == "https:")
	{
		scrapeResult = (fetchHTML(req, res, curl));
	}

	// if a regex expression was supplied then do regex search on the resulting output
	if(req.regexParams.re != "") processRegex(req,res);

	return scrapeResult;

}

bool fetchFile(ScrapeRequest & req, ScrapeResult & res)
{

	int urlLength(req.connParams.url.length());

	std::cout << "Fetching: " << req.connParams.url.substr(7,urlLength) << std::endl;

	std::ifstream file(req.connParams.url.substr(7,urlLength));
	std::string str;

	if(!file) { res.errorMsg = "Unable to open file."; return 0; }

	while(std::getline(file, str))
	{
		res.result += str;
		res.result.push_back('\n');
	}


	return 1;
}

bool fetchHTML(ScrapeRequest & req, ScrapeResult & res, CURL * curl)
{
	std::cout << "Fetching: " << req.connParams.url << std::endl;


	if(!req.connParams.useragent.length()) req.connParams.useragent = ("Mozilla/5.0 (X11; Ubuntu; Linux i686; rv:18.0) Gecko/20100101 Firefox/18.0");

	std::cout << "Request URL: " << req.connParams.url << std::endl;
	std::cout << "Useragent: " << req.connParams.useragent << std::endl;


	MemoryStruct chunk = getMemChunk();

	//curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // enables tracking of cookies for duration of curl seession
	//curl_easy_setopt(curl, CURLOPT_COOKIE, cookie.c_str() );

	curl_easy_setopt(curl, CURLOPT_URL, req.connParams.url.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, req.connParams.useragent.c_str() );
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	curl_easy_setopt(curl, CURLOPT_COOKIEFILE, ""); // enables tracking of cookies for duration of curl seession

	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

	//curl_easy_setopt(curl, CURLOPT_VERBOSE, 1);

	// Apply cookies if supplied
	if(req.connParams.cookies.size() > 0)
	{
		std::string combinedCookies;

		for(std::map<std::string,std::string>::iterator cIter = req.connParams.cookies.begin(); cIter != req.connParams.cookies.end(); cIter++)
			combinedCookies += cIter->first + "=" + cIter->second + ";";

		curl_easy_setopt(curl, CURLOPT_COOKIE, combinedCookies.c_str() );

		std::cout << "Setting cookies: " << combinedCookies << std::endl;


	}

	// Configure header with multipart/form-data fields
	if(req.connParams.multipartFields.size() > 0)
	{
		struct curl_httppost 	*formpost	= NULL;
		struct curl_httppost 	*lastptr	= NULL;
		struct curl_slist 		*headerlist	= NULL;

		static const char buff[] = "Expect:"; // this will remove the Expect header by not providing any value for it


		// add the Expect header to the main headerlist
		headerlist = curl_slist_append(headerlist,buff);

		// insert main headerlist into curl
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);

		for(std::list<std::pair<std::string,std::string> >::iterator i = req.connParams.multipartFields.begin(); i != req.connParams.multipartFields.end(); i++)
		{

			std::cout << "Setting header field: " << i->first << "=" << i->second << std::endl;

			curl_formadd(&formpost,
						 &lastptr,
						  CURLFORM_COPYNAME, (*i).first.c_str(),
						  CURLFORM_COPYCONTENTS, (*i).second.c_str(),
						  CURLFORM_END);

		}

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);


	} // header update


	// DO THAT FETCH!!!
	CURLcode curlres( curl_easy_perform(curl) );

	if (curlres == CURLE_OK)
	{
		res.result = (char*)chunk.memory;

		// decode cookies and convert into a single string
		struct curl_slist * siteCookies;

		curl_easy_getinfo(curl, CURLINFO_COOKIELIST, &siteCookies);

		boost::regex cookieRegex("^(?:.*?\\s){5}(.*?)\\s+(.*?)$", boost::regex::icase | boost::regex::perl);
		boost::sregex_iterator end;


		while (siteCookies)
		{
			  std::string cookieline = siteCookies->data;

			  boost::sregex_iterator it(cookieline.begin(), cookieline.end(), cookieRegex);

			  if(it != end)
			  {

				std::string key((*it)[1].first, (*it)[1].second);
				std::string val((*it)[2].first, (*it)[2].second);

				res.cookies[key] = val;

			  }

			  siteCookies = siteCookies->next;

		} // loop every cookie line


	}
	else
	{
		res.errorMsg = curl_easy_strerror(curlres);
	}


	if(chunk.memory) free(chunk.memory);

    return (curlres == CURLE_OK) ? 1 : 0;

}

void processRegex(ScrapeRequest & req, ScrapeResult & res)
{

	std::cout << "Processing Regex Expression: " << req.regexParams.re << std::endl;


	boost::sregex_iterator matchIter(res.result.begin(), res.result.end(),
			boost::regex(req.regexParams.re, boost::regex::icase | boost::regex::perl));

	boost::sregex_iterator end;

	res.regexMatches.clear();

	// loop all matches
	for(; matchIter != end; ++matchIter)
	{

		// for each sub-expression create a map of <tagname,sub_expression_result>
		std::map<std::string,std::string> subExpressionMap;

		for(size_t tagIndex(0); tagIndex < req.regexParams.tags.size(); tagIndex++)
		{
			subExpressionMap.insert(std::make_pair(req.regexParams.tags.at(tagIndex),
													std::string((*matchIter)[tagIndex+1].first, (*matchIter)[tagIndex+1].second)));
		}

		res.regexMatches.push_back(subExpressionMap);

	} // eof for each match iterator


}


Scraper::~Scraper()
{
		curl_easy_cleanup(curl);
}

