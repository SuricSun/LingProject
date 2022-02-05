#include"CompileCommon.h"

using namespace LingCommon::Util::IO;

const regex LingLan::Compile::LanguageRulesFile::SymbolNameRegex = regex((char*)u8"[a-zA-Z_]+[\\w_]*");
const u8string LingLan::Compile::LanguageRulesFile::DelimiterSymbolName = u8string(u8"__d__");
const u8string LingLan::Compile::LanguageRulesFile::AugSymbol = u8string(u8"__aug__");

void LingLan::Compile::LanguageRulesFile::handleTerminal(u8string* p_tmpStr, FileInputStreamReader* p_fir) {

	match_results<const char8_t*> match = {};
	const char8_t* p_beginCh = nullptr;
	const char8_t* p_endCh = nullptr;

	___DEL___
		u8string* p_newName = nullptr;
	___DEL___
		u8string* p_newBody = nullptr;

	//直接读取名字加上空格加上后面所有内容
	// * 读取名字
	//这里直接访问at(1)了，默认p_tmpStr最小为2个字符，上层调用者注意
	p_beginCh = addr(p_tmpStr->at(1));
	p_endCh = addr(p_tmpStr->at(p_tmpStr->size() - 1)) + 1;
	if (regex_search(p_beginCh, p_endCh, match, LanguageRulesFile::SymbolNameRegex) == false) {
		throw new LanguageRulesFileException(u8"A terminal head was not found");
	}
	//检查是否出现在开头
	if (match[0].first != p_beginCh) {
		throw new LanguageRulesFileException(u8"A terminal head name format is illegal");
	}
	//检查名字是否出现
	p_newName = new u8string(match[0].first, match[0].second);
	if (this->terminalMap.find(p_newName) != this->terminalMap.cend()) {
		del(p_newName);
		throw new LanguageRulesFileException(u8"A terminal head name is duplicated");
	}
	//检查空格
	p_beginCh = match[0].second;
	if (p_beginCh >= p_endCh) {
		del(p_newName);
		throw new LanguageRulesFileException(u8"A terminal body is ignored");
	}
	if (deref(p_beginCh) != ' ') {
		del(p_newName);
		throw new LanguageRulesFileException(u8"A terminal body format is wrong");
	}
	// * 把body加入map
	p_beginCh++;
	if (p_beginCh >= p_endCh) {
		del(p_newName);
		throw new LanguageRulesFileException(u8"A terminal body is ignored");
	}
	p_newBody = new u8string(p_beginCh, p_endCh);
	this->terminalMap.emplace(p_newName, p_newBody);
	//加入vec里面，因为terminal有顺序要求
	this->terminalVec.emplace_back(p_newName);
}

void LingLan::Compile::LanguageRulesFile::handleNonTerminal(u8string* p_tmpStr, FileInputStreamReader* p_fir) {

	match_results<const char8_t*> match = {};
	const char8_t* p_beginCh = nullptr;
	const char8_t* p_endCh = nullptr;

	___DEL___
		u8string* p_newName = nullptr;

	//读取第一行的名字，然后一直读取下面的Continue部分
	// * 读取NonTerminal名字
	//这里直接访问at(1)了，默认p_tmpStr最小为2个字符，上层调用者注意
	p_beginCh = addr(p_tmpStr->at(1));
	p_endCh = addr(p_tmpStr->at(p_tmpStr->size() - 1)) + 1;
	if (regex_search(p_beginCh, p_endCh, match, LanguageRulesFile::SymbolNameRegex) == false) {
		throw new LanguageRulesFileException(u8"A non terminal name is ignored");
	}
	if (match[0].first != p_beginCh) {
		throw new LanguageRulesFileException(u8"A non terminal name format is wrong");
	}
	// * 检查是否存在这个名字
	p_newName = new u8string(match[0].first, match[0].second);
	if (this->nonTerminalMap.find(p_newName) != this->nonTerminalMap.cend()) {
		del(p_newName);
		throw new LanguageRulesFileException(u8"A non terminal name is duplicated");
	}
	// * 加入这个名字
	set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
	>* p_bodySet = new set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
	>();
	this->nonTerminalMap.emplace(p_newName, p_bodySet);
	// * 检查下面的Continue部分
	//一直循环直到读取到出现NonTerminalStart或者TerminalStart
	___DEL___
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>* p_newVec;
	while (p_fir->isEOF() == false) {
		p_fir->readLine(p_tmpStr);
		if (p_tmpStr->size() >= 1) {
			if (p_tmpStr->at(0) == LanguageRulesFile::TerminalStart
				|| p_tmpStr->at(0) == LanguageRulesFile::NonTerminalStart) {

				//跳出
				break;
			} else if (p_tmpStr->at(0) == LanguageRulesFile::NonTerminalContinue) {
				//继续读取新的body
				p_newVec = new Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>();

				if (p_tmpStr->size() == 1) {
					//derive blank
					//直接加入
				} else {
					//从当前的tmpStr里面读取所有的符号
					p_beginCh = addr(p_tmpStr->at(1));
					p_endCh = addr(p_tmpStr->at(p_tmpStr->size() - 1)) + 1;
					//开始循环match
					//流程：match名字，如果到了结尾跳出，match空格
					while (true) {
						//match name
						if (regex_search(p_beginCh, p_endCh, match, LanguageRulesFile::SymbolNameRegex) == false) {
							throw new LanguageRulesFileException(u8"A non terminal body has wrong format");
						}
						if (match[0].first != p_beginCh) {
							throw new LanguageRulesFileException(u8"A non terminal body has wrong format");
						}
						//加入body vector
						p_newVec->getInnerVector()->push_back(new u8string(match[0].first, match[0].second));
						p_beginCh = match[0].second;
						//check end of str
						if (p_beginCh >= p_endCh) {
							break;
						}
						//match space char
						if (deref(p_beginCh) != ' ') {
							throw new LanguageRulesFileException(u8"A non terminal body has wrong format");
						}
						p_beginCh++;
					}
				}

				//把body加入body set
				//遍历当前set是否有已经相等的vector存在
				
				if (p_bodySet->emplace(p_newVec).second == false) {
					//已经存在同name同body的情况
					throw new LanguageRulesFileException(u8"A non terminal has multiple same body");
				}
			}
		}
	}
}

