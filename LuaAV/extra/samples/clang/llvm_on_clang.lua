local clang = require("clang")
compiler = clang.Compiler()
compiler:include(app.resourcepath.."/Headers")
--compiler:define("__STDC_LIMIT_MACROS")

-- the following code defines a new Lua module
-- it is a binding of the LLVM library to Lua


local code = [=[

#include <stdio.h>





#define IT(n) for(int i=0; i<n; ++i)

template<typename T, typename Super>
struct Vec : public Super {
	using Super::set;
	using Super::dim;
	using Super::operator[];
	
	// no members, pure methods; most of the methods currently in Vec<N,T>
	// except for the [] operators, which will be defined in Super.
	// also, since N isn't defined, also assume a method dim() in Super.

	/// @param[in] v1		value to initialize first element
	/// @param[in] v2		value to initialize second element
	/// @param[in] v3		value to initialize third element
	Vec(const T& v1, const T& v2, const T& v3){ set(v1, v2, v3); }
	
	Vec& operator +=(const Vec& v){ IT(dim()) (*this)[i] += v[i]; return *this; }
	
	void test() {
		printf("dim: %d\n", dim());
		printf("element 0: %f\n", (float)(*this)[0]);
	}
};

template<typename T>
struct VecSuper3 {
	union {
		struct { T x, y, z; };
		T elems;
	};
	
	VecSuper3& set(const T& v1, const T& v2, const T& v3){
		x = v1; y = v2; z = v3;
		return *this;
	}

	/// Set element at index with no bounds checking
	T& operator[](int i){ return elems[i];}
	
	/// Get element at index with no bounds checking
	const T& operator[](int i) const { return elems[i]; }
	
	/// instead of N:
	static unsigned dim() { return 3; }

	// plus any methods that only make sense for vectors of dim 3
};

// ... etc for VecSuper2, VecSuper4 etc.

typedef Vec<float, VecSuper3<float> > Vec3f;			

void test() {
	Vec3f example(1, 2, 3);
	printf("%f, %f, %f\n", example.x, example.y, example.z);
}

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include "lua.hpp"
#include "lua_glue.h"
#include "lua_utility.h"

#include "llvm/Analysis/Verifier.h"
#include "llvm/Assembly/AssemblyAnnotationWriter.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/DerivedTypes.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/LLVMContext.h"
#include "llvm/Module.h"
#include "llvm/PassManager.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Target/TargetSelect.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"

#include <sstream>

using namespace llvm;

static llvm::ExecutionEngine * EE = 0;

template<> const char * Glue<Module>::usr_name() { return "Module"; }
template<> Module * Glue<Module>::usr_new(lua_State * L) {
	return new Module(luaL_checkstring(L, 1), getGlobalContext());
}
template<> int Glue<Module>::usr_tostring(lua_State * L, Module * u) { 
	const std::string & id = u->getModuleIdentifier();
	lua_pushfstring(L, "%s(%s)", usr_name(), id.c_str()); 
	return 1; 
}
static int module_bitcode(lua_State * L) { 
	Module * u = Glue<Module>::checkto(L, 1);
	AssemblyAnnotationWriter aaw;
	std::string os;
	raw_string_ostream s(os);
	u->print(s, &aaw);
	lua_pushstring(L, os.c_str());
	return 1;
}

static int module_optimize(lua_State * L) {
	Module * u = Glue<Module>::checkto(L, 1);
	std::string olevel = luaL_optstring(L, 2, "O2");
	llvm::TargetData * targetdata = new llvm::TargetData(u);
	llvm::PassManager pm;
	pm.add(targetdata);
	
	if (olevel == std::string("verify")) {
		pm.add(llvm::createVerifierPass(llvm::PrintMessageAction));
	} else if (olevel == std::string("O1")) {
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
	
	pm.run(*u);
}

static int module_verify(lua_State * L) {
	Module * u = Glue<Module>::checkto(L, 1);
	std::string errorInfo;
	lua_pushboolean(L, !llvm::verifyModule(*u, llvm::PrintMessageAction, &errorInfo));
	lua_pushstring(L, errorInfo.c_str());
	return 2;
}

static int module_jit(lua_State * L) {
	Module * u = Glue<Module>::checkto(L, 1);
	std::string err;
	if (EE==0) {
		EE = llvm::EngineBuilder(u)
			.setEngineKind(llvm::EngineKind::JIT)
			.setErrorStr(&err)
			.setOptLevel(llvm::CodeGenOpt::Default)
			//.setMAttrs("-avx")
			.setMCPU("core2")
			.create();
		
		if (EE == 0) {
			printf("Failed to create Execution Engine: %s", err.c_str());
			return 0;
		}
	} else {
		EE->addModule(u);
	}
	
	// initialize the statics:
	// EE->runStaticConstructorsDestructors(mImpl->module, false);	
	
	return 0;
}

int module_jit_getfunctionptr(lua_State *L) {
	Module * u = Glue<Module>::checkto(L, 1);
	std::string funcname = luaL_checkstring(L, 2);
	if (EE) {
		llvm::StringRef fname = llvm::StringRef(funcname);
		llvm::Function * f = u->getFunction(fname);
		if (f) {
			lua_pushlightuserdata(L, EE->getPointerToFunction(f));
			return 1;
		}
	}
	return 0;
}
int module_jit_getluafunction(lua_State *L) {
	Module * u = Glue<Module>::checkto(L, 1);
	std::string funcname = luaL_checkstring(L, 2);
	if (EE) {
		llvm::StringRef fname = llvm::StringRef(funcname);
		llvm::Function * f = u->getFunction(fname);
		if (f) {
			lua_pushcfunction(L, (lua_CFunction)EE->getPointerToFunction(f));
			return 1;
		}
	}
	return 0;
}
int module_jit_getglobalptr(lua_State *L) {
	Module * u = Glue<Module>::checkto(L, 1);
	std::string globalname = luaL_checkstring(L, 2);
	if (EE) {
		const llvm::GlobalVariable * GV = u->getGlobalVariable(globalname);
		if (GV) {
			lua_pushlightuserdata(L, EE->getOrEmitGlobalVariable(GV));
			return 1;
		} else {
			printf("global %s not found\n", globalname.data());
		}
	}
	return 0;
}

template<> void Glue<Module>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, module_bitcode); lua_setfield(L, -2, "bitcode");
	lua_pushcfunction(L, module_optimize); lua_setfield(L, -2, "optimize");
	lua_pushcfunction(L, module_verify); lua_setfield(L, -2, "verify");
	lua_pushcfunction(L, module_jit); lua_setfield(L, -2, "jit");
	lua_pushcfunction(L, module_jit_getfunctionptr); lua_setfield(L, -2, "getfunctionptr");
	lua_pushcfunction(L, module_jit_getluafunction); lua_setfield(L, -2, "getluafunction");
	lua_pushcfunction(L, module_jit_getglobalptr); lua_setfield(L, -2, "getglobalptr");
}

// used by almost all LLVM types:
template<typename T>
int llvm_print(lua_State * L, T * u) {
	std::string os;
	raw_string_ostream s(os);
	u->print(s);
	lua_pushfstring(L, "%s(%s)", Glue<T>::usr_name(), os.c_str());
	return 1;
}

template<> const char * Glue<Type>::usr_name() { return "Type"; }
template<> int Glue<Type>::usr_tostring(lua_State * L, Type * t) { return llvm_print(L, t); }

int type_isIntegerTy(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isIntegerTy());
	return 1;
}
int type_getTypeID(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushinteger(L, t->getTypeID());
	return 1;
}
int type_isFloatingPointTy(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isFloatingPointTy());
	return 1;
}
int type_isAbstract(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isAbstract());
	return 1;
}
int type_isSized(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	lua_pushboolean(L, t->isSized());
	return 1;
}
int type_isValidReturnType(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L);
	lua_pushboolean(L, FunctionType::isValidReturnType(t));
	return 1;
}
/*

static int type_pointer(lua_State * L) {
	const Type * t = Glue<Type>::checkto(L, 1);
	int addressSpace = luaL_optinteger(L, 2, 0);
	if (t->getTypeID() == Type::VoidTyID) {
		// special case for void *:
		return Glue<PointerType>::push(L, PointerType::get(Type::getInt8Ty(getGlobalContext()), addressSpace));
	}
	return Glue<PointerType>::push(L, PointerType::get(t, addressSpace));
}
*/

