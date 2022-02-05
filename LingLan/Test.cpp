#include"Test.h"

#include"CompilePack.h"
#include"CompileCommon.h"

using namespace LingLan::Compile::CompileWrapper;
using namespace LingLan::Compile;
void Test() {

	try {
		SerializableCompilePack spack;
		DeserializedCompilePack dpack;

		//spack.initFromLanFile(u"C:\\Users\\suric\\Desktop\\languagerules.txt");
		//spack.serialize((WCHAR*)L"c:\\work\\test dir\\brand new.bin");

		dpack.deserialize((WCHAR*)L"c:\\work\\test dir\\brand new.bin");

		vector<LingLan::Compile::Token*>* p_vec;
		vector<LingLan::Compile::Grammatical::LALRAction*>* p_ac;
		p_vec = dpack.lexer.lexicalAnalyze(u8"a=f()+b(a+func(f(2)+w+fuc(1+w(2+f()))))+1", 42 - 1);
		p_ac = dpack.parser.parse(p_vec);

		//LanguageRulesFile rulesFile;
		//rulesFile.setFilePath(u"C:\\Users\\suric\\Desktop\\languagerules.txt");
		//rulesFile.format();
		//rulesFile.format();

	} catch (LingCommon::Exception::BaseException* p) {
		p->showMsg();
		del(p);
	}
}
