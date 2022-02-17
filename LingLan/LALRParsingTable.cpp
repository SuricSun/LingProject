#include"LALRParser.h"

using namespace LingLan::Compile;
using namespace LingLan::Compile::Grammatical;

bool LingLan::Compile::Grammatical::LALRItem::isOver() const {

	return (this->pos >= this->p_prod->body.size());
}

bool LingLan::Compile::Grammatical::LALRItem::coreEqual(const LALRItem* p_item) const {

	if (this->p_prod == p_item->p_prod
		&&
		this->pos == p_item->pos) {

		return true;
	} else {
		return false;
	}
}

bool LingLan::Compile::Grammatical::LALRItem::mergeToSelf(const LALRItem* p_item) {

	bool realMerge = false;
	//�ں�spreadFather
	u32 preSize = this->spreadFather.size();
	this->spreadFather.insert(p_item->spreadFather.cbegin(), p_item->spreadFather.cend());
	if (this->spreadFather.size() > preSize) {
		realMerge = true;
	}
	//�ں�scndCpnt
	preSize = this->sc_lookAhead.size();
	this->sc_lookAhead.insert(p_item->sc_lookAhead.cbegin(), p_item->sc_lookAhead.cend());
	if (this->sc_lookAhead.size() > preSize) {
		realMerge = realMerge & true;
	} else {
		realMerge = false;
	}
	return realMerge;
}

bool LingLan::Compile::Grammatical::LALRItem::mergeToSelfOnlyLookAhead(const LALRItem* p_item) {

	//�ں�scndCpnt
	u32 preSize = this->sc_lookAhead.size();
	this->sc_lookAhead.insert(p_item->sc_lookAhead.cbegin(), p_item->sc_lookAhead.cend());
	if (this->sc_lookAhead.size() > preSize) {
		return true;
	}
	return false;
}

bool LingLan::Compile::Grammatical::LALRState::coreEqual(const LALRState* p_state) const {

	if (this->kernelItemCnt != p_state->kernelItemCnt) {
		return false;
	}
	u32 inKernelSize = p_state->kernelItemCnt;
	vector<LALRItem*>::const_iterator thisB = this->items.cbegin();
	vector<LALRItem*>::const_iterator thisE = this->items.cend();
	vector<LALRItem*>::const_iterator thisIt;
	//����input������item
	for (u32 i = 0; i < inKernelSize; i++) {
		//Ҫ�ҵ�һ��coreEqual��item
		thisIt = thisB;
		while (thisIt != thisE) {
			if (deref(thisIt)->coreEqual(p_state->items.at(i))) {
				break;
			}
			thisIt++;
		}
		if (thisIt == thisE) {
			return false;
		}
	}
	return true;
}

bool LingLan::Compile::Grammatical::LALRState::mergeToSelf(const LALRState* p_state) const {

	u32 size = p_state->kernelItemCnt;
	vector<LALRItem*>::const_iterator thisB = this->items.cbegin();
	vector<LALRItem*>::const_iterator thisE = this->items.cbegin() + this->kernelItemCnt;
	vector<LALRItem*>::const_iterator thisIt;
	set<SymbolTerminal*>* p_thisScndCpnt = nullptr;
	set<SymbolTerminal*>* p_inScndCpnt = nullptr;
	u32 preSize = 0;
	bool realMerge = false;
	//����input������kernel item
	for (u32 i = 0; i < size; i++) {
		//Ҫ�ҵ�һ��coreEqual��item
		thisIt = thisB;
		while (thisIt != thisE) {
			if (deref(thisIt)->coreEqual(p_state->items.at(i))) {
				break;
			}
			thisIt++;
		}
		if (thisIt == thisE) {
			//�Ҳ���,������
			continue;
		}
		//merge
		if (deref(thisIt)->mergeToSelf(p_state->items.at(i))) {
			//��һ������merge����
			realMerge = true;
		}
	}
	return realMerge;
}

void LingLan::Compile::Grammatical::LALRState::clear() {

	u64 size = this->items.size();
	for (u64 i = 0; i < size; i++) {
		del(this->items.at(i));
	}
	this->items.clear();
}

LingLan::Compile::Grammatical::LALRState::~LALRState() {

	this->clear();
}

