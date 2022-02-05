#pragma once

#include"__CompileShared.h"
#include"../LingCommon/ExtStd.h"
#include"../LingCommon/FileStream.h"

using namespace LingCommon;
using namespace LingCommon::Util::ExtStd;
using namespace LingCommon::Util::IO;
//using namespace std;

namespace LingLan {

	namespace Compile {

		class Symbol {
		public:
			enum class Type : u8 {
				Terminal,
				NonTerminal,
				EndMarker
			};
			Type type = Type::EndMarker;
			u8string name;
		};

		class SymbolTerminal : public Symbol {
		public:
			u8string regexStr;
			regex regexPattern;
			SymbolTerminal() {
				this->type = Type::Terminal;
			}
		};

		class SymbolNonTerminal : public Symbol {
		public:
			SymbolNonTerminal() {
				this->type = Type::NonTerminal;
			}
		};

		class SymbolEndMarker : public SymbolTerminal {
		public:
			SymbolEndMarker() {
				this->type = Type::EndMarker;
			}
		};

		class Token {
		public:
			/// <summary>
			/// �������ļ��е�����
			/// </summary>
			SymbolTerminal* p_symbolTerminal;
			/// <summary>
			/// Lexer�����б����Token�����������ı�
			/// </summary>
			u8string detailedStr;
		};

		class Production {
		public:
			SymbolNonTerminal* p_head = nullptr;
			vector<Symbol*> body;
		};

		/// <summary>
		/// ��ÿһ��terminal��nonterminal��ȡ�����ֿ�����
		/// </summary>
		class LanguageRulesFile {
		public:
			/**
			* �������ļ��е�symbol name�ĸ�ʽ
			*/
			static const regex SymbolNameRegex;

			/**
			 * �ʷ��������и���Ҫ�ġ��ָ�������������ָ����õ��ǲ��ᱻ���뵽�������У����������ԵĿո����
			 */
			static const u8string DelimiterSymbolName;

			/**
			 * aug symbol������
			 */
			static const u8string AugSymbol;

			static const char8_t TerminalStart = ':';
			static const char8_t NonTerminalStart = '>';
			static const char8_t NonTerminalContinue = '=';

			static const char8_t ShouldReserveInAstTree = '@';
			static const char8_t ShouldNotReserveInAstTree = '#';
		public:
			u16string filePath;
			//��ʽ��֮���terminal����
			//Main Container
			map<u8string*, u8string*, DereferenceLess<u8string*>> terminalMap;
			//����terminal map,����vec�����˳�����ļ������˳��
			vector<u8string*> terminalVec;
			//��ʽ��֮���nonTerminal����, һ��name���ܶ�Ӧ���body
			//Main Container
			map<
				u8string*,
				set<
					Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
					DereferenceLess<
						Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
					>
				>*,
				DereferenceLess<u8string*>
			> nonTerminalMap;

			void handleTerminal(u8string* p_tmpStr, FileInputStreamReader* p_fir);
			void handleNonTerminal(u8string* p_tmpStr, FileInputStreamReader* p_fir);
		public:
			LanguageRulesFile();
			void setFilePath(const char16_t* p_path);
			void setFilePath(const u16string& path);
			void format();
			void clear();
			~LanguageRulesFile();
		};

		class LanguageRulesFileException : public Exception::BaseException {
		public:
			LanguageRulesFileException(const char8_t* p_info);
		};

		class LexerException : public Exception::BaseException {
		public:
			LexerException(const char8_t* p_info);
		};

		class LALRParserException : public Exception::BaseException {
		public:
			LALRParserException(const char8_t* p_info);
		};
	}
}
