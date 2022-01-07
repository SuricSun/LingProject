#include"CompileCommon.h"
#include"..\LingCommon\FileStream.h"

using namespace LingCommon::Util::IO;

const regex LingLan::Compile::LanguageRulesFile::TokenNameRegex = regex((char*)u8"[a-zA-Z_]+[\\w_]*");
const u8string LingLan::Compile::LanguageRulesFile::DelimiterTokenName = u8string(u8"__d__");

LingLan::Compile::LanguageRulesFile::LanguageRulesFile() {
}

void LingLan::Compile::LanguageRulesFile::setFilePath(const char16_t* p_path) {

	this->filePath.assign(p_path);
}

void LingLan::Compile::LanguageRulesFile::setFilePath(const u16string& path) {

	this->filePath = path;
}

void LingLan::Compile::LanguageRulesFile::format() {

	FileInputStreamReader fir;
	fir.open((WCHAR*)this->filePath.c_str());

	u8string tmpStr = {};
	while (fir.isEOF() == false) {
		fir.readLine(&tmpStr);
		if (tmpStr.size() > 0) {
			if (tmpStr[0] == LanguageRulesFile::TokenStart) {
				tmpStr[0] = (u8)(tmpStr.size() - 1);
				this->formattedSymbolTerminalDef.append(tmpStr);
			} else if (tmpStr[0] == LanguageRulesFile::ProdStart) {
				tmpStr[0] = (u8)(tmpStr.size() - 1);
				this->formattedProductionDef.append(tmpStr);
			}
		}
	}
}

u8string* LingLan::Compile::LanguageRulesFile::getFormattedSymbolTerminalDef() {
	return &(this->formattedSymbolTerminalDef);
}

u8string* LingLan::Compile::LanguageRulesFile::getFormattedProductionDef() {
	return &(this->formattedProductionDef);
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
