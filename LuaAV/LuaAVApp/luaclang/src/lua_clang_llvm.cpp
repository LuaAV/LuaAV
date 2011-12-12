#include "clang/Analysis/PathDiagnostic.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/Decl.h"
#include "clang/Basic/Builtins.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/FileManager.h"
#include "clang/CodeGen/ModuleBuilder.h"
#include "clang/Driver/Driver.h"
#include "clang/Frontend/CompileOptions.h"
#include "clang/Frontend/InitHeaderSearch.h"
#include "clang/Frontend/InitPreprocessor.h"
#include "clang/Frontend/TextDiagnosticBuffer.h"
#include "clang/Frontend/TextDiagnosticPrinter.h"
#include "clang/Lex/HeaderSearch.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Sema/ParseAST.h"

#include "llvm/ADT/OwningPtr.h"
#include "llvm/ADT/SmallPtrSet.h"
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Config/config.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/ExecutionEngine/JITEventListener.h"
#include "llvm/Linker.h"
#include "llvm/Module.h"
#include "llvm/ModuleProvider.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Type.h"
#include "llvm/PassManager.h"
#include "llvm/Support/Allocator.h"
#include "llvm/Support/DataTypes.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/PassNameParser.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/System/DynamicLibrary.h"
#include "llvm/System/Host.h"
#include "llvm/System/Path.h"
#include "llvm/System/Signals.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ValueSymbolTable.h"

#include "luaglue.h"

#if defined(WIN32) || defined(__WINDOWS_MM__)
	#define LUA_CLANG_WIN32 1
	#ifdef LUA_CLANG_EXPORTS
		#define LUA_CLANG_API __declspec(dllexport)
	#else
		#define LUA_CLANG_API __declspec(dllimport)
	#endif
#elif defined( __APPLE__ ) && defined( __MACH__ )
	#define LUA_CLANG_OSX 1
	#define LUA_CLANG_API extern
#else
	#define LUA_CLANG_LINUX 1
	#define LUA_CLANG_API extern
#endif

#define ddebug(...) 
//#define ddebug(...) printf(__VA_ARGS__)

// used by almost all LLVM types:
template<typename T>
int llvm_print(lua_State * L, T * u) {
	std::stringstream s;
	u->print(s);
	lua_pushfstring(L, "%s", s.str().c_str());
	return 1;
}

/* 
	ModuleProvider
*/
#pragma mark ModuleProvider
template<> const char * Glue<ModuleProvider>::usr_name() { return "ModuleProvider"; }
template<> int Glue<ModuleProvider>::usr_tostring(lua_State * L, ModuleProvider * u) {
	lua_pushfstring(L, "%s: %s(%p)", usr_name(), u->getModule()->getModuleIdentifier().c_str(), u);
	return 1;
}
// retrieve Module from Compiler object instead:
//template<> ModuleProvider * Glue<ModuleProvider>::usr_new(lua_State * L) {
//	//Module * m = Glue<Module>::checkto(L, 1);
//	//return new ExistingModuleProvider(m);
//	const char * modulename = luaL_checkstring(L, 1);
//	Module * M = new Module(modulename, getGlobalContext()); 
//	return createModuleProviderFromJIT(L, M);
//}
//template<> void Glue<ModuleProvider>::usr_gc(lua_State * L, ModuleProvider * mp) {
////	printf("~module %p\n", mp);
//	std::string err;
//	ExecutionEngine * ee = getEE();
//	Module * m = mp->getModule();
//	if (m) {
//		ee->runStaticConstructorsDestructors(m, true);
//		ee->clearGlobalMappingsFromModule(m);
//		
//		// iterate functions and remove from EE:
//		for (Module::iterator i = m->begin(), e = m->end(); i != e; ++i) {
//			ee->freeMachineCodeForFunction(i);
//		}
//		
//		m->dropAllReferences();
//	}
//	ee->removeModuleProvider(mp, &err);
//	// this isn't safe, since we may be using things created in m:
//	//ee->deleteModuleProvider(mp);
//	
//	printf("removed %s %s\n", m->getModuleIdentifier().data(), err.data());
////	delete m;
//}
//static int moduleprovider_module(lua_State * L) {
//	ModuleProvider * mp = Glue<ModuleProvider>::checkto(L, 1);
//	Glue<Module>::push(L, mp->getModule());
//	return 1;
//}
static int moduleprovider_dump(lua_State * L) {
	Module * m = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	m->dump();
	return 0;
}
int moduleprovider_addTypeName(lua_State * L) {
	Module * m = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	const Type * ty = Glue<Type>::checkto(L, 2);
	const char * name = luaL_checkstring(L, 3);
	lua_pushboolean(L, m->addTypeName(name, ty));
	return 1;
}
int moduleprovider_getTypeName(lua_State * L) {
	Module * m = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	const Type * ty = Glue<Type>::checkto(L, 2);
	lua_pushstring(L, m->getTypeName(ty).c_str());
	return 1;
}
int moduleprovider_getTypeByName(lua_State * L) {
	Module * m = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	const char * name = luaL_checkstring(L, 2);
	const Type * t = m->getTypeByName(name);
	if (t == 0)
		return luaL_error(L, "Type %s not found", name);
	return Glue<Type>::push(L, (Type *)t);
}
// do these via Compiler object instead:
//int moduleprovider_writeBitcodeFile(lua_State * L) {
//	Module * m = Glue<ModuleProvider>::checkto(L, 1)->getModule();
//	std::ofstream ofile(luaL_checkstring(L, 2), std::ios_base::out | std::ios_base::trunc);
//	WriteBitcodeToFile(m, ofile);
//	ofile.close();
//	return 0;
//}
//static int moduleprovider_link(lua_State * L) {
//	std::string err;
//	Module * self = Glue<ModuleProvider>::checkto(L, 1)->getModule();
//	Module * mod = Glue<ModuleProvider>::checkto(L, 2)->getModule();
//	llvm::Linker::LinkModules(self, mod, &err);
//	if (err.length())
//		luaL_error(L, err.c_str());
//	// mod can't be used anymore, as it has been subsumed into self:
//	lua_pushnil(L);
//	lua_setmetatable(L, 2);
//	return 0;
//}
//int moduleprovider_optimize(lua_State * L) {
//	Module * M = Glue<ModuleProvider>::checkto(L, 1)->getModule();
//	bool DisableOptimizations = lua_toboolean(L, 2);
//	bool DisableInline = lua_toboolean(L, 3);
//	optimize_module(M, DisableOptimizations, DisableInline);
//	return 0;
//}
//int moduleprovider_functions(lua_State * L) {
//	Module * M = Glue<ModuleProvider>::checkto(L, 1)->getModule();
//	lua_newtable(L);
//	for (Module::iterator i = M->begin(), e = M->end(); i != e; ++i) {
//		lua_pushstring(L, i->getName().data());
//		Glue<Function>::push(L, i);
//		lua_rawset(L, -3);
//	}
//	return 1;
//}

template<> void Glue<ModuleProvider> :: usr_mt(lua_State * L, int mt) {
	//lua_pushcfunction(L, moduleprovider_module); lua_setfield(L, -2, "getModule");
	lua_pushcfunction(L, moduleprovider_dump); lua_setfield(L, mt, "dump");
	lua_pushcfunction(L, moduleprovider_addTypeName); lua_setfield(L, mt, "addTypeName");
	lua_pushcfunction(L, moduleprovider_getTypeName); lua_setfield(L, mt, "getTypeName");
	lua_pushcfunction(L, moduleprovider_getTypeByName); lua_setfield(L, mt, "getTypeByName");
	lua_pushcfunction(L, Glue<Function>::create);	lua_setfield(L, mt, "Function");
	// get these via Compiler object instead:
	//lua_pushcfunction(L, moduleprovider_writeBitcodeFile); lua_setfield(L, mt, "writeBitcodeFile");
	//lua_pushcfunction(L, moduleprovider_link); lua_setfield(L, mt, "link");
	//lua_pushcfunction(L, moduleprovider_optimize); lua_setfield(L, mt, "optimize");
	//lua_pushcfunction(L, moduleprovider_functions); lua_setfield(L, mt, "functions");
	//lua_pushcfunction(L, ee_call); lua_setfield(L, mt, "call");
	//lua_pushcfunction(L, ee_pushluafunction); lua_setfield(L, mt, "pushluafunction");
}