LingLan::Compile::Grammatical::LALRParsingTable::LALRParsingTable() {
}

LingLan::Compile::Grammatical::LALRParsingTable::LALRParsingTable(const LALRParsingTable& tbl) {

	(*this) = tbl;
}

void LingLan::Compile::Grammatical::LALRParsingTable::operator=(const LALRParsingTable& tbl) {

	this->clear();

	this->actionMapTable = tbl.actionMapTable;
	this->allAction = tbl.allAction;
	this->p_augGrammar = tbl.p_augGrammar;
	this->shallowClear = tbl.shallowClear;
}

void LingLan::Compile::Grammatical::LALRParsingTable::init(AugGrammar* p_augGrammar) {

	this->clear();

	this->p_augGrammar = p_augGrammar;

	//������˳��Ҫ��
	this->initBlankSet();
	this->initFirstMap();
	this->initActionTable();
}

void LingLan::Compile::Grammatical::LALRParsingTable::clear() {

	//set<SymbolNonTerminal*> blankSet;
	this->blankSet.clear();

	//map<SymbolNonTerminal*, set<SymbolTerminal*>*> firstMap;
	map<SymbolNonTerminal*, set<SymbolTerminal*>*>::const_iterator
		mit = this->firstMap.cbegin();
	map<SymbolNonTerminal*, set<SymbolTerminal*>*>::const_iterator
		mend = this->firstMap.cend();
	while (mit != mend) {
		del(mit->second);
		mit++;
	}

	//vector<map<Symbol*, LALRAction*>> actionMapTable;
	this->actionMapTable.clear();

	//vector<LALRAction*> allAction;
	if (this->shallowClear == false) {
		u32 forCnt = this->allAction.size();
		for (u32 i = 0; i < forCnt; i++) {
			del(this->allAction.at(i));
		}
	}
	this->allAction.clear();
}

void LingLan::Compile::Grammatical::LALRParsingTable::setShallowClear(bool yesOrNo) {

	this->shallowClear = yesOrNo;
}

LingLan::Compile::Grammatical::LALRParsingTable::~LALRParsingTable() {

	this->clear();
}

void LingLan::Compile::Grammatical::LALRParsingTable::initBlankSet() {

	//vector<SymbolNonTerminal*>* p_allNonTerminals 
	//	= addr(this->p_augGrammar->allNonTerminals);
	map<SymbolNonTerminal*, vector<Production*>*>* p_sameHeadProdMap
		= addr(this->p_augGrammar->sameHeadProdMap);

	//��������Symbol����Ƿ�����blank
	map<SymbolNonTerminal*, vector<Production*>*>::const_iterator mainIt;
	map<SymbolNonTerminal*, vector<Production*>*>::const_iterator mainEnd = p_sameHeadProdMap->cend();
	SymbolNonTerminal* p_curSymbol = nullptr;
	vector<Production*>* p_allProdOfSymbol = nullptr;
	vector<Symbol*>* p_curProdBody = nullptr;
	Symbol* p_curBodySymbol = nullptr;
	bool everFoundOne = true;
	while (true) {
		if (everFoundOne == false) {
			break;
		}
		everFoundOne = false;
		//ѭ������Symbol
		mainIt = p_sameHeadProdMap->cbegin();
		while (mainIt != mainEnd) {
			p_curSymbol = deref(mainIt).first;
			//������token name�Ѿ������Ϳ���������
			//��������map�в�Ҫ���
			if (this->blankSet.find(p_curSymbol) != this->blankSet.cend()) {
				mainIt++;
				continue;
			}
			//ѭ����Symbol������Prod
			p_allProdOfSymbol = deref(mainIt).second;
			for (u32 i = 0; i < p_allProdOfSymbol->size(); i++) {
				p_curProdBody = addr(p_allProdOfSymbol->at(i)->body);
				bool deriveBlank = true;
				set<SymbolNonTerminal*>::const_iterator
					mappedValue;
				//ѭ����prod������body symbol
				for (u32 j = 0; j < p_curProdBody->size(); j++) {
					p_curBodySymbol = p_curProdBody->at(j);
					//�����terminal�϶��ǲ�����������
					if (p_curBodySymbol->type == Symbol::Type::Terminal) {
						deriveBlank = false;
						break;
					} else {
						//������Ƿ���ֹ�����鿴blankMap�Ƿ�֮ǰ��ӹ�
						//δ��Ӿ������δ֪
						mappedValue = this->blankSet.find((SymbolNonTerminal*)(p_curBodySymbol));
						if (mappedValue == this->blankSet.cend()) {
							//���δ֪��ֱ�Ӽ��費����blank
							deriveBlank = false;
							break;
						}
					}
				}
				if (deriveBlank == true) {
					everFoundOne = true;
					this->blankSet.emplace(p_curSymbol);
					break;
				}
			}
			mainIt++;
		}
	}
}