template<> void Glue<Type>::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, type_isIntegerTy); lua_setfield(L, mt, "isIntegerTy");
	lua_pushcfunction(L, type_isFloatingPointTy); lua_setfield(L, mt, "isFloatingPointTy");
	lua_pushcfunction(L, type_isAbstract); lua_setfield(L, mt, "isAbstract");
	lua_pushcfunction(L, type_isSized); lua_setfield(L, mt, "isSized");
	lua_pushcfunction(L, type_isValidReturnType); lua_setfield(L, mt, "isValidReturnType");
	lua_pushcfunction(L, type_getTypeID); lua_setfield(L, mt, "getTypeID");
	
	lua_pushcfunction(L, Glue<Constant>::create); lua_setfield(L, mt, "__call");
	
	/*
	//lua_pushcfunction(L, type_modname); lua_setfield(L, mt, "name");
	//lua_pushcfunction(L, type_sizeinbits); lua_setfield(L, mt, "sizeinbits");
	//lua_pushcfunction(L, type_sizeABI); lua_setfield(L, mt, "size");
	//lua_pushcfunction(L, ee_offsetOf); lua_setfield(L, mt, "offsetOf");
	lua_pushcfunction(L, type_pointer); lua_setfield(L, mt, "pointer");
	*/
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
static int structtype_getElementType(lua_State * L) {
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
template<> void Glue<StructType>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, structtype_len); lua_setfield(L, -2, "__len");
	lua_pushcfunction(L, structtype_getElementType); lua_setfield(L, -2, "type");
	lua_pushcfunction(L, structtype_gettypes); lua_setfield(L, -2, "types");
}

