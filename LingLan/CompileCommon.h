#pragma once

#include"__CompileShared.h"
#include"../LingCommon/ExtStd.h"

using namespace LingCommon;
using namespace LingCommon::Util::ExtStd;
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
			vector<bool> reserveInAstTree;
		};

		class LanguageRulesFile {
		public:
			/**
			* 定义在文件中的tokenName部分的格式
			*/
			static const regex TokenNameRegex;
			/**
			 * 词法解析中有个重要的“分隔符”，他们起分隔作用但是不会被加入到解析流中，如大多数语言的空格符号
			 */
			static const char8_t TokenStart = ':';
			static const char8_t ProdStart = '>';

			static const u8string DelimiterTokenName;
			static const char8_t ShouldReserveInAstTree = '@';
			static const char8_t ShouldNotReserveInAstTree = '#';
		protected:
			u16string filePath;
			//格式化之后的Token配置
			u8string formattedSymbolTerminalDef;
			//格式化之后的Production配置
			u8string formattedProductionDef;
		public:
			LanguageRulesFile();
			void setFilePath(const char16_t* p_path);
			void setFilePath(const u16string& path);
			void format();
			u8string* getFormattedSymbolTerminalDef();
			u8string* getFormattedProductionDef();
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
