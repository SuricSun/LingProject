#include "CompilePack.h"

using namespace LingLan;
using namespace LingLan::Compile::Grammatical;

u32 LingLan::Compile::CompileWrapper::SerializableCompilePack::getSymbolTerminalSize(SymbolTerminal* p_sym) {

	//objSize: 4byte
	//type: 1byte
	//name: 2byte [size], [size] byte str
	//regexStr: 2byte [size], [size] byte str
	return 9 + p_sym->name.size() + p_sym->regexStr.size();
}

u32 LingLan::Compile::CompileWrapper::SerializableCompilePack::getSymbolTerminalEndMarkerSize(SymbolEndMarker* p_sym) {

	//objSize: 4byte
	//type: 1byte
	//name: na
	//regexStr: na
	return 5;
}

u32 LingLan::Compile::CompileWrapper::SerializableCompilePack::getSymbolNonTerminalSize(SymbolNonTerminal* p_sym) {

	//objSize: 4byte
	//type: 1byte
	//name: 2byte [size], [size] byte str
	return 7 + p_sym->name.size();
}

u32 LingLan::Compile::CompileWrapper::SerializableCompilePack::getProductionSize(Production* p_prod) {

	//objSize: 4byte
	//SymbolNonTerminal* p_head: 8byte
	//vector<Symbol*> body: 2byte [size], [size] * 8byte
	//vector<bool> reserveInAstTree: 2byte [size], [size] * 1byte
	return 16 + p_prod->body.size() * 8 + p_prod->reserveInAstTree.size();
}