/*
	SequentialType : CompositeType : DerivedType : Type
*/
#pragma mark SequentialType
template<> const char * Glue<SequentialType>::usr_name() { return "SequentialType"; }
template<> const char * Glue<SequentialType>::usr_supername() { return "Type"; }
template<> int Glue<SequentialType>::usr_tostring(lua_State * L, SequentialType * t) { return llvm_print<SequentialType>(L, t); }
static int sequentialtype_getElementType(lua_State * L) {
	SequentialType * u = Glue<SequentialType>::checkto(L, 1);
	return Glue<Type>::push(L, (Type *)u->getElementType());
}
template<> void Glue<SequentialType>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, sequentialtype_getElementType); lua_setfield(L, -2, "type");
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
template<> void Glue<ArrayType>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, arraytype_len); lua_setfield(L, -2, "__len");
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
template<> void Glue<VectorType>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, vectortype_len); lua_setfield(L, -2, "__len");
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
static int functiontype_isVarArg(lua_State * L) {
	FunctionType * f = Glue<FunctionType>::checkto(L, 1);
	lua_pushinteger(L, f->isVarArg());
	return 1;
}
static int functiontype_getParamType(lua_State * L) {
	FunctionType * u = Glue<FunctionType>::checkto(L, 1);
	unsigned int i = luaL_checkinteger(L, 2);
	if (i >= u->getNumParams())
		return luaL_error(L, "FunctionType has only %d params (requested %d)", u->getNumParams(), i);
	return Glue<Type>::push(L, (Type *)u->getParamType(i));
}
static int functiontype_getReturnType(lua_State * L) {
	FunctionType * u = Glue<FunctionType>::checkto(L, 1);
	return Glue<Type>::push(L, (Type *)u->getReturnType());
}
static int functiontype_len(lua_State * L) {
	FunctionType * u = Glue<FunctionType>::checkto(L, 1);
	lua_pushinteger(L, u->getNumParams());
	return 1;
}
template<> void Glue<FunctionType>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, functiontype_len); lua_setfield(L, -2, "__len");
	lua_pushcfunction(L, functiontype_isVarArg); lua_setfield(L, -2, "isVarArg");
	lua_pushcfunction(L, functiontype_getParamType); lua_setfield(L, -2, "getParamType");
	lua_pushcfunction(L, functiontype_getReturnType); lua_setfield(L, -2, "getReturnType");
}

