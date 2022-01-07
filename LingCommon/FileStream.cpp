#include"FileStream.h"

using namespace LingCommon;

u64 LingCommon::Util::IO::FileInputStream::getFilePointer() {

	LARGE_INTEGER li;
	li.QuadPart = 0;
	LARGE_INTEGER liNew;
	if (SetFilePointerEx(this->fileHandle, li, &liNew, FILE_CURRENT) == false) {

		throw new IOException(u8"Move file pointer failed");
	}
	return liNew.QuadPart;
}

LingCommon::u64 LingCommon::Util::IO::FileInputStream::read_inner(void* p_readBuffer, u32 bytesToRead) {

	if (p_readBuffer == nullptr || bytesToRead < 1) {

		throw new Exception::InvalidArgException(u8"Read buffer is null or read bytes too small");
	}
	if (this->fileHandle == INVALID_HANDLE_VALUE) {

		throw new IOException(u8"File not opened yet");
	}
	u64 bytesRead = 0;
	if (ReadFile(this->fileHandle, p_readBuffer, bytesToRead, (DWORD*)&bytesRead, nullptr) == false) {

		throw new IOException(u8"Read file failed");
	}
	return bytesRead;
}

void LingCommon::Util::IO::FileInputStream::moveFilePointer_inner(i64 distance) {

	if (this->fileHandle == INVALID_HANDLE_VALUE) {
		throw new IOException(u8"File not opened yet");
	}

	LARGE_INTEGER li;
	li.QuadPart = distance;
	LARGE_INTEGER liNew;
	if (SetFilePointerEx(this->fileHandle, li, &liNew, FILE_CURRENT) == false) {
		throw new IOException(u8"Move file pointer failed");
	}
}

void LingCommon::Util::IO::FileInputStream::readNxtCache() {

	this->readCacheAt(this->cacheStartFilePtr + this->cacheReadableSize);
}

void LingCommon::Util::IO::FileInputStream::readCacheAt(u64 filePtr) {

	this->moveFilePointer_inner(filePtr - this->getFilePointer());
	this->cacheStartFilePtr = this->getFilePointer();
	//����read inner֮��this->getFilePointer()�ķ���ֵ�ͻ������ƶ���
	//����Ҫ����֮ǰ��ֵ��cacheStartFilePtr
	u64 actualRead = this->read_inner(this->p_cache, this->cacheSize);
	this->cacheReadPos = 0;
	this->cacheReadableSize = actualRead;
}

LingCommon::Util::IO::FileInputStream::FileInputStream() {

	this->p_cache = new u8[this->cacheSize];
}

LingCommon::Util::IO::FileInputStream::FileInputStream(WCHAR* p_filePath) {

	this->p_cache = new u8[this->cacheSize];
	this->open(p_filePath);
}

void LingCommon::Util::IO::FileInputStream::open(WCHAR* p_filePath) {

	this->close();
	if (p_filePath == nullptr) {
		throw new Exception::InvalidArgException(u8"Null path");
	}
	HANDLE hFile = CreateFile(p_filePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		throw new IOException(u8"Can't open file");
	}
	this->fileHandle = hFile;
	this->fileSize = this->getSize();
	//������
	this->readCacheAt(0);
}