void LingLan::Compile::Grammatical::LALRParsingTable::initFirstMap() {

	vector<SymbolNonTerminal*>* p_allNonTerminals
		= addr(this->p_augGrammar->allNonTerminals);

	map<SymbolNonTerminal*, vector<Production*>*>* p_sameHeadProdMap
		= addr(this->p_augGrammar->sameHeadProdMap);

	this->firstMap.clear();
	//��ʼ����Map
	for (u32 i = 0; i < p_allNonTerminals->size(); i++) {
		this->firstMap.emplace(p_allNonTerminals->at(i), new set<SymbolTerminal*>());
	}

	set<SymbolTerminal*>* p_curSymbolFirstSet = nullptr;

	map<SymbolNonTerminal*, set<SymbolNonTerminal*>*> whoDependOnWho;
	//��ʼ����Map
	for (u32 i = 0; i < p_allNonTerminals->size(); i++) {
		whoDependOnWho.emplace(p_allNonTerminals->at(i), new set<SymbolNonTerminal*>());
	}

	map<SymbolNonTerminal*, vector<Production*>*>::const_iterator mainIt = p_sameHeadProdMap->cbegin();
	map<SymbolNonTerminal*, vector<Production*>*>::const_iterator mainEnd = p_sameHeadProdMap->cend();
	SymbolNonTerminal* p_curSymbol = nullptr;
	vector<Production*>* p_allProdOfSymbol = nullptr;
	vector<Symbol*>* p_curProdBody = nullptr;
	Symbol* p_curBodySymbol = nullptr;

	/*
	* Init
	*/
	//ѭ������Symbol,�����������п��ж���first
	while (mainIt != mainEnd) {
		p_curSymbol = deref(mainIt).first;
		p_curSymbolFirstSet = deref(this->firstMap.find(p_curSymbol)).second;
		//ѭ����Symbol������Prod
		p_allProdOfSymbol = deref(mainIt).second;
		for (u32 i = 0; i < p_allProdOfSymbol->size(); i++) {
			p_curProdBody = addr(p_allProdOfSymbol->at(i)->body);
			//ѭ����prod������body symbol
			for (u32 j = 0; j < p_curProdBody->size(); j++) {
				p_curBodySymbol = p_curProdBody->at(j);
				//����ֹ���ͼ��벢������prod
				if (p_curBodySymbol->type == Symbol::Type::Terminal) {
					p_curSymbolFirstSet->emplace((SymbolTerminal*)p_curBodySymbol);
					break;
				} else {
					//����ֹ��������depend
					deref(whoDependOnWho.find(p_curSymbol)).second->emplace((SymbolNonTerminal*)p_curBodySymbol);
					if (this->blankSet.find((SymbolNonTerminal*)p_curBodySymbol) == this->blankSet.cend()) {
						//������blank��������һ��prod
						break;
					}
				}
			}
		}
		mainIt++;
	}

	/*
	* Then
	*/
	bool everFoundOne = true;
	map<SymbolNonTerminal*, set<SymbolTerminal*>*>::const_iterator itOfFirstMap;
	set<SymbolNonTerminal*>* p_allDependsSet = nullptr;
	set<SymbolNonTerminal*>::const_iterator allDependsIt;
	set<SymbolTerminal*>* fuckTheNameSet = nullptr;
	u32 preSize = 0;
	//��ʼ��ɢ
	while (true) {
		if (everFoundOne == false) {
			break;
		}
		everFoundOne = false;
		//ѭ��firstMap������NonTerminal��������ɢ
		for (itOfFirstMap = this->firstMap.cbegin(); itOfFirstMap != this->firstMap.cend(); itOfFirstMap++) {
			p_curSymbolFirstSet = deref(itOfFirstMap).second;
			p_allDependsSet = deref(whoDependOnWho.find(deref(itOfFirstMap).first)).second;
			preSize = p_curSymbolFirstSet->size();
			//ѭ����ǰNonTerminal����������������
			for (allDependsIt = p_allDependsSet->cbegin(); allDependsIt != p_allDependsSet->cend(); allDependsIt++) {
				fuckTheNameSet = deref(this->firstMap.find(deref(allDependsIt))).second;
				p_curSymbolFirstSet->insert(fuckTheNameSet->cbegin(), fuckTheNameSet->cend());
			}
			//����Ƿ�����Ԫ�ؼ���
			if (preSize < p_curSymbolFirstSet->size()) {
				everFoundOne = true;
			}
		}
	}
}