/*
	Value
*/
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
static int value_getType(lua_State * L) {
	Value * u = Glue<Value>::checkto(L);
	return Glue<Type>::push(L, (Type *)u->getType());
}
static int value_getName(lua_State * L) {
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
template<> void Glue<Value>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, value_getType); lua_setfield(L, -2, "getType");
	lua_pushcfunction(L, value_getName); lua_setfield(L, -2, "getName");
	lua_pushcfunction(L, value_replace); lua_setfield(L, -2, "replace");
	lua_pushcfunction(L, value_uses); lua_setfield(L, -2, "uses");
}

static int constant_Double(lua_State * L) {
	Glue<Value>::push(L, ConstantFP::get(Type::getDoubleTy(getGlobalContext()), lua_tonumber(L, 1)));
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
template<> void Glue<Argument>::usr_mt(lua_State * L) {
//	lua_pushcfunction(L, argument_parent); 		lua_setfield(L, -2, "parent");
//	lua_pushcfunction(L, argument_argno); 		lua_setfield(L, -2, "argno");
//	lua_pushcfunction(L, argument_byval); 		lua_setfield(L, -2, "byval");
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
template<> void Glue<Instruction>::usr_mt(lua_State * L) {
	lua_pushcfunction(L, inst_parent); lua_setfield(L, -2, "parent");
	lua_pushcfunction(L, inst_erase); lua_setfield(L, -2, "erase");
	lua_pushcfunction(L, inst_opcode); lua_setfield(L, -2, "opcode");
}

/*
	PHINode : Instruction
*/
#pragma mark PHINode
template<> const char * Glue<PHINode>::usr_name() { return "PHINode"; }
template<> const char * Glue<PHINode>::usr_supername() { return "Instruction"; }
template<> int Glue<PHINode>::usr_tostring(lua_State * L, PHINode * u) { return llvm_print<PHINode>(L, u); }
static int phi_addIncoming(lua_State * L) {
	PHINode * u = Glue<PHINode>::checkto(L, 1);
	Value * v = Glue<Value>::checkto(L, 2);
	BasicBlock * block = Glue<BasicBlock>::checkto(L, 3);
	u->addIncoming(v, block);
	return 0;
}
template<> void Glue<PHINode>::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, phi_addIncoming); lua_setfield(L, mt, "addIncoming");
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
template<> void Glue<BasicBlock>::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, block_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, block_instruction); lua_setfield(L, mt, "instruction");
	lua_pushcfunction(L, block_parent); lua_setfield(L, mt, "parent");
	lua_pushcfunction(L, block_front); lua_setfield(L, mt, "front");
	lua_pushcfunction(L, block_back); lua_setfield(L, mt, "back");
	lua_pushcfunction(L, block_erase); lua_setfield(L, mt, "erase");
	lua_pushcfunction(L, block_terminator); lua_setfield(L, mt, "terminator");
}

/*
	Constant : User : Value
*/
template<> const char * Glue<Constant>::usr_name() { return "Constant"; }
template<> const char * Glue<Constant>::usr_supername() { return "Value"; }
template<> int Glue<Constant>::usr_tostring(lua_State * L, Constant * u) { return llvm_print<Constant>(L, u); }
template<> Constant * Glue<Constant>::usr_new(lua_State * L) {
	// check if first argument is a type:
	Type * ty = Glue<Type>::to(L, 1);
	if (ty) {
		if (ty == Type::getDoubleTy(getGlobalContext())) {
			return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), lua_tonumber(L, 2));
		} else if (ty == Type::getFloatTy(getGlobalContext())) {
			return ConstantFP::get(Type::getFloatTy(getGlobalContext()), lua_tonumber(L, 2));
		} else if (ty == Type::getInt1Ty(getGlobalContext())) {
			return ConstantInt::get((getGlobalContext()), APInt(1, lua_toboolean(L, 2)));
		} else if (ty == Type::getInt8Ty(getGlobalContext())) {
			return ConstantInt::get((getGlobalContext()), APInt(8, lua_tonumber(L, 2)));
		} else if (ty == Type::getInt16Ty(getGlobalContext())) {
			return ConstantInt::get((getGlobalContext()), APInt(16, lua_tonumber(L, 2)));
		} else if (ty == Type::getInt32Ty(getGlobalContext())) {
			return ConstantInt::get((getGlobalContext()), APInt(32, lua_tonumber(L, 2)));
		} else if (ty == Type::getInt64Ty(getGlobalContext())) {
			return ConstantInt::get((getGlobalContext()), APInt(64, lua_tonumber(L, 2)));
		}
		return 0;
	}
	return (Constant *)Glue<Value>::usr_reinterpret(L, 1);
}


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