u64 LingCommon::Util::IO::FileInputStream::read(void* p_readBuffer, u64 bytesToRead) {

	if ((this->cacheStartFilePtr + this->cacheReadPos) >= this->fileSize) {
		return 0;
	}
	//������
	//����Ƿ����л���
	if (this->cacheReadPos < 0 || this->cacheReadPos >= this->cacheReadableSize) {
		//���¶�ȡ����
		this->readCacheAt(this->cacheStartFilePtr + this->cacheReadPos);
	}
	//���ռ��Ƿ��㹻
	u64 remainBytesToRead = bytesToRead;
	u8* p_u8ReadBuffer = (u8*)p_readBuffer;
	while (true) {
		u64 remainCacheSize = this->cacheReadableSize - this->cacheReadPos;
		if (remainCacheSize < remainBytesToRead) {
			CopyMemory(addr(p_u8ReadBuffer[bytesToRead - remainBytesToRead]), addr(this->p_cache[this->cacheReadPos]), remainCacheSize);
			this->cacheReadPos += remainCacheSize;
			remainBytesToRead -= remainCacheSize;
			//���Ƿ��ܹ����»���
			if (this->cacheReadableSize < this->cacheSize) {
				//û�пɶ�ȡ��������,���ض�ȡ�Ĵ�С
				return bytesToRead - remainBytesToRead;
			} else {
				this->readNxtCache();
			}
		} else {
			//���һ�ζ�ȡ
			CopyMemory(addr(p_u8ReadBuffer[bytesToRead - remainBytesToRead]), addr(this->p_cache[cacheReadPos]), remainBytesToRead);
			this->cacheReadPos += remainBytesToRead;
			return bytesToRead;
		}
	}
}

u8 LingCommon::Util::IO::FileInputStream::readOneByte() {

	//������
	u8 u;
	this->read(&u, 1);
	return u;
}

LingCommon::u64 LingCommon::Util::IO::FileInputStream::getSize() {

	if (this->fileHandle == INVALID_HANDLE_VALUE) {

		throw new IOException(u8"File not opened yet");
	}
	LARGE_INTEGER li = {};
	if (GetFileSizeEx(this->fileHandle, &li) == false) {

		throw new IOException(u8"Get file size failed");
	}
	return li.QuadPart;
}

void LingCommon::Util::IO::FileInputStream::moveFilePointer(i64 distance) {

	this->cacheReadPos += distance;
}

void LingCommon::Util::IO::FileInputStream::close() {

	if (this->fileHandle != INVALID_HANDLE_VALUE) {

		CloseHandle(this->fileHandle);
		this->fileHandle = INVALID_HANDLE_VALUE;
	}
}

bool LingCommon::Util::IO::FileInputStream::isOpen() {

	return (this->fileHandle != INVALID_HANDLE_VALUE);
}

bool LingCommon::Util::IO::FileInputStream::isEOF() {

	if ((this->cacheStartFilePtr + this->cacheReadPos) >= this->fileSize) {
		return true;
	} else {
		return false;
	}
}

LingCommon::Util::IO::FileInputStream::~FileInputStream() {

	this->close();
	delete[] this->p_cache;
}

LingCommon::Util::IO::FileOutputStream::FileOutputStream() {
}

LingCommon::Util::IO::FileOutputStream::FileOutputStream(WCHAR* p_filePath) {

	this->open(p_filePath);
}

void LingCommon::Util::IO::FileOutputStream::open(WCHAR* p_filePath) {

	this->close();

	if (p_filePath == nullptr) {

		throw new Exception::InvalidArgException(u8"Null path");
	}

	HANDLE hFile = CreateFile(p_filePath, GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE) {

		throw new IOException(u8"Can't open file");
	}

	this->fileHandle = hFile;
}

void LingCommon::Util::IO::FileOutputStream::write(void* p_writeBuffer, u32 bytesToWrite) {

	u32 bytesWritten = 0;
	if (p_writeBuffer == nullptr || bytesToWrite < 1) {
		throw new Exception::InvalidArgException(u8"Write buffer is null or write bytes too small");
	}
	if (this->fileHandle == INVALID_HANDLE_VALUE) {
		throw new IOException(u8"File not opened yet");
	}
	if (WriteFile(this->fileHandle, p_writeBuffer, bytesToWrite, (LPDWORD)&bytesWritten, nullptr) == false) {
		throw new IOException(u8"Write file failed");
	}
}

u64 LingCommon::Util::IO::FileOutputStream::getFilePointer() {

	LARGE_INTEGER li;
	li.QuadPart = 0;
	LARGE_INTEGER liNew;
	if (SetFilePointerEx(this->fileHandle, li, &liNew, FILE_CURRENT) == false) {
		throw new IOException(u8"Get file pointer failed");
	}
	return liNew.QuadPart;
}