u32 LingLan::Compile::CompileWrapper::SerializableCompilePack::getLALRActionSize(LALRAction* p_act) {

	//objSize: 4byte
	//ActionType actType: 1byte 
	//u32 shiftToState: 4byte
	//Production* p_reduceWithProd: 8byte
	return 17;
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::writeSymbolTerminal(SymbolTerminal* p_sym, Util::IO::FileOutputStream* p_fout) {

	//objSize: 4byte
	u32 objSize = this->getSymbolTerminalSize(p_sym);
	p_fout->write(&objSize, 4);
	//type: 1byte
	u8 type = (u8)p_sym->type;
	p_fout->write(&(type), 1);
	//name: 2byte [size], [size] byte str
	u16 size = p_sym->name.size();
	p_fout->write(&size, 2);
	p_fout->write(p_sym->name.data(), size);
	//regexStr: 2byte [size], [size] byte str
	size = p_sym->regexStr.size();
	p_fout->write(&size, 2);
	p_fout->write(p_sym->regexStr.data(), size);
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::writeSymbolTerminalEndMarker(SymbolEndMarker* p_sym, Util::IO::FileOutputStream* p_fout) {

	//objSize: 4byte
	u32 objSize = this->getSymbolTerminalEndMarkerSize(p_sym);
	p_fout->write(&objSize, 4);
	//type: 1byte
	u8 type = (u8)p_sym->type;
	p_fout->write(&(type), 1);
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::writeSymbolNonTerminal(SymbolNonTerminal* p_sym, Util::IO::FileOutputStream* p_fout) {

	//objSize: 4byte
	u32 objSize = this->getSymbolNonTerminalSize(p_sym);
	p_fout->write(&objSize, 4);
	//type: 1byte
	u8 type = (u8)p_sym->type;
	p_fout->write(&(type), 1);
	//name: 2byte [size], [size] byte str
	u16 size = p_sym->name.size();
	p_fout->write(&size, 2);
	p_fout->write(p_sym->name.data(), size);
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::writeProduction(Production* p_prod, map<SymbolTerminal*, u64>* p_terminalReaddress, map<SymbolNonTerminal*, u64>* p_nonTerminalReaddress, Util::IO::FileOutputStream* p_fout) {

	//objSize: 4byte
	u32 objSize = this->getProductionSize(p_prod);
	p_fout->write(&objSize, 4);
	//SymbolNonTerminal* p_head: 8byte
	u64 ptr = 0;
	assert(p_prod->p_head->type == Symbol::Type::NonTerminal);
	ptr = (u64)(p_nonTerminalReaddress->find((SymbolNonTerminal*)p_prod->p_head)->second);//需要重定向
	p_fout->write(&ptr, 8);
	//vector<Symbol*> body: 2byte [size], [size] * 8byte
	//一个循环
	u16 size = p_prod->body.size();
	p_fout->write(&size, 2);
	Symbol* p_curSym = nullptr;
	for (u32 i = 0; i < size; i++) {
		p_curSym = p_prod->body.at(i);
		if (p_curSym->type == Symbol::Type::Terminal) {
			ptr = p_terminalReaddress->find((SymbolTerminal*)p_curSym)->second;//需要重定向
			p_fout->write(&ptr, 8);
		} else {
			//else is NonTerimal never EndMarker
			ptr = p_nonTerminalReaddress->find((SymbolNonTerminal*)p_curSym)->second;//需要重定向
			p_fout->write(&ptr, 8);
		}
	}
	//vector<bool> reserveInAstTree: 2byte [size], [size] * 1byte
	size = p_prod->reserveInAstTree.size();
	p_fout->write(&size, 2);
	u8 boolVal = 0;
	for (u32 i = 0; i < size; i++) {
		boolVal = p_prod->reserveInAstTree.at(i);
		p_fout->write(addr(boolVal), 1);
	}
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::writeLALRAction(LALRAction* p_act, map<Production*, u64>* p_prodReaddress, Util::IO::FileOutputStream* p_fout) {

	//objSize: 4byte
	u32 objSize = this->getLALRActionSize(p_act);
	p_fout->write(&objSize, 4);
	//ActionType actType: 1byte 
	u8 actType = (u8)p_act->actType;
	p_fout->write(&actType, 1);
	//u32 shiftToState: 4byte
	u32 shiftToState = p_act->shiftToState;
	p_fout->write(&shiftToState, 4);
	//Production* p_reduceWithProd: 8byte
	u64 ptr = 0;
	if (p_act->p_reduceWithProd != nullptr) {
		ptr = p_prodReaddress->find(p_act->p_reduceWithProd)->second;//需要重定向
	}
	p_fout->write(&ptr, 8);
}


void LingLan::Compile::CompileWrapper::SerializableCompilePack::initFromLanFile(const char16_t* p_lanFile) {
	
	LanguageRulesFile lrf;
	lrf.setFilePath(p_lanFile);
	lrf.format();

	this->lexer.init(lrf.getFormattedSymbolTerminalDef());
	this->augGrammar.init(lrf.getFormattedProductionDef(), addr(this->lexer));
	this->parsingTable.init(addr(this->augGrammar));
	this->parser.init(addr(this->parsingTable));
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::initFromLanFile(LanguageRulesFile* p_lanFile) {

	this->lexer.init(p_lanFile->getFormattedSymbolTerminalDef());
	this->augGrammar.init(p_lanFile->getFormattedProductionDef(), addr(this->lexer));
	this->parsingTable.init(addr(this->augGrammar));
	this->parser.init(addr(this->parsingTable));
}

void LingLan::Compile::CompileWrapper::SerializableCompilePack::serialize(WCHAR* p_path) {

	map<SymbolTerminal*, u64> terminalReaddress;
	u64 terminalSize = 12;
	map<SymbolEndMarker*, u64> terminalEndMarkerReaddress;
	u64 terminalEndMarkerSize = 12;
	map<SymbolNonTerminal*, u64> nonTerminalReaddress;
	u64 nonTerminalSize = 12;
	map<Production*, u64> prodReaddress;
	u64 prodSize = 12;
	map<LALRAction*, u64> lalrActionReaddress;
	u64 lalrActionSize = 12;

	u32 forSize = 0;
	// * 遍历Lexer的Terminal和EndMarker
	vector<SymbolTerminal*>* p_terminalVec = addr(this->lexer.allSymbolTerminal);
	forSize = p_terminalVec->size();
	for (u32 i = 0; i < forSize; i++) {
		if (terminalReaddress.emplace(p_terminalVec->at(i), terminalSize).second) {
			terminalSize += this->getSymbolTerminalSize(p_terminalVec->at(i));
		}
	}
	terminalEndMarkerSize += terminalSize;
	if (terminalEndMarkerReaddress.emplace(addr(this->lexer.endMarker), terminalEndMarkerSize).second) {
		terminalEndMarkerSize += this->getSymbolTerminalEndMarkerSize(addr(this->lexer.endMarker));
	}
	// * 遍历AugGrammar的NonTerminal和Prod
	nonTerminalSize += terminalEndMarkerSize;
	vector<SymbolNonTerminal*>* p_nonTerminalVec = addr(this->augGrammar.allNonTerminals);
	forSize = p_nonTerminalVec->size();
	for (u32 i = 0; i < forSize; i++) {
		if (nonTerminalReaddress.emplace(p_nonTerminalVec->at(i), nonTerminalSize).second) {
			nonTerminalSize += this->getSymbolNonTerminalSize(p_nonTerminalVec->at(i));
		}
	}
	prodSize += nonTerminalSize;
	vector<Production*>* p_prodVec = addr(this->augGrammar.productions);
	forSize = p_prodVec->size();
	for (u32 i = 0; i < forSize; i++) {
		if (prodReaddress.emplace(p_prodVec->at(i), prodSize).second) {
			prodSize += this->getProductionSize(p_prodVec->at(i));
		}
	}
	// * 遍历action
	lalrActionSize += prodSize;
	vector<LALRAction*>* p_actVec = addr(this->parsingTable.allAction);
	forSize = p_actVec->size();
	for (u32 i = 0; i < forSize; i++) {
		if (lalrActionReaddress.emplace(p_actVec->at(i), lalrActionSize).second) {
			lalrActionSize += this->getLALRActionSize(p_actVec->at(i));
		}
	}

	// * 开始写文件

	Util::IO::FileOutputStream fout;
	fout.open(p_path);

	u32 objCnt = 0;
	// * readdress完成现在遍历所有obj并写入

	// * terminal

	assert(sizeof(terminalSize) == 8);

	//写入obj区域大小
	fout.write(&terminalSize, 8);
	//写入Obj cnt
	objCnt = p_terminalVec->size();
	fout.write(&objCnt, 4);

	for (u32 i = 0; i < objCnt; i++) {
		this->writeSymbolTerminal(p_terminalVec->at(i), addr(fout));
	}

	// * terminal end marker

	assert(sizeof(terminalEndMarkerSize) == 8);

	//写入obj区域大小
	fout.write(&terminalEndMarkerSize, 8);
	//写入Obj cnt
	objCnt = 1;
	fout.write(&objCnt, 4);

	this->writeSymbolTerminalEndMarker(addr(this->lexer.endMarker), addr(fout));

	// * nonTerminal

	assert(sizeof(nonTerminalSize) == 8);

	//写入obj区域大小
	fout.write(&nonTerminalSize, 8);
	//写入Obj cnt
	objCnt = p_nonTerminalVec->size();
	fout.write(&objCnt, 4);

	for (u32 i = 0; i < objCnt; i++) {
		//对于每个obj，写入
		this->writeSymbolNonTerminal(p_nonTerminalVec->at(i), addr(fout));
	}

	// * prod

	assert(sizeof(prodSize) == 8);

	//写入obj区域大小
	fout.write(&prodSize, 8);
	//写入Obj cnt
	objCnt = p_prodVec->size();
	fout.write(&objCnt, 4);

	for (u32 i = 0; i < objCnt; i++) {
		//对于每个obj，写入
		this->writeProduction(p_prodVec->at(i), addr(terminalReaddress), addr(nonTerminalReaddress), addr(fout));
	}

	// * action

	assert(sizeof(lalrActionSize) == 8);

	//写入obj区域大小
	fout.write(&lalrActionSize, 8);
	//写入Obj cnt
	objCnt = p_actVec->size();
	fout.write(&objCnt, 4);

	for (u32 i = 0; i < objCnt; i++) {
		//对于每个obj，写入
		this->writeLALRAction(p_actVec->at(i), addr(prodReaddress), &fout);
	}

	// * 写入map array
	//格式: 4byte mapCnt, map, map, map...
	//map格式: 4byte mapPairCnt, 8byte Symbol*, 8byte Action*
	//写入大小
	vector<map<Symbol*, LALRAction*>>* p_actionTable = addr(this->parsingTable.actionMapTable);
	map<Symbol*, LALRAction*>* p_curMap = nullptr;
	map<Symbol*, LALRAction*>::const_iterator cit_map;
	map<Symbol*, LALRAction*>::const_iterator cend_map;
	Symbol* p_curSymbol = nullptr;
	u32 stateCnt = p_actionTable->size();
	u32 mapSize = 0;
	u64 ptr = 0;
	fout.write(&stateCnt, 4);
	for (u32 i = 0; i < stateCnt; i++) {
		p_curMap = addr(p_actionTable->at(i));
		//循环map
		//写入map大小
		mapSize = p_curMap->size();
		fout.write(&mapSize, 4);
		cit_map = p_curMap->cbegin();
		cend_map = p_curMap->cend();
		while (cit_map != cend_map) {
			p_curSymbol = cit_map->first;
			//写入Symbol地址，重定向
			if (p_curSymbol->type == Symbol::Type::Terminal) {
				ptr = terminalReaddress.find((SymbolTerminal*)p_curSymbol)->second;//需要重定向
				fout.write(&ptr, 8);
			} else if (p_curSymbol->type == Symbol::Type::EndMarker) {
				ptr = terminalEndMarkerReaddress.find((SymbolEndMarker*)p_curSymbol)->second;//需要重定向
				fout.write(&ptr, 8);
			} else {
				//else is NonTerimal
				ptr = nonTerminalReaddress.find((SymbolNonTerminal*)p_curSymbol)->second;//需要重定向
				fout.write(&ptr, 8);
			}
			//写入Action地址，重定向
			ptr = lalrActionReaddress.find(cit_map->second)->second;
			fout.write(&ptr, 8);
			cit_map++;
		}
	}

	fout.close();
}

LingLan::Compile::CompileWrapper::DeserializedCompilePack::DeserializedCompilePack() {
}

void LingLan::Compile::CompileWrapper::DeserializedCompilePack::deserialize(WCHAR* p_path) {

	this->clear();

	this->lexer.setShallowClear(true);
	this->augGrammar.setShallowClear(true);
	this->parsingTable.setShallowClear(true);

	Util::IO::FileInputStream fin;
	fin.open(p_path);

	map<u64, SymbolTerminal*> terminalReaddress;
	map<u64, SymbolEndMarker*> terminalEndMarkerReaddress;
	map<u64, SymbolNonTerminal*> nonTerminalReaddress;
	map<u64, Production*> prodReaddress;
	map<u64, LALRAction*> actReaddress;

	u64 junkBin = 0;
	u64 bytesRead = 0;
	u64 regionSize = 0;
	u32 objCnt = 0;
	u16 strSize = 0;
	u8* p_str = nullptr;

	// * 读取SymbolTerminal
	fin.read(addr(regionSize), 8);
	bytesRead += 8;
	fin.read(addr(objCnt), 4);
	bytesRead += 4;
	this->ter.resize(objCnt);
	for (u32 i = 0; i < objCnt; i++) {
		//加入remap
		this->lexer.allSymbolTerminal.emplace_back(addr(this->ter.at(i)));
		terminalReaddress.emplace(bytesRead, addr(this->ter.at(i)));
		//objSize: 4byte
		fin.read(&junkBin, 4);
		bytesRead += 4;
		//type: 1byte
		this->ter.at(i).type = (Symbol::Type)fin.readOneByte();
		bytesRead += 1;
		//name: 2byte [size], [size] byte str
		fin.read(&strSize, 2);
		bytesRead += 2;
		___DEL___
			p_str = new u8[strSize];
		fin.read(p_str, strSize);
		bytesRead += strSize;
		this->ter.at(i).name.assign((char8_t*)p_str, strSize);
		dela(p_str);
		//regexStr: 2byte [size], [size] byte str
		fin.read(&strSize, 2);
		bytesRead += 2;
		___DEL___
			p_str = new u8[strSize];
		fin.read(p_str, strSize);
		bytesRead += strSize;
		this->ter.at(i).regexStr.assign((char8_t*)p_str, strSize);
		dela(p_str);
		this->ter.at(i).regexPattern.assign((char*)this->lexer.allSymbolTerminal.at(i)->regexStr.c_str());
	}

	// * 读取SymbolTerminalEndMarker
	fin.read(addr(regionSize), 8);
	bytesRead += 8;
	fin.read(addr(objCnt), 4);
	bytesRead += 4;
	//加入remap
	terminalEndMarkerReaddress.emplace(bytesRead, addr(this->lexer.endMarker));
	//objSize: 4byte
	fin.read(&junkBin, 4);
	bytesRead += 4;
	//type: 1byte
	this->lexer.endMarker.type = (Symbol::Type)fin.readOneByte();
	bytesRead += 1;

	// * 读取SymbolNonTerminal
	fin.read(addr(regionSize), 8);
	bytesRead += 8;
	fin.read(addr(objCnt), 4);
	bytesRead += 4;
	this->nonTer.resize(objCnt);
	for (u32 i = 0; i < objCnt; i++) {
		//加入remap
		this->augGrammar.allNonTerminals.emplace_back(addr(this->nonTer.at(i)));
		nonTerminalReaddress.emplace(bytesRead, addr(this->nonTer.at(i)));
		//objSize: 4byte
		fin.read(&junkBin, 4);
		bytesRead += 4;
		//type: 1byte
		this->nonTer.at(i).type = (Symbol::Type)fin.readOneByte();
		bytesRead += 1;
		//name: 2byte [size], [size] byte str
		fin.read(&strSize, 2);
		bytesRead += 2;
		___DEL___
			p_str = new u8[strSize];
		fin.read(p_str, strSize);
		bytesRead += strSize;
		this->nonTer.at(i).name.assign((char8_t*)p_str, strSize);
		dela(p_str);
	}

	// * prod
	map<u64, SymbolTerminal*>::const_iterator cit_ter;
	map<u64, SymbolNonTerminal*>::const_iterator cit_nonTer;
	fin.read(addr(regionSize), 8);
	bytesRead += 8;
	fin.read(addr(objCnt), 4);
	bytesRead += 4;
	this->prod.resize(objCnt);
	u64 ptr = 0;
	u16 vecSize = 0;
	for (u32 i = 0; i < objCnt; i++) {
		//加入remap
		this->augGrammar.productions.emplace_back(addr(this->prod.at(i)));
		prodReaddress.emplace(bytesRead, addr(this->prod.at(i)));
		//objSize: 4byte
		fin.read(&junkBin, 4);
		bytesRead += 4;
		//SymbolNonTerminal* p_head: 8byte
		fin.read(&ptr, 8);
		bytesRead += 8;
		this->prod.at(i).p_head = nonTerminalReaddress.find(ptr)->second;
		//vector<Symbol*> body: size * 8byte
		fin.read(&vecSize, 2);
		bytesRead += 2;
		for (; vecSize > 0; vecSize--) {
			//读取ptr并且重定向
			fin.read(&ptr, 8);
			bytesRead += 8;
			cit_ter = terminalReaddress.find(ptr);
			if (cit_ter == terminalReaddress.cend()) {
				this->prod.at(i).body.emplace_back(nonTerminalReaddress.find(ptr)->second);
			} else {
				this->prod.at(i).body.emplace_back(cit_ter->second);
			}
		}
		//vector<bool> reserveInAstTree: size * 1byte
		u8 boolVal = 0;
		fin.read(&vecSize, 2);
		bytesRead += 2;
		for (; vecSize > 0; vecSize--) {
			fin.read(&boolVal, 1);
			bytesRead += 1;
			this->prod.at(i).reserveInAstTree.emplace_back(boolVal);
		}
	}
	// * act
	fin.read(addr(regionSize), 8);
	bytesRead += 8;
	fin.read(addr(objCnt), 4);
	bytesRead += 4;
	this->act.resize(objCnt);
	u8 actType = 0;
	u32 shiftToState = 0;
	ptr = 0;
	for (u32 i = 0; i < objCnt; i++) {
		//加入remap
		this->parsingTable.allAction.emplace_back(addr(this->act.at(i)));
		actReaddress.emplace(bytesRead, addr(this->act.at(i)));
		//objSize: 4byte
		fin.read(&junkBin, 4);
		bytesRead += 4;
		//ActionType actType: 1byte
		fin.read(&actType, 1);
		bytesRead += 1;
		this->act.at(i).actType = (LALRAction::ActionType)actType;
		//u32 shiftToState: 4byte
		fin.read(&shiftToState, 4);
		bytesRead += 4;
		this->act.at(i).shiftToState = shiftToState;
		//Production* p_reduceWithProd: 8byte
		fin.read(&ptr, 8);
		bytesRead += 8;
		if (ptr != 0) {
			this->act.at(i).p_reduceWithProd = prodReaddress.find(ptr)->second;
		} else {
			this->act.at(i).p_reduceWithProd = nullptr;
		}
	}
	// * map array
	//格式: 4byte mapCnt, map, map, map...
	//map格式: 4byte mapPairCnt, 8byte Symbol*, 8byte Action*
	map<u64, SymbolEndMarker*>::const_iterator cit_terEM;
	Symbol* p_symbol = nullptr;
	LALRAction* p_act = nullptr;
	u32 mapCnt = 0;
	u32 mapPairCnt = 0;
	fin.read(&mapCnt, 4);
	this->parsingTable.actionMapTable.resize(mapCnt);
	for (u32 i = 0; i < mapCnt; i++) {
		fin.read(&mapPairCnt, 4);
		for (; mapPairCnt > 0; mapPairCnt--) {
			//读取两个ptr然后重定向
			fin.read(&ptr, 8);
			cit_ter = terminalReaddress.find(ptr);
			if (cit_ter == terminalReaddress.cend()) {
				cit_nonTer = nonTerminalReaddress.find(ptr);
				if (cit_nonTer == nonTerminalReaddress.cend()) {
					p_symbol = terminalEndMarkerReaddress.find(ptr)->second;
				} else {
					p_symbol = cit_nonTer->second;
				}
			} else {
				p_symbol = cit_ter->second;
			}
			//读取act ptr
			fin.read(&ptr, 8);
			p_act = actReaddress.find(ptr)->second;
			this->parsingTable.actionMapTable.at(i).emplace(p_symbol, p_act);
		}
	}

	this->augGrammar.p_lexer = addr(this->lexer);
	this->parsingTable.p_augGrammar = addr(this->augGrammar);
	this->parser.p_parsingTable = addr(this->parsingTable);

	fin.close();
}

void LingLan::Compile::CompileWrapper::DeserializedCompilePack::clear() {

	this->lexer = Lexer();
	this->augGrammar = AugGrammar();
	this->parsingTable = LALRParsingTable();
	this->parser = LALRParser();

	this->ter.clear();
	this->nonTer.clear();
	this->prod.clear();
	this->act.clear();
}

LingLan::Compile::CompileWrapper::DeserializedCompilePack::~DeserializedCompilePack() {
}

LingLan::Compile::CompileWrapper::SerializableCompilePack::~SerializableCompilePack() {
}
