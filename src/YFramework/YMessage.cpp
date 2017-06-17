#include <yolo/YFramework/YMessage.h>
#include <sstream>

using boost::property_tree::ptree_bad_data;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

namespace yolo {

YMessage::YMessage()
{
    _bin = nullptr;
    _binary = false;
    _opcode = 0;
    _serialized = nullptr;
}

YMessage::~YMessage()
{
    if(_bin != nullptr) {
	delete _bin;
    }

    if(_serialized != nullptr) {
	delete _serialized;
    }
}

void
YMessage::setPropertyTree(ptree pt) {
    this->_pt = pt;
}

bool
YMessage::set(string name, string value)
{
    bool success = true;

    try {
	_pt.put(name, value);
	_binary = false;
    } catch(ptree_bad_data& ex) {
	success = false;
    }

    return success;
}

bool
YMessage::set(string name, int value)
{
    bool success = true;

    try {
	_pt.put(name, value);
	_binary = false;
    } catch(ptree_bad_data& ex) {
	success = false;
    }

    return success;
}

bool
YMessage::set(string name, double value)
{
    bool success = true;

    try {
	_pt.put(name, value);
	_binary = false;
    } catch(ptree_bad_data& ex) {
	success = false;
    }

    return success;
}

bool
YMessage::set(byte* data, uint length)
{
    bool success = true;

    if(data == nullptr || length == 0) {
	success = false;
    } else {
	if(_bin != nullptr) {
	    delete _bin;
	}

	_bin = new Binary(data, length);
	_binary = true;
    }

    return success;
}

string
YMessage::getString(string name)
{
    return _pt.get<string>(name);
}

int
YMessage::getInt(string name)
{
    return _pt.get<int>(name);
}

double
YMessage::getDouble(string name)
{
    return _pt.get<double>(name);
}

byte*
YMessage::getBinary(uint& length)
{
    byte* result = nullptr;
    length = 0;

    if(_bin != nullptr) {
	result = _bin->get(length);
    }

    return result;
}

byte*
YMessage::serialize(uint& length)
{
    if(_serialized != nullptr) {
	delete[] _serialized;
	_serialized = nullptr;
    }
    if(_binary)
    {
	uint payloadLength = 0;
	byte* data = _bin->get(payloadLength);

	length = payloadLength + sizeof(uint) * 2;

	_serialized = new byte[length];
	memset(_serialized, 0, length);
	memcpy(_serialized, &this->_opcode, sizeof(uint));
	memcpy(_serialized + sizeof(uint), &length, sizeof(uint));
	memcpy(_serialized + sizeof(uint)*2, data, payloadLength);
    }
    else
    {
	stringstream os;
	write_json(os, _pt);

	string payload = os.str();
	
	length = payload.length() + sizeof(uint) * 2 + 1;

	_serialized = new byte[length];
	memset(_serialized, 0, length);
	memcpy(_serialized, &this->_opcode, sizeof(uint));
	memcpy(_serialized + sizeof(uint), &length, sizeof(uint));
	memcpy(_serialized + sizeof(uint)*2, payload.c_str(), payload.length());
    }

    return _serialized;
}

bool
YMessage::deserialize(byte* data, uint length)
{
    bool success = true;

    uint opcode, payloadLength;

    memcpy(&opcode, data, sizeof(uint));
    memcpy(&payloadLength, data + sizeof(uint), sizeof(uint));

    if(_binary)
    {
	if(_bin != nullptr) {
	    delete _bin;
	}
	_bin = new Binary(data + sizeof(int) * 2, payloadLength);
    }
    else
    {
	string payload;
	payload = (char*)(data + sizeof(uint) * 2);

	istringstream is(payload);
	read_json(is, _pt);
    }

    return success;
}

}