void LingLan::Compile::Grammatical::LALRParsingTable::initActionTable() {

	//eff_initAllState
	//eff_spread

	___DEL___
		vector<LALRState*>* p_allStates = new vector<LALRState*>();
	___DEL___
		set<LALRState*>* p_needUpdateVec = new set<LALRState*>();

	this->eff_initAllStates(p_allStates, p_needUpdateVec);
	this->eff_spread(p_allStates, p_needUpdateVec);

	//DELETE ALL STATES, p_needUpdateVec is a subSet of p_allStates so it needn't be deleted
	u32 size = p_allStates->size();
	for (u32 i = 0; i < size; i++) {
		del(p_allStates->at(i));
	}
	del(p_allStates);
	del(p_needUpdateVec);
}

bool LingLan::Compile::Grammatical::LALRParsingTable::first(vector<Symbol*>* p_inBody, u16 from, u16 to, set<SymbolTerminal*>* p_inSet) {

	//����[from, to)�����ǰһ��derive blank
	Symbol* p_curBodySymbol = nullptr;
	set<SymbolTerminal*>* p_curBodySymbolFirstSet = nullptr;
	u8 i = from;
	for (; i < to; i++) {
		p_curBodySymbol = p_inBody->at(i);
		if (p_curBodySymbol->type == Symbol::Type::Terminal) {
			p_inSet->emplace((SymbolTerminal*)p_curBodySymbol);
			break;
		}
		p_curBodySymbolFirstSet = deref(this->firstMap.find((SymbolNonTerminal*)p_curBodySymbol)).second;
		//����
		p_inSet->insert(p_curBodySymbolFirstSet->cbegin(), p_curBodySymbolFirstSet->cend());
		if (this->blankSet.find((SymbolNonTerminal*)p_curBodySymbol) == this->blankSet.cend()) {
			break;
		}
	}
	//�Ƿ�������
	return (i >= to);
}

LALRState* LingLan::Compile::Grammatical::LALRParsingTable::eff_goto(vector<LALRItem*>* p_itemVec, Symbol* p_goToSymbol) {

	LALRState* p_retState = new LALRState();
	LALRItem* p_curItem = nullptr;
	LALRItem* p_newItem = nullptr;
	for (u32 i = 0; i < p_itemVec->size(); i++) {
		p_curItem = p_itemVec->at(i);
		if (p_curItem->isOver()) {
			continue;
		}
		if (p_curItem->p_prod->body.at(p_curItem->pos) == p_goToSymbol) {
			p_newItem = new LALRItem();
			p_newItem->p_prod = p_curItem->p_prod;
			p_newItem->pos = p_curItem->pos + 1;
			p_newItem->isKernelItem = true;
			p_newItem->sc_lookAhead.insert(p_curItem->sc_lookAhead.cbegin(), p_curItem->sc_lookAhead.cend());
			p_newItem->spreadFather.insert(p_curItem);
			p_newItem->p_lalrState = p_retState;
			p_retState->items.emplace_back(p_newItem);
		}
	}

	p_retState->kernelItemCnt = p_retState->items.size();

	return p_retState;
}

