/*
 * ScrapeRequest.h
 *
 *  Created on: Apr 29, 2014
 *      Author: max
 */

#ifndef SCRAPEREQUEST_H_
#define SCRAPEREQUEST_H_

#include <iostream>
#include <vector>
#include <list>
#include <map>

class ScrapeRequest {
public:


	struct ConnectionParameters
	{
		std::string url;
		std::string useragent;

		std::map<std::string,std::string> cookies;

		std::list<std::pair<std::string,std::string> > multipartFields; // fields name,value pairs for multipart/form-data submissions

	} connParams;

	struct RegexParameters
	{
		std::string re; // regex expression
		std::vector<std::string> tags; // sub-expression names within each  match e.g. match1[tag1,tag2,tag3], match2[tag1,tag2,tag3], etc...

	} regexParams;

	void clear() {

		connParams.url = ""; connParams.useragent = "";
		connParams.cookies.clear(); connParams.multipartFields.clear();

		regexParams.re = ""; regexParams.tags.clear();
	}

	virtual ~ScrapeRequest();
};

#endif /* SCRAPEREQUEST_H_ */
