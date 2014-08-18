#include <iostream>
<<<<<<< HEAD
#include "Scraper.h"
=======
#include <Scraper.h>
>>>>>>> parent of 6fb9367... Delete main.cpp
#include "ScrapeRequest.h"

int main()
{

	Scraper scraper;

	// Fetch initial FD login page and grab cookies like session ID
	ScrapeRequest request;
	ScrapeResult  result;

<<<<<<< HEAD
	//request.connParams = { "https://www.fanduel.com/p/login#login" };
	request.connParams = { "http://maksimkneller.com" };
=======
	request.connParams = { "https://www.fanduel.com/p/login#login" };
>>>>>>> parent of 6fb9367... Delete main.cpp

		//request.regexParams = { "<title><!\\[CDATA\\[(.*?)\\]\\]></title>" };
		//request.regexParams.tags.push_back("title");

		//  request.connParams = { "file:///tmp/test.txt" };


	bool scrpResult(scraper.scrape(request, result));

	if(!scrpResult) std::cout << "Error scraping: " << request.connParams.url << " " << result.errorMsg << std::endl;

<<<<<<< HEAD
	exit(1);
=======
>>>>>>> parent of 6fb9367... Delete main.cpp

		/*
		if(result.regexMatches.size() > 0)
		{
			for(std::vector<std::map<std::string, std::string> >::iterator resIter = result.regexMatches.begin(); resIter != result.regexMatches.end(); resIter++)
			{
				std::cout << "MATCH\n";
				std::map<std::string, std::string> resMap = *resIter;

				for(std::map<std::string,std::string>::iterator mapIter = resMap.begin(); mapIter != resMap.end(); mapIter++)
					std::cout << mapIter->first << ": " << mapIter->second << "\t"; std::cout << std::endl;

			}
		}
		else
		{
			std::cout << "No regex matches.\n";
		}
		*/

		//std::cout << result.result << std::endl;
	//for(std::map<std::string,std::string>::iterator cIter = result.cookies.begin(); cIter != result.cookies.end(); cIter++)
	//std::cout << "Cookie: " << cIter->first << " : " << cIter->second << std::endl;

	// update request with hidden multipart fields from previous fetch to use for login
	request.connParams.multipartFields.push_back(std::make_pair("cc_session_id", result.cookies["PHPSESSID"]));
	request.connParams.multipartFields.push_back(std::make_pair("cc_action", "cca_login"));
	request.connParams.multipartFields.push_back(std::make_pair("cc_failure_url", "https://www.fanduel.com/p/LoginPp"));
	request.connParams.multipartFields.push_back(std::make_pair("cc_success_url", "https://www.fanduel.com/"));
	request.connParams.multipartFields.push_back(std::make_pair("checkbox_remember", "1"));

	// update request with email and password login
	request.connParams.multipartFields.push_back(std::make_pair("email", "mknell01@gmail.com"));
	request.connParams.multipartFields.push_back(std::make_pair("password", "Fpinkytoe31"));

	// apply cookies from previous request
	request.connParams.cookies = result.cookies;

	// authentication URL
	request.connParams.url = "https://www.fanduel.com/c/CCAuth";

	result.clear();


	scrpResult = scraper.scrape(request, result);

	if(scrpResult) std::cout << result.result;

	request.clear();

	return 0;
}












