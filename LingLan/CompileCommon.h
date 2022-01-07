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
			vector<bool> reserveInAstTree;
		};

		class LanguageRulesFile {
		public:
			/**
			* �������ļ��е�tokenName���ֵĸ�ʽ
			*/
			static const regex TokenNameRegex;
			/**
			 * �ʷ��������и���Ҫ�ġ��ָ�������������ָ����õ��ǲ��ᱻ���뵽�������У����������ԵĿո����
			 */
			static const char8_t TokenStart = ':';
			static const char8_t ProdStart = '>';

			static const u8string DelimiterTokenName;
			static const char8_t ShouldReserveInAstTree = '@';
			static const char8_t ShouldNotReserveInAstTree = '#';
		protected:
			u16string filePath;
			//��ʽ��֮���Token����
			u8string formattedSymbolTerminalDef;
			//��ʽ��֮���Production����
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