void LingLan::Compile::Grammatical::LALRParsingTable::eff_closure(LALRState* p_inState) {

	vector<LALRItem*>* p_itemVec = addr(p_inState->items);
	u32 curItemIdx = 0;
	LALRItem* p_curItem = nullptr;
	bool deriveBlank = false;

	map<SymbolNonTerminal*, vector<Production*>*>* p_sameHeadProdMap
		= addr(this->p_augGrammar->sameHeadProdMap);

	vector<Production*>* p_allProdWithSameHead = nullptr;
	Production* p_prodIt = nullptr;
	set<SymbolTerminal*> firstSet;

	vector<LALRItem*>* curVec = new vector<LALRItem*>();
	vector<LALRItem*>* nxtVec = new vector<LALRItem*>();
	vector<LALRItem*>* tmpVec = nullptr;

	//��ʼ��,����ľ���kernel items
	for (u32 i = 0; i < p_itemVec->size(); i++) {
		curVec->emplace_back(p_itemVec->at(i));
	}

	//Start Closure
	while (true) {
		if (curVec->size() <= 0) {
			//��ǰlist����ʲô��û�У��˳�ѭ��
			break;
		}
		//clear nxtNeedUpdateState
		nxtVec->clear();
		for (u32 i = 0; i < curVec->size(); i++) {
			p_curItem = curVec->at(i);
			//���û��isOver���ܼ���
			if (p_curItem->isOver()) {
				continue;
			}
			//�����Non-Terminal���ܼ���
			if (p_curItem->p_prod->body.at(p_curItem->pos)->type == Symbol::Type::Terminal) {
				continue;
			}
			//���Ҷ�Ӧ������prod��������Ӧ��scndCpnt������֮ǰ�����Ƿ�item�Ѿ����ڣ����ڵĻ���merge item
			//���ڵ�ǰitem���ɵ��µ�prod��ת��Ϊitem������Ѵ��ھ��ںϷ������
			firstSet.clear();
			deriveBlank = this->first(addr(p_curItem->p_prod->body), p_curItem->pos + 1, p_curItem->p_prod->body.size(), &firstSet);
			p_allProdWithSameHead = deref(p_sameHeadProdMap->find((SymbolNonTerminal*)(p_curItem->p_prod->body.at(p_curItem->pos)))).second;
			for (u32 j = 0; j < p_allProdWithSameHead->size(); j++) {
				p_prodIt = p_allProdWithSameHead->at(j);
				LALRItem* p_newItem = new LALRItem();
				p_newItem->p_prod = p_prodIt;
				p_newItem->pos = 0;
				p_newItem->sc_lookAhead.insert(firstSet.cbegin(), firstSet.cend());
				p_newItem->p_lalrState = p_inState;
				if (deriveBlank) {
					//��ζ�ŵ�ǰ��closure��Item��spread
					p_newItem->sc_lookAhead.insert(
						p_curItem->sc_lookAhead.cbegin(),
						p_curItem->sc_lookAhead.cend()
					);
				}
				//����Ƿ����
				u32 foundIdx = 0;
				for (; foundIdx < p_itemVec->size(); foundIdx++) {
					if (p_newItem->coreEqual(p_itemVec->at(foundIdx))) {
						break;
					}
				}
				if (foundIdx >= p_itemVec->size()) {
					//didnt found
					//add one
					p_itemVec->emplace_back(p_newItem);
					//add to nxtNeedUpdateState
					if (find(nxtVec->cbegin(), nxtVec->cend(), p_newItem) == nxtVec->cend()) {
						nxtVec->emplace_back(p_newItem);
					}
				} else {
					//found��mergeToSelf
					//kernel item��fatherֻ����������state��
					//�������merge
					if (p_itemVec->at(foundIdx)->mergeToSelfOnlyLookAhead(p_newItem)) {
						//add to nxtList
						if (find(nxtVec->cbegin(), nxtVec->cend(), p_itemVec->at(foundIdx)) == nxtVec->cend()) {
							nxtVec->emplace_back(p_itemVec->at(foundIdx));
						}
					}
					del(p_newItem);
				}
			}
		}
		tmpVec = curVec;
		curVec = nxtVec;
		nxtVec = tmpVec;
	}
}

