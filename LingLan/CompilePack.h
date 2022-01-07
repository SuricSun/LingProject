#pragma once

#include"__CompileShared.h"

#include"..\LingCommon\FileStream.h"
#include"CompileCommon.h"
#include"Lexer.h"
#include"LALRParser.h"

using namespace LingCommon;
using namespace LingLan::Compile::Lexical;
using namespace LingLan::Compile::Grammatical;

namespace LingLan {

	namespace Compile {

		namespace CompileWrapper {

			class SerializableCompilePack {
			protected:
				u32 getSymbolTerminalSize(SymbolTerminal* p_sym);
				u32 getSymbolTerminalEndMarkerSize(SymbolEndMarker* p_sym);
				u32 getSymbolNonTerminalSize(SymbolNonTerminal* p_sym);
				u32 getProductionSize(Production* p_prod);
				u32 getLALRActionSize(LALRAction* p_act);
				void writeSymbolTerminal(SymbolTerminal* p_sym, Util::IO::FileOutputStream* p_fout);
				void writeSymbolTerminalEndMarker(SymbolEndMarker* p_sym, Util::IO::FileOutputStream* p_fout);
				void writeSymbolNonTerminal(SymbolNonTerminal* p_sym, Util::IO::FileOutputStream* p_fout);
				void writeProduction(Production* p_prod, map<SymbolTerminal*, u64>* p_terminalReaddress, map<SymbolNonTerminal*, u64>* p_nonTerminalReaddress, Util::IO::FileOutputStream* p_fout);
				void writeLALRAction(LALRAction* p_act, map<Production*, u64>* p_prodReaddress, Util::IO::FileOutputStream* p_fout);
			public:
				Lexer lexer;
				AugGrammar augGrammar;
				LALRParsingTable parsingTable;
				LALRParser parser;

				void initFromLanFile(const char16_t* p_lanFile);
				void initFromLanFile(LanguageRulesFile* p_lanFile);
				void serialize(WCHAR* p_path);
				~SerializableCompilePack();
			};

			class DeserializedCompilePack {
			public:
				Lexer lexer;
				AugGrammar augGrammar;
				LALRParsingTable parsingTable;
				LALRParser parser;

				vector<SymbolTerminal> ter;
				vector<Production> prod;
				vector<SymbolNonTerminal> nonTer;
				vector<LALRAction> act;
			public:
				DeserializedCompilePack();
				void deserialize(WCHAR* p_path);
				void clear();
				~DeserializedCompilePack();
			};
		}
	}
}