/*
	Type
*/
#pragma mark Type
template<> const char * Glue<Type>::usr_name() { return "Type"; }
template<> int Glue<Type>::usr_tostring(lua_State * L, Type * t) { return llvm_print(L, t); }

// type call with no argument is a constant constructor
// type call with an argument is a cast operator
//static int type_call(lua_State * L) {
//	const Type * t = Glue<Type>::checkto(L, 1);
//	if (!lua_isuserdata(L, 2)) {
//		// constant initializer:
//		switch (t->getTypeID()) {
//			case Type::VoidTyID:
//				return Glue<Constant>::push(L, ConstantPointerNull::get((const PointerType *)Type::getVoidTy(getGlobalContext())));
//			case Type::FloatTyID:
//			case Type::DoubleTyID:
//				return Glue<Constant>::push(L, ConstantFP::get(t, luaL_optnumber(L, 2, 0.)));
//			case Type::IntegerTyID:
//				return Glue<Constant>::push(L, ConstantInt::get(t, luaL_optinteger(L, 2, 0)));
//		}
//		return luaL_error(L, "Type cannot be constructed as a constant");
//	} else {
//		// cast:
//		Value * v = Glue<Value>::checkto(L, 2);
//		const Type * t2 = v->getType();
//		IRBuilder<> * b = getLModule(L)->getBuilder();
//
//		// can't cast void:
//		if (t->getTypeID() == Type::VoidTyID || t2->getTypeID() == Type::VoidTyID)
//			return luaL_error(L, "Cannot cast to/from Void");
//
//		// ptr to ptr
//		if (t->getTypeID() == Type::PointerTyID && t2->getTypeID() == Type::PointerTyID) {
//			return Glue<Value>::push(L, b->CreateBitCast(v, t, "cast"));
//		}
//
//		// int to float:
//		if (t->isInteger() && t2->isFloatingPoint())
//			return Glue<Value>::push(L, b->CreateFPToSI(v, t, "intcast"));
//		// float to int
//		if (t->isFloatingPoint() && t2->isInteger())
//			return Glue<Value>::push(L, b->CreateSIToFP(v, t, "floatcast"));
//
//		// int to int
//		if (t->isInteger() == t2->isInteger()) {
//			const IntegerType * it = (IntegerType *)t;
//			const IntegerType * it2 = (IntegerType *)t2;
//			if (it->getBitWidth() > it2->getBitWidth()) {
//				return Glue<Value>::push(L, b->CreateZExt(v, it, "trunc"));
//			} else if (it->getBitWidth() < it2->getBitWidth()) {
//				return Glue<Value>::push(L, b->CreateTrunc(v, it, "trunc"));
//			} else {
//				return 1; // no cast required
//			}
//		}
//
//		return luaL_error(L, "unrecognized cast");
//	}
//}

int type_id(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushinteger(L, t->getTypeID());
	return 1;
}
int type_isinteger(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isInteger());
	return 1;
}
int type_isfloatingpoint(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isFloatingPoint());
	return 1;
}
int type_isabstract(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isAbstract());
	return 1;
}
int type_issized(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isSized());
	return 1;
}
int type_isValidReturnType(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L);
	lua_pushboolean(L, FunctionType::isValidReturnType(t));
	return 1;
}
//int type_sizeABI(lua_State * L) {
//	const Type * t = Glue<Type>::checkto(L, 1);
//	ExecutionEngine * ee = LModule::getEE();
//	const TargetData * td = ee->getTargetData();
//	/// getTypeSizeInBits - Return the number of bits necessary to hold the
//	/// specified type.  For example, returns 36 for i36 and 80 for x86_fp80.
//	lua_pushinteger(L, td->getABITypeSize(t));
//	return 1;
//}
int type_sizeinbits(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	if (EE == 0) luaL_error(L, "No Execution Engine");
	const TargetData * td = EE->getTargetData();
	/// getTypeSizeInBits - Return the number of bits necessary to hold the
	/// specified type.  For example, returns 36 for i36 and 80 for x86_fp80.
	lua_pushinteger(L, td->getTypeSizeInBits(t));
	return 1;
}
static int type_pointer(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	int addressSpace = luaL_optinteger(L, 2, 0);
	if (t->getTypeID() == Type::VoidTyID) {
		// special case for void *:
		return Glue<PointerType>::push(L, PointerType::get(Type::getInt8Ty(getGlobalContext()), addressSpace));
	}
	return Glue<PointerType>::push(L, PointerType::get(t, addressSpace));
}
static int type_eq(lua_State * L) {
	//lua::dump(L, "eq");
	const Type * a = Glue<Type>::checkto(L, 1);
	const Type * b = Glue<Type>::checkto(L, 2);
	//ddebug("%p %p\n", a, b);
	lua_pushboolean(L, a == b);
	return 1;
}
int type_modname(lua_State * L) {
	Module * M = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	const Type * t = Glue<Type>::checkto(L, 2);
	lua_pushstring(L, M->getTypeName(t).c_str());
	return 1;
}
template<> void Glue<Type>::usr_mt(lua_State * L, int mt) {
	//lua_pushcfunction(L, type_call); lua_setfield(L, -2, "__call");
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
	lua_pushcfunction(L, type_pointer); lua_setfield(L, mt, "pointer");
	lua_pushcfunction(L, type_pointer); lua_setfield(L, mt, "ptr");
	lua_pushcfunction(L, type_modname); lua_setfield(L, mt, "name");
	lua_pushcfunction(L, type_isinteger); lua_setfield(L, mt, "isinteger");
	lua_pushcfunction(L, type_isfloatingpoint); lua_setfield(L, mt, "isfloatingpoint");
	lua_pushcfunction(L, type_isabstract); lua_setfield(L, mt, "isabstract");
	lua_pushcfunction(L, type_issized); lua_setfield(L, mt, "issized");
	lua_pushcfunction(L, type_sizeinbits); lua_setfield(L, mt, "sizeinbits");
	lua_pushcfunction(L, type_isValidReturnType); lua_setfield(L, mt, "isValidReturnType");
	//lua_pushcfunction(L, type_sizeABI); lua_setfield(L, mt, "size");
	lua_pushcfunction(L, type_id); lua_setfield(L, mt, "id");
	//lua_pushcfunction(L, ee_offsetOf); lua_setfield(L, mt, "offsetOf");
	
	Glue<Type>::push(L, (Type *)Type::getVoidTy(getGlobalContext())); lua_setfield(L, mt, "Void");
	Glue<Type>::push(L, (Type *)Type::getLabelTy(getGlobalContext())); lua_setfield(L, mt, "Label");
	Glue<Type>::push(L, (Type *)Type::getFloatTy(getGlobalContext())); lua_setfield(L, mt, "Float");
	Glue<Type>::push(L, (Type *)Type::getDoubleTy(getGlobalContext())); lua_setfield(L, mt, "Double");
	Glue<Type>::push(L, (Type *)Type::getInt1Ty(getGlobalContext())); lua_setfield(L, mt, "Int1");
	Glue<Type>::push(L, (Type *)Type::getInt8Ty(getGlobalContext())); lua_setfield(L, mt, "Int8");
	Glue<Type>::push(L, (Type *)Type::getInt16Ty(getGlobalContext())); lua_setfield(L, mt, "Int16");
	Glue<Type>::push(L, (Type *)Type::getInt32Ty(getGlobalContext())); lua_setfield(L, mt, "Int32");
	Glue<Type>::push(L, (Type *)Type::getInt64Ty(getGlobalContext())); lua_setfield(L, mt, "Int64");
}

