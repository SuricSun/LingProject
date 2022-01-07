#pragma once

#include"__UtilShared.h"

using namespace std;

namespace LingCommon {

	namespace Util {

		namespace DSA {

			class UTFStringAffairException : public Exception::BaseException {
			public:
				UTFStringAffairException(const char8_t* p_info);
			};

			class UTFStringAffair {
			public:
				/// <summary>
				/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
				/// </summary>
				/// <param name="from">in</param>
				/// <param name="to">in</param>
				static void UTF8To16(const u8string* from, u16string* to);
				/// <summary>
				/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
				/// </summary>
				/// <param name="from">in</param>
				/// <param name="to">in</param>
				static void UTF8To16(const u8string& from, u16string& to);
				/// <summary>
				/// ���toSizeInByteΪ0����ô��������ת������Ҫ�Ļ�������С��In Bytes��to����������
				/// </summary>
				/// <param name="from">in</param>
				/// <param name="fromSizeInByte">in</param>
				/// <param name="to">in</param>
				/// <param name="toSizeInByte">in</param>
				/// <returns></returns>
				static u64 UTF8To16(const char8_t* from, u64 fromSizeInByte, char16_t* to, u64 toSizeInByte);
				/// <summary>
				/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
				/// </summary>
				/// <param name="from">in</param>
				/// <param name="to">in</param>
				static void UTF16To8(const u16string* from, u8string* to);
				/// <summary>
				/// from�Ǵ�ת���ַ�����to�ǽ���ַ�����ȷ��to����û����Ҫ������Ϊ����Ķ����ᱻ��д
				/// </summary>
				/// <param name="from">in</param>
				/// <param name="to">in</param>
				static void UTF16To8(const u16string& from, u8string& to);
				/// <summary>
				/// ���toSizeInByteΪ0����ô��������ת������Ҫ�Ļ�������С��In Bytes��to����������
				/// </summary>
				/// <param name="from">in</param>
				/// <param name="fromSizeInByte">in</param>
				/// <param name="to">in</param>
				/// <param name="toSizeinByte">in</param>
				/// <returns></returns>
				static u64 UTF16To8(const char16_t* from, u64 fromSizeInByte, char8_t* to, u64 toSizeInByte);
				/// <summary>
				/// �����ַ����ַ���Ŀ����byte��Ŀ
				/// </summary>
				/// <param name="str"></param>
				/// <param name="maxSize">�����0�Ļ�����һֱ��</param>
				/// <returns>����Ҳ������ַ�����ô����0�����򷵻س��ȣ�������β���ַ�</returns>
				static u64 UTF8StrLen(const char8_t* str, u64 maxSize);
				/// <summary>
				/// �����ַ����ַ���Ŀ����byte��Ŀ
				/// </summary>
				/// <param name="str"></param>
				/// <param name="maxSize">�����0�Ļ�����һֱ��</param>
				/// <returns>����Ҳ������ַ�����ô����0�����򷵻س��ȣ�������β���ַ�</returns>
				static u64 UTF16StrLen(const char16_t* str, u64 maxSize);
			};
		}
	}
}