LingLan::Compile::LanguageRulesFile::LanguageRulesFile() {
}

void LingLan::Compile::LanguageRulesFile::setFilePath(const char16_t* p_path) {

	this->filePath.assign(p_path);
}

void LingLan::Compile::LanguageRulesFile::setFilePath(const u16string& path) {

	this->filePath = path;
}

void LingLan::Compile::LanguageRulesFile::format() {

	this->clear();

	FileInputStreamReader fir;
	fir.open((WCHAR*)this->filePath.c_str());

	u8string tmpStr = {};

	fir.readLine(&tmpStr);
	while (fir.isEOF() == false) {
		//大小必须大于等于2（一个头符号和实际内容）
		if (tmpStr.size() >= 2) {
			if (tmpStr[0] == LanguageRulesFile::TerminalStart) {
				//如果是terminal
				this->handleTerminal(addr(tmpStr), addr(fir));
				fir.readLine(addr(tmpStr));

			} else if (tmpStr[0] == LanguageRulesFile::NonTerminalStart) {
				//如果是NonTerminal
				this->handleNonTerminal(addr(tmpStr), addr(fir));
				//fir.readLine(addr(tmpStr)); 不需要，handleNonTerminal函数里面已经调用过了
			} else {
				//否则就是注释空行什么的
				fir.readLine(addr(tmpStr));
			}
		}
	}

	//检擦是否是augGrammar
	map<
		u8string*,
		set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
		>*,
		DereferenceLess<u8string*>
	>::const_iterator cit;

	cit = this->nonTerminalMap.find((u8string*)addr(LanguageRulesFile::AugSymbol));

	if (cit == this->nonTerminalMap.cend()) {
		throw new LanguageRulesFileException(u8"non terminal __aug__ is not defined");
	}

	if (cit->second->size() != 1) {
		throw new LanguageRulesFileException(u8"non terminal __aug__ can only has one and only one body");
	}
}

void LingLan::Compile::LanguageRulesFile::clear() {

	map<u8string*, u8string*, DereferenceLess<u8string*>>::const_iterator cit_terName2Str = this->terminalMap.cbegin();
	map<u8string*, u8string*, DereferenceLess<u8string*>>::const_iterator cend_terName2Str = this->terminalMap.cend();

	while (cit_terName2Str != cend_terName2Str) {
		del(cit_terName2Str->first);
		del(cit_terName2Str->second);
		cit_terName2Str++;
	}

	this->terminalMap.clear();

	map<
		u8string*,
		set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
		>*,
		DereferenceLess<u8string*>
	>::const_iterator cit_nonTerName2Set = this->nonTerminalMap.cbegin();
	map<
		u8string*,
		set<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
		>*,
		DereferenceLess<u8string*>
	>::const_iterator cend_nonTerName2Set = this->nonTerminalMap.cend();

	set <
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
	>::const_iterator cit_vec;
	set <
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*,
		DereferenceLess<
		Ext_Vector<u8string*, DereferenceLess<u8string*>, SafeDeleteLooper<u8string*>>*
		>
	>::const_iterator cend_set;

	while (cit_nonTerName2Set != cend_nonTerName2Set) {
		del(cit_nonTerName2Set->first);
		//删除set里面的所有vec里面的所有string
		cit_vec = cit_nonTerName2Set->second->cbegin();
		cend_set = cit_nonTerName2Set->second->cend();
		while (cit_vec != cend_set) {
			deref(cit_vec)->loop();
			del(deref(cit_vec));
			cit_vec++;
		}
		del(cit_nonTerName2Set->second);

		cit_nonTerName2Set++;
	}

	this->nonTerminalMap.clear();
}

LingLan::Compile::LanguageRulesFile::~LanguageRulesFile() {

	this->clear();
}

LingLan::Compile::LanguageRulesFileException::LanguageRulesFileException(const char8_t* p_info) : Exception::BaseException(p_info) {

	this->className.assign(u8"LanguageRulesFileException");
}

LingLan::Compile::LexerException::LexerException(const char8_t* p_info) : Exception::BaseException(p_info) {

	this->className.assign(u8"LexerException");
}

LingLan::Compile::LALRParserException::LALRParserException(const char8_t* p_info) : Exception::BaseException(p_info) {

	this->className.assign(u8"LALRParserException");
}
