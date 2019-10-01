#include "console.h"

#include "readandparse.h"

Console::Console(std::string _path, std::string _rawPath, int _id, bool _debug): path(_path), rawPath(_rawPath), id(_id), debug(_debug), parser(new JoyfileParser(_path)) {}
