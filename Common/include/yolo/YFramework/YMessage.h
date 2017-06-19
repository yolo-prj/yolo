#ifndef _YOLO_MESSAGE_H_
#define _YOLO_MESSAGE_H_

#include <iostream>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using boost::property_tree::ptree;

using namespace std;

typedef unsigned char byte;
typedef unsigned int uint;
typedef unsigned short ushort;

namespace yolo {

class YMessage
{
private:
    class Binary {
    public:
	Binary(byte* data, uint length) {
	    this->data = new byte[length];
	    memcpy(this->data, data, length);
	    this->length = length;
	}
	~Binary() {
	    delete[] data;
	}
	byte* get(uint& length) { 
	    length = this->length;
	    return data; 
	}
    private:
	Binary() {}
	byte* data;
	uint length;
    };

public:
    YMessage();
    ~YMessage();

public:
    void	setPropertyTree(ptree pt);
    // for string and primitive types
    bool        set(string name, string value);
    bool        set(string name, int value);
    bool        set(string name, double value);
    // for binary type
    bool	set(byte* data, uint length);

    string	getString(string name);
    int		getInt(string name);
    double	getDouble(string name);
    byte*	getBinary(uint& length);

    inline bool	binary() { return _binary; }
    inline void	setOpcode(uint opcode) { _opcode = opcode; }
    inline uint	getOpcode() { return _opcode; }


    byte*	serialize(uint& length);
    bool	deserialize(byte* data, uint length);

private:
    uint	_opcode;
    ptree       _pt;
    Binary*     _bin;
    bool	_binary;

    byte*	_serialized;
};

} // namespace yolo


#endif