/*
	StructType : CompositeType : DerivedType : Type
*/
#pragma mark StructType
template<> const char * Glue<StructType>::usr_name() { return "StructType"; }
template<> const char * Glue<StructType>::usr_supername() { return "Type"; }
template<> int Glue<StructType>::usr_tostring(lua_State * L, StructType * t) { return llvm_print<StructType>(L, t); }
template<> StructType * Glue<StructType>::usr_new(lua_State * L) {
	std::vector<const Type *> types;
	if (lua_type(L, 1) == LUA_TTABLE) {
		int ntypes = lua_objlen(L, 1);
		for (int i=1; i<= ntypes; i++) {
			lua_rawgeti(L, 1, i);
			types.push_back(Glue<Type>::checkto(L, -1));
			lua_pop(L, 1);
		}
	}
	bool isPacked = false; // this is true for unions, I think??
	return StructType::get((getGlobalContext()), types, isPacked);
}
static int structtype_len(lua_State * L) {
	StructType * u = Glue<StructType>::checkto(L, 1);
	lua_pushinteger(L, u->getNumElements());
	return 1;
}
static int structtype_getelementtype(lua_State * L) {
	StructType * u = Glue<StructType>::checkto(L, 1);
	unsigned int i = luaL_checkinteger(L, 2);
	if (i >= u->getNumElements())
		return luaL_error(L, "StructType has only %d elements (requested %d)", u->getNumElements(), i);
	return Glue<Type>::push(L, (Type *)u->getElementType(i));
}
static int structtype_gettypes(lua_State * L) {
	StructType * u = Glue<StructType>::checkto(L, 1);
	for (unsigned int i=0; i< u->getNumElements(); i++) {
		Glue<Type>::push(L, (Type *)u->getElementType(i));
	}
	return u->getNumElements();
}
template<> void Glue<StructType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, structtype_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
	lua_pushcfunction(L, structtype_getelementtype); lua_setfield(L, mt, "type");
	lua_pushcfunction(L, structtype_gettypes); lua_setfield(L, mt, "types");
}

/*
	SequentialType : CompositeType : DerivedType : Type
*/
#pragma mark SequentialType
template<> const char * Glue<SequentialType>::usr_name() { return "SequentialType"; }
template<> const char * Glue<SequentialType>::usr_supername() { return "Type"; }
template<> int Glue<SequentialType>::usr_tostring(lua_State * L, SequentialType * t) { return llvm_print<SequentialType>(L, t); }
static int sequentialtype_getelementtype(lua_State * L) {
	SequentialType * u = Glue<SequentialType>::checkto(L, 1);
	return Glue<Type>::push(L, (Type *)u->getElementType());
}
template<> void Glue<SequentialType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
	lua_pushcfunction(L, sequentialtype_getelementtype); lua_setfield(L, mt, "type");
}

/*
	PointerType : SequentialType
*/
#pragma mark PointerType
template<> const char * Glue<PointerType>::usr_name() { return "PointerType"; }
template<> const char * Glue<PointerType>::usr_supername() { return "SequentialType"; }
template<> int Glue<PointerType>::usr_tostring(lua_State * L, PointerType * t) { return llvm_print<PointerType>(L, t); }
template<> PointerType * Glue<PointerType>::usr_new(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	int addressSpace = luaL_optinteger(L, 2, 0);
	return PointerType::get(t, addressSpace);
}
template<> void Glue<PointerType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
}

/*
	ArrayType : SequentialType
*/
#pragma mark ArrayType
template<> const char * Glue<ArrayType>::usr_name() { return "ArrayType"; }
template<> const char * Glue<ArrayType>::usr_supername() { return "SequentialType"; }
template<> int Glue<ArrayType>::usr_tostring(lua_State * L, ArrayType * t) { return llvm_print<ArrayType>(L, t); }
template<> ArrayType * Glue<ArrayType>::usr_new(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	int len = luaL_optinteger(L, 2, 1);
	return ArrayType::get(t, len);
}
static int arraytype_len(lua_State * L) {
	ArrayType * u = Glue<ArrayType>::checkto(L, 1);
	lua_pushinteger(L, u->getNumElements());
	return 1;
}
template<> void Glue<ArrayType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, arraytype_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
}

/*
	VectorType : SequentialType
*/
#pragma mark VectorType
template<> const char * Glue<VectorType>::usr_name() { return "VectorType"; }
template<> const char * Glue<VectorType>::usr_supername() { return "SequentialType"; }
template<> int Glue<VectorType>::usr_tostring(lua_State * L, VectorType * t) { return llvm_print<VectorType>(L, t); }
template<> VectorType * Glue<VectorType>::usr_new(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	int len = luaL_optinteger(L, 2, 1);
	return VectorType::get(t, len);
}
static int vectortype_len(lua_State * L) {
	VectorType * u = Glue<VectorType>::checkto(L, 1);
	lua_pushinteger(L, u->getNumElements());
	return 1;
}
template<> void Glue<VectorType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, vectortype_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
}

/*
	OpaqueType : DerivedType : Type
*/
#pragma mark OpaqueType
template<> const char * Glue<OpaqueType>::usr_name() { return "OpaqueType"; }
template<> const char * Glue<OpaqueType>::usr_supername() { return "Type"; }
template<> int Glue<OpaqueType>::usr_tostring(lua_State * L, OpaqueType * t) { return llvm_print<OpaqueType>(L, t); }
template<> OpaqueType * Glue<OpaqueType>::usr_new(lua_State * L) {
	return OpaqueType::get(getGlobalContext());
}
template<> void Glue<OpaqueType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
}

/*
	FunctionType : DerivedType : Type
*/
#pragma mark FunctionType
template<> const char * Glue<FunctionType>::usr_name() { return "FunctionType"; }
template<> const char * Glue<FunctionType>::usr_supername() { return "Type"; }
template<> int Glue<FunctionType>::usr_tostring(lua_State * L, FunctionType * t) { return llvm_print<FunctionType>(L, t); }
template<> FunctionType * Glue<FunctionType>::usr_new(lua_State * L) {
	const Type * ret = Glue<Type>::checkto(L, 1);
	std::vector<const Type *> types;
	int ntypes = lua_gettop(L) - 1;
	for (int i=0; i<ntypes; i++)
		types.push_back(Glue<Type>::checkto(L, i+2));
	bool isVarArg = false; // this is true for unions, I think??
	return FunctionType::get(ret, types, isVarArg);
}
static int functiontype_isvararg(lua_State * L) {
	FunctionType * f = Glue<FunctionType>::checkto(L, 1);
	lua_pushinteger(L, f->isVarArg());
	return 1;
}
static int functiontype_getparamtype(lua_State * L) {
	FunctionType * u = Glue<FunctionType>::checkto(L, 1);
	unsigned int i = luaL_checkinteger(L, 2);
	if (i >= u->getNumParams())
		return luaL_error(L, "FunctionType has only %d params (requested %d)", u->getNumParams(), i);
	return Glue<Type>::push(L, (Type *)u->getParamType(i));
}
static int functiontype_getreturntype(lua_State * L) {
	FunctionType * u = Glue<FunctionType>::checkto(L, 1);
	return Glue<Type>::push(L, (Type *)u->getReturnType());
}
static int functiontype_len(lua_State * L) {
	FunctionType * u = Glue<FunctionType>::checkto(L, 1);
	lua_pushinteger(L, u->getNumParams());
	return 1;
}
template<> void Glue<FunctionType>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, type_eq); lua_setfield(L, mt, "__eq");
	lua_pushcfunction(L, functiontype_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, functiontype_isvararg); lua_setfield(L, mt, "isvararg");
	lua_pushcfunction(L, functiontype_getparamtype); lua_setfield(L, mt, "param");
	lua_pushcfunction(L, functiontype_getreturntype); lua_setfield(L, mt, "ret");
	lua_pushcfunction(L, Glue<Function>::create); lua_setfield(L, mt, "__call");
}

