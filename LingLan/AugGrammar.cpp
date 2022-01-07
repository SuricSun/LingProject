#include"LALRParser.h"

LingLan::Compile::Grammatical::AugGrammar::AugGrammar() {
}

LingLan::Compile::Grammatical::AugGrammar::AugGrammar(const AugGrammar& ag) {

	(*this) = ag;
}

void LingLan::Compile::Grammatical::AugGrammar::operator=(const AugGrammar& ag) {

	this->clear();

	this->allNonTerminals = ag.allNonTerminals;
	this->productions = ag.productions;
	this->p_lexer = ag.p_lexer;
	this->sameHeadProdMap = ag.sameHeadProdMap;
	this->shallowClear = ag.shallowClear;
}

void LingLan::Compile::Grammatical::AugGrammar::init(u8string* p_prodDef, Lexical::Lexer* _p_lxr) {

	this->clear();

	this->p_lexer = _p_lxr;

	vector<SymbolTerminal*>* p_allSymbolTerminal = &(_p_lxr->allSymbolTerminal);
	map<u8string*, SymbolTerminal*, Util::ExtStd::DereferenceLess<u8string*>>* p_nameToSymbolTerminalMap = &(_p_lxr->nameToSymbolTerminalMap);

	map<u8string*, SymbolNonTerminal*, Util::ExtStd::DereferenceLess<u8string*>> nameToNonTerminalSet;

	___DEL___
		set<u8string*, Util::ExtStd::DereferenceLess<u8string*>> prodRawStrSet;

	//循环读取每条
	match_results<const char8_t*> match = {};
	const char8_t* p_curProdBegin = nullptr;
	const char8_t* p_curProdEnd = nullptr;
	const char8_t* p_prodDefEnd = nullptr;

	___DEL___
		u8string* p_tmpProdRawStr = nullptr;
	//___DEL___
	//	SymbolNonTerminal* p_tmpNonTerminal = nullptr;
	//___DEL___
	//	Production* p_tmpProd = nullptr;

	map<u8string*, SymbolTerminal*, Util::ExtStd::DereferenceLess<u8string*>>::const_iterator cit;
	map<u8string*, SymbolNonTerminal*, Util::ExtStd::DereferenceLess<u8string*>>::const_iterator cit_another;
	set<u8string*, Util::ExtStd::DereferenceLess<u8string*>>::const_iterator cit_prodRawStr;
	vector<const char8_t*> prodItVec = {};

	p_prodDefEnd = addr(p_prodDef->at(p_prodDef->size() - 1)) + 1;
	p_curProdBegin = addr(p_prodDef->at(1));
	p_curProdEnd = p_curProdBegin + p_prodDef->at(0);

	while (true) {
		prodItVec.clear();
		if (regex_search(p_curProdBegin, p_curProdEnd, match, LanguageRulesFile::TokenNameRegex) == false) {
			throw new LanguageRulesFileException(u8"prod no head");
		}
		//得到结果，检查匹配的起始点是否是起点
		if (match[0].first != p_curProdBegin) {
			throw new LanguageRulesFileException(u8"prod head err");
		}
		//加入两个it到末尾
		prodItVec.emplace_back(match[0].first);
		prodItVec.emplace_back(match[0].second);
		//检查end是不是等号
		if (deref(match[0].second) != '=') {
			throw new LanguageRulesFileException(u8"no \"=\"");
		}
		//检查等号后面是否有内容
		const char8_t* p_prodBodyIt = match[0].second + 1;
		if (p_prodBodyIt == p_curProdEnd) {
			//此prod派生空
		} else {
			//读取prod body
			//循环过程：1.读取修饰符 2.读取symbol 3.循环
			while (p_prodBodyIt != p_curProdEnd) {
				//1.
				if (*p_prodBodyIt == LanguageRulesFile::ShouldReserveInAstTree) {

				} else if (*p_prodBodyIt == LanguageRulesFile::ShouldNotReserveInAstTree) {

				} else {
					throw new LanguageRulesFileException(u8"prod body no @ or #");
				}
				//2.
				p_prodBodyIt++;
				if (regex_search(p_prodBodyIt, p_curProdEnd, match, LanguageRulesFile::TokenNameRegex) == false) {
					throw new LanguageRulesFileException(u8"prod body no symbol");
				}
				//得到结果，检查匹配的起始点是否是起点
				if (match[0].first != p_prodBodyIt) {
					throw new LanguageRulesFileException(u8"prod body symbol err");
				}
				//加入两个it到末尾
				prodItVec.emplace_back(match[0].first);
				prodItVec.emplace_back(match[0].second);
				//3.
				p_prodBodyIt = match[0].second;
				if (p_prodBodyIt == p_curProdEnd) {
					break;
				}
			}
		}
		//加入RawSet
		p_tmpProdRawStr = new u8string(p_curProdBegin, p_curProdEnd);
		if (prodRawStrSet.insert(p_tmpProdRawStr).second == false) {
			del(p_tmpProdRawStr);
			throw new LanguageRulesFileException(u8"duplicated prod def");
		}
		//循环所有检测出来的Symbol,构建一个Prod
		___DEL___
			u8string* p_tmpName = nullptr;
		this->productions.emplace_back(new Production());
		for (int i = 0; i < prodItVec.size(); i += 2) {
			p_tmpName = new u8string(prodItVec[i], prodItVec[i + 1]);
			cit = p_nameToSymbolTerminalMap->find(p_tmpName);
			if (cit != p_nameToSymbolTerminalMap->cend()) {
				if (i == 0) {
					//i=0说明当前为prod head，prod head的symbol必须是non terminal
					del(p_tmpName);
					del(this->productions.back());
					this->productions.pop_back();
					throw new LanguageRulesFileException(u8"prod head symbol is terminal and non terminal");
				} else {
					this->productions.back()->body.emplace_back(deref(cit).second);
					if (deref(prodItVec[i] - 1) == LanguageRulesFile::ShouldReserveInAstTree) {
						this->productions.back()->reserveInAstTree.emplace_back(true);
					} else if (deref(prodItVec[i] - 1) == LanguageRulesFile::ShouldNotReserveInAstTree) {
						this->productions.back()->reserveInAstTree.emplace_back(false);
					}
				}
			} else {
				cit_another = nameToNonTerminalSet.find(p_tmpName);
				if (cit_another != nameToNonTerminalSet.cend()) {
					if (i == 0) {
						this->productions.back()->p_head = deref(cit_another).second;
					} else {
						this->productions.back()->body.emplace_back(deref(cit_another).second);
						if (deref(prodItVec[i] - 1) == LanguageRulesFile::ShouldReserveInAstTree) {
							this->productions.back()->reserveInAstTree.emplace_back(true);
						} else if (deref(prodItVec[i] - 1) == LanguageRulesFile::ShouldNotReserveInAstTree) {
							this->productions.back()->reserveInAstTree.emplace_back(false);
						}
					}
				} else {
					//不存在这个Symbol，那就创建
					//一定是NonTerminal，因为Terminal只能定义在terminalDef里面
					this->allNonTerminals.emplace_back(new SymbolNonTerminal());
					this->allNonTerminals.back()->type = Symbol::Type::NonTerminal;
					this->allNonTerminals.back()->name.assign(*p_tmpName);
					nameToNonTerminalSet.emplace(addr(this->allNonTerminals.back()->name), this->allNonTerminals.back());
					if (i == 0) {
						this->productions.back()->p_head = this->allNonTerminals.back();
					} else {
						this->productions.back()->body.emplace_back(this->allNonTerminals.back());
						if (deref(prodItVec[i] - 1) == LanguageRulesFile::ShouldReserveInAstTree) {
							this->productions.back()->reserveInAstTree.emplace_back(true);
						} else if (deref(prodItVec[i] - 1) == LanguageRulesFile::ShouldNotReserveInAstTree) {
							this->productions.back()->reserveInAstTree.emplace_back(false);
						}
					}
				}
			}
			del(p_tmpName);
		}
		//加入samehead prod
		map<SymbolNonTerminal*, vector<Production*>*>::const_iterator
			fuckItName;
		fuckItName = this->sameHeadProdMap.find(this->productions.back()->p_head);
		if (fuckItName == this->sameHeadProdMap.cend()) {
			//加入
			deref(this->sameHeadProdMap.emplace(this->productions.back()->p_head, new vector<Production*>()).first)
				.second->emplace_back(this->productions.back());
		} else {
			deref(fuckItName).second->emplace_back(this->productions.back());
		}
		//更新迭代器
		p_curProdBegin = p_curProdEnd;
		if (p_curProdEnd == p_prodDefEnd) {
			break;
		}
		p_curProdEnd = p_curProdBegin + deref(p_curProdBegin) + 1;
		p_curProdBegin++;
	}

	//检查是否为AugGrammar
	if (deref(this->sameHeadProdMap.find(this->allNonTerminals.at(0))).second->size() != 1) {
		throw new LanguageRulesFileException(u8"not a AugGrammar");
	}

	//最后一步遍历所有prod的body，看body里面的所有symbol是否存在(已定义)
	//也就是检查AllNonTerminal与sameProdHeadMap必须相等
	for (i32 i = 0; i < this->allNonTerminals.size(); i++) {
		if (this->sameHeadProdMap.contains(this->allNonTerminals.at(i)) == false) {
			throw new LanguageRulesFileException(u8"a symbol is not define but appeared in prod body");
		}
	}
}

void LingLan::Compile::Grammatical::AugGrammar::clear() {

	map<SymbolNonTerminal*, vector<Production*>*>::const_iterator
		cit = this->sameHeadProdMap.cbegin();
	map<SymbolNonTerminal*, vector<Production*>*>::const_iterator
		cend = this->sameHeadProdMap.cend();
	while (cit != cend) {
		del(cit->second);
		cit++;
	}

	if (this->shallowClear == false) {
		u32 cnt = this->allNonTerminals.size();
		for (u32 i = 0; i < cnt; i++) {
			del(this->allNonTerminals.at(i));
		}
		cnt = this->productions.size();
		for (u32 i = 0; i < cnt; i++) {
			del(this->productions.at(i));
		}
	}

	this->allNonTerminals.clear();
	this->productions.clear();
}

void LingLan::Compile::Grammatical::AugGrammar::setShallowClear(bool yesOrNo) {

	this->shallowClear = yesOrNo;
}

LingLan::Compile::Grammatical::AugGrammar::~AugGrammar() {

	this->clear();
}
