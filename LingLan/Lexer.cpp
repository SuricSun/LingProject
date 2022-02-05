#include "Lexer.h"

using namespace LingLan::Compile;
using namespace LingLan::Compile::Lexical;

LingLan::Compile::Lexical::Lexer::Lexer() {

}

void LingLan::Compile::Lexical::Lexer::init(LanguageRulesFile* p_rulesfFile) {

	this->clear();

	map<u8string*, u8string*, DereferenceLess<u8string*>>* p_terminalMap = addr(p_rulesfFile->terminalMap);
	vector<u8string*>* p_terminalVec = addr(p_rulesfFile->terminalVec);

	SymbolTerminal* p_tmp;
	//循环vec
	u64 length = p_terminalVec->size();
	for (u64 i = 0; i < length; i++) {
		//创建SymbolTerminal，加入对应的map
		p_tmp = new SymbolTerminal();
		p_tmp->name.assign(deref(p_terminalVec->at(i)));
		p_tmp->regexStr.assign(deref(p_terminalMap->find(p_terminalVec->at(i))->second));
		p_tmp->regexPattern.assign((char*)p_tmp->regexStr.c_str());
		//加入
		this->allSymbolTerminal.emplace_back(p_tmp);
		this->nameToSymbolTerminalMap.emplace(addr(p_tmp->name), p_tmp);
	}
}

LingLan::Compile::Lexical::Lexer::Lexer(const Lexer& lxr) {

	(*this) = lxr;
}

vector<Token*>* LingLan::Compile::Lexical::Lexer::lexicalAnalyze(const char8_t* p_inputProgramStr, u64 sizeInByte) {
	
	//结果vector
	vector<Token*>* p_retVec = new vector<Token*>();
	p_retVec->reserve(32);
	//一个循环match所有的变量
	const char8_t* p_curStart = p_inputProgramStr;
	const char8_t* p_end = p_curStart + sizeInByte;
	match_results<const char8_t*> m;
	u32 regexTokenCnt = this->allSymbolTerminal.size();
	SymbolTerminal* p_curSymbolTerminal = nullptr;
	while (p_curStart < p_end) {
		//循环所有regex token进行匹配
		u32 i = 0;
		for (; i < regexTokenCnt; i++) {
			p_curSymbolTerminal = this->allSymbolTerminal[i];
			if (regex_search(p_curStart, p_end, m, p_curSymbolTerminal->regexPattern)) {
				if (m[0].first == p_curStart) {
					//match到了
					//只有非delimiter才能加入
					if (p_curSymbolTerminal->name != LanguageRulesFile::DelimiterSymbolName) {
						//加入
						Token* p_tmp = new Token();
						p_tmp->p_symbolTerminal = p_curSymbolTerminal;
						p_tmp->detailedStr.assign(m[0].first, m[0].second);
						p_retVec->emplace_back(p_tmp);
					}
					break;
				}
			}
		}
		if (i >= regexTokenCnt) {
			//匹配失败
			throw new LexerException(u8"Lexer failed");
		}
		p_curStart = m[0].second;
	}

	//加入EndMarker
	Token* p_tmp = new Token();
	p_tmp->p_symbolTerminal = addr(this->endMarker);
	p_retVec->emplace_back(p_tmp);

	return p_retVec;
}

void LingLan::Compile::Lexical::Lexer::operator=(const Lexer& lxr) {

	this->clear();

	this->allSymbolTerminal = lxr.allSymbolTerminal;
	this->nameToSymbolTerminalMap = lxr.nameToSymbolTerminalMap;
	this->shallowClear = lxr.shallowClear;
}

vector<Token*>* LingLan::Compile::Lexical::Lexer::lexicalAnalyze(const u8string* p_inputProgramStr) {

	return new vector<Token*>();
}

void LingLan::Compile::Lexical::Lexer::clear() {

	if (this->shallowClear == false) {
		u32 cnt = this->allSymbolTerminal.size();
		for (u32 i = 0; i < cnt; i++) {
			del(this->allSymbolTerminal.at(i));
		}
	}

	this->allSymbolTerminal.clear();
	this->nameToSymbolTerminalMap.clear();
}

LingLan::Compile::Lexical::Lexer::~Lexer() {

	this->clear();
}

void LingLan::Compile::Lexical::Lexer::setShallowClear(bool yesOrNo) {

	this->shallowClear = yesOrNo;
}