/*
	Value
*/
#pragma mark Value
template<> const char * Glue<Value>::usr_name() { return "Value"; }
template<> int Glue<Value>::usr_tostring(lua_State * L, Value * u) { return llvm_print<Value>(L, u); }
template<> Value * Glue<Value>::usr_reinterpret(lua_State * L, int idx) {
	if (lua_isnoneornil(L, idx))
		return 0;
	double n;
	switch (lua_type(L, idx)) {
		case LUA_TBOOLEAN:
			// equiv: const bool;
			return ConstantInt::get((getGlobalContext()), APInt(1, lua_toboolean(L, idx)));
		case LUA_TNUMBER:
			n = lua_tonumber(L, idx);
			if (fmod(n, 1.0) == 0.)
				// equiv: const sint32_t;
				return ConstantInt::get((getGlobalContext()), APInt(32, lua_tonumber(L, idx)));
			else	
				// equiv: const double;
				return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), lua_tonumber(L, idx));
		case LUA_TSTRING:
			// equiv: const char * (null terminated)
			return ConstantArray::get((getGlobalContext()), std::string(lua_tostring(L, idx)), true); // true == null terminate
		case LUA_TUSERDATA:	{	 
			// makes sense only if it is a Value:
			Value * u = Glue<Value>::checkto(L, idx);
			return u;
		}
		case LUA_TLIGHTUSERDATA: // pointers can't be typeless constants, so exchange for null
		case LUA_TNIL:
		default:				// thread, function & table make no sense for an LLVM Value *
			luaL_error(L, "cannot interpret Value from Lua type '%s'\n", lua_typename(L, lua_type(L, idx)));
			break;
	}
	return 0;
}
static int value_type(lua_State * L) {
	Value * u = Glue<Value>::checkto(L);
	return Glue<Type>::push(L, (Type *)u->getType());
}
static int value_name(lua_State * L) {
	Value * u = Glue<Value>::checkto(L);
	if (lua_isstring(L, 2)) {
		const char * name = lua_tostring(L, 2);
		u->setName(name);
		lua_pushvalue(L, 1);
		return 1;
	}
	lua_pushstring(L, u->getNameStr().c_str());
	return 1;
}
static int value_replace(lua_State * L) {
	Value * u = Glue<Value>::checkto(L, 1);
	Value * v = Glue<Value>::checkto(L, 2);
	u->replaceAllUsesWith(v);
	return 0;
}
static int value_uses(lua_State * L) {
	Value * u = Glue<Value>::checkto(L, 1);
	lua_pushinteger(L, u->getNumUses());
	return 1;
}
template<> void Glue<Value>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, value_type); lua_setfield(L, mt, "type");
	lua_pushcfunction(L, value_name); lua_setfield(L, mt, "name");
	lua_pushcfunction(L, value_replace); lua_setfield(L, mt, "replace");
	lua_pushcfunction(L, value_uses); lua_setfield(L, mt, "uses");
}

/*
	Argument : Value
*/
#pragma mark Argument
template<> const char * Glue<Argument>::usr_name() { return "Argument"; }
template<> const char * Glue<Argument>::usr_supername() { return "Value"; }
template<> int Glue<Argument>::usr_tostring(lua_State * L, Argument * u) { return llvm_print<Argument>(L, u); }
static int argument_parent(lua_State * L) {
	Argument * u = Glue<Argument>::checkto(L, 1);
	return Glue<Function>::push(L, u->getParent());
}
static int argument_argno(lua_State * L) {
	Argument * u = Glue<Argument>::checkto(L, 1);
	lua_pushinteger(L, u->getArgNo());
	return 1;
}
static int argument_byval(lua_State * L) {
	Argument * u = Glue<Argument>::checkto(L, 1);
	lua_pushboolean(L, u->hasByValAttr());
	return 1;
}
template<> void Glue<Argument>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, argument_parent); 
	lua_setfield(L, mt, "parent");
	lua_pushcfunction(L, argument_argno); 
	lua_setfield(L, mt, "argno");
	lua_pushcfunction(L, argument_byval); 
	lua_setfield(L, mt, "byval");
}

/*
	Instruction : User : Value
*/
#pragma mark Instruction
template<> const char * Glue<Instruction>::usr_name() { return "Instruction"; }
template<> const char * Glue<Instruction>::usr_supername() { return "Value"; }
template<> int Glue<Instruction>::usr_tostring(lua_State * L, Instruction * u) { return llvm_print<Instruction>(L, u); }
static int inst_parent(lua_State * L) {
	Instruction * u = Glue<Instruction>::checkto(L, 1);
	return Glue<BasicBlock>::push(L, u->getParent());
}
static int inst_erase(lua_State * L) {
	Instruction * f = Glue<Instruction>::checkto(L, 1);
	f->eraseFromParent();
	Glue<Instruction>::erase(L, 1);
	return 0;
}
static int inst_opcode(lua_State * L) {
	Instruction * u = Glue<Instruction>::checkto(L, 1);
	lua_pushstring(L, u->getOpcodeName());
	return 1;
}
template<> void Glue<Instruction>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, inst_parent); lua_setfield(L, mt, "parent");
	lua_pushcfunction(L, inst_erase); lua_setfield(L, mt, "erase");
	lua_pushcfunction(L, inst_opcode); lua_setfield(L, mt, "opcode");
}

/*
	PHINode : Instruction
*/
#pragma mark PHINode
template<> const char * Glue<PHINode>::usr_name() { return "PHINode"; }
template<> const char * Glue<PHINode>::usr_supername() { return "Instruction"; }
template<> int Glue<PHINode>::usr_tostring(lua_State * L, PHINode * u) { return llvm_print<PHINode>(L, u); }
static int phi_addincoming(lua_State * L) {
	PHINode * u = Glue<PHINode>::checkto(L, 1);
	Value * v = Glue<Value>::checkto(L, 2);
	BasicBlock * block = Glue<BasicBlock>::checkto(L, 3);
	u->addIncoming(v, block);
	return 0;
}
template<> void Glue<PHINode>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, phi_addincoming); lua_setfield(L, mt, "addincoming");
}


/*
	BasicBlock : Value
*/
#pragma mark BasicBlock
template<> const char * Glue<BasicBlock>::usr_name() { return "BasicBlock"; }
template<> const char * Glue<BasicBlock>::usr_supername() { return "Value"; }
template<> int Glue<BasicBlock>::usr_tostring(lua_State * L, BasicBlock * u) { return llvm_print<BasicBlock>(L, u); }
template<> BasicBlock * Glue<BasicBlock>::usr_new(lua_State * L) {
	const char * name = luaL_optstring(L, 1, "noname");
	Function * parent = Glue<Function>::to(L, 2);
	BasicBlock * insertbefore = Glue<BasicBlock>::to(L, 3);
	return BasicBlock::Create((getGlobalContext()), name, parent, insertbefore);
}
static int block_parent(lua_State * L) {
	BasicBlock * u = Glue<BasicBlock>::checkto(L, 1);
	return Glue<Function>::push(L, u->getParent());
}
static int block_erase(lua_State * L) {
	BasicBlock * f = Glue<BasicBlock>::checkto(L, 1);
	f->eraseFromParent();
	Glue<BasicBlock>::erase(L, 1);
	return 0;
}
static int block_terminator(lua_State * L) {
	BasicBlock * f = Glue<BasicBlock>::checkto(L, 1);
	return Glue<Instruction>::push(L, f->getTerminator());
}
static int block_instruction(lua_State * L) {
	BasicBlock * f = Glue<BasicBlock>::checkto(L, 1);
	unsigned int i = luaL_checkinteger(L, 2);
	if (i >= f->size())
		return luaL_error(L, "Function has only %d arguments (requested %d)", f->size(), i);
	BasicBlock::iterator it = f->begin();
	while (i--) it++;
	return Glue<Instruction>::push(L, it);
}
static int block_front(lua_State * L) {
	BasicBlock * f = Glue<BasicBlock>::checkto(L, 1);
	return Glue<Instruction>::push(L, &f->front());
}
static int block_back(lua_State * L) {
	BasicBlock * f = Glue<BasicBlock>::checkto(L, 1);
	return Glue<Instruction>::push(L, &f->back());
}
static int block_len(lua_State * L) {
	BasicBlock * f = Glue<BasicBlock>::checkto(L, 1);
	lua_pushinteger(L, f->size());
	return 1;
}
//static int block_setinsertpoint(lua_State * L) {
//	BasicBlock * block = Glue<BasicBlock>::checkto(L, 1);
//	getLModule(L)->getBuilder()->SetInsertPoint(block);
//	return 0;
//}
//static int block_ret(lua_State * L) {
//	IRBuilder<> * b = getLModule(L)->getBuilder();
//	BasicBlock * block = Glue<BasicBlock>::checkto(L, 1);
//	Value * v = Glue<Value>::to(L, 2);
//	Function * f = block->getParent();
//	if (f) {
//		// check types:
//		const Type * retType = f->getFunctionType()->getReturnType();
//		if (retType->getTypeID() == Type::VoidTyID) {
//			if (v)
//				return luaL_error(L, "current function returns void");
//			return Glue<Instruction>::push(L, b->CreateRetVoid());
//		}
//		if (retType != v->getType())
//			luaL_error(L, "return type mismatch");
//		return Glue<Instruction>::push(L, b->CreateRet(v));
//	}
//	if (v)
//		return Glue<Instruction>::push(L, b->CreateRet(v));
//	return Glue<Instruction>::push(L, b->CreateRetVoid());
//}
template<> void Glue<BasicBlock>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, block_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, block_instruction); lua_setfield(L, mt, "instruction");
	lua_pushcfunction(L, block_parent); lua_setfield(L, mt, "parent");
	lua_pushcfunction(L, block_front); lua_setfield(L, mt, "front");
	lua_pushcfunction(L, block_back); lua_setfield(L, mt, "back");
	lua_pushcfunction(L, block_erase); lua_setfield(L, mt, "erase");
	lua_pushcfunction(L, block_terminator); lua_setfield(L, mt, "terminator");
