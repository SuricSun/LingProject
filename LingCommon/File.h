#pragma once

#include"__UtilShared.h"

#include"IOCommon.h"

using namespace std;

namespace LingCommon {

	namespace Util {

		namespace IO {

			/// <summary>
			/// 操作File属性，不保存任何句柄，只保存一个filePath字符串
			/// </summary>
			class File {
			public:
				enum class Type : u8 {
					File, Dir, NotExist, Undef
				};
			protected:
				u16string path;
				Type type = Type::Undef;
			public:
				void setPath(const char16_t* p_path);
				void setPath(const u16string& str);
				Type probeFileType();
				void createFile() const;
				void createDir() const;
				void deleteFile() const;
				void deleteDir() const;
				vector<File>* listFiles();
				vector<File>* listFilesWithWildcards(const char16_t* p_wildcardFileName);
				const u16string& getCurFilePath() const;
			};
		}
	}
}
