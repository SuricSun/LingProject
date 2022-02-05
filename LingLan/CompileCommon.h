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
			/// 在配置文件中的名字
			/// </summary>
			SymbolTerminal* p_symbolTerminal;
			/// <summary>
			/// Lexer过程中被这个Token解析出来的文本
			/// </summary>
			u8string detailedStr;
		};

		class Production {
		public:
			SymbolNonTerminal* p_head = nullptr;
			vector<Symbol*> body;
		};

		/// <summary>
		/// 把每一个terminal和nonterminal读取出来分开存贮
		/// </summary>
		class LanguageRulesFile {
		public:
			/**
			* 定义在文件中的symbol name的格式
			*/
			static const regex SymbolNameRegex;

			/**
			 * 词法解析中有个重要的“分隔符”，他们起分隔作用但是不会被加入到解析流中，如大多数语言的空格符号
			 */
			static const u8string DelimiterSymbolName;

			/**
			 * aug symbol的名字
			 */
			static const u8string AugSymbol;

			static const char8_t TerminalStart = ':';
			static const char8_t NonTerminalStart = '>';
			static const char8_t NonTerminalContinue = '=';

			static const char8_t ShouldReserveInAstTree = '@';
			static const char8_t ShouldNotReserveInAstTree = '#';
		public:
			u16string filePath;
			//格式化之后的terminal配置
			//Main Container
			map<u8string*, u8string*, DereferenceLess<u8string*>> terminalMap;
			//引用terminal map,不过vec里面的顺序是文件定义的顺序
			vector<u8string*> terminalVec;
			//格式化之后的nonTerminal配置, 一个name可能对应多个body
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