//	lua_pushcfunction(L, block_setinsertpoint); lua_setfield(L, mt, "setinsertpoint");
//	lua_pushcfunction(L, block_ret); lua_setfield(L, mt, "ret");
}

/*
	Constant : User : Value
*/
#pragma mark Constant
template<> const char * Glue<Constant>::usr_name() { return "Constant"; }
template<> const char * Glue<Constant>::usr_supername() { return "Value"; }
template<> int Glue<Constant>::usr_tostring(lua_State * L, Constant * u) { return llvm_print<Constant>(L, u); }
template<> Constant * Glue<Constant>::usr_reinterpret(lua_State * L, int idx) {
	return (Constant *)Glue<Value>::usr_reinterpret(L, idx);
}
// destroyConstant... todo: call this on all constants registered so far, when the module closes

/*
	GlobalValue : Constant
*/
#pragma mark GlobalValue
template<> const char * Glue<GlobalValue>::usr_name() { return "GlobalValue"; }
template<> const char * Glue<GlobalValue>::usr_supername() { return "Constant"; }
template<> int Glue<GlobalValue>::usr_tostring(lua_State * L, GlobalValue * u) { return llvm_print<GlobalValue>(L, u); }
static int global_linkage(lua_State * L) {
	GlobalValue * u = Glue<GlobalValue>::checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		u->setLinkage((GlobalValue::LinkageTypes)lua_tointeger(L, 2));
	}
	lua_pushinteger(L, u->getLinkage());
	return 1;
}
static int global_isdeclaration(lua_State * L) {
	GlobalValue * f = Glue<GlobalValue>::checkto(L, 1);
	lua_pushboolean(L, f->isDeclaration());
	return 1;
}


static int global_getptr(lua_State * L) {
	GlobalValue * u = Glue<GlobalValue>::checkto(L, 1);
	if (EE==NULL) luaL_error(L, "No Execution Engine");
	void * ptr = EE->getPointerToGlobalIfAvailable(u);
	if (ptr) {
		lua_pushlightuserdata(L, ptr);
		return 1;
	}
	return 0;
}

template<> void Glue<GlobalValue>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, global_linkage); lua_setfield(L, mt, "linkage");
	lua_pushcfunction(L, global_isdeclaration); lua_setfield(L, mt, "isdeclaration");
	lua_pushcfunction(L, global_getptr); lua_setfield(L, mt, "getptr");
	//lua_pushcfunction(L, ee_addGlobalMapping); lua_setfield(L, mt, "addGlobalMapping");
}
// likely methods:	getParent() (returns a Module *)


/*
	GlobalVariable : GlobalValue
*/
#pragma mark GlobalVariable
template<> const char * Glue<GlobalVariable>::usr_name() { return "GlobalVariable"; }
template<> const char * Glue<GlobalVariable>::usr_supername() { return "GlobalValue"; }
template<> int Glue<GlobalVariable>::usr_tostring(lua_State * L, GlobalVariable * u) { return llvm_print<GlobalVariable>(L, u); }
template<> GlobalVariable * Glue<GlobalVariable>::usr_new(lua_State * L) {
	Module * m = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	const Type * t = Glue<Type>::checkto(L, 2);
	const char * name  = luaL_checkstring(L, 3);
	bool isConstant = lua_toboolean(L, 4);
	Constant * initializer = 0;
	if (lua_isuserdata(L, 5)) {
		initializer = Glue<Constant>::to(L, 5);
		if (initializer->getType() != t) {
			luaL_error(L, "initializer must match type");
			return 0;
		}
	}
	GlobalValue::LinkageTypes lt = (GlobalValue::LinkageTypes)luaL_optinteger(L, 5, GlobalValue::ExternalLinkage);
	return new GlobalVariable((getGlobalContext()), t, isConstant, lt, initializer, name, m);
}
static int globalvar_erase(lua_State * L) {
	GlobalVariable * u = Glue<GlobalVariable>::checkto(L, 1);
	u->eraseFromParent();
	Glue<GlobalVariable>::erase(L, 1);
	return 0;
}
static int globalvar_isconstant(lua_State * L) {
	GlobalVariable * u = Glue<GlobalVariable>::checkto(L, 1);
	lua_pushboolean(L, u->isConstant());
	return 1;
}
static int globalvar_initializer(lua_State * L) {
	GlobalVariable * u = Glue<GlobalVariable>::checkto(L, 1);
	if (u->hasInitializer())
		return Glue<Constant>::push(L, u->getInitializer());
	return 0;
}
template<> void Glue<GlobalVariable>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, globalvar_erase); lua_setfield(L, mt, "erase");
	lua_pushcfunction(L, globalvar_initializer); lua_setfield(L, mt, "initializer");
	lua_pushcfunction(L, globalvar_isconstant); lua_setfield(L, mt, "isconstant");
}

