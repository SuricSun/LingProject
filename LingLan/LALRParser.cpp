#include "LALRParser.h"

using namespace LingLan::Compile::Grammatical;

LingLan::Compile::Grammatical::LALRParser::LALRParser() {
}

LingLan::Compile::Grammatical::LALRParser::LALRParser(const LALRParser& psr) {

	//一行代码就没必要调用 operator= 了
	this->p_parsingTable = psr.p_parsingTable;
}

void LingLan::Compile::Grammatical::LALRParser::operator=(const LALRParser& psr) {

	this->p_parsingTable = psr.p_parsingTable;
}

void LingLan::Compile::Grammatical::LALRParser::init(LALRParsingTable* _p_parsingTable) {

	this->p_parsingTable = _p_parsingTable;
}

vector<LALRAction*>* LingLan::Compile::Grammatical::LALRParser::parse(vector<Token*>* p_inProg) {

	vector<LALRAction*>* p_outActionStack = new vector<LALRAction*>();
	vector<u32> stateStack;

	stateStack.emplace_back(0);

	vector<map<Symbol*, LALRAction*>>* p_tbl = addr(this->p_parsingTable->actionMapTable);
	map<Symbol*, LALRAction*>* p_curStateMap = nullptr;
	map<Symbol*, LALRAction*>::const_iterator foundActionIt;
	LALRAction* p_foundAction = nullptr;
	u32 curParseTokenIdx = 0;
	SymbolTerminal* p_curParseSymbolTerminal = p_inProg->at(curParseTokenIdx)->p_symbolTerminal;

	while (true) {

		//查表
		p_curStateMap = addr(p_tbl->at(stateStack.back()));
		foundActionIt = p_curStateMap->find(p_curParseSymbolTerminal);
		if (foundActionIt != p_curStateMap->cend()) {
			p_foundAction = deref(foundActionIt).second;
			if (p_foundAction->actType == LALRAction::ActionType::ShiftToState) {
				p_outActionStack->emplace_back(p_foundAction);
				stateStack.emplace_back(p_foundAction->shiftToState);
				curParseTokenIdx++;
				p_curParseSymbolTerminal = p_inProg->at(curParseTokenIdx)->p_symbolTerminal;
			} else if (p_foundAction->actType == LALRAction::ActionType::ReduceWithProd) {
				p_outActionStack->emplace_back(p_foundAction);
				//回退stateStack栈
				stateStack.erase(stateStack.cend() - p_foundAction->p_reduceWithProd->body.size(), stateStack.cend());
				//stateStack.add(
				//	this.actionMapList
				//	.get(stateStack.get(stateStack.size() - 1))
				//	.get(action.reduceWithProd.getHead().getName())
				//	.shiftToState);
				//不能用p_curStateMap变凉了因为上面一行更新了stateStack
				stateStack.emplace_back(
					deref(p_tbl->at(stateStack.back()).find(p_foundAction->p_reduceWithProd->p_head)).second->shiftToState
				);
			} else {
				//ACC
				p_outActionStack->emplace_back(p_foundAction);
				return p_outActionStack;
			}
		} else {
			throw new LanguageRulesFileException((u8string(u8"Error Parsing Token \"").append(p_inProg->at(curParseTokenIdx)->detailedStr).append(u8"\" at pos ").append((char8_t*)to_string(curParseTokenIdx).c_str())).c_str());
		}
	}

	return nullptr;
}
