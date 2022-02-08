#pragma once

#include"__CompileShared.h"

#include"..\LingCommon\FileStream.h"
#include"..\LingCommon\ExtStd.h"
#include"CompileCommon.h"
#include"Lexer.h"

using namespace std;

namespace LingLan {

	namespace Compile {

		namespace Grammatical {

			class AugGrammar {
			public:
				/// <summary>
				/// Main Owner of productions
				/// </summary>
				vector<Production*> productions;
				/// <summary>
				/// Main Owner of nonTerminals
				/// </summary>
				vector<SymbolNonTerminal*> allNonTerminals;
				map<SymbolNonTerminal*, vector<Production*>*> sameHeadProdMap;
				Lexical::Lexer* p_lexer = nullptr;
				bool shallowClear = false;
			public:
				AugGrammar();
				AugGrammar(const AugGrammar& ag);
				void operator=(const AugGrammar& ag);
				void init(LanguageRulesFile* p_rulesFile, Lexical::Lexer* p_lexer);
				void clear();
				void setShallowClear(bool yesOrNo);
				~AugGrammar();
			};

			//前向声明
			class LALRState;
			class LALRItem {
			public:
				Production* p_prod = nullptr;
				u8 pos = 0;
				/// <summary>
				/// "sc" is short for "second conponent"
				/// </summary>
				set<SymbolTerminal*> sc_lookAhead;
				/// <summary>
				/// 扩散到哪些kernel item
				/// </summary>
				set<LALRItem*> spreadFather;
				/// <summary>
				/// 默认为假，别瞎改
				/// </summary>
				bool isKernelItem = false;
				/// <summary>
				/// 包含这个item的lalrState
				/// </summary>
				LALRState* p_lalrState;

				bool isOver() const;
				//bool equal(const LALRItem* p_item) const;
				bool coreEqual(const LALRItem* p_item) const;
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_item"></param>
				/// <returns>true mergeToSelf or false mergeToSelf</returns>
				bool mergeToSelf(const LALRItem* p_item);
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_item"></param>
				/// <returns></returns>
				bool mergeToSelfOnlyLookAhead(const LALRItem* p_item);
				//~LALRItem();
			};

			class LALRState {
			public:
				vector<LALRItem*> items;
				u32 kernelItemCnt = 0;

				bool coreEqual(const LALRState* p_state) const;
				/// <summary>
				/// 只merge两个item的kernel item
				/// </summary>
				/// <param name="p_state"></param>
				/// <returns>true mergeToSelf or false mergeToSelf</returns>
				bool mergeToSelf(const LALRState* p_state) const;
				//~LALRState();
				void clear();
				~LALRState();
			};

			class LALRAction {
			public:
				enum class ActionType : u8 {
					ShiftToState,
					ReduceWithProd,
					Accept
				};
				ActionType actType = ActionType::Accept;
				u32 shiftToState = 0;
				Production* p_reduceWithProd = nullptr;
			};

			class LALRParsingTable {
			protected:
				void initBlankSet();
				void initFirstMap();
				void initActionTable();
				/// <summary>
				/// [from, to)
				/// </summary>
				/// <param name="p_inBody"></param>
				/// <param name="from"></param>
				/// <param name="to"></param>
				/// <param name="p_inSet"></param>
				/// <returns>true if derive blank, false else</returns>
				bool first(vector<Symbol*>* p_inBody, u16 from, u16 to, set<SymbolTerminal*>* p_inSet);
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_state"></param>
				/// <param name="p_goToSymbol"></param>
				/// <returns>返回值手动释放</returns>
				LALRState* eff_goto(vector<LALRItem*>* p_itemVec, Symbol* p_goToSymbol);
				/// <summary>
				/// 在原数据上操作
				/// </summary>
				/// <param name="p_itemVec"></param>
				void eff_closure(LALRState* p_inState);
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_itemVec"></param>
				//void closure(vector<LALRItem*>* p_itemVec);
				/// <summary>
				/// 
				/// </summary>
				void eff_initAllStates(vector<LALRState*>* p_allStates, set<LALRState*>* p_needUpdateVec);
				/// <summary>
				/// 
				/// </summary>
				void eff_spread(vector<LALRState*>* p_allStates, set<LALRState*>* p_needUpdateVec);
			public:
				set<SymbolNonTerminal*> blankSet;
				map<SymbolNonTerminal*, set<SymbolTerminal*>*> firstMap;
				vector<LALRAction*> allAction;
				vector<map<Symbol*, LALRAction*>> actionMapTable;
				AugGrammar* p_augGrammar = nullptr;
				bool shallowClear = false;
			public:
				LALRParsingTable();
				LALRParsingTable(const LALRParsingTable& tbl);
				void operator=(const LALRParsingTable& tbl);
				/// <summary>
				/// 返回actionMapTable
				/// </summary>
				/// <param name="p_augGrammar"></param>
				/// <returns></returns>
				void init(AugGrammar* p_augGrammar);
				void clear();
				void setShallowClear(bool yesOrNo);
				~LALRParsingTable();
			};

			class LALRParser {
			public:
				LALRParsingTable* p_parsingTable = nullptr;
			public:
				LALRParser();
				LALRParser(const LALRParser& psr);
				void operator=(const LALRParser& psr);
				void init(LALRParsingTable* _p_parsingTable);
				/// <summary>
				/// 
				/// </summary>
				/// <param name="p_inProg"></param>
				/// <returns></returns>
				vector<LALRAction*>* parse(vector<Token*>* p_inProg);
			};
		}
	}
}
