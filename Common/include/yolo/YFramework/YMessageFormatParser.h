#ifndef _YOLO_MESSAGE_FORMAT_PARSER_H_
#define _YOLO_MESSAGE_FORMAT_PARSER_H_

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include "YMessage.h"

using boost::property_tree::ptree;
using boost::property_tree::read_json;

using namespace std;

namespace yolo {

class YMessageFormatParser
{
private:
    YMessageFormatParser() {}

public:
    YMessageFormatParser(string jsonpath);
    ~YMessageFormatParser();

    bool	parse();

    YMessage	getMessage(uint opcode);

private:
    string	_jsonpath;
    ptree	_msgtree;

private:
    ptree	getPropertyTree(uint opcode);
};

} // namespace yolo

#endif