/*
	Function : GlobalValue
	
	TODO: 
	When a function is destroyed, we should call
		engine->freeMachineCodeForFunction(function);
        function->eraseFromParent();
	But this isn't safe if another function is using it... so perhaps let the module destructor do it?
*/
#pragma mark Function
template<> const char * Glue<Function>::usr_name() { return "Function"; }
template<> const char * Glue<Function>::usr_supername() { return "GlobalValue"; }
template<> int Glue<Function>::usr_tostring(lua_State * L, Function * u) { return llvm_print<Function>(L, u); }
template<> Function * Glue<Function>::usr_new(lua_State * L) {
	Module * M = Glue<ModuleProvider>::checkto(L, 1)->getModule();
	std::string name  = luaL_checkstring(L, 2);
	
	// if argument 3 is nil, then search for a pre-existing function of that name
	if (lua_isnoneornil(L, 3))
		return M->getFunction(name.data());
		
	// else generate a new function:
	const FunctionType * ft = Glue<FunctionType>::checkto(L, 3);
	GlobalValue::LinkageTypes lt = (GlobalValue::LinkageTypes)luaL_optinteger(L, 4, GlobalValue::ExternalLinkage);
	bool noAutoName = (bool)lua_isboolean(L, 4) && (bool)lua_toboolean(L, 5); // default false
	Function * F = Function::Create(ft, lt, name, M);
	if (noAutoName && F->getName() != name) {
		F->eraseFromParent();
		luaL_error(L, "Function %s already exists", name.c_str());
		return 0;
	}
	unsigned int i=0;
	Function::arg_iterator AI = F->arg_begin();
	for (; i < F->getFunctionType()->getNumParams(); ++AI, ++i) {
		char argname[16];
		sprintf(argname, "arg%i", i);
		AI->setName(argname);
	}
	return F;
}
static int function_intrinsic(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	lua_pushinteger(L, f->getIntrinsicID());
	return 1;
}
static int function_deletebody(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	f->deleteBody();
	return 0;
}
static int function_erase(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	f->eraseFromParent();
	Glue<Function>::erase(L, 1);
	return 0;
}
static int function_callingconv(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		unsigned cc = lua_tonumber(L, 2);
		f->setCallingConv(cc);
	}
	lua_pushinteger(L, f->getCallingConv());
	return 1;
}
static int function_argument(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	unsigned int i = luaL_checkinteger(L, 2);
	if (i >= f->getFunctionType()->getNumParams())
		return luaL_error(L, "Function has only %d arguments (requested %d)", f->getFunctionType()->getNumParams(), i);
	Function::arg_iterator it = f->arg_begin();
	while (i--) it++;
	return Glue<Argument>::push(L, it);
}
static int function_len(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	lua_pushinteger(L, f->size());
	return 1;
}
static int function_block(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	unsigned int i = luaL_checkinteger(L, 2);
	if (i >= f->size())
		return luaL_error(L, "Function has only %d blocks (requested %d)", f->size(), i);
	Function::iterator it = f->begin();
	while (i--) it++;
	return Glue<BasicBlock>::push(L, it);
}
static int function_verify(lua_State * L) {
	Function * u = Glue<Function>::to(L, 1);
	lua_pushboolean(L, u && (verifyFunction(*u, ReturnStatusAction) == false));
	return 1;
}
// trick here is knowing what the right
static int function_getptr(lua_State * L) {
	Function * u = Glue<Function>::to(L, 1);
	if (EE==NULL) luaL_error(L, "No Execution Engine");
	// NOTE: this causes codegeneration of F and any dependents:
	void * f = EE->getPointerToFunction(u);
	lua_pushlightuserdata(L, f);
	return 1;
}
//static int function_optimize(lua_State * L) {
//	Function * f = Glue<Function>::checkto(L, 1);
//	lua_pushboolean(L, getLModule(L)->optimize(f));
//	return 1;
//}
static int function_pushcfunction(lua_State * L) {
	Function * u = Glue<Function>::checkto(L, 1);
	if (EE==NULL) luaL_error(L, "No Execution Engine");
	void * f = EE->getPointerToFunction(u);
	if(f) { // && u->getFunctionType() != luaFunctionTy) {
		lua_pushcfunction(L, (lua_CFunction)f);
		return 1;
	}
	else {
		return 0;
	}
}
template<> void Glue<Function>::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, function_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, function_intrinsic); lua_setfield(L, mt, "intrinsic");
	lua_pushcfunction(L, function_deletebody); lua_setfield(L, mt, "deletebody");
	lua_pushcfunction(L, function_erase); lua_setfield(L, mt, "erase");
	lua_pushcfunction(L, function_callingconv); lua_setfield(L, mt, "callingconv");
	lua_pushcfunction(L, function_argument); lua_setfield(L, mt, "argument");
	lua_pushcfunction(L, function_block); lua_setfield(L, mt, "block");
	lua_pushcfunction(L, function_verify); lua_setfield(L, mt, "verify");
	lua_pushcfunction(L, function_getptr); lua_setfield(L, mt, "getptr");
	//lua_pushcfunction(L, function_optimize); lua_setfield(L, mt, "optimize");
	lua_pushcfunction(L, function_pushcfunction); lua_setfield(L, mt, "pushcfunction");
}

/*
	IRBuilder
*/
#pragma mark IRBuilder
template<> const char * Glue<IRBuilder<> >::usr_name() { return "IRBuilder"; }
template<> IRBuilder<> * Glue<IRBuilder<> >::usr_new(lua_State * L) {
	return new IRBuilder<>(getGlobalContext());
}

static int createPHI(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Type * ty = Glue<Type>::checkto(L, 2);
	const char * name = luaL_optstring(L, 3, "phi");
	return Glue<PHINode>::push(L, b->CreatePHI(ty, name));
}
static int createBr(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	BasicBlock * branch = Glue<BasicBlock>::checkto(L, 2);
	return Glue<Instruction>::push(L, b->CreateBr(branch));
}
static int createCondBr(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * condition = Glue<Value>::checkto(L, 2);
	if (condition->getType() != Type::getInt1Ty(getGlobalContext()))
		return luaL_error(L, "condition must be of Int1 type");
	BasicBlock * iftrue = Glue<BasicBlock>::checkto(L, 3);
	BasicBlock * iffalse = Glue<BasicBlock>::checkto(L, 4);
	return Glue<Instruction>::push(L, b->CreateCondBr(condition, iftrue, iffalse));
}
static int createRet(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	BasicBlock * block = b->GetInsertBlock();
	if (block == 0)
		return luaL_error(L, "not currently building a block");
	Value * v = Glue<Value>::to(L, 2);
	Function * f = block->getParent();
	if (f) {
		// check types:
		const Type * retType = f->getFunctionType()->getReturnType();
		if (retType->getTypeID() == Type::VoidTyID) {
			return Glue<Instruction>::push(L, b->CreateRetVoid());
		}
		if (v == NULL)
			return luaL_error(L, "function requires a return value");
		if (retType->getTypeID() != v->getType()->getTypeID()) {
			//ddebug("%i %i\n", retType->getTypeID(), v->getType()->getTypeID());
			luaL_error(L, "return type mismatch");
		}
		return Glue<Instruction>::push(L, b->CreateRet(v));
	}
	if (v)
		return Glue<Instruction>::push(L, b->CreateRet(v));
	return Glue<Instruction>::push(L, b->CreateRetVoid());
}
static int createAdd(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	return Glue<Value>::push(L, b->CreateAdd(x, y, "add"));
}
static int createSub(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	return Glue<Value>::push(L, b->CreateSub(x, y, "sub"));
}
static int createMul(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	return Glue<Value>::push(L, b->CreateMul(x, y, "mul"));
}
static int createDiv(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		// todo check for unsignedness
		return Glue<Value>::push(L, b->CreateSDiv(x, y, "sdiv"));
	}
	return Glue<Value>::push(L, b->CreateFDiv(x, y, "fdiv"));
}
static int createRem(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		// todo check for unsignedness
		return Glue<Value>::push(L, b->CreateSRem(x, y, "srem"));
	}
	return Glue<Value>::push(L, b->CreateFRem(x, y, "frem"));
}
static int createNeg(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	return Glue<Value>::push(L, b->CreateNeg(Glue<Value>::checkto(L, 2), "neg"));
}
static int createNot(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	return Glue<Value>::push(L, b->CreateNot(Glue<Value>::checkto(L, 2), "not"));
}
static int createCmpEQ(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	// for int: option indicates unsigned; for float: fail if either is nan;
	bool option = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		return Glue<Value>::push(L, b->CreateICmpEQ(x, y, "icmp_eq"));
	}
	if (!option)
		return Glue<Value>::push(L, b->CreateFCmpOEQ(x, y, "fcmp_oeq"));
	return Glue<Value>::push(L, b->CreateFCmpUEQ(x, y, "fcmp_ueq"));
}
static int createCmpNE(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	// for int: option indicates unsigned; for float: fail if either is nan;
	bool option = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		return Glue<Value>::push(L, b->CreateICmpNE(x, y, "icmp_ne"));
	}
	if (!option)
		return Glue<Value>::push(L, b->CreateFCmpONE(x, y, "fcmp_one"));
	return Glue<Value>::push(L, b->CreateFCmpUNE(x, y, "fcmp_une"));
}
static int createCmpGT(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	// for int: option indicates unsigned; for float: fail if either is nan;
	bool option = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		if (option)
			return Glue<Value>::push(L, b->CreateICmpUGT(x, y, "icmp_ugt"));
		return Glue<Value>::push(L, b->CreateICmpSGT(x, y, "icmp_sgt"));
	}
	if (!option)
		return Glue<Value>::push(L, b->CreateFCmpOGT(x, y, "fcmp_ogt"));
	return Glue<Value>::push(L, b->CreateFCmpUGT(x, y, "fcmp_ugt"));
}
static int createCmpLT(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	// for int: option indicates unsigned; for float: fail if either is nan;
	bool option = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		if (option)
			return Glue<Value>::push(L, b->CreateICmpULT(x, y, "icmp_ult"));
		return Glue<Value>::push(L, b->CreateICmpSLT(x, y, "icmp_slt"));
	}
	if (!option)
		return Glue<Value>::push(L, b->CreateFCmpOLT(x, y, "fcmp_olt"));
	return Glue<Value>::push(L, b->CreateFCmpULT(x, y, "fcmp_ult"));
}
static int createCmpGE(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	// for int: option indicates unsigned; for float: fail if either is nan;
	bool option = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		if (option)
			return Glue<Value>::push(L, b->CreateICmpUGE(x, y, "icmp_uge"));
		return Glue<Value>::push(L, b->CreateICmpSGE(x, y, "icmp_sge"));
	}
	if (!option)
		return Glue<Value>::push(L, b->CreateFCmpOGE(x, y, "fcmp_oge"));
	return Glue<Value>::push(L, b->CreateFCmpUGE(x, y, "fcmp_uge"));
}
static int createCmpLE(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	// for int: option indicates unsigned; for float: fail if either is nan;
	bool option = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	bool ix = x->getType()->isInteger();
	bool iy = y->getType()->isInteger();
	if (ix && iy) {
		if (option)
			return Glue<Value>::push(L, b->CreateICmpULE(x, y, "icmp_ule"));
		return Glue<Value>::push(L, b->CreateICmpSLE(x, y, "icmp_sle"));
	}
	if (!option)
		return Glue<Value>::push(L, b->CreateFCmpOLE(x, y, "fcmp_ole"));
	return Glue<Value>::push(L, b->CreateFCmpULE(x, y, "fcmp_ule"));
}

