#include "StartupInfoCollector.h"

LingVM::Startup::StartupInfoCollector::StartupInfoCollector() {
}

void LingVM::Startup::StartupInfoCollector::collectMachineInfo() {

	GetSystemInfo(addr(this->machineInfo));
}

void LingVM::Startup::StartupInfoCollector::addVMArgument(u8string* p_name, u8string* p_value) {

	map<u8string*, set<u8string*, DereferenceLess<u8string*>>*, DereferenceLess<u8string*>>::const_iterator
		it;
	set<u8string*, DereferenceLess<u8string*>>* p_set = nullptr;
	//是否已经存在，不存在就加入
	it = this->vmArguments.find(p_name);
	if (it == this->vmArguments.cend()) {
		p_set = new set<u8string*, DereferenceLess<u8string*>>();
		this->vmArguments.emplace(
			new u8string(deref(p_name)),
			p_set
		);
	} else {
		p_set = it->second;
	}
	//加入
	if (p_set->find(p_value) == p_set->cend()) {
		p_set->emplace(new u8string(deref(p_value)));
	}
}

set<u8string*, DereferenceLess<u8string*>>* LingVM::Startup::StartupInfoCollector::getVMArgement(u8string* p_name) {

	map<u8string*, set<u8string*, DereferenceLess<u8string*>>*, DereferenceLess<u8string*>>::const_iterator
		it = this->vmArguments.find(p_name);
	if (it == this->vmArguments.cend()) {
		return nullptr;
	} else {
		return it->second;
	}
}

void LingVM::Startup::StartupInfoCollector::addVMArgument(const char8_t* p_name, const char8_t* p_value) {

	u8string* p_newName = new u8string(p_name);
	u8string* p_newValue = new u8string(p_value);
	this->addVMArgument(p_newName, p_newValue);
	del(p_newName);
	del(p_newValue);
}

set<u8string*, DereferenceLess<u8string*>>* LingVM::Startup::StartupInfoCollector::getVMArgement(const char8_t* p_name) {

	u8string* p_newName = new u8string(p_name);
	set<u8string*, DereferenceLess<u8string*>>* p_ret =
		this->getVMArgement(p_newName);
	del(p_newName);
	return p_ret;
}

void LingVM::Startup::StartupInfoCollector::scanBackendCompiler() {

	//得到环境变量backendcompilerpath
	set<u8string*, DereferenceLess<u8string*>>*
		p_set = this->getVMArgement(u8"backendcompilerpath");
	if (p_set == nullptr) {
		throw new Exception::NotFoundException(u8"not found");
	}
	set<u8string*, DereferenceLess<u8string*>>::const_iterator
		cit = p_set->cbegin();
	set<u8string*, DereferenceLess<u8string*>>::const_iterator
		cend = p_set->cend();
	File file;
	u16string tmpString = {};
	vector<File>* p_vec = nullptr;
	BackendCompilerWrapper* p_backendCompilerWrapper = nullptr;
	while (cit != cend) {
		UTFStringAffair::UTF8To16(deref(cit), addr(tmpString));
		file.setPath(tmpString);
		//如果这个arg是一个目录
		if (file.probeFileType() == File::Type::Dir) {
			p_vec = file.listFilesWithWildcards(u"*.dll");
			for (int i = 0; i < p_vec->size(); i++) {
				//循环目录中的所有dll
				p_backendCompilerWrapper = new BackendCompilerWrapper();
				try {
					p_backendCompilerWrapper->tryGetBackendCompilerFromDLL(addr(p_vec->at(i).getCurFilePath()));
					u8string* p_fileExt = p_backendCompilerWrapper->p_backendCompiler->getFileExtension();
					if (this->backendCompiler.find(p_fileExt) == this->backendCompiler.cend()) {
						this->backendCompiler.emplace(p_fileExt, p_backendCompilerWrapper);
					} else {
						del(p_fileExt);
						del(p_backendCompilerWrapper);
					}
				} catch (Exception::NotFoundException* p) {
					//没找到的话捕获异常直接跳过就好了
					del(p);
					del(p_backendCompilerWrapper);
				}
			}
		}
		cit++;
	}
}

