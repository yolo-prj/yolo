#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <functional>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <yolo/YFramework/YMessage.h>
#include <yolo/YFramework/YMessageFormatParser.h>

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;


using namespace yolo;

class EventHandler
{
public:
    void handler1(ptree pt) {
	std::cout << "\nthis is handler #1" << std::endl;
	for(auto& p : pt) {
	    std::cout << p.first << " : " << pt.get<std::string>(p.first) << std::endl;
	}
    }
    void handler2(ptree pt) {
	std::cout << "\nthis is handler #2" << std::endl;
	for(auto& p : pt) {
	    std::cout << p.first << " : " << pt.get<std::string>(p.first) << std::endl;
	}
    }
    void handler3(ptree pt) {
	std::cout << "\nthis is handler #3" << std::endl;
	for(auto& p : pt) {
	    std::cout << p.first << " : " << pt.get<std::string>(p.first) << std::endl;
	}
    }

private:
    std::map<int, std::function<void(ptree)>> handlerMap;
    void init() {
	std::function<void(ptree)> i = std::bind(&EventHandler::handler1, this, std::placeholders::_1);
	//this->handlerMap.insert(make_pair(4, std::bind(&EventHandler::handler1, this, _1)));
    }
};

std::map<int, std::function<void (ptree)>> mapHandler;
EventHandler ev;

void handler1(ptree pt) {
    std::cout << "\nthis is handler #1" << std::endl;
    /*
    std::string s;
    std::stringstream os;
    write_json(os, pt);
    std::cout << os.str() << std::endl;
    */
    //std::cout << pt.get<int>("opcode") << std::endl;

    for(auto& p : pt) {
	std::cout << p.first << " : " << pt.get<std::string>(p.first) << std::endl;
    }
}
void handler2(ptree pt) {
    std::cout << "\nthis is handler #2" << std::endl;
    for(auto& p : pt) {
	std::cout << p.first << " : " << pt.get<std::string>(p.first) << std::endl;
    }
}
void handler3(ptree pt) {
    std::cout << "\nthis is handler #3" << std::endl;
    for(auto& p : pt) {
	std::cout << p.first << " : " << pt.get<std::string>(p.first) << std::endl;
    }
}

void init() {
    /*
    mapHandler.insert( make_pair(4, handler1) );
    mapHandler.insert( make_pair(2, handler2) );
    mapHandler.insert( make_pair(3, handler3) );
    */
    /*
    mapHandler.insert( make_pair(4, boost::bind(&EventHandler::handler1, &ev, _1) ));
    mapHandler.insert( make_pair(2, boost::bind(&EventHandler::handler2, &ev, _1) ));
    mapHandler.insert( make_pair(3, boost::bind(&EventHandler::handler3, &ev, _1) ));
    */
    mapHandler.insert( make_pair(4, std::bind(&EventHandler::handler1, &ev, std::placeholders::_1) ));
    mapHandler.insert( make_pair(2, std::bind(&EventHandler::handler2, &ev, std::placeholders::_1) ));
    mapHandler.insert( make_pair(3, std::bind(&EventHandler::handler3, &ev, std::placeholders::_1) ));
}


void example() {
    // Write json.
    ptree pt;
    pt.put ("foo", "bar");
    pt.put ("val", 10);
    std::ostringstream buf; 
    write_json (buf, pt, false);
    std::string json = buf.str(); // {"foo":"bar"}

    std::cout << json << std::endl;

    // Read json.
    ptree pt2;
    std::istringstream is (json);
    read_json (is, pt2);
    std::string foo = pt2.get<std::string> ("foo");
    int val = pt2.get<int> ("val");
    
    std::cout << foo << std::endl;
    std::cout << val << std::endl;

    ptree pt3;
    std::ifstream ifs("test.json");
    read_json(ifs, pt3);
    std::string foo2 = pt3.get<std::string>("foo");
    int val2 = pt3.get<int>("val");

    std::cout << "foo : " << foo2 << std::endl;
    std::cout << "val : " << val2 << std::endl;

    pt3.put("val", 12);
    ptree pt4 = pt3;

    std::ofstream ofs("out.json");
    write_json(ofs, pt4, false);
}

std::string map2json (const std::map<std::string, std::string>& map) {
    ptree pt; 
    for (auto& entry: map) 
	pt.put (entry.first, entry.second);
    std::ostringstream buf; 
    write_json (buf, pt, false); 
    return buf.str();
}

void test() {
    ptree pt;
    ptree children;
    ptree child1, child2, child3;

    child1.put("childkeyA", 1);
    child1.put("childkeyB", 2);

    child2.put("childkeyA", 3);
    child2.put("childkeyB", 4);

    child3.put("childkeyA", 5);
    child3.put("childkeyB", 6);

    children.push_back(std::make_pair("", child1));
    children.push_back(std::make_pair("", child2));
    children.push_back(std::make_pair("", child3));

    pt.put("testkey", "testvalue");
    pt.add_child("MyArray", children);

    write_json("test2.json", pt);
}

void mytest() {
    init();
    ptree pt;

    read_json("msgformat.json", pt);


    /*
    ptree child = pt.get_child("msg2");

    std::ostringstream os;
    write_json(os, child, false);

    std::cout << os.str() << std::endl;
    */

    /*
    for(auto tr = pt.begin(); tr != pt.end(); tr++) {
	auto& child = tr->second;
	int opcode = child.get<int>("opcode");
	int key = atoi(tr->first.c_str());
	std::cout << "opcode : " << opcode << std::endl;

	auto itr = mapHandler.find(key);
	if(itr != mapHandler.end())
	    itr->second(child);
	else
	    std::cerr << "no opcode!" << std::endl;
    }
    */

    unsigned char data[] = {0x12, 0x21, 0x33, 0x42, 0x0f, 0x19};

    for(auto itr = pt.begin(); itr != pt.end(); itr++) {
	YMessage msg;
	msg.setPropertyTree(itr->second);
	msg.set(data, sizeof(data));

	uint len;
	byte* d;

	d = msg.getBinary(len);

	std::cout << "opcode is " << msg.getString("opcode") << std::endl;
	std::cout << "bin len is " << len << std::endl;
    }

}

void mytest2()
{
    YMessageFormatParser parser("msgformat.json");

    if(parser.parse()) {
	YMessage msg = parser.getMessage(2);
	msg.set("cmd", 10);

	byte* data;
	uint len;

	data = msg.serialize(len);

	for(int i=0; i<len; i++) {
	    std::cout << (char)data[i];
	}
	std::cout << std::endl;

	YMessage msg2;
	msg2.deserialize(data, len);

	std::cout << "cmd : " << msg2.getInt("cmd") << std::endl;
    }
}


int main() {
    //example();
    //test();
    mytest();
    mytest2();
    return 0;
}