static int createFToI(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	// option indicates unsigned
	bool option = lua_isboolean(L, 3) && lua_toboolean(L, 3);
	if (!x->getType()->isFloatingPoint())
		luaL_error(L, "attempt to cast from non-floating point type");
	if (!option)
		return Glue<Value>::push(L, b->CreateFPToSI(x, Type::getInt32Ty(getGlobalContext()), "fptosi"));
	return Glue<Value>::push(L, b->CreateFPToUI(x, Type::getInt32Ty(getGlobalContext()), "fptoui"));
}
static int createIToF(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	// option indicates unsigned
	bool option = lua_isboolean(L, 3) && lua_toboolean(L, 3);
	if (!x->getType()->isInteger())
		luaL_error(L, "attempt to cast from non-integer type");
	if (!option)
		return Glue<Value>::push(L, b->CreateSIToFP(x, Type::getDoubleTy(getGlobalContext()), "sitofp"));
	return Glue<Value>::push(L, b->CreateUIToFP(x, Type::getDoubleTy(getGlobalContext()), "uitofp"));
}

static int createCall(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Module * m = Glue<ModuleProvider>::checkto(L, 2)->getModule();	
	// get args:
	std::vector<Value *> args;
	for (int i=4; i<=lua_gettop(L); i++) {
		args.push_back(Glue<Value>::checkto(L, i));
	}
	// function may be by Function or by name:
	if (lua_isstring(L, 3)) {
		const char * fname = lua_tostring(L, 3);
		Function * f = m->getFunction(fname);
		if (f == 0)
			return luaL_error(L, "function %s not found", lua_tostring(L, 3));
		if (args.size() < f->getFunctionType()->getNumParams())
			return luaL_error(L, "insufficient arguments");

		// ok it's a function. what return type?
		if (f->getReturnType() == Type::getVoidTy(getGlobalContext())) {
			// functions returning void shouldn't be named:
			b->CreateCall(f, args.begin(), args.end());
			return 0;
		}
		return Glue<Value>::push(L, b->CreateCall(f, args.begin(), args.end(), f->getNameStr().c_str()));
	}
	Function * f = Glue<Function>::checkto(L, 3);
	FunctionType * ft = (FunctionType *)f->getFunctionType();
	if (args.size() < ft->getNumParams())
		return luaL_error(L, "insufficient arguments");
	if (f->getReturnType() == Type::getVoidTy(getGlobalContext())) {
		// functions returning void shouldn't be named:
		b->CreateCall(f, args.begin(), args.end());
		return 0;
	}
	return Glue<Value>::push(L, b->CreateCall(f, args.begin(), args.end(), "call"));
}
static int createExtractValue(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * agg = Glue<Value>::checkto(L, 2);
	if (!agg->getType()->isAggregateType())
		return luaL_error(L, "not an aggregate type");
	unsigned idx = luaL_optinteger(L, 3, 0);
	// todo: verify that agg.indices > idx
	return Glue<Value>::push(L, b->CreateExtractValue(agg, idx, "extract"));
}
static int createInsertValue(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * agg = Glue<Value>::checkto(L, 2);
	if (!agg->getType()->isAggregateType())
		return luaL_error(L, "not an aggregate type");
	unsigned idx = luaL_optinteger(L, 3, 0);
	// todo: verify that agg.indices > idx
	Value * val = Glue<Value>::checkto(L, 4);
	// verify that types match
	return Glue<Value>::push(L, b->CreateInsertValue(agg, val, idx, "insert"));
}

Value * toInteger(lua_State * L, int idx = 1, int def = 0) {
	if (lua_isnumber(L, idx))
		return ConstantInt::get((getGlobalContext()), APInt(32, lua_tointeger(L, idx)));
	Value * res = Glue<Value>::to(L, idx);
	if (res == 0)
		return ConstantInt::get((getGlobalContext()), APInt(32, def));
	return res;
}

