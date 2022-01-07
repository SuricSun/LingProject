#include "File.h"

using namespace LingCommon::Util::IO;

void LingCommon::Util::IO::File::setPath(const char16_t* p_path) {

	this->path.assign((char16_t*)p_path);
	this->type = Type::Undef;
}

void LingCommon::Util::IO::File::setPath(const u16string& str) {

	this->path = str;
	this->type = Type::Undef;
}

File::Type LingCommon::Util::IO::File::probeFileType() {

	if (type == Type::Undef) {
		DWORD dw = 0;
		dw = GetFileAttributesW((WCHAR*)this->path.c_str());
		if (dw == INVALID_FILE_ATTRIBUTES) {
			this->type = Type::NotExist;
			return Type::NotExist;
		} else if (dw & FILE_ATTRIBUTE_DIRECTORY) {
			this->type = Type::Dir;
			return Type::Dir;
		} else {
			this->type = Type::File;
			return Type::File;
		}
	} else {
		return this->type;
	}
}

void LingCommon::Util::IO::File::createFile() const {

	HANDLE hFile = CreateFileW((WCHAR*)this->path.c_str(), 0, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {
		throw new IOException(u8"Can't create file");
	}

	if (CloseHandle(hFile) == false) {
		throw new IOException(u8"Can't close file");
	}
}

void LingCommon::Util::IO::File::createDir() const {

	if (CreateDirectoryW((WCHAR*)this->path.c_str(), nullptr) == false) {
		throw new IOException(u8"Can't create directory");
	}
}

void LingCommon::Util::IO::File::deleteFile() const {

	if (DeleteFileW((WCHAR*)this->path.c_str()) == 0) {
		throw new IOException(u8"Can't delete file");
	}
}

void LingCommon::Util::IO::File::deleteDir() const {

	if (RemoveDirectoryW((WCHAR*)this->path.c_str()) == 0) {
		throw new IOException(u8"Can't delete directory");
	}
}

vector<File>* LingCommon::Util::IO::File::listFiles() {

	return this->listFilesWithWildcards(u"*");
}

vector<File>* LingCommon::Util::IO::File::listFilesWithWildcards(const char16_t* p_wildcardFileName) {
	
	u16string searchPath = this->path + u16string(u"\\") + u16string(p_wildcardFileName);

	WIN32_FIND_DATA findData = {};
	HANDLE findHandle = FindFirstFileW((WCHAR*)searchPath.c_str(), addr(findData));
	if (findHandle == INVALID_HANDLE_VALUE) {
		throw new IOException(u8"Can't find first file");
	}

	vector<File>* p_vec = new vector<File>();
	u32 fileCnt = 0;

	do {
		p_vec->emplace_back();
		p_vec->back().setPath(this->path + u16string(u"\\") + u16string((char16_t*)(findData.cFileName)));
		fileCnt++;
	} while (FindNextFileW(findHandle, addr(findData)) != 0);

	if (FindClose(findHandle) == 0) {
		throw new IOException(u8"Can't close find handle");
	}

	return p_vec;
}

const u16string& LingCommon::Util::IO::File::getCurFilePath() const {

	return this->path;
}