//void LingLan::Compile::Grammatical::LALRParsingTable::closure(vector<LALRItem*>* p_itemVec) {
//
//	u32 curItemIdx = 0;
//	LALRItem* p_curItem = nullptr;
//	bool deriveBlank = false;
//
//	map<SymbolNonTerminal*, vector<Production*>*>* p_sameHeadProdMap
//		= addr(this->p_augGrammar->sameHeadProdMap);
//
//	vector<Production*>* p_allProdWithSameHead = nullptr;
//	Production* p_prodIt = nullptr;
//	set<SymbolTerminal*> firstSet;
//
//	vector<u32>* curNeedUpdateState = new vector<u32>();
//	vector<u32>* nxtNeedUpdateState = new vector<u32>();
//	vector<u32>* tmpNeedUpdateState = nullptr;
//
//	//��ʼ��
//	for (u32 i = 0; i < p_itemVec->size(); i++) {
//		curNeedUpdateState->emplace_back(i);
//	}
//
//	//Start Closure
//	while (true) {
//		if (curNeedUpdateState->size() <= 0) {
//			//��ǰlist����ʲô��û�У��˳�ѭ��
//			break;
//		}
//		//clear nxtNeedUpdateState
//		nxtNeedUpdateState->clear();
//		for (u32 i = 0; i < curNeedUpdateState->size(); i++) {
//
//			curItemIdx = curNeedUpdateState->at(i);
//			p_curItem = p_itemVec->at(curItemIdx);
//			//���û��isOver���ܼ���
//			if (p_curItem->isOver()) {
//				continue;
//			}
//			//�����Non-Terminal���ܼ���
//			if (p_curItem->p_prod->body.at(p_curItem->pos)->type == Symbol::Type::Terminal) {
//				continue;
//			}
//			//���Ҷ�Ӧ������prod��������Ӧ��scndCpnt������֮ǰ�����Ƿ�item�Ѿ����ڣ����ڵĻ���merge item
//			//���ڵ�ǰitem���ɵ��µ�prod��ת��Ϊitem������Ѵ��ھ��ںϷ������
//			firstSet.clear();
//			deriveBlank = this->first(addr(p_curItem->p_prod->body), p_curItem->pos + 1, p_curItem->p_prod->body.size(), &firstSet);
//			p_allProdWithSameHead = deref(p_sameHeadProdMap->find((SymbolNonTerminal*)(p_curItem->p_prod->body.at(p_curItem->pos)))).second;
//			for (u32 j = 0; j < p_allProdWithSameHead->size(); j++) {
//				p_prodIt = p_allProdWithSameHead->at(j);
//				LALRItem* p_newItem = new LALRItem();
//				p_newItem->p_prod = p_prodIt;
//				p_newItem->pos = 0;
//				p_newItem->sc_lookAhead.insert(firstSet.cbegin(), firstSet.cend());
//				if (deriveBlank) {
//					//��curItem��scndCpnt����
//					p_newItem->sc_lookAhead.insert(p_curItem->sc_lookAhead.cbegin(), p_curItem->sc_lookAhead.cend());
//				}
//				//����Ƿ����
//				u32 foundIdx = 0;
//				for (; foundIdx < p_itemVec->size(); foundIdx++) {
//					if (p_newItem->coreEqual(p_itemVec->at(foundIdx))) {
//						break;
//					}
//				}
//				if (foundIdx >= p_itemVec->size()) {
//					//didnt found
//					//add one
//					p_itemVec->emplace_back(p_newItem);
//					//add to nxtNeedUpdateState
//					if (find(nxtNeedUpdateState->cbegin(), nxtNeedUpdateState->cend(), foundIdx) == nxtNeedUpdateState->cend()) {
//						nxtNeedUpdateState->emplace_back(foundIdx);
//					}
//				} else {
//					//found��mergeToSelf
//					//�������merge
//					if (p_itemVec->at(foundIdx)->mergeToSelfOnlyLookAhead(p_newItem)) {
//						//�������ǰ��
//						if (foundIdx < curItemIdx) {
//							//add to nxtList
//							if (find(nxtNeedUpdateState->cbegin(), nxtNeedUpdateState->cend(), foundIdx) == nxtNeedUpdateState->cend()) {
//								nxtNeedUpdateState->emplace_back(foundIdx);
//							}
//						}
//					}
//					del(p_newItem);
//				}
//			}
//		}
//		tmpNeedUpdateState = curNeedUpdateState;
//		curNeedUpdateState = nxtNeedUpdateState;
//		nxtNeedUpdateState = tmpNeedUpdateState;
//	}
//}