/*
static int global_getptr(lua_State * L) {
	GlobalValue * u = Glue<GlobalValue>::checkto(L, 1);
	if (EE==NULL) luaL_error(L, "No Execution Engine");
	void * ptr = EE->getPointerToGlobalIfAvailable(u);
	if (ptr) {
		lua_pushlightuserdata(L, ptr);
		return 1;
	}
	return 0;
}*/

template<> void Glue<GlobalValue>::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, global_linkage); lua_setfield(L, mt, "linkage");
	lua_pushcfunction(L, global_isdeclaration); lua_setfield(L, mt, "isdeclaration");
	//lua_pushcfunction(L, global_getptr); lua_setfield(L, mt, "getptr");
	//lua_pushcfunction(L, ee_addGlobalMapping); lua_setfield(L, mt, "addGlobalMapping");
}
// likely methods:	getParent() (returns a Module *)


/*
	GlobalVariable : GlobalValue

#pragma mark GlobalVariable
template<> const char * Glue<GlobalVariable>::usr_name() { return "GlobalVariable"; }
template<> const char * Glue<GlobalVariable>::usr_supername() { return "GlobalValue"; }
template<> int Glue<GlobalVariable>::usr_tostring(lua_State * L, GlobalVariable * u) { return llvm_print<GlobalVariable>(L, u); }
template<> GlobalVariable * Glue<GlobalVariable>::usr_new(lua_State * L) {
	Module * m = Glue<Module>::checkto(L, 1);
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
template<> void Glue<GlobalVariable>::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, globalvar_erase); lua_setfield(L, mt, "erase");
	lua_pushcfunction(L, globalvar_initializer); lua_setfield(L, mt, "initializer");
	lua_pushcfunction(L, globalvar_isconstant); lua_setfield(L, mt, "isconstant");
}*/

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
	Module * M = Glue<Module>::checkto(L, 1);
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
/*static int function_callingconv(lua_State * L) {
	Function * f = Glue<Function>::checkto(L, 1);
	if (lua_isnumber(L, 2)) {
		unsigned cc = lua_tonumber(L, 2);
		f->setCallingConv(cc);
	}
	lua_pushinteger(L, f->getCallingConv());
	return 1;
}*/
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
/*static int function_getptr(lua_State * L) {
	Function * u = Glue<Function>::to(L, 1);
	if (EE==NULL) luaL_error(L, "No Execution Engine");
	// NOTE: this causes codegeneration of F and any dependents:
	void * f = EE->getPointerToFunction(u);
	lua_pushlightuserdata(L, f);
	return 1;
}*/
//static int function_optimize(lua_State * L) {
//	Function * f = Glue<Function>::checkto(L, 1);
//	lua_pushboolean(L, getLModule(L)->optimize(f));
//	return 1;
//}
/*
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
}*/
template<> void Glue<Function>::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, function_len); lua_setfield(L, mt, "__len");
	lua_pushcfunction(L, function_intrinsic); lua_setfield(L, mt, "intrinsic");
	lua_pushcfunction(L, function_deletebody); lua_setfield(L, mt, "deletebody");
	lua_pushcfunction(L, function_erase); lua_setfield(L, mt, "erase");
	//lua_pushcfunction(L, function_callingconv); lua_setfield(L, mt, "callingconv");
	lua_pushcfunction(L, function_argument); lua_setfield(L, mt, "argument");
	lua_pushcfunction(L, function_block); lua_setfield(L, mt, "block");
	lua_pushcfunction(L, function_verify); lua_setfield(L, mt, "verify");
	//lua_pushcfunction(L, function_getptr); lua_setfield(L, mt, "getptr");
	//lua_pushcfunction(L, function_optimize); lua_setfield(L, mt, "optimize");
	//lua_pushcfunction(L, function_pushcfunction); lua_setfield(L, mt, "pushcfunction");
}

