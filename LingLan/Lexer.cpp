#include "Lexer.h"

using namespace LingLan::Compile;
using namespace LingLan::Compile::Lexical;

LingLan::Compile::Lexical::Lexer::Lexer() {

}

void LingLan::Compile::Lexical::Lexer::init(u8string* p_stringAllTokenDef) {

	this->clear();

	match_results<u8string::const_iterator> result = {};
	u8string::const_iterator cbegin_curTokenDef = {};
	u8string::const_iterator cend_curTokenDef = {};

	cbegin_curTokenDef = p_stringAllTokenDef->cbegin() + 1;
	cend_curTokenDef = cbegin_curTokenDef + p_stringAllTokenDef->at(0);
	while (true) {
		if (regex_search(cbegin_curTokenDef, cend_curTokenDef, result, LanguageRulesFile::TokenNameRegex) == false) {
			throw new LanguageRulesFileException(u8"No Desc");
		}
		//得到结果，检查匹配的起始点是否是起点
		if (result[0].first != cbegin_curTokenDef) {
			throw new LanguageRulesFileException(u8"No Desc");
		}
		//检查end是不是等号
		if ((*result[0].second) != '=') {
			throw new LanguageRulesFileException(u8"No Desc");
		}
		//后面全是Regex文本了
		//p_tmp = new SymbolTerminal();
		//p_tmp->name.assign(result[0].first, result[0].second);
		//p_tmp->regexStr.assign(result[0].second + 1, cend_curTokenDef);
		//p_tmp->regexPattern.assign(result[0].second + 1, cend_curTokenDef);
		this->allSymbolTerminal.emplace_back(new SymbolTerminal());
		this->allSymbolTerminal.back()->name.assign(result[0].first, result[0].second);
		this->allSymbolTerminal.back()->regexStr.assign(result[0].second + 1, cend_curTokenDef);
		this->allSymbolTerminal.back()->regexPattern.assign(result[0].second + 1, cend_curTokenDef);
		//不重复才能加入
		//if (this->nameToSymbolTerminalMap.find(&p_tmp->name) != this->nameToSymbolTerminalMap.cend()) {
		//	delete p_tmp;
		//	throw new LanguageRulesFileException(u8"Duplicated Def");
		//}
		if (this->nameToSymbolTerminalMap.emplace(addr(this->allSymbolTerminal.back()->name), this->allSymbolTerminal.back()).second == false) {
			del(this->allSymbolTerminal.back());
			this->allSymbolTerminal.pop_back();
			throw new LanguageRulesFileException(u8"Duplicated Def");
		}
		//更新迭代器
		cbegin_curTokenDef = cend_curTokenDef;
		if (cbegin_curTokenDef == p_stringAllTokenDef->cend()) {
			break;
		}
		cend_curTokenDef = cbegin_curTokenDef + (*cbegin_curTokenDef) + 1;
		cbegin_curTokenDef++;
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
					if (p_curSymbolTerminal->name != LanguageRulesFile::DelimiterTokenName) {
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