void LingLan::Compile::Grammatical::LALRParsingTable::eff_initAllStates(vector<LALRState*>* p_allStates, set<LALRState*>* p_needUpdateVec) {

	vector<SymbolNonTerminal*>* p_allNonTerminals = addr(this->p_augGrammar->allNonTerminals);
	map<SymbolNonTerminal*, vector<Production*>*>* p_sameHeadProdMap = addr(this->p_augGrammar->sameHeadProdMap);

	vector<map<Symbol*, LALRAction*>>* p_actionMapTable = addr(this->actionMapTable);

	LALRItem* p_initItem = new LALRItem();
	p_initItem->p_prod = deref(p_sameHeadProdMap->find(p_allNonTerminals->at(0))).second->at(0);
	p_initItem->pos = 0;
	/*
	* ��ֱ��ɵ��
	* p_initItem->sc_lookAhead.emplace(new SymbolEndMarker());
	*/
	p_initItem->sc_lookAhead.emplace(addr(this->p_augGrammar->p_lexer->endMarker));
	/*
	* ��ֱ��ɵ�� ^^^^^
	*/
	p_initItem->isKernelItem = true;
	LALRState* p_initState = new LALRState();
	p_initState->items.emplace_back(p_initItem);
	p_initState->kernelItemCnt = p_initState->items.size();
	p_allStates->emplace_back(p_initState);
	this->eff_closure(p_initState);
	p_actionMapTable->emplace_back();

	//��������
	set<Symbol*> hasBeenGotoSymbol;

	//����coreItem State
	//ֻ�����Է����ɵ�scndCpnt��item֮��scndCpnt�ļ̳й�ϵ
	u32 curStateIdx = 0;
	u32 curItemIdx = 0;
	LALRState* p_curState = nullptr;
	LALRState* p_newState = nullptr;
	LALRItem* p_curItem = nullptr;
	Symbol* p_curGotoSymbol = nullptr;
	/*
	* ��������State
	* (ֻ��������CoreItem��spreadFather)
	*/
	for (curStateIdx = 0; curStateIdx < p_allStates->size(); curStateIdx++) {
		hasBeenGotoSymbol.clear();
		p_curState = p_allStates->at(curStateIdx);
		//����ÿ����state�е�item
		for (curItemIdx = 0; curItemIdx < p_curState->items.size(); curItemIdx++) {
			p_curItem = p_curState->items.at(curItemIdx);
			//�Ƿ����ʸ�goto
			if (p_curItem->isOver()) {
				continue;
			}
			p_curGotoSymbol = p_curItem->p_prod->body.at(p_curItem->pos);
			//û��goto�����ܼ���
			if (hasBeenGotoSymbol.emplace(p_curGotoSymbol).second == false) {
				//�Ѵ���
				continue;
			}
			p_newState = this->eff_goto(addr(p_curState->items), p_curGotoSymbol);
			//����²�����newGotoState�Ƿ�CoreExist
			u32 foundIdx = 0;
			for (; foundIdx < p_allStates->size(); foundIdx++) {
				if (p_allStates->at(foundIdx)->coreEqual(p_newState)) {
					break;
				}
			}
			if (foundIdx >= p_allStates->size()) {
				//nothing found
				//then add
				//closure
				this->eff_closure(p_newState);
				p_allStates->emplace_back(p_newState);
				p_actionMapTable->emplace_back();
			} else {
				//mergeToSelf if exists
				p_allStates->at(foundIdx)->mergeToSelf(p_newState);

				//��Ϊ������һ�Σ�����Ҫ����linkһ��
				p_needUpdateVec->emplace(p_allStates->at(foundIdx));

				del(p_newState);
			}
			//���Shift Map
			this->allAction.emplace_back(new LALRAction());
			this->allAction.back()->actType = LALRAction::ActionType::ShiftToState;
			this->allAction.back()->shiftToState = foundIdx;
			p_actionMapTable->at(curStateIdx).emplace(p_curGotoSymbol, this->allAction.back());
		}
	}
}