/*
	IRBuilder
*/
#pragma mark IRBuilder
template<> const char * Glue<IRBuilder<> >::usr_name() { return "IRBuilder"; }
template<> IRBuilder<> * Glue<IRBuilder<> >::usr_new(lua_State * L) {
	return new IRBuilder<>(getGlobalContext());
}
static int setInsertPoint(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	BasicBlock * bb = Glue<BasicBlock>::checkto(L, 2);
	b->SetInsertPoint(bb);
	return 0;
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
	if (ix && iy) {
		return Glue<Value>::push(L, b->CreateAdd(x, y, "add"));
	} else if (!ix && !iy) {
		return Glue<Value>::push(L, b->CreateFAdd(x, y, "add"));
	} else {
		luaL_error(L, "operand types do not match");
	}
}
static int createSub(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
	if (ix && iy) {
		return Glue<Value>::push(L, b->CreateSub(x, y, "sub"));
	} else if (!ix && !iy) {
		return Glue<Value>::push(L, b->CreateFSub(x, y, "sub"));
	} else {
		luaL_error(L, "operand types do not match");
	}
}
static int createMul(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
	if (ix && iy) {
		return Glue<Value>::push(L, b->CreateMul(x, y, "mul"));
	} else if (!ix && !iy) {
		return Glue<Value>::push(L, b->CreateFMul(x, y, "mul"));
	} else {
		luaL_error(L, "operand types do not match");
	}
}
static int createDiv(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
	if (ix && iy) {
		// todo check for unsignedness
		return Glue<Value>::push(L, b->CreateSDiv(x, y, "sdiv"));
	} else if (!ix && !iy) {
		return Glue<Value>::push(L, b->CreateFDiv(x, y, "fdiv"));
	} else {
		luaL_error(L, "operand types do not match");
	}
}
static int createRem(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Value * x = Glue<Value>::checkto(L, 2);
	Value * y = Glue<Value>::checkto(L, 3);
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
	if (ix && iy) {
		// todo check for unsignedness
		return Glue<Value>::push(L, b->CreateSRem(x, y, "srem"));
	} else if (!ix && !iy) {
		return Glue<Value>::push(L, b->CreateFRem(x, y, "frem"));
	} else {
		luaL_error(L, "operand types do not match");
	}
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
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
	bool ix = x->getType()->isIntegerTy();
	bool iy = y->getType()->isIntegerTy();
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
	if (!x->getType()->isFloatingPointTy())
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
	if (!x->getType()->isIntegerTy())
		luaL_error(L, "attempt to cast from non-integer type");
	if (!option)
		return Glue<Value>::push(L, b->CreateSIToFP(x, Type::getDoubleTy(getGlobalContext()), "sitofp"));
	return Glue<Value>::push(L, b->CreateUIToFP(x, Type::getDoubleTy(getGlobalContext()), "uitofp"));
}

