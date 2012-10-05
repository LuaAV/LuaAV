#include "system/al_Compiler.hpp"

//#include "clang/Checker/BugReporter/PathDiagnostic.h"
#include "clang/AST/ASTContext.h"
//#include "clang/AST/Decl.h"
#include "clang/Basic/Builtins.h"
//#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TargetInfo.h"
//#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/SourceManager.h"
//#include "clang/Basic/FileManager.h"
//#include "clang/CodeGen/CodeGenOptions.h"
#include "clang/CodeGen/ModuleBuilder.h"
//#include "clang/Driver/Driver.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Frontend/FrontendOptions.h"
//#include "clang/Frontend/HeaderSearchOptions.h"
//#include "clang/Frontend/PreprocessorOptions.h"
#include "clang/Frontend/Utils.h"
//#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
//#include "clang/Sema/CodeCompleteConsumer.h"
// lua_upvalueindex
//#include "llvm/ADT/OwningPtr.h"
//#include "llvm/ADT/SmallPtrSet.h"
//#include "llvm/ADT/SmallString.h"
//#include "llvm/ADT/StringExtras.h"
#include "llvm/Analysis/Verifier.h"
//#include "llvm/Bitcode/ReaderWriter.h"
//#include "llvm/Analysis/Verifier.h"
//#include "llvm/Config/config.h"
//#include "llvm/DerivedTypes.h"
//#include "llvm/ExecutionEngine/ExecutionEngine.h"
//#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/JITEventListener.h"
#include "llvm/Linker.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
//#include "llvm/Support/Allocator.h"
#include "llvm/Support/ErrorHandling.h"
//#include "llvm/Support/IRBuilder.h"
//#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
//#include "llvm/Support/PassNameParser.h"
//#include "llvm/Support/PrettyStackTrace.h"
//#include "llvm/Support/raw_ostream.h"
//#include "llvm/System/DynamicLibrary.h"
//#include "llvm/System/Host.h"
//#include "llvm/System/Path.h"
//#include "llvm/System/Signals.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Type.h"
//#include "llvm/ValueSymbolTable.h"

#include <stdio.h>

using namespace clang;

struct ForceJITLinking forcejitlinking;
static llvm::ExecutionEngine * EE = 0;
static llvm::LLVMContext llvmContext;

class JITListener : public llvm::JITEventListener {
public:
	JITListener() : llvm::JITEventListener() {}
	virtual ~JITListener() {}
	virtual void NotifyFunctionEmitted(const llvm::Function &F,
										 void *Code, size_t Size,
										 const llvm::JITEvent_EmittedFunctionDetails &Details) {
		printf("JIT emitted Function %s at %p, size %d\n", 
			F.getName().data(), Code, (int)Size);
	}
	virtual void NotifyFreeingMachineCode(void *OldPtr) {
		printf("JIT freed %p\n", OldPtr);
	}
};
static JITListener gJITEventListener;
static int verboseJIT = false;

static void llvmErrorHandler(void * user_data, const std::string& reason) {
	printf("llvm error %s\n", reason.data());
}

static void llvmInit() {
	static bool initialized = false;
	if (!initialized) {
		llvm::InitializeNativeTarget();
		llvm::InitializeNativeTargetAsmPrinter();
		//llvm::InitializeAllTargets();
		//llvm::InitializeAllAsmPrinters();
		//llvm::InitializeAllAsmParsers();
		//llvm::install_fatal_error_handler(llvmErrorHandler, NULL);
		initialized = true;
	}
}


namespace al {

class ModuleImpl {
public:
	llvm::Module * module;
	std::string name;
	//llvm::LLVMContext llvmContext;
	
	ModuleImpl(std::string name) : module(NULL), name(name) {}
	
	bool valid() { return module != NULL; }
	
