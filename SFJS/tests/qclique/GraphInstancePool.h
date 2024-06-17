#pragma once

#include <vector>
#include <string>

class GraphInstancePool {
public:
	static std::vector<std::string> getRafaelInstances()
	{
		std::vector<std::string> list;
        list.push_back("3-FullIns_3");
		list.push_back("494bus");
		list.push_back("662bus");
		list.push_back("anna");/**/
		list.push_back("attiro");/**/
		list.push_back("chesapeake");/**/
		list.push_back("david");/**/
		list.push_back("dolphins");/**/
		list.push_back("high-tech");/**/
		list.push_back("huck");/**/
		list.push_back("ieeebus");/**/
		list.push_back("jean");/**/
		list.push_back("karate");/**/
		list.push_back("krebs");/**/
		list.push_back("mexican");/**/
		list.push_back("prison");/**/
		list.push_back("sanjuansur");
		list.push_back("sawmill");/**/
		list.push_back("sfi");/**/
		list.push_back("tailorS1");/**/
		list.push_back("tailorS2");
		list.push_back("usair");
		list.push_back("yeast");
		return list;
	};

	static std::vector<std::string> getRafaelSmallPoolInstances()
	{
		std::vector<std::string> list;
		list.push_back("anna");/**/
		list.push_back("mexican");/**/
		list.push_back("4-FullIns_3");
		return list;
	};

    
    static std::vector<std::string> getXXLInstances()
    {
        std::vector<std::string> list;
        list.push_back("as-22july06");/**/
        list.push_back("astro-ph");/**/
        list.push_back("audikw_1");
        list.push_back("belgium_osm");
        list.push_back("cage15");
        list.push_back("caidaRouterLevel");
        return list;
    };
    
    static std::vector<std::string> getLInstances()
    {
        std::vector<std::string> list;
        //list.push_back("494_bus");/**///true
        list.push_back("685_bus");/**///true
        list.push_back("bcspwr06");//false
        list.push_back("bcsstk13");//true
        list.push_back("bcsstk15");//true
        list.push_back("bcsstk28");//true
       list.push_back("man_5976");//false
        list.push_back("rdb5000");//true
        return list;
    };


	static std::vector<std::string> getSelectedRafaelInstances()
	{
		std::vector<std::string> list;
        list.push_back("Memento");
        list.push_back("The_X_Files");
        list.push_back("Alien_3");
        list.push_back("Batman_Returns");
        list.push_back("3-FullIns_3");
        list.push_back("4-FullIns_3");
		list.push_back("anna");/**/
		list.push_back("attiro");/**/
		list.push_back("chesapeake");/**/
		list.push_back("david");/**/
		list.push_back("dolphins");/**/
		list.push_back("high-tech");/**/
		list.push_back("ieeebus");/**/
		list.push_back("jean");/**/
		list.push_back("karate");/**/
		list.push_back("krebs");/**/
        list.push_back("myciel6");/**/
		list.push_back("mexican");/**/
		list.push_back("prison");/**/
		list.push_back("sanjuansur");/**/
		list.push_back("sawmill");/**/
		list.push_back("sfi");/**/
		list.push_back("tailorS1");/**/
		return list;
	};
};
