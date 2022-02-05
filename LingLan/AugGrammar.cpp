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

void LingLan::Compile::Grammatical::AugGrammar::init(LanguageRulesFile* p_rulesFile, Lexical::Lexer* _p_lxr) {

	this->clear();

	this->p_lexer = _p_lxr;

	map<u8string*, SymbolTerminal*, Util::ExtStd::DereferenceLess<u8string*>>::const_iterator cit_terName2Obj;
	//�ȴ�������NonTerminalȻ�󴴽�����productions
	map<
		u8string*,
		set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
		>*,
		DereferenceLess<u8string*>
	>* p_nonTerminalMap = addr(p_rulesFile->nonTerminalMap);
	map<
		u8string*,
		set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
		>*,
		DereferenceLess<u8string*>
	>::const_iterator cit_nonTerName2Set = p_nonTerminalMap->cbegin();
	map<
		u8string*,
		set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
		>*,
		DereferenceLess<u8string*>
	>::const_iterator cend_nonTerName2Set = p_nonTerminalMap->cend();

	set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
	>::const_iterator cit_vec;
	set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
	>::const_iterator cend_vec;

	//����Ҫ����ͷţ���Ϊ�������Լ���
	map<u8string*, SymbolNonTerminal*, Util::ExtStd::DereferenceLess<u8string*>> nameToSymbolNonTerminalMap;

	map<u8string*, SymbolNonTerminal*, Util::ExtStd::DereferenceLess<u8string*>>::const_iterator cit_nonTerName2Obj;
	// * ��������NonTerminal
	SymbolNonTerminal* p_tmp;
	while (cit_nonTerName2Set != cend_nonTerName2Set) {
		//�ȼ���Ƿ���������Ѿ���ΪTer���ֹ�
		if (_p_lxr->nameToSymbolTerminalMap.find(cit_nonTerName2Set->first) != _p_lxr->nameToSymbolTerminalMap.cend()) {
			throw new LanguageRulesFileException(u8"A terminal and a non terminal had the same name");
		}
		p_tmp = new SymbolNonTerminal();
		p_tmp->name.assign(deref(cit_nonTerName2Set->first));
		this->allNonTerminals.emplace_back(p_tmp);
		this->sameHeadProdMap.emplace(p_tmp, new vector<Production*>());
		nameToSymbolNonTerminalMap.emplace(addr(p_tmp->name), p_tmp);
		cit_nonTerName2Set++;
	}

	// * ��������Production
	vector<Production*>* p_curSameHeadProdVec = nullptr;
	Production* p_prodTmp = nullptr;
	cit_nonTerName2Set = p_nonTerminalMap->cbegin();
	SymbolNonTerminal* p_curProdHead = nullptr;
	Symbol* p_bodySymbol = nullptr;
	vector<u8string*>* p_curVec = nullptr;
	u8string* p_u8str = nullptr;
	//��������set
	u64 length = this->allNonTerminals.size();
	for (u64 i = 0; i < length; i++) {
		p_curProdHead = this->allNonTerminals.at(i);
		p_curSameHeadProdVec = this->sameHeadProdMap.find(p_curProdHead)->second;
		cit_nonTerName2Set = p_nonTerminalMap->find(addr(p_curProdHead->name));
		//�����õ���set�����vec������production
		cit_vec = cit_nonTerName2Set->second->cbegin();
		cend_vec = cit_nonTerName2Set->second->cend();
		while (cit_vec != cend_vec) {
			//���ڵ�ǰ��vec
			//ѭ������
			p_prodTmp = new Production();
			p_prodTmp->p_head = p_curProdHead;
			p_curVec = deref(cit_vec)->getInnerVector();
			u64 lengthInner = p_curVec->size();
			for (u64 j = 0; j < lengthInner; j++) {
				p_u8str = p_curVec->at(j);
				//�����ter����nonTer
				if ((cit_terName2Obj = _p_lxr->nameToSymbolTerminalMap.find(p_u8str)) != _p_lxr->nameToSymbolTerminalMap.cend()) {
					//����һ��ter
					p_prodTmp->body.emplace_back(cit_terName2Obj->second);
				} else if ((cit_nonTerName2Obj = nameToSymbolNonTerminalMap.find(p_u8str)) != nameToSymbolNonTerminalMap.cend()) {
					//����һ��nonTer
					p_prodTmp->body.emplace_back(cit_nonTerName2Obj->second);
				} else {
					//���body symbolδ����
					throw new LanguageRulesFileException(u8"A production body symbol is not defined");
				}
			}
			//��һ��production����
			this->productions.emplace_back(p_prodTmp);
			p_curSameHeadProdVec->emplace_back(p_prodTmp);

			cit_vec++;
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