void LingCommon::Util::IO::FileOutputStream::flush() {

	if (this->fileHandle == INVALID_HANDLE_VALUE) {
		throw new IOException(u8"File not opened yet");
	}
	if (FlushFileBuffers(this->fileHandle) == false) {
		throw new IOException(u8"Can't flush file buffer");
	}
}

void LingCommon::Util::IO::FileOutputStream::close() {

	if (this->fileHandle != INVALID_HANDLE_VALUE) {
		CloseHandle(this->fileHandle);
		this->fileHandle = INVALID_HANDLE_VALUE;
	}
}

bool LingCommon::Util::IO::FileOutputStream::isOpen() {

	return (this->fileHandle != INVALID_HANDLE_VALUE);
}

LingCommon::Util::IO::FileOutputStream::~FileOutputStream() {

	this->close();
}

LingCommon::Util::IO::FileInputStreamReader::FileInputStreamReader() {
}

u64 LingCommon::Util::IO::FileInputStreamReader::readLine(u8* p_buffer, u64 bufferSize) {

	if (p_buffer == nullptr) {
		throw new Exception::InvalidArgException(u8"p_buffer is null");
	}
	u64 charCntRead = 0;
	u8 tmpChar;

	//���贫���buffer��С���㹻����һ��
	bool bufferSizeIsGood = false;

	//ϵͳĬ�ϻ���Buffering�������Լ���������

	//��ȡ�ǻ��з�
	int i = 0;
	for (; i < bufferSize; i++) {
		if (this->isEOF()) {
			break;
		}
		tmpChar = this->readOneByte();
		if (tmpChar == '\r' || tmpChar == '\n') {
			//�����if˵�������buffer��С��������һ��
			bufferSizeIsGood = true;
			//��ȡ���з�
			while (true) {
				if (this->isEOF()) {
					break;
				}
				tmpChar = this->readOneByte();
				if (tmpChar != '\r' && tmpChar != '\n') {
					//�ļ�ָ�����
					this->moveFilePointer(-1);
					break;
				}
			}
			break;
		}
		p_buffer[i] = tmpChar;
		charCntRead++;
	}

	if (bufferSizeIsGood) {
		return charCntRead;
	} else {
		return 0;
	}
}

void LingCommon::Util::IO::FileInputStreamReader::readLine(vector<u8>* p_vector) {

	if (p_vector == nullptr) {
		throw new Exception::InvalidArgException(u8"p_vector is null");
	}

	p_vector->clear();

	u8 tmpChar;
	while (true) {
		if (this->isEOF()) {
			break;
		}
		tmpChar = this->readOneByte();
		if (tmpChar == '\r' || tmpChar == '\n') {
			//�����if˵�������buffer��С��������һ��
			//��ȡ���з�
			while (true) {
				if (this->isEOF()) {
					break;
				}
				tmpChar = this->readOneByte();
				if (tmpChar != '\r' && tmpChar != '\n') {
					//�ļ�ָ�����
					this->moveFilePointer(-1);
					break;
				}
			}
			break;
		}
		p_vector->push_back(tmpChar);
	}
}

void LingCommon::Util::IO::FileInputStreamReader::readLine(u8string* p_string) {

	if (p_string == nullptr) {
		throw new Exception::InvalidArgException(u8"p_string is null");
	}

	p_string->clear();

	u8 tmpChar;
	while (true) {
		if (this->isEOF()) {
			break;
		}
		tmpChar = this->readOneByte();
		if (tmpChar == '\r' || tmpChar == '\n') {
			//�����if˵�������buffer��С��������һ��
			//��ȡ���з�
			while (true) {
				if (this->isEOF()) {
					break;
				}
				tmpChar = this->readOneByte();
				if (tmpChar != '\r' && tmpChar != '\n') {
					//�ļ�ָ�����
					this->moveFilePointer(-1);
					break;
				}
			}
			break;
		}
		p_string->push_back(tmpChar);
	}
}