static int createMalloc(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	const Type * t = Glue<Type>::checkto(L, 2);
	if (!t->isSized())
		return luaL_error(L, "type is not sized");
	Value * sz = toInteger(L, 3, 1);
	if (!sz->getType()->isInteger())
		return luaL_error(L, "size is not integer");
	return Glue<Instruction>::push(L, b->CreateMalloc(t, sz, "mem"));
}
static int createAlloca(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	// todo: get the current function, and insert this instruction in its entry block
	// (necessary for mem2reg optimizations to work)
	/*
	IRBuilder TmpB(&TheFunction->getEntryBlock(),
                 TheFunction->getEntryBlock().begin());
  return TmpB.CreateAlloca(Type::DoubleTy, 0, VarName.c_str());
	*/
	const Type * t = Glue<Type>::checkto(L, 2);
	if (!t->isSized())
		return luaL_error(L, "type is not sized");
	Value * sz = toInteger(L, 3, 1);
	if (!sz->getType()->isInteger())
		return luaL_error(L, "size is not integer");
	return Glue<Instruction>::push(L, b->CreateAlloca(t, sz, "mem"));
}
static int createFree(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * p = Glue<Value>::checkto(L, 2);
	if (p->getType()->getTypeID() != Type::PointerTyID)
		return luaL_error(L, "not a pointer type");
	return Glue<Instruction>::push(L, b->CreateFree(p));
}
static int createLoad(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * p = Glue<Value>::checkto(L, 2);
	if (p->getType()->getTypeID() != Type::PointerTyID)
		return luaL_error(L, "not a pointer type");
	bool isVolatile = lua_isboolean(L, 3) && lua_toboolean(L, 3);
	return Glue<Instruction>::push(L, b->CreateLoad(p, isVolatile, "load"));
}
static int createStore(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * p = Glue<Value>::checkto(L, 2);
	Value * v = Glue<Value>::checkto(L, 3);
	if (p->getType()->getTypeID() != Type::PointerTyID)
		return luaL_error(L, "not a pointer type");
	if (p->getType()->getTypeID() != PointerType::get(v->getType(), 0)->getTypeID())
		return luaL_error(L, "mismatch type");
	bool isVolatile = lua_isboolean(L, 4) && lua_toboolean(L, 4);
	return Glue<Instruction>::push(L, b->CreateStore(v, p, isVolatile));
}
static int createGEP(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * p = Glue<Value>::checkto(L, 2);
	if (p->getType()->getTypeID() != Type::PointerTyID)
		return luaL_error(L, "not a pointer type");
	std::vector<Value *> indices;
	for (int i=3; i<=lua_gettop(L); i++) {
		// todo: it's easy to index out of range here, and this causes a crash
		// but how to avoid this is hard...
		// if value is a constant, then we can try to calculate, but too much is runtime dependent
		Value * idx = toInteger(L, i, 0);
		if (!idx->getType()->isInteger())
			return luaL_error(L, "indices must be integers");
		indices.push_back(idx);
	}
	return Glue<Value>::push(L, b->CreateGEP(p, indices.begin(), indices.end(), "ptr"));
}
static int createBitCast(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * v = Glue<Value>::checkto(L, 2);
	const Type * vty = v->getType();
	if (vty->isAggregateType())
		return luaL_error(L, "cannot cast aggregate type");
	const Type * ty = Glue<Type>::checkto(L, 3);
	if (ty->isAggregateType())
		return luaL_error(L, "cannot cast to aggregate type");
	if (vty->getTypeID() == Type::PointerTyID && ty->getTypeID() != Type::PointerTyID)
		return luaL_error(L, "pointers can only be cast to pointer types");
	return Glue<Value>::push(L, b->CreateBitCast(v, ty, "cast"));
}
template<> void Glue<IRBuilder<> >::usr_mt(lua_State * L, int mt) {
	lua_pushcfunction(L, createPHI); lua_setfield(L, mt, "PHI");
	lua_pushcfunction(L, createBr); lua_setfield(L, mt, "Br");
	lua_pushcfunction(L, createCondBr); lua_setfield(L, mt, "CondBr");
	lua_pushcfunction(L, createRet); lua_setfield(L, mt, "Ret");
	lua_pushcfunction(L, createAdd); lua_setfield(L, mt, "Add");
	lua_pushcfunction(L, createSub); lua_setfield(L, mt, "Sub");
	lua_pushcfunction(L, createDiv); lua_setfield(L, mt, "Div");
	lua_pushcfunction(L, createMul); lua_setfield(L, mt, "Mul");
	lua_pushcfunction(L, createRem); lua_setfield(L, mt, "Rem");
	lua_pushcfunction(L, createNeg); lua_setfield(L, mt, "Neg");
	lua_pushcfunction(L, createNot); lua_setfield(L, mt, "Not");
	lua_pushcfunction(L, createCmpEQ); lua_setfield(L, mt, "CmpEQ");
	lua_pushcfunction(L, createCmpNE); lua_setfield(L, mt, "CmpNE");
	lua_pushcfunction(L, createCmpLE); lua_setfield(L, mt, "CmpLE");
	lua_pushcfunction(L, createCmpLT); lua_setfield(L, mt, "CmpLT");
	lua_pushcfunction(L, createCmpGE); lua_setfield(L, mt, "CmpGE");
	lua_pushcfunction(L, createCmpGT); lua_setfield(L, mt, "CmpGT");
	lua_pushcfunction(L, createFToI); lua_setfield(L, mt, "FToI");
	lua_pushcfunction(L, createIToF); lua_setfield(L, mt, "IToF");
	lua_pushcfunction(L, createBitCast); lua_setfield(L, mt, "BitCast");
	lua_pushcfunction(L, createCall); lua_setfield(L, mt, "Call");
	lua_pushcfunction(L, createExtractValue); lua_setfield(L, mt, "ExtractValue");
	lua_pushcfunction(L, createInsertValue); lua_setfield(L, mt, "InsertValue");
	lua_pushcfunction(L, createMalloc); lua_setfield(L, mt, "Malloc");
	lua_pushcfunction(L, createAlloca); lua_setfield(L, mt, "Alloca");
	lua_pushcfunction(L, createFree); lua_setfield(L, mt, "Free");
	lua_pushcfunction(L, createLoad); lua_setfield(L, mt, "Load");
	lua_pushcfunction(L, createStore); lua_setfield(L, mt, "Store");
	lua_pushcfunction(L, createGEP); lua_setfield(L, mt, "GEP");
}

/*
	DynamicLibrary:
*/
#pragma mark DynamicLibrary
int LoadLibraryPermanently(lua_State * L) {
	const char * filename = luaL_checkstring(L, 1);
	std::string ErrMsg;
	bool res = llvm::sys::DynamicLibrary::LoadLibraryPermanently(filename, &ErrMsg);
	lua_pushboolean(L, res);
	lua_pushstring(L, ErrMsg.c_str());
	return 2;
}

int SearchForAddressOfSymbol(lua_State * L) {
	const char * symbolName = luaL_checkstring(L, 1);
	lua_pushlightuserdata(L, llvm::sys::DynamicLibrary::SearchForAddressOfSymbol(symbolName));
	return 1;
} 

int AddSymbol(lua_State * L) {
	const char * symbolName = luaL_checkstring(L, 1);
	void * symbolValue = lua_touserdata(L, 2);
	llvm::sys::DynamicLibrary::AddSymbol(symbolName, symbolValue);
	return 0;
}

int luaopen_clang_llvm(lua_State * L) {

	const char * libname = "clang.llvm";
	struct luaL_reg lib[] = {
		{"LoadLibraryPermanently", LoadLibraryPermanently },
		{"SearchForAddressOfSymbol", SearchForAddressOfSymbol },
		{"AddSymbol", AddSymbol },
		{NULL, NULL},
	};
	luaL_register(L, libname, lib);	
	
	
	Glue<llvm::ModuleProvider>::define(L);	
	
	Glue<llvm::Type>::define(L);			
	Glue<llvm::StructType>::define(L);		
	Glue<llvm::SequentialType>::define(L);
	Glue<llvm::PointerType>::define(L);		
	Glue<llvm::ArrayType>::define(L);		
	Glue<llvm::VectorType>::define(L);		
	Glue<llvm::OpaqueType>::define(L);		
	Glue<llvm::FunctionType>::define(L);			
	
	Glue<llvm::Value>::define(L);		
	Glue<llvm::Argument>::define(L);		
	Glue<llvm::Instruction>::define(L);		
	Glue<llvm::PHINode>::define(L);			
	Glue<llvm::BasicBlock>::define(L);		
	Glue<llvm::Constant>::define(L);		
	Glue<llvm::GlobalValue>::define(L);		
	Glue<llvm::Function>::define(L);		
	Glue<llvm::GlobalVariable>::define(L);	
	
	Glue<IRBuilder<> >::define(L);			
	
	Glue<llvm::ModuleProvider>::register_table(L);
	
	Glue<llvm::Type>::register_table(L);
	Glue<llvm::StructType>::register_ctor(L);
	Glue<llvm::SequentialType>::register_ctor(L);
	Glue<llvm::PointerType>::register_ctor(L);
	Glue<llvm::ArrayType>::register_ctor(L);
	Glue<llvm::VectorType>::register_ctor(L);
	Glue<llvm::OpaqueType>::register_ctor(L);
	Glue<llvm::FunctionType>::register_ctor(L);
	
	Glue<llvm::Value>::register_table(L);
	Glue<llvm::Argument>::register_table(L);
	Glue<llvm::Instruction>::register_table(L);
	Glue<llvm::PHINode>::register_table(L);
	Glue<llvm::BasicBlock>::register_ctor(L);
	Glue<llvm::Constant>::register_table(L);
	Glue<llvm::GlobalValue>::register_table(L);
	Glue<llvm::Function>::register_ctor(L);
	Glue<llvm::GlobalVariable>::register_ctor(L);
	
	Glue<llvm::IRBuilder<> >::register_ctor(L);
	
	lua_newtable(L);
	lua_pushinteger(L, GlobalValue::ExternalLinkage); lua_setfield(L, -2, "External");
	lua_pushinteger(L, GlobalValue::LinkOnceAnyLinkage); lua_setfield(L, -2, "LinkOnce");
	lua_pushinteger(L, GlobalValue::WeakAnyLinkage); lua_setfield(L, -2, "Weak");
	lua_pushinteger(L, GlobalValue::AppendingLinkage); lua_setfield(L, -2, "Appending");
	lua_pushinteger(L, GlobalValue::InternalLinkage); lua_setfield(L, -2, "Internal");
	lua_pushinteger(L, GlobalValue::DLLImportLinkage); lua_setfield(L, -2, "DLLImport");
	lua_pushinteger(L, GlobalValue::DLLExportLinkage); lua_setfield(L, -2, "DLLExport");
	lua_pushinteger(L, GlobalValue::ExternalWeakLinkage); lua_setfield(L, -2, "ExternalWeak");
	lua_pushinteger(L, GlobalValue::GhostLinkage); lua_setfield(L, -2, "Ghost");
	lua_pushinteger(L, GlobalValue::CommonLinkage); lua_setfield(L, -2, "Common");
	lua_setfield(L, -2, "Linkage");
	
	
	
	return 1;
}