	bool link(llvm::Module * child) {
		std::string err;
		if (module == NULL) {
			module = child;
		} else {
			llvm::Linker::LinkModules(module, child, &err);
		}
		if (err.length()) {
			printf("link error %s\n", err.data());
			return false;
		}
		return true;
	}
};

///! Use this to implement garbage collection on JITs.
//class JITGCList {
//public:
//	///! add a JIT to the list of garbage collected JITs
//	void add(JIT * j) { if (j) { mList.push_front(j); } }
//	///! trigger the unjit() of any unreferenced (collectible) JITs in the list
//	/// call periodically!
//	void sweep();
//protected:
//	std::list<JIT *> mList;
//};

std::list<JIT *> JITGCList;

Compiler::Compiler() : mImpl(NULL) { llvmInit(); }

Compiler::~Compiler() {
	clear();
}

void Compiler :: clear() {
	if (mImpl) {
		if (mImpl->module)
			delete mImpl->module;
		delete mImpl;
		mImpl = NULL;
	}
}

bool Compiler :: compile(std::string code, std::string name) {
	if (!mImpl) mImpl = new ModuleImpl(name);
	
	#ifdef AL_OSX
	static std::string virtual_include_path("/usr/local/include/");
	#else
	static std::string virtual_include_path("/usr/include/");
	#endif
	
	llvm::MemoryBuffer * buffer = llvm::MemoryBuffer::getMemBufferCopy(code, "src");
	
	CompilerInstance CI;
	CI.createDiagnostics(0, NULL);
	Diagnostic & Diags = CI.getDiagnostics();	
	TextDiagnosticBuffer * client = new TextDiagnosticBuffer;
	Diags.setClient(client);
	CompilerInvocation::CreateFromArgs(CI.getInvocation(), NULL, NULL, Diags);
	
	LangOptions& lang = CI.getInvocation().getLangOpts();
	// The fateful line
	lang.CPlusPlus = options.CPlusPlus;
	lang.Bool = 1;
	lang.BCPLComment = 1;
	lang.RTTI = 0;
	lang.PICLevel = 1;
	lang.InlineVisibilityHidden = 1;

	CI.createSourceManager();
	CI.getSourceManager().createMainFileIDForMemBuffer(buffer);
	CI.createFileManager();
	
	// Create the target instance.
	CI.setTarget(TargetInfo::CreateTargetInfo(CI.getDiagnostics(), CI.getTargetOpts()));
//	TargetOptions targetopts = Compiler.getTargetOpts();
//	printf("triple %s\n", targetopts.Triple.data());
//	printf("CPU %s\n", targetopts.CPU.data());
//	printf("ABI %s\n", targetopts.ABI.data());
//	for (int i=0; i<targetopts.Features.size(); i++)
//		printf("Feature %s\n", targetopts.Features[i].data());
	
	// add header file remappings:
	for (unsigned int i=0; i<options.headers.size(); i++) {
		llvm::StringRef remapped_name(virtual_include_path + options.headers[i].first);
		llvm::StringRef remapped_data(options.headers[i].second);
		llvm::MemoryBuffer * remapped_buffer = llvm::MemoryBuffer::getMemBufferCopy(remapped_data, llvm::StringRef(options.headers[i].first));
		CI.getPreprocessorOpts().addRemappedFile(remapped_name, remapped_buffer);
	}
	
	// Defines:
	std::string defines;
	for (unsigned int i=0; i<options.defines.size(); i++) {
		//defines += options.defines[i];
		CI.getPreprocessorOpts().addMacroDef(llvm::StringRef(options.defines[i]));
	}
	CI.createPreprocessor();
	Preprocessor &PP = CI.getPreprocessor();
	

	// Header paths:
	HeaderSearchOptions& headeropts = CI.getHeaderSearchOpts();
	headeropts.AddPath(virtual_include_path, clang::frontend::Quoted, true, false, false);
	for (unsigned int i=0; i<options.system_includes.size(); i++) {
		headeropts.AddPath(options.system_includes[i], clang::frontend::Angled, true, false, false/* true ? */);
	}
	for (unsigned int i=0; i<options.user_includes.size(); i++) {
		headeropts.AddPath(options.user_includes[i], clang::frontend::Quoted, true, false, false/* true ? */);
	}
	ApplyHeaderSearchOptions(PP.getHeaderSearchInfo(), headeropts, lang, CI.getTarget().getTriple());
	
	
	
//	//	// list standard invocation args:
//	std::vector<std::string> Args;
//	CI.getInvocation().toArgs(Args);
//	for (int i=0; i<Args.size(); i++) {
//		printf("%d %s\n", i, Args[i].data());
//	}
	
	PP.getBuiltinInfo().InitializeBuiltins(PP.getIdentifierTable(), PP.getLangOptions().NoBuiltin);
			
	
	CI.createASTContext();
//	llvm::SmallVector<const char *, 32> BuiltinNames;
//	printf("NoBuiltins: %d\n", Compiler.getASTContext().getLangOptions().NoBuiltin);
//	Compiler.getASTContext().BuiltinInfo.GetBuiltinNames(BuiltinNames, Compiler.getASTContext().getLangOptions().NoBuiltin);
//	for (int i = 0; i<BuiltinNames.size(); i++) {
//		printf("Builtin %s\n", BuiltinNames[i]);
//	}
	
			
	CodeGenOptions CGO;
//	  AsmVerbose = 0;
//    CXAAtExit = 1;
//    CXXCtorDtorAliases = 0;
//    DataSections = 0;
//    DebugInfo = 0;
//    DisableFPElim = 0;
//    DisableLLVMOpts = 0;
//    DisableRedZone = 0;
//    EmitDeclMetadata = 0;
//    FunctionSections = 0;
//    HiddenWeakTemplateVTables = 0;
//    HiddenWeakVTables = 0;
//    InstrumentFunctions = 0;
//    MergeAllConstants = 1;
//    NoCommon = 0;
//    NoImplicitFloat = 0;
//    NoZeroInitializedInBSS = 0;
//    ObjCDispatchMethod = Legacy;
//    OmitLeafFramePointer = 0;
//    OptimizationLevel = 0;
//    OptimizeSize = 0;
//    RelaxAll = 0;
//    SimplifyLibCalls = 1;
//    SoftFloat = 0;
//    TimePasses = 0;
//    UnitAtATime = 1;
//    UnrollLoops = 0;
//    UnwindTables = 0;
//    VerifyModule = 1;
//
//    Inlining = NoInlining;
//    RelocationModel = "pic";
	CGO.CXAAtExit = 0;
//	CGO.VerifyModule = 0;	// is it necessary?
	CodeGenerator * codegen = CreateLLVMCodeGen(Diags, "mymodule", CGO, /*mImpl->*/llvmContext );
	
	ParseAST(CI.getPreprocessor(),
			codegen,
			CI.getASTContext(),
			/* PrintState= */ false,
			true,
			0);
	
	llvm::Module* module = codegen->ReleaseModule();
	
	
	if (module) {
		delete codegen;
		mImpl->link(module);
		return true;
	}
	
	printf("compile errors\n");
	
	int ecount = 0;
	
	for(TextDiagnosticBuffer::const_iterator it = client->err_begin();
		it != client->err_end();
		++it)
	{
		FullSourceLoc SourceLoc(it->first, CI.getSourceManager());
		int LineNum = SourceLoc.getInstantiationLineNumber();
		int ColNum = SourceLoc.getInstantiationColumnNumber();
		//const char * bufname = SourceLoc.getManager().getBufferName(SourceLoc);
		const char * bufname = name.c_str();
		printf("error %s line %d col %d: %s\n", bufname, LineNum, ColNum, it->second.data());
		
		
		int FileOffset = SourceLoc.getManager().getFileOffset(SourceLoc);

		//std::pair< const char *, const char * > LocBD = SourceLoc.getBufferData();
		llvm::StringRef buf = SourceLoc.getBufferData();
; 
		
		char errstr[128];
		int start = (FileOffset < 10) ? 0 : FileOffset-10;
		strncpy(errstr, buf.data()+start, 10);
		errstr[10] = '^';
		strncpy(errstr+11, buf.data()+start+10, 10);
		errstr[21] = '\0';			

		printf("%s %d:%d\n'%s'\n%s\n", 
							SourceLoc.getManager().getBufferName(SourceLoc),
							LineNum, ColNum, 
							errstr,
							it->second.data());
	
		ecount++;
		if(ecount > 250) break;
	}
	
	delete codegen;
	return false;
}

bool Compiler :: readbitcode(std::string path) {
	//if (!mImpl) mImpl = new ModuleImpl; //etc.
	printf("readbitcode: not yet implemented\n");
	return true;
}

void Compiler :: dump() {
	if (mImpl) mImpl->module->dump();
}

std::string Compiler :: getDataLayout() {
	return mImpl ? mImpl->module->getDataLayout() : "?";
}

std::string Compiler :: getTargetTriple() {
	return mImpl ? mImpl->module->getTargetTriple() : "?";
}


/// GetX86CpuIDAndInfo - Execute the specified cpuid and return the 4 values in the
/// specified arguments.  If we can't run cpuid on the host, return false.
static bool GetX86CpuIDAndInfo(unsigned value, unsigned *rEAX,
                               unsigned *rEBX, unsigned *rECX, unsigned *rEDX) {
#if defined(__x86_64__) || defined(_M_AMD64) || defined (_M_X64)
#if defined(__GNUC__)
    // gcc doesn't know cpuid would clobber ebx/rbx. Preseve it manually.
    asm ("movq\t%%rbx, %%rsi\n\t"
         "cpuid\n\t"
         "xchgq\t%%rbx, %%rsi\n\t"
         : "=a" (*rEAX),
         "=S" (*rEBX),
         "=c" (*rECX),
         "=d" (*rEDX)
         :  "a" (value));
    return true;
#elif defined(_MSC_VER)
    int registers[4];
    __cpuid(registers, value);
    *rEAX = registers[0];
    *rEBX = registers[1];
    *rECX = registers[2];
    *rEDX = registers[3];
    return true;
#endif
#elif defined(i386) || defined(__i386__) || defined(__x86__) || defined(_M_IX86)
#if defined(__GNUC__)
    asm ("movl\t%%ebx, %%esi\n\t"
         "cpuid\n\t"
         "xchgl\t%%ebx, %%esi\n\t"
         : "=a" (*rEAX),
         "=S" (*rEBX),
         "=c" (*rECX),
         "=d" (*rEDX)
         :  "a" (value));
    return true;
#elif defined(_MSC_VER)
    __asm {
        mov   eax,value
        cpuid
        mov   esi,rEAX
        mov   dword ptr [esi],eax
        mov   esi,rEBX
        mov   dword ptr [esi],ebx
        mov   esi,rECX
        mov   dword ptr [esi],ecx
        mov   esi,rEDX
        mov   dword ptr [esi],edx
    }
    return true;
#endif
#endif
    return false;
}

static void DetectX86FamilyModel(unsigned EAX, unsigned *o_Family, unsigned *o_Model) {
    unsigned Family = (EAX >> 8) & 0xf; // Bits 8 - 11
    unsigned Model  = (EAX >> 4) & 0xf; // Bits 4 - 7
    if (Family == 6 || Family == 0xf) {
        if (Family == 0xf)
            // Examine extended family ID if family ID is F.
            Family += (EAX >> 20) & 0xff;    // Bits 20 - 27
        // Examine extended model ID if family ID is 6 or F.
        Model += ((EAX >> 16) & 0xf) << 4; // Bits 16 - 19
    }
    
    *o_Family = Family;
    *o_Model = Model;
}

bool is_sandy_bridge() {
    unsigned EAX = 0, EBX = 0, ECX = 0, EDX = 0;
    if(!GetX86CpuIDAndInfo(0x1, &EAX, &EBX, &ECX, &EDX)) return false;
    
    unsigned Family = 0;
    unsigned Model  = 0;
    DetectX86FamilyModel(EAX, &Family, &Model);
    
    return Family == 6 && Model == 42;
}

JIT * Compiler :: jit() {
	std::string err;
	if (mImpl) {
		if (EE==0) {
		
			if (is_sandy_bridge()) {
				EE = llvm::EngineBuilder(mImpl->module)
				.setEngineKind(llvm::EngineKind::JIT)
				.setErrorStr(&err)
				.setOptLevel(llvm::CodeGenOpt::Default)
				//.setMAttrs("-avx")
				.setMCPU("core2")
				.create();
			} else {
				EE = llvm::ExecutionEngine::createJIT(
					mImpl->module,	
					&err,		// error string
					0,			// JITMemoryManager
					llvm::CodeGenOpt::Default,	// JIT slowly (None, Less, Default, Aggressive)
					false		// allocate GlobalVariables separately from code
				);
			}
			
			if (EE == 0) {
				printf("Failed to create Execution Engine: %s", err.data());
				return 0;
			}
			
			// turn this off when not debugging:
			if (verboseJIT) EE->RegisterJITEventListener(&gJITEventListener);
			//EE->InstallLazyFunctionCreator(lazyfunctioncreator);
			//EE->DisableGVCompilation();
		
		} else {
			EE->addModule(mImpl->module);
		}
		
		// initialize the statics:
		EE->runStaticConstructorsDestructors(mImpl->module, false);
		
		// create JIT and transfer ownership of module to it:
		JIT * jit = new JIT;
		jit->mImpl = mImpl;
		// for garbage collection:
		JITGCList.push_back(jit);
		
		mImpl = NULL;
		return jit;
	}
	return NULL;
}

JIT::JIT() : mRefs(0) {}

void JIT :: unjit() {
	//printf("unjit %p\n", this);
	
	if (valid() && mImpl->valid()) {
		// TODO: why does this not work???
		//EE->runStaticConstructorsDestructors(mImpl->module, true);
		
		// apparently not necessary:
//		llvm::Module::FunctionListType & flist = mImpl->module->getFunctionList();
//		for (llvm::Module::FunctionListType::iterator iter= flist.begin(); iter != flist.end(); iter++) {
//			//printf("function %s %d\n", iter->getName().data(), iter->isIntrinsic());
//			EE->freeMachineCodeForFunction(iter);
//		}	
//		llvm::Module::GlobalListType & glist = mImpl->module->getGlobalList();
//		for (llvm::Module::GlobalListType::iterator iter= glist.begin(); iter != glist.end(); iter++) {
//			printf("global %s %d\n", iter->getName().data(), iter->isDeclaration());
//			//EE->freeMachineCodeForFunction(iter);
//			EE->updateGlobalMapping(iter, NULL);
//		}
		
		EE->removeModule(mImpl->module);	
		delete mImpl->module;
		delete mImpl;
		mImpl = 0;
	}
}


void JIT :: dump() {
	if (valid()) mImpl->module->dump();
}

void * JIT :: getfunctionptr(std::string funcname) {
	if (valid()) {
		llvm::StringRef fname = llvm::StringRef(funcname);
		llvm::Function * f = mImpl->module->getFunction(fname);
		if (f) {
			return EE->getPointerToFunction(f);
		}
	}
	return NULL;
}
void * JIT :: getglobalptr(std::string globalname) {
	if (valid()) {
		const llvm::GlobalVariable * GV = mImpl->module->getGlobalVariable(globalname);
		if (GV) {
			return EE->getOrEmitGlobalVariable(GV);
		} else {
			printf("global %s not found\n", globalname.data());
		}
	}
	return NULL;
}

void JIT :: sweep() {
	std::list<JIT *>::iterator it = JITGCList.begin();
	while (it != JITGCList.end()) {
		JIT * j = *it;
		if (j == NULL) {
			it = JITGCList.erase(it);
		} else if (j->valid()) {
			if (j->refs() <= 0) {
				it = JITGCList.erase(it);
				// TODO: uncomment this and fix whatever bugs it reveals:
				delete j;
			} else {
				it++;
			}
		} else {
			it++;
		}
	}
}

void JIT :: verbose(bool v) { verboseJIT = v; }

JitObject :: JitObject(JIT * jit) : mJIT(jit) { 
	mJIT->retain(); 
}
JitObject :: ~JitObject() { 
	//printf("~JitObject %p %d\n", this, mJIT->refs());
	mJIT->release();
}

bool Compiler :: writebitcode(std::string path) {
	printf("writebitcode not yet enabled\n");
	return true;
}	

void Compiler :: optimize(std::string olevel) {
	if (mImpl == 0) return;
	
	llvm::TargetData * targetdata = new llvm::TargetData(mImpl->module);
	llvm::PassManager pm;
	pm.add(targetdata);
	
	if (olevel == std::string("O1")) {
		pm.add(llvm::createPromoteMemoryToRegisterPass());
		pm.add(llvm::createInstructionCombiningPass());
		pm.add(llvm::createCFGSimplificationPass());
		pm.add(llvm::createVerifierPass(llvm::PrintMessageAction));
	} else if (olevel == std::string("O3")) {
		pm.add(llvm::createCFGSimplificationPass());
		pm.add(llvm::createScalarReplAggregatesPass());
		pm.add(llvm::createInstructionCombiningPass());
		//pm.add(llvm::createRaiseAllocationsPass());   // call %malloc -> malloc inst
		pm.add(llvm::createCFGSimplificationPass());       // Clean up disgusting code
		pm.add(llvm::createPromoteMemoryToRegisterPass()); // Kill useless allocas
		pm.add(llvm::createGlobalOptimizerPass());       // OptLevel out global vars
		pm.add(llvm::createGlobalDCEPass());          // Remove unused fns and globs
		pm.add(llvm::createIPConstantPropagationPass()); // IP Constant Propagation
		pm.add(llvm::createDeadArgEliminationPass());   // Dead argument elimination
		pm.add(llvm::createInstructionCombiningPass());   // Clean up after IPCP & DAE
		pm.add(llvm::createCFGSimplificationPass());      // Clean up after IPCP & DAE
		pm.add(llvm::createPruneEHPass());               // Remove dead EH info
		pm.add(llvm::createFunctionAttrsPass());         // Deduce function attrs
		pm.add(llvm::createFunctionInliningPass());      // Inline small functions
		pm.add(llvm::createArgumentPromotionPass());  // Scalarize uninlined fn args
		pm.add(llvm::createSimplifyLibCallsPass());    // Library Call Optimizations
		pm.add(llvm::createInstructionCombiningPass());  // Cleanup for scalarrepl.
		pm.add(llvm::createJumpThreadingPass());         // Thread jumps.
		pm.add(llvm::createCFGSimplificationPass());     // Merge & remove BBs
		pm.add(llvm::createScalarReplAggregatesPass());  // Break up aggregate allocas
		pm.add(llvm::createInstructionCombiningPass());  // Combine silly seq's
		//pm.add(llvm::createCondPropagationPass());       // Propagate conditionals
		pm.add(llvm::createTailCallEliminationPass());   // Eliminate tail calls
		pm.add(llvm::createCFGSimplificationPass());     // Merge & remove BBs
		pm.add(llvm::createReassociatePass());           // Reassociate expressions
		pm.add(llvm::createLoopRotatePass());            // Rotate Loop
		pm.add(llvm::createLICMPass());                  // Hoist loop invariants
		pm.add(llvm::createLoopUnswitchPass());
		pm.add(llvm::createLoopIndexSplitPass());        // Split loop index
		pm.add(llvm::createInstructionCombiningPass());
		pm.add(llvm::createIndVarSimplifyPass());        // Canonicalize indvars
		pm.add(llvm::createLoopDeletionPass());          // Delete dead loops
		pm.add(llvm::createLoopUnrollPass());          // Unroll small loops
		pm.add(llvm::createInstructionCombiningPass()); // Clean up after the unroller
		pm.add(llvm::createGVNPass());                   // Remove redundancies
		pm.add(llvm::createMemCpyOptPass());            // Remove memcpy / form memset
		pm.add(llvm::createSCCPPass());                  // Constant prop with SCCP
		pm.add(llvm::createInstructionCombiningPass());
		//pm.add(llvm::createCondPropagationPass());       // Propagate conditionals
		pm.add(llvm::createDeadStoreEliminationPass());  // Delete dead stores
		pm.add(llvm::createAggressiveDCEPass());   // Delete dead instructions
		pm.add(llvm::createCFGSimplificationPass());     // Merge & remove BBs
		pm.add(llvm::createStripDeadPrototypesPass()); // Get rid of dead prototypes
		pm.add(llvm::createDeadTypeEliminationPass());   // Eliminate dead types
		pm.add(llvm::createConstantMergePass());       // Merge dup global constants
		pm.add(llvm::createVerifierPass(llvm::PrintMessageAction));
	} else {
		pm.add(llvm::createCFGSimplificationPass());
		pm.add(llvm::createFunctionInliningPass());
		pm.add(llvm::createJumpThreadingPass());
		pm.add(llvm::createPromoteMemoryToRegisterPass());
		pm.add(llvm::createInstructionCombiningPass());
		pm.add(llvm::createCFGSimplificationPass());
		pm.add(llvm::createScalarReplAggregatesPass());
		pm.add(llvm::createLICMPass());
		//pm.add(llvm::createCondPropagationPass());
		pm.add(llvm::createGVNPass());
		pm.add(llvm::createSCCPPass());
		pm.add(llvm::createAggressiveDCEPass());
		pm.add(llvm::createCFGSimplificationPass());
		pm.add(llvm::createVerifierPass(llvm::PrintMessageAction));
	}
	
	pm.run(*mImpl->module);
}	

} // al::
