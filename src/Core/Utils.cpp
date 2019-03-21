#include "Utils.h"

namespace raw
{
std::vector<char> readFile(const std::string& filename) {
	std::string path = filename;
	char pBuf[1024];
#ifdef XWIN_WIN32

	_getcwd(pBuf, 1024);
	path = pBuf;
	path += "\\";
#else
	getcwd(pBuf, 1024);
	path = pBuf;
	path += "/";
#endif
	path += filename;
	std::ifstream file(path, std::ios::ate | std::ios::binary);
	bool exists = (bool)file;

	if (!exists || !file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
};
}