static int createCall(lua_State * L) {
	IRBuilder<> * b = Glue<IRBuilder<> >::checkto(L, 1);
	Module * m = Glue<Module>::to(L, 2);
	std::vector<Value *> args;
	if (m) {
		const char * fname = luaL_checkstring(L, 3);
		Function * f = m->getFunction(fname);
		if (f == 0)
			return luaL_error(L, "function %s not found", fname);
		// get args:
		for (int i=4; i<=lua_gettop(L); i++) {
			args.push_back(Glue<Value>::checkto(L, i));
		}
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
	Function * f = Glue<Function>::checkto(L, 2);
	FunctionType * ft = (FunctionType *)f->getFunctionType();
	// get args:
	for (int i=3; i<=lua_gettop(L); i++) {
		args.push_back(Glue<Value>::checkto(L, i));
	}
	if (args.size() < ft->getNumParams())
		return luaL_error(L, "insufficient arguments");
	if (f->getReturnType() == Type::getVoidTy(getGlobalContext())) {
		// functions returning void shouldn't be named:
		b->CreateCall(f, args.begin(), args.end());
		return 0;
	}
	return Glue<Value>::push(L, b->CreateCall(f, args.begin(), args.end(), "call"));
	return 0;
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
	if (!sz->getType()->isIntegerTy())
		return luaL_error(L, "size is not integer");
	return Glue<Instruction>::push(L, b->CreateAlloca(t, sz, "mem"));
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
		if (!idx->getType()->isIntegerTy())
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
template<> void Glue<IRBuilder<> >::usr_mt(lua_State * L) {
	int mt = lua_gettop(L);
	lua_pushcfunction(L, setInsertPoint); lua_setfield(L, mt, "setInsertPoint");
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
	lua_pushcfunction(L, createAlloca); lua_setfield(L, mt, "Alloca");
	lua_pushcfunction(L, createLoad); lua_setfield(L, mt, "Load");
	lua_pushcfunction(L, createStore); lua_setfield(L, mt, "Store");
	lua_pushcfunction(L, createGEP); lua_setfield(L, mt, "GEP");
}


extern "C" int luaopen_llvm(lua_State * L) {
	lua_newtable(L);
	
	Glue<Module>::define(L);
	
	Glue<Type>::define(L);	
	Glue<StructType>::define(L);
	Glue<SequentialType>::define(L);
	Glue<PointerType>::define(L);
	Glue<ArrayType>::define(L);
	Glue<VectorType>::define(L);
	Glue<OpaqueType>::define(L);
	Glue<FunctionType>::define(L);
	
	Glue<Value>::define(L);	
	Glue<Argument>::define(L);	
	Glue<Instruction>::define(L);	
	Glue<PHINode>::define(L);	
	Glue<BasicBlock>::define(L);		
	Glue<Constant>::define(L);
	Glue<GlobalValue>::define(L);
	Glue<Function>::define(L);
	
	Glue<IRBuilder<> >::define(L); 	
	
	Glue<Module>::register_ctor(L);
	
	Glue<StructType>::register_ctor(L);
	Glue<PointerType>::register_ctor(L);
	Glue<ArrayType>::register_ctor(L);
	Glue<VectorType>::register_ctor(L);
	Glue<OpaqueType>::register_ctor(L);
	Glue<FunctionType>::register_ctor(L);
	
	Glue<Constant>::register_ctor(L);		
	Glue<Function>::register_ctor(L);		
	Glue<BasicBlock>::register_ctor(L);	
	
	Glue<IRBuilder<> >::register_ctor(L);
	
	//lua_pushcfunction(L, constant_Double); lua_setfield(L, -2, "
	
	Glue<Type>::push(L, (Type *)Type::getVoidTy(getGlobalContext())); lua_setfield(L, -2, "Void");
	Glue<Type>::push(L, (Type *)Type::getLabelTy(getGlobalContext())); lua_setfield(L, -2, "Label");
	Glue<Type>::push(L, (Type *)Type::getFloatTy(getGlobalContext())); lua_setfield(L, -2, "Float");
	Glue<Type>::push(L, (Type *)Type::getDoubleTy(getGlobalContext())); lua_setfield(L, -2, "Double");
	Glue<Type>::push(L, (Type *)Type::getInt1Ty(getGlobalContext())); lua_setfield(L, -2, "Int1");
	Glue<Type>::push(L, (Type *)Type::getInt8Ty(getGlobalContext())); lua_setfield(L, -2, "Int8");
	Glue<Type>::push(L, (Type *)Type::getInt16Ty(getGlobalContext())); lua_setfield(L, -2, "Int16");
	Glue<Type>::push(L, (Type *)Type::getInt32Ty(getGlobalContext())); lua_setfield(L, -2, "Int32");
	Glue<Type>::push(L, (Type *)Type::getInt64Ty(getGlobalContext())); lua_setfield(L, -2, "Int64");
	
	return 1;
}

]=]

-- compile the code above:
assert(compiler:compile(code))
compiler:optimize()
local jit = assert(compiler:jit())
local luaopen_llvm = assert(jit:pushcfunction("luaopen_llvm"))

-- call the JIT compiled function to load this module into Lua:
local llvm = luaopen_llvm()

-- show what is in the module:
for k, v in pairs(llvm) do
	print("llvm.", k, v)
end

