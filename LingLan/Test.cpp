#include"Test.h"

#include"CompilePack.h"
using namespace LingLan::Compile::CompileWrapper;

void Test() {

	try {
		SerializableCompilePack spack;
		DeserializedCompilePack dpack;

		spack.initFromLanFile(u"C:\\Users\\suric\\Desktop\\Itera Doc.txt");
		spack.serialize((WCHAR*)L"c:\\work\\test dir\\brand new.bin");

		dpack.deserialize((WCHAR*)L"c:\\work\\test dir\\brand new.bin");

		vector<LingLan::Compile::Token*>* p_vec;
		vector<LingLan::Compile::Grammatical::LALRAction*>* p_ac;
		p_vec = dpack.lexer.lexicalAnalyze(u8"id=func(func(a*b+func()*c-v-func()*d/func(a)))+", 48 - 1);
		p_ac = dpack.parser.parse(p_vec);
	} catch (LingCommon::Exception::BaseException* p) {
		p->showMsg();
		del(p);
	}
}
