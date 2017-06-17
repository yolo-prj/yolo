#include <yolo/YFramework/YMessageFormatParser.h>


namespace yolo
{
YMessageFormatParser::YMessageFormatParser(string jsonpath)
{
    _jsonpath = jsonpath;
}

YMessageFormatParser::~YMessageFormatParser()
{
}

bool
YMessageFormatParser::parse()
{
    bool success = true;

    try {
	read_json(_jsonpath, _msgtree);
    } catch(...) {
	success = false;
    }

    return success;
}

ptree
YMessageFormatParser::getPropertyTree(uint opcode)
{
    ptree pt;

    char buf[100];
    sprintf(buf, "%d", opcode);

    string op = buf;

    pt = _msgtree.get_child(op);

    return pt;
}

YMessage
YMessageFormatParser::getMessage(uint opcode)
{
    YMessage msg;
    
    ptree pt = getPropertyTree(opcode);

    msg.setOpcode(opcode);
    msg.setPropertyTree(pt);

    return msg;
}

}