------------------------------------------------------------------------
-- Demonstration 
------------------------------------------------------------------------

-- this is the function we are going to build
--[[
; as Scheme code:
(define (factorial n)
 (if (= n 0)
     1
     (* n (factorial (- n 1)))))

// as C code:
double factorial(double n);     
double factorial(double n) {
	if (n==0) {
		return 1;
	} else {
		return n * factorial(n-1);
	}
}

// non-recursive version:
double factorial(double n) {
	int r = 1;
	while (n > 0) {
		r = r * n;
		n = n - 1;
	}
	return r;
}

// but how do we bind with this?
int lua_factorial(lua_State * L) {
	int n = luaL_checknumber(L, 1);
	lua_pushnumber(L, factorial(n));
	return 1;
}

// LLVM IR (SSA):
define i32 @factorial(i32 %arg0) {
entry:
  br label %loopcond

loopcond:                                         ; preds = %loopbody, %entry
  %n = phi i32 [ %arg0, %entry ], [ %sub, %loopbody ]
  %r = phi i32 [ 1, %entry ], [ %mul, %loopbody ]
  %icmp_sgt = icmp sgt i32 %n, 0
  br i1 %icmp_sgt, label %loopbody, label %loopend

loopbody:                                         ; preds = %loopcond
  %mul = mul i32 %r, %n
  %sub = sub i32 %n, 1
  br label %loopcond

loopend:                                          ; preds = %loopcond
  ret i32 %r
}
--]]


-- create a new module to contain LLVM IR:
local M = llvm.Module("myModule")
-- create a utility object for building IR:
local ib = llvm.IRBuilder()

-- create opaque type for lua_State:
local luaStatePointerTy = llvm.PointerType(llvm.OpaqueType())
local luaNumberTy = llvm.Double

-- declarations:
local luaL_checknumber = llvm.Function(M, "luaL_checknumber", llvm.FunctionType(luaNumberTy, luaStatePointerTy, llvm.Int32))
local lua_pushnumber = llvm.Function(M, "lua_pushnumber", llvm.FunctionType(llvm.Void, luaStatePointerTy, llvm.Double))

-- create a new function, with signature: int function(int)
local factorial = llvm.Function(M, "factorial", llvm.FunctionType(luaNumberTy, luaNumberTy))
local n = factorial:argument(0)
local entry = llvm.BasicBlock("entry", factorial)
local loopcond = llvm.BasicBlock("loopcond", factorial)
local loopbody = llvm.BasicBlock("loopbody", factorial)
local loopend = llvm.BasicBlock("loopend", factorial)

ib:setInsertPoint(entry)
ib:Br(loopcond)

ib:setInsertPoint(loopcond)
local nphi = ib:PHI(luaNumberTy, "n")
nphi:addIncoming(n, entry)
local rphi = ib:PHI(luaNumberTy, "r")
rphi:addIncoming(luaNumberTy(1), entry)
local cond = ib:CmpGT(nphi, luaNumberTy(0))
ib:CondBr(cond, loopbody, loopend)

ib:setInsertPoint(loopbody)
local r1 = ib:Mul(rphi, nphi)
rphi:addIncoming(r1, loopbody)
local n1 = ib:Sub(nphi, luaNumberTy(1))
nphi:addIncoming(n1, loopbody)
ib:Br(loopcond)

ib:setInsertPoint(loopend)
ib:Ret(rphi)


-- create the lua binding function:
local F = llvm.Function(M, "lua_factorial", llvm.FunctionType(llvm.Int32, luaStatePointerTy))
local L = F:argument(0)
local entry = llvm.BasicBlock("entry", F)
ib:setInsertPoint(entry)
local n = ib:Call(luaL_checknumber, L, llvm.Int32(1))
local r = ib:Call(factorial, n)
ib:Call(lua_pushnumber, L, r)
ib:Ret(llvm.Int32(1))


-- show the result:
print(M:bitcode())

M:optimize()
-- show the optimized version (has inlined factorial into lua_factorial)
print(M:bitcode())

-- Next: compile it!
M:jit()
local factorial = M:getluafunction("lua_factorial")
print(factorial)

-- next: use it!
for i = 0, 4 do
	print(i, factorial(i))
end
