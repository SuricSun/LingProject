#pragma once

#include"__UtilShared.h"

#include"IOCommon.h"
#include"File.h"

using namespace std;

namespace LingCommon {

	namespace Util {

		namespace IO {

			class FileInputStream {
			protected:
				HANDLE fileHandle = INVALID_HANDLE_VALUE;
				u8* p_cache = nullptr;
				u64 cacheStartFilePtr = 0;
				u32 cacheSize = 2048;
				u64 cacheReadableSize = 0;
				u64 cacheReadPos = 0;
				u64 fileSize = 0;
				u64 getFilePointer();
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_rcvBuffer"></param>
				/// <param name="bytesToRead"></param>
				/// <returns>返回实际上读取的字节数</returns>
				u64 read_inner(void* p_readBuffer, u32 bytesToRead);
				void moveFilePointer_inner(i64 distance);
				void readNxtCache();
				void readCacheAt(u64 cacheStartFilePtr);
			public:
				FileInputStream();
				FileInputStream(WCHAR* p_filePath);
				void open(WCHAR* p_filePath);
				u64 read(void* p_readBuffer, u64 bytesToRead);
				u8 readOneByte();
				u64 getSize();
				void moveFilePointer(i64 distance);
				void close();
				bool isOpen();
				bool isEOF();
				~FileInputStream();
			};

			class FileOutputStream {
			protected:
				HANDLE fileHandle = INVALID_HANDLE_VALUE;
			public:
				FileOutputStream();
				FileOutputStream(WCHAR* p_filePath);
				void open(WCHAR* p_filePath);
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_writeBuffer"></param>
				/// <param name="bytesToWrite"></param>
				void write(void* p_writeBuffer, u32 bytesToWrite);
				u64 getFilePointer();
				void flush();
				void close();
				bool isOpen();
				~FileOutputStream();
			};

			class FileInputStreamReader : public FileInputStream {
			protected:
			public:
				FileInputStreamReader();
				/// <summary>
				/// 读取一行，如果p_buffer不够大则返回0，否则返回一行的大小(不包括换行)
				/// </summary>
				/// <param name="p_buffer"></param>
				/// <param name="bufferSize"></param>
				/// <returns></returns>
				u64 readLine(u8* p_buffer, u64 bufferSize);
				void readLine(vector<u8>* p_vector);
				void readLine(u8string* p_string);
			};
		}
	}
}