void LingLan::Compile::Grammatical::LALRParsingTable::eff_spread(vector<LALRState*>* p_allStates, set<LALRState*>* p_needUpdateVec) {

	u32 stateCnt = p_allStates->size();
	u32 kernerItemCnt = 0;
	LALRState* p_curState = nullptr;
	LALRItem* p_curItem = nullptr;
	set<LALRState*>* curNeedUpdateState = p_needUpdateVec;
	set<LALRState*>* nxtNeedUpdateState = new set<LALRState*>();
	set<LALRState*>* tmpNeedUpdateState = nullptr;
	set<LALRState*>::const_iterator needUpdateStateIt;
	set<LALRState*>::const_iterator needUpdateStateE;
	set<LALRItem*>::const_iterator fatherBegin;
	set<LALRItem*>::const_iterator fatherEnd;

	while (true) {

		if (curNeedUpdateState->size() <= 0) {
			//��ǰlist����ʲô��û�У��˳�ѭ��
			break;
		}
		//clear nxtNeedUpdateState
		nxtNeedUpdateState->clear();
		
		//ѭ������curUpdateState����closure
		needUpdateStateIt = curNeedUpdateState->cbegin();
		needUpdateStateE = curNeedUpdateState->cend();
		while (needUpdateStateIt != needUpdateStateE) {
			this->eff_closure(deref(needUpdateStateIt));
			needUpdateStateIt++;
		}

		//ѭ������State
		for (u32 s = 0; s < stateCnt; s++) {
			p_curState = p_allStates->at(s);
			kernerItemCnt = p_curState->items.size();
			//ѭ������state������item
			for (u32 i = 0; i < kernerItemCnt; i++) {
				p_curItem = p_curState->items.at(i);
				//ѭ�����item������fatherItem������ڵ�ǰneedUpdateState�����merge
				//������state�����item��һ����merge���ͼ�����һ��needUpdateState
				fatherBegin = p_curItem->spreadFather.cbegin();
				fatherEnd = p_curItem->spreadFather.cend();
				//���ڴ�item�������е�father�и����Լ�
				while (fatherBegin != fatherEnd) {
					if (curNeedUpdateState->find(deref(fatherBegin)->p_lalrState) == curNeedUpdateState->cend()) {
						fatherBegin++;
						continue;
					}
					if (p_curItem->mergeToSelfOnlyLookAhead(deref(fatherBegin))) {
						//��һ����merge������stateҪ������һ��nxtVec
						nxtNeedUpdateState->emplace(p_curItem->p_lalrState);
					}
					fatherBegin++;
				}
			}
		}

		tmpNeedUpdateState = curNeedUpdateState;
		curNeedUpdateState = nxtNeedUpdateState;
		nxtNeedUpdateState = tmpNeedUpdateState;
	}

	// * ����actionMapTable

	vector<map<Symbol*, LALRAction*>>* p_actionMapTable = addr(this->actionMapTable);

	set<SymbolTerminal*>::const_iterator scBegin;
	set<SymbolTerminal*>::const_iterator scEnd;
	u32 itemCnt = 0;
	//����Reduce Table
	//��������State��is-over item�������map��������ظ������쳣
	for (u32 s = 0; s < stateCnt; s++) {
		p_curState = p_allStates->at(s);
		itemCnt = p_curState->items.size();
		//ѭ������item
		for (u32 i = 0; i < itemCnt; i++) {
			p_curItem = p_curState->items.at(i);
			if (p_curItem->isOver()) {
				//ѭ������scndCpnt(look ahead)
				scBegin = p_curItem->sc_lookAhead.cbegin();
				scEnd = p_curItem->sc_lookAhead.cend();
				while (scBegin != scEnd) {
					//���Reduce Map
					//�����ǰ��endmaker��item prod idx = 0������Acc��
					this->allAction.emplace_back(new LALRAction());
					if (deref(scBegin)->type == Symbol::Type::EndMarker
						&&
						p_curItem->p_prod == this->p_augGrammar->productions.at(0)) {

						this->allAction.back()->actType = LALRAction::ActionType::Accept;
					} else {
						this->allAction.back()->actType = LALRAction::ActionType::ReduceWithProd;
					}
					this->allAction.back()->p_reduceWithProd = p_curItem->p_prod;
					if (p_actionMapTable->at(s).emplace(deref(scBegin), this->allAction.back()).second == false) {
						//���쳣
						throw new LALRParserException(u8"�﷨��ͻ������LALR�﷨");
					}
					scBegin++;
				}
			}
		}
	}
}