LingVM::Startup::StartupInfoCollector::~StartupInfoCollector() {

	map<u8string*, set<u8string*, DereferenceLess<u8string*>>*>::const_iterator
		it = this->vmArguments.cbegin();
	map<u8string*, set<u8string*, DereferenceLess<u8string*>>*>::const_iterator
		cend = this->vmArguments.cend();

	set<u8string*, DereferenceLess<u8string*>>::const_iterator
		setit;
	set<u8string*, DereferenceLess<u8string*>>::const_iterator
		setcend;

	while (it != cend) {
		del(it->first);
		setit = it->second->cbegin();
		setcend = it->second->cend();
		while (setit != setcend) {
			del(deref(setit));
			setit++;
		}
		del(it->second);
		it++;
	}

	map<u8string*, BackendCompilerWrapper*, DereferenceLess<u8string*>>::const_iterator
		citit = this->backendCompiler.cbegin();
	map<u8string*, BackendCompilerWrapper*, DereferenceLess<u8string*>>::const_iterator
		cendend = this->backendCompiler.cend();

	while (citit != cendend) {
		del(citit->first);
		del(citit->second);
		citit++;
	}
}

void LingVM::Startup::BackendCompilerWrapper::setAllMemberNullptr() {

	this->getLingVMBackendCompiler = nullptr;
	this->destroyLingVMBackendCompiler = nullptr;
	this->p_backendCompiler = nullptr;
}

void LingVM::Startup::BackendCompilerWrapper::tryGetBackendCompilerFromDLL(const u8string* p_filePath) {
	
	if (p_filePath == nullptr) {
		throw new Exception::InvalidArgException(u8"no file path");
	}
	u16string str;
	UTFStringAffair::UTF8To16(p_filePath, addr(str));
	tryGetBackendCompilerFromDLL(addr(str));
}

void LingVM::Startup::BackendCompilerWrapper::tryGetBackendCompilerFromDLL(const u16string* p_filePath) {
	
	if (p_filePath == nullptr) {
		throw new Exception::InvalidArgException(u8"no file path");
	}
	tryGetBackendCompilerFromDLL(p_filePath->c_str());
}

void LingVM::Startup::BackendCompilerWrapper::tryGetBackendCompilerFromDLL(const char8_t* p_filePath) {
	
	if (p_filePath == nullptr) {
		throw new Exception::InvalidArgException(u8"no file path");
	}
	___DEL___
		char16_t* p_buffer = nullptr;
	u64 bufferSize = UTFStringAffair::UTF8To16(p_filePath, UTFStringAffair::UTF8StrLen(p_filePath, 0), nullptr, 0);
	p_buffer = new char16_t[bufferSize];
	UTFStringAffair::UTF8To16(p_filePath, UTFStringAffair::UTF8StrLen(p_filePath, 0), p_buffer, bufferSize);
	tryGetBackendCompilerFromDLL(p_buffer);
	dela(p_buffer);
}

void LingVM::Startup::BackendCompilerWrapper::tryGetBackendCompilerFromDLL(const char16_t* p_filePath) {
	
	if (p_filePath == nullptr) {
		throw new Exception::InvalidArgException(u8"no file path");
	}
	HMODULE dll = LoadLibraryW((LPCWSTR)p_filePath);
	if (dll == NULL) {
		throw new Exception::NotFoundException(u8"not found");
	}
	
	this->getLingVMBackendCompiler = 
		(FuncGetLingVMBackendCompiler)GetProcAddress(dll, (LPCSTR)u8"__GetLingVMBackendCompiler__");
	
	this->destroyLingVMBackendCompiler = 
		(FuncDestroyLingVMBackendCompiler)GetProcAddress(dll, (LPCSTR)u8"__DestroyLingVMBackendCompiler__");
	
	if (this->getLingVMBackendCompiler == nullptr || this->destroyLingVMBackendCompiler == nullptr) {
		this->setAllMemberNullptr();
		throw new Exception::NotFoundException(u8"not found");
	}
	this->p_backendCompiler = this->getLingVMBackendCompiler();
	if (p_backendCompiler == nullptr) {
		this->setAllMemberNullptr();
		throw new Exception::NotFoundException(u8"not found");
	}
}

LingVM::Startup::BackendCompilerWrapper::~BackendCompilerWrapper() {

	if (this->getLingVMBackendCompiler != nullptr 
		&& 
		this->destroyLingVMBackendCompiler != nullptr 
		&& 
		this->p_backendCompiler != nullptr) {
		
		this->destroyLingVMBackendCompiler(this->p_backendCompiler);
	}
}

LingVM::Startup::StartupException::StartupException(const char8_t* p_info) : Exception::BaseException(p_info) {

	this->className.assign(u8"StartupException");
}
