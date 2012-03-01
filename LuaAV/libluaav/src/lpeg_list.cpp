#include "luaav_state.hpp"

/*
** $Id: lpeg_list.cpp 4216 2011-05-13 17:18:06Z whsmith $
** LPeg - PEG pattern matching for Lua
** Copyright 2007, Lua.org & PUC-Rio  (see 'lpeg.html' for license)
** written by Roberto Ierusalimschy
*/


#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "lua.h"
#include "lauxlib.h"


#define VERSION         "0.9"
#define PATTERN_T       "pattern"

/* maximum call/backtrack levels */
#define MAXBACK         400

/* initial size for capture's list */
#define IMAXCAPTURES    900


/* index, on Lua stack, for subject */
#define SUBJIDX         2

/* number of fixed arguments to 'match' (before capture arguments) */
#define FIXEDARGS       3

/* index, on Lua stack, for substitution value cache */
#define subscache(cs)   ((cs)->ptop + 1)

/* index, on Lua stack, for capture list */
#define caplistidx(ptop)        ((ptop) + 2)

/* index, on Lua stack, for pattern's fenv */
#define penvidx(ptop)   ((ptop) + 3)

/* index, on Lua stack, for table holding lists being traversed */
#define plistidx(ptop)  ((ptop) + 4)

/* index, on Lua stack, for table holding runtime capture values */
#define pdyncapidx(ptop) ((ptop) + 5)

typedef unsigned char byte;


#define CHARSETSIZE             ((UCHAR_MAX/CHAR_BIT) + 1)


typedef byte Charset[CHARSETSIZE];


typedef enum StrKind {
  Sstring, Slist, Sempty
} StrKind;

typedef struct Stream {
  StrKind kind;
  union {
    struct {
      const char *o;
      const char *s;
      const char *e;
    } s;
    struct {
      int ref;
      int cur;
    } l;
  } u;
} Stream;


typedef Stream *(*PattFunc) (const void *ud, Stream *s);


/* Virtual Machine's instructions */
typedef enum Opcode {
  IAny, IChar, ISet, ISpan,
  IRet, IEnd,
  IChoice, IJmp, ICall, IOpenCall,
  ICommit, IPartialCommit, IBackCommit, IFailTwice, IFail, IGiveup,
  IFunc,
  IFullCapture, IEmptyCapture, IEmptyCaptureIdx,
  IOpenCapture, ICloseCapture, ICloseRunTime, IOpen, IClose, INotAny, IString,
  IPartialCloseCommit, IAnySpan
} Opcode;


#define ISJMP           1
#define ISCHECK         (ISJMP << 1)
#define ISNOFAIL        (ISCHECK << 1)
#define ISCAPTURE       (ISNOFAIL << 1)
#define ISMOVABLE       (ISCAPTURE << 1)
#define ISFENVOFF       (ISMOVABLE << 1)
#define HASCHARSET      (ISFENVOFF << 1)

static const byte opproperties[] = {
  /* IAny */            ISCHECK,
  /* IChar */           ISCHECK,
  /* ISet */            ISCHECK | HASCHARSET,
  /* ISpan */           ISNOFAIL | HASCHARSET,
  /* IRet */            0,
  /* IEnd */            0,
  /* IChoice */         ISJMP,
  /* IJmp */            ISJMP | ISNOFAIL,
  /* ICall */           ISJMP,
  /* IOpenCall */       ISFENVOFF,
  /* ICommit */         ISJMP,
  /* IPartialCommit */  ISJMP,
  /* IBackCommit */     ISJMP,
  /* IFailTwice */      0,
  /* IFail */           0,
  /* IGiveup */         0,
  /* IFunc */           0,
  /* IFullCapture */    ISCAPTURE | ISNOFAIL | ISFENVOFF,
  /* IEmptyCapture */   ISCAPTURE | ISNOFAIL | ISMOVABLE,
  /* IEmptyCaptureIdx */ISCAPTURE | ISNOFAIL | ISMOVABLE | ISFENVOFF,
  /* IOpenCapture */    ISCAPTURE | ISNOFAIL | ISMOVABLE | ISFENVOFF,
  /* ICloseCapture */   ISCAPTURE | ISNOFAIL | ISMOVABLE | ISFENVOFF,
  /* ICloseRunTime */   ISCAPTURE | ISFENVOFF,
  /* IOpen */           0,
  /* IClose */          0,
  /* INotAny */         ISCHECK,
  /* IString */         ISCHECK | ISFENVOFF,
  /* IPartialCloseCommit */    ISJMP,
  /* IAnySpan */        ISNOFAIL
};


typedef union Instruction {
  struct Inst {
    byte code;
    byte aux;
    short offset;
  } i;
  PattFunc f;
  byte buff[1];
} Instruction;

static const Instruction giveup = {{IGiveup, 0, 0}};

#define getkind(op)     ((op)->i.aux & 0xF)
#define getoff(op)      (((op)->i.aux >> 4) & 0xF)

#define dest(p,x)       ((x) + ((p)+(x))->i.offset)

#define MAXOFF          0xF

#define isprop(op,p)    (opproperties[(op)->i.code] & (p))
#define isjmp(op)       isprop(op, ISJMP)
#define iscapture(op)   isprop(op, ISCAPTURE)
#define ischeck(op)     (isprop(op, ISCHECK) && (op)->i.offset == 0)
#define istest(op)      (isprop(op, ISCHECK) && (op)->i.offset != 0)
#define isnofail(op)    isprop(op, ISNOFAIL)
#define ismovable(op)   isprop(op, ISMOVABLE)
#define isfenvoff(op)   isprop(op, ISFENVOFF)
#define hascharset(op)  isprop(op, HASCHARSET)


/* kinds of captures */
typedef enum CapKind {
  Cclose, Cposition, Cconst, Cbackref, Carg, Csimple, Ctable, Cfunction,
  Cquery, Cstring, Csubst, Cfold, Cruntime, Cgroup
} CapKind;

#define iscapnosize(k)  ((k) == Cposition || (k) == Cconst)


typedef struct Capture {
  Stream s;  /* position */
  short idx;
  byte kind;
  byte siz;
} Capture;

/* maximum size (in elements) for a pattern */
#define MAXPATTSIZE     (SHRT_MAX - 10)


/* size (in elements) for an instruction plus extra l bytes */
#define instsize(l)     (((l) - 1)/sizeof(Instruction) + 2)


/* size (in elements) for a ISet instruction */
#define CHARSETINSTSIZE         instsize(CHARSETSIZE)



#define loopset(v,b)    { int v; for (v = 0; v < CHARSETSIZE; v++) b; }


#define testchar(st,c)  (((int)(st)[((c) >> 3)] & (1 << ((c) & 7))))
#define setchar(st,c)   ((st)[(c) >> 3] |= (1 << ((c) & 7)))



static int sizei (const Instruction *i) {
  if (hascharset(i)) return CHARSETINSTSIZE;
  else if (i->i.code == IString) return 2;
  else if (i->i.code == IFunc) return i->i.offset;
  else return 1;
}


static const char *val2str (lua_State *L, int idx) {
  const char *k = lua_tostring(L, idx);
  if (k != NULL)
    return lua_pushfstring(L, "rule '%s'", k);
  else
    return lua_pushfstring(L, "rule <a %s>", luaL_typename(L, -1));
}


static int getposition (lua_State *L, int t, int i) {
  int res;
  lua_getfenv(L, -1);
  lua_rawgeti(L, -1, i);  /* get key from pattern's environment */
  lua_gettable(L, t);  /* get position from positions table */
  res = lua_tointeger(L, -1);
  if (res == 0) {  /* key has no registered position? */
    lua_rawgeti(L, -2, i);  /* get key again */
    return luaL_error(L, "%s is not defined in given grammar", val2str(L, -1));
  }
  lua_pop(L, 2);  /* remove environment and position */
  return res;
}



/*
** {======================================================
** Printing patterns
** =======================================================
*/


static void printcharset (const Charset st) {
  int i;
  printf("[");
  for (i = 0; i <= UCHAR_MAX; i++) {
    int first = i;
    while (testchar(st, i) && i <= UCHAR_MAX) i++;
    if (i - 1 == first)  /* unary range? */
      printf("(%02x)", first);
    else if (i - 1 > first)  /* non-empty range? */
      printf("(%02x-%02x)", first, i - 1);
  }
  printf("]");
}


static void printcapkind (int kind) {
  const char *const modes[] = {
    "close", "position", "constant", "backref",
    "argument", "simple", "table", "function",
    "query", "string", "substitution", "fold",
    "runtime", "group" };
  printf("%s", modes[kind]);
}


static void printjmp (const Instruction *op, const Instruction *p) {
  printf("-> ");
  if (p->i.offset == 0) printf("FAIL");
  else printf("%d", (int)(dest(0, p) - op));
}


static void printinst (const Instruction *op, const Instruction *p) {
  const char *const names[] = {
    "any", "char", "set", "span",
    "ret", "end",
    "choice", "jmp", "call", "open_call",
    "commit", "partial_commit", "back_commit", "failtwice", "fail", "giveup",
     "func",
     "fullcapture", "emptycapture", "emptycaptureidx", "opencapture",
    "closecapture", "closeruntime", "open", "close", "notany", "string",
    "partialclosecommit", "anyspan"
  };
  printf("%02ld: %s ", (long)(p - op), names[p->i.code]);
  switch ((Opcode)p->i.code) {
    case IChar: {
      printf("'%c'", p->i.aux);
      printjmp(op, p);
      break;
    }
    case INotAny: {
      printf("(!.) ");
      printjmp(op, p);
      break;
    }
    case IAny: {
      printf("* %d", p->i.aux);
      printjmp(op, p);
      break;
    }
    case IAnySpan: {
      printf("(.*) ");
      break;
    }
    case IString: {
      printf("%d", (p+1)->i.offset);
      printjmp(op, p);
      break;
    }
    case IFullCapture: case IOpenCapture:
    case IEmptyCapture: case IEmptyCaptureIdx:
    case ICloseCapture: case ICloseRunTime: {
      printcapkind(getkind(p));
      printf("(n = %d)  (off = %d)", getoff(p), p->i.offset);
      break;
    }
    case ISet: {
      printcharset((p+1)->buff);
      printjmp(op, p);
      break;
    }
    case ISpan: {
      printcharset((p+1)->buff);
      break;
    }
    case IOpenCall: {
      printf("-> %d", p->i.offset);
      break;
    }
    case IChoice: {
      printjmp(op, p);
      printf(" (%d)", p->i.aux);
      break;
    }
    case IJmp: case ICall: case ICommit:
    case IPartialCloseCommit: case IPartialCommit: case IBackCommit: {
      printjmp(op, p);
      break;
    }
    default: break;
  }
  printf("\n");
}


static void printpatt (Instruction *p) {
  Instruction *op = p;
  for (;;) {
    printinst(op, p);
    if (p->i.code == IEnd) break;
    p += sizei(p);
  }
}

static void printpatts (Instruction *p, int size) {
  Instruction *op = p;
  printf("size: %d\n", size);
  for (;size > 0;) {
    printinst(op, p);
    size -= sizei(p);
    p += sizei(p);
  }
}


static void printcap (Capture *cap) {
  printcapkind(cap->kind);
  switch(cap->s.kind) {
    case Slist: printf(" (idx: %d - size: %d) -> r: %i c: %i\n", cap->idx, cap->siz, cap->s.u.l.ref, cap->s.u.l.cur); break;
    case Sstring: printf(" (idx: %d - size: %d) -> %s\n", cap->idx, cap->siz, cap->s.u.s.o); break;
  }
}


static void printcaplist (Capture *cap) {
  for (; cap->s.kind != Sempty; cap++) printcap(cap);
}

/* }====================================================== */




/*
** {======================================================
** Virtual Machine
** =======================================================
*/


typedef struct Stack {
  Stream s;
  const Instruction *p;
  int caplevel;
} Stack;


static int runtimecap (lua_State *L, Capture *close, Capture *ocap,
                       Stream *s, int ptop);


static Capture *doublecap (lua_State *L, Capture *cap, int captop, int ptop) {
  Capture *newc;
  if (captop >= INT_MAX/((int)sizeof(Capture) * 2))
    luaL_error(L, "too many captures");
  newc = (Capture *)lua_newuserdata(L, captop * 2 * sizeof(Capture));
  memcpy(newc, cap, captop * sizeof(Capture));
  lua_replace(L, caplistidx(ptop));
  return newc;
}


static void adddyncaptures (lua_State *L, Stream *s, Capture *base, int n, int fd, int ptop) {
  int i;
  assert(base[0].kind == Cruntime && base[0].siz == 0);
  lua_pushvalue(L, fd);
  base[0].idx = luaL_ref(L, pdyncapidx(ptop));
  for (i = 1; i < n; i++) {  /* add extra captures */
    base[i].siz = 1;  /* mark it as closed */
    base[i].s = *s;
    base[i].kind = Cruntime;
    lua_pushvalue(L, fd + i);
    base[i].idx = luaL_ref(L, pdyncapidx(ptop));
  }
  base[n].kind = Cclose;  /* add closing entry */
  base[n].siz = 1;
  base[n].s = *s;
}


#define condfailed(p)   { int f = p->i.offset; if (f) p+=f; else goto fail; }

static Stream *match (lua_State *L,
                      Stream *s,
                      Instruction *op, Capture *capture, int ptop) {
  Stack stackbase[MAXBACK];
  Stack *stacklimit = stackbase + MAXBACK;
  Stack *stack = stackbase;  /* point to first empty slot in stack */
  int capsize = IMAXCAPTURES;
  int captop = 0;  /* point to first empty slot in captures */
  const Instruction *p = op;
  stack->p = &giveup; stack->s = *s; stack->caplevel = 0; stack++;
  for (;;) {
#if defined(DEBUG)
      printf("stck: %d c: %d  ", stack - stackbase, captop);
      printinst(op, p);
#endif
    switch ((Opcode)p->i.code) {
      case IEnd: {
        assert(stack == stackbase + 1);
        capture[captop].kind = Cclose;
        capture[captop].s.kind = Sempty;
        return s;
      }
      case IGiveup: {
        assert(stack == stackbase);
        return NULL;
      }
      case IRet: {
        assert(stack > stackbase && (stack - 1)->s.kind == Sempty);
        p = (--stack)->p;
        continue;
      }
      case INotAny: {
        switch(s->kind) {
          case Sstring: {
            if (s->u.s.s >= s->u.s.e) p++;
            else condfailed(p);
            break;
          }
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            if(s->u.l.cur > lua_objlen(L, -1)) { lua_pop(L, 1); p++; }
            else { lua_pop(L, 1); condfailed(p); }
            break;
          }
          default: condfailed(p);
        }
        continue;
      }
      case IAny: {
        int n = p->i.aux;
        switch(s->kind) {
          case Sstring: {
            if (n <= s->u.s.e - s->u.s.s) { p++; s->u.s.s += n; }
            else condfailed(p);
            break;
          }
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            if(n <= lua_objlen(L, -1) - s->u.l.cur + 1) { lua_pop(L, 1); p++; s->u.l.cur += n; }
            else { lua_pop(L, 1); condfailed(p); }
            break;
          }
          default: condfailed(p);
        }
        continue;
      }
      case IChar: {
        switch(s->kind) {
          case Sstring: {
            if (s->u.s.s < s->u.s.e &&
                (byte)(*(s->u.s.s)) == p->i.aux) { p++; s->u.s.s++; }
            else condfailed(p);
            break;
          }
          case Slist: {
            const char *c; size_t siz;
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            lua_rawgeti(L, -1, s->u.l.cur);
            if(lua_type(L, -1) != LUA_TSTRING) {
              lua_pop(L, 2); condfailed(p); break;
            }
            c = lua_tolstring(L, -1, &siz);
            if(siz == 1 && (byte)*c == p->i.aux) { p++; s->u.l.cur++; }
            else { lua_pop(L, 2); condfailed(p); break; }
            lua_pop(L, 2);
            break;
          }
          default: { condfailed(p); }
        }
        continue;
      }
      case ISet: {
        switch(s->kind) {
          case Sstring: {
            int c = (byte)*(s->u.s.s);
            if (s->u.s.s < s->u.s.e && testchar((p+1)->buff, c))
              { p += CHARSETINSTSIZE; s->u.s.s++; }
            else condfailed(p);
            break;
          }
          case Slist: {
            const char *c; size_t siz;
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            lua_rawgeti(L, -1, s->u.l.cur);
            if(lua_type(L, -1) != LUA_TSTRING) {
              lua_pop(L, 2); condfailed(p); break;
            }
            c = lua_tolstring(L, -1, &siz);
            if(siz == 1 && testchar((p+1)->buff, (byte)*c))
              { p += CHARSETINSTSIZE; s->u.l.cur++; }
            else { lua_pop(L, 2); condfailed(p); break; }
            lua_pop(L, 2);
            break;
          }
          default: condfailed(p);
        }
        continue;
      }
      case IAnySpan: {
        switch(s->kind) {
          case Sstring: {
            s->u.s.s = s->u.s.e;
            break;
          }
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            s->u.l.cur = lua_objlen(L, -1) + 1;
            lua_pop(L, 1);
            break;
          }
        }
        p++;
        continue;
      }
      case ISpan: {
        switch(s->kind) {
          case Sstring: {
            for (; s->u.s.s < s->u.s.e; s->u.s.s++) {
              int c = (byte)*(s->u.s.s);
              if (!testchar((p+1)->buff, c)) break;
            }
            break;
          }
          case Slist: {
            const char *c; size_t siz;
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            for(;; s->u.l.cur++) {
              lua_rawgeti(L, -1, s->u.l.cur);
              if(lua_type(L, -1) != LUA_TSTRING) { lua_pop(L, 1); break; }
              c = lua_tolstring(L, -1, &siz);
              if(siz != 1 || !testchar((p+1)->buff, (byte)*c))
                { lua_pop(L, 1); break; }
              lua_pop(L, 1);
            }
            lua_pop(L, 1);
          }
        }
        p += CHARSETINSTSIZE;
        continue;
      }
      case IString: {
        switch(s->kind) {
          case Sstring: { condfailed(p); break; }
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            lua_rawgeti(L, -1, s->u.l.cur);
            lua_rawgeti(L, penvidx(ptop), (p+1)->i.offset);
            if(lua_rawequal(L, -1, -2) == 0) {
              lua_pop(L, 3); condfailed(p); break;
            }
            lua_pop(L, 3);
            p += 2;
            s->u.l.cur++;
            break;
          }
          default: { condfailed(p); }
        }
        continue;
      }
      case IFunc: {
        Stream *r = (p+1)->f((p+2)->buff, s);
        if (r == NULL) goto fail;
        s = r;
        p += p->i.offset;
        continue;
      }
      case IJmp: {
        p += p->i.offset;
        continue;
      }
      case IOpen: {
        switch(s->kind) {
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1));
            lua_rawgeti(L, -1, s->u.l.cur);
            lua_remove(L, -2);
            switch(lua_type(L, -1)) {
              case LUA_TTABLE: {
                if (stack >= stacklimit)
                  return (luaL_error(L, "too many pending calls/choices"),
                          (Stream *)0);
                stack->p = NULL;
                stack->s = *s;
                stack->caplevel = captop;
                stack++;
                s->kind = Slist;
                s->u.l.ref = luaL_ref(L, plistidx(ptop));
                s->u.l.cur = 1;
                p++;
                break;
              }
              case LUA_TSTRING: {
                size_t siz;
                if (stack >= stacklimit)
                  return (luaL_error(L, "too many pending calls/choices"),
                          (Stream *)0);
                stack->p = NULL;
                stack->s = *s;
                stack->caplevel = captop;
                stack++;
                s->kind = Sstring;
                s->u.s.o = lua_tolstring(L, -1, &siz);
                lua_pop(L, 1);
                s->u.s.s = s->u.s.o;
                s->u.s.e = s->u.s.o + siz;
                p++;
                break;
              }
              default: lua_pop(L, 1); condfailed(p);
            }
            break;
          }
          default: condfailed(p);
        }
        continue;
      }
      case IClose: {
        switch(s->kind) {
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            assert(lua_istable(L, -1) && stack > stackbase);
            lua_rawgeti(L, -1, s->u.l.cur);
            if(!lua_isnil(L, -1)) {
              lua_pop(L, 2);
              condfailed(p);
              break;
            } else {
              --stack;
              *s = stack->s;
              s->u.l.cur++;
              p++;
            }
            lua_pop(L, 2); break;
          }
          case Sstring: {
            if(s->u.s.s < s->u.s.e) { goto fail; }
            else {
              --stack;
              *s = stack->s;
              s->u.l.cur++;
              p++;
            }
            break;
          }
          default: goto fail;
        }
        continue;
      }
      case IChoice: {
        if (stack >= stacklimit)
          luaL_error(L, "too many pending calls/choices");
        stack->p = dest(0, p);
        stack->s = *s;
        stack->caplevel = captop;
        switch(s->kind) {
          case Slist: stack->s.u.l.cur -= p->i.aux; break;
          case Sstring: stack->s.u.s.s -= p->i.aux; break;
        }
        stack++;
        p++;
        continue;
      }
      case ICall: {
        if (stack >= stacklimit)
          return (luaL_error(L, "too many pending calls/choices"), (Stream *)0);
        stack->s.kind = Sempty;
        stack->p = p + 1;  /* save return address */
        stack++;
        p += p->i.offset;
        continue;
      }
      case ICommit: {
        assert(stack > stackbase);
        stack--;
        p += p->i.offset;
        continue;
      }
      case IPartialCommit: {
        assert(stack > stackbase);
        (stack - 1)->s = *s;
        (stack - 1)->caplevel = captop;
        p += p->i.offset;
        continue;
      }
      case IPartialCloseCommit: {
        switch(s->kind) {
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
            lua_rawgeti(L, -1, s->u.l.cur);
            if(!lua_isnil(L, -1)) {
              lua_pop(L, 2); goto fail;
            }
            lua_pop(L, 2); break;
          }
          case Sstring: {
            if(s->u.s.s < s->u.s.e) { goto fail; }
            break;
          }
          default: goto fail;
        }
        (stack - 2)->s.u.l.cur++;
        (stack - 2)->caplevel = captop;
        lua_rawgeti(L, plistidx(ptop), (stack - 2)->s.u.l.ref);
        lua_rawgeti(L, -1, (stack - 2)->s.u.l.cur);
        switch(lua_type(L, -1)) {
          case LUA_TTABLE: {
            s->kind = Slist;
            s->u.l.ref = luaL_ref(L, plistidx(ptop));
            s->u.l.cur = 1;
            break;
          }
          case LUA_TSTRING: {
            size_t siz;
            s->kind = Sstring;
            s->u.s.o = lua_tolstring(L, -1, &siz);
            lua_pop(L, 1);
            s->u.s.s = s->u.s.o;
            s->u.s.e = s->u.s.o + siz;
            break;
          }
          default: lua_pop(L, 2); goto fail;
        }
        p += p->i.offset;
        lua_pop(L, 1);
        continue;
      }
      case IBackCommit: {
        assert(stack > stackbase);
        *s = (--stack)->s;
        p += p->i.offset;
        continue;
      }
      case IFailTwice:
        assert(stack > stackbase);
        stack--;
        /* go through */
      case IFail:
      fail: { /* pattern failed: try to backtrack */
        do {  /* remove pending calls/lists */
          assert(stack > stackbase);
          --stack;
          *s = stack->s;
        } while (s->kind == Sempty || stack->p == NULL);
        captop = stack->caplevel;
        p = stack->p;
        continue;
      }
      case ICloseRunTime: {
        int fr = lua_gettop(L) + 1;  /* stack index of first result */
        int ncap = runtimecap(L, capture + captop, capture, s, ptop);
        lua_Integer res = lua_tointeger(L, fr) - 1;  /* offset */
        int n = lua_gettop(L) - fr;  /* number of new captures */
        if (res == -1) {  /* may not be a number */
          if (!lua_toboolean(L, fr)) {  /* false value? */
            lua_settop(L, fr - 1);  /* remove results */
            goto fail;  /* and fail */
          }
          else if (lua_isboolean(L, fr)) { /* true? keep current position */
            switch(s->kind) {
            case Sstring: res = s->u.s.s - s->u.s.o; break;
            case Slist: res = s->u.l.cur - 2; break;
            default:
              return (luaL_error(L, "dynamic captures not supported for this stream"), (Stream*)0);
            }
          }
        }
        switch(s->kind) {
          case Sstring: {
            if (res < s->u.s.s - s->u.s.o || res > s->u.s.e - s->u.s.o)
              luaL_error(L, "invalid position returned by match-time capture");
            s->u.s.s = s->u.s.o + res;  /* update current position */
            break;
          }
          case Slist: {
            lua_rawgeti(L, plistidx(ptop), s->u.l.ref); assert(lua_istable(L, -1));
            res = res + 1;
            if (res + 1 < s->u.l.cur || res > (int)lua_objlen(L, -1))
              luaL_error(L, "invalid position returned by match-time capture");
            lua_pop(L, 1);
            s->u.l.cur = res + 1;
          }
        }
        captop -= ncap;  /* remove nested captures */
        lua_remove(L, fr);  /* remove first result (offset) */
        if (n > 0) {  /* captures? */
          if ((captop += n + 1) >= capsize) {
            capture = doublecap(L, capture, captop, ptop);
            capsize = 2 * captop;
          }
          adddyncaptures(L, s, capture + captop - n - 1, n, fr, ptop);
          lua_pop(L, n);
        }
        p++;
        continue;
      }
      case ICloseCapture: {
        switch(s->kind) {
          case Sstring: {
            const char *s1 = s->u.s.s - getoff(p);
            assert(captop > 0);
            if (capture[captop - 1].siz == 0 &&
                s1 - capture[captop - 1].s.u.s.s < UCHAR_MAX) {
              capture[captop - 1].siz = s1 - capture[captop - 1].s.u.s.s + 1;
              p++;
              continue;
            }
            else {
              capture[captop].siz = 1;  /* mark entry as closed */
              goto capture;
            }
            break;
          }
          case Slist: {
            int cur1 = s->u.l.cur - getoff(p);
            assert(captop > 0);
            if (capture[captop - 1].siz == 0 &&
                cur1 - capture[captop - 1].s.u.l.cur < UCHAR_MAX) {
              capture[captop - 1].siz = cur1 -
                capture[captop - 1].s.u.l.cur + 1;
              p++;
              continue;
            }
            else {
              capture[captop].siz = 1;  /* mark entry as closed */
              goto capture;
            }
          }
        default: return
            (luaL_error(L, "closing capture over invalid subject"), (Stream*)0);
        }
      }
      case IEmptyCapture: case IEmptyCaptureIdx:
        capture[captop].siz = 1;  /* mark entry as closed */
        goto capture;
      case IOpenCapture:
        capture[captop].siz = 0;  /* mark entry as open */
        goto capture;
      case IFullCapture:
        capture[captop].siz = getoff(p) + 1;  /* save capture size */
      capture: {
        capture[captop].s = *s;
        switch(s->kind) {
          case Slist: capture[captop].s.u.l.cur -= getoff(p); break;
          case Sstring: capture[captop].s.u.s.s -= getoff(p); break;
          default:
            return (luaL_error(L, "closing capture over invalid subject"),
                    (Stream*)0);
        }
        capture[captop].idx = p->i.offset;
        capture[captop].kind = getkind(p);
        if (++captop >= capsize) {
          capture = doublecap(L, capture, captop, ptop);
          capsize = 2 * captop;
        }
        p++;
        continue;
      }
      case IOpenCall: {
        lua_rawgeti(L, penvidx(ptop), p->i.offset);
        luaL_error(L, "reference to %s outside a grammar", val2str(L, -1));
      }
      default: assert(0); return NULL;
    }
  }
}

/* }====================================================== */


/*
** {======================================================
** Verifier
** =======================================================
*/


static int verify (lua_State *L, Instruction *op, const Instruction *p,
                   Instruction *e, int postable, int rule) {
  Stream dummy_s;
  Stream dummy_l;
  Stack back[MAXBACK];
  int backtop = 0;  /* point to first empty slot in back */
  dummy_s.kind = Sstring;
  dummy_l.kind = Slist;
  while (p != e) {
    switch ((Opcode)p->i.code) {
      case IRet: {
        p = back[--backtop].p;
        continue;
      }
      case IChoice: {
        if (backtop >= MAXBACK)
          return luaL_error(L, "too many pending calls/choices");
        back[backtop].p = dest(0, p);
        back[backtop++].s = dummy_s;
        p++;
        continue;
      }
      case IOpen:
      {
        goto fail;
      }
      case ICall: {
        assert((p + 1)->i.code != IRet);  /* no tail call */
        if (backtop >= MAXBACK)
          return luaL_error(L, "too many pending calls/choices");
        back[backtop].s.kind = Sempty;
        back[backtop++].p = p + 1;
        goto dojmp;
      }
      case IOpenCall: {
        int i;
        if (postable == 0)  /* grammar still not fixed? */
          goto fail;  /* to be verified later */
        for (i = 0; i < backtop; i++) {
          if (back[i].s.kind == Sempty && back[i].p == p + 1)
            return luaL_error(L, "%s is left recursive", val2str(L, rule));
        }
        if (backtop >= MAXBACK)
          return luaL_error(L, "too many pending calls/choices");
        back[backtop].s.kind = Sempty;
        back[backtop++].p = p + 1;
        p = op + getposition(L, postable, p->i.offset);
        continue;
      }
      case IBackCommit:
      case ICommit: {
        assert(backtop > 0 && p->i.offset > 0);
        backtop--;
        goto dojmp;
      }
      case IPartialCloseCommit:
      case IPartialCommit: {
        assert(backtop > 0);
        if (p->i.offset > 0) goto dojmp;  /* forward jump */
        else {  /* loop will be detected when checking corresponding rule */
          assert(postable != 0);
          backtop--;
          p++;  /* just go on now */
          continue;
        }
      }
      case IClose:
      {
        assert(backtop > 0);
        backtop--;
        assert(back[backtop].s.kind == Slist);
        p++;
        continue;
      }
      case INotAny:
      case IString:
      case IAny:
      case IChar:
      case ISet: {
        if (p->i.offset == 0) goto fail;
        /* else goto dojmp; go through */
      }
      case IJmp:
      dojmp: {
        p += p->i.offset;
        continue;
      }
      case IFailTwice:  /* 'not' predicate */
        goto fail;  /* body could have failed; try to backtrack it */
      case IFail: {
        if (p > op && (p - 1)->i.code == IBackCommit) {  /* 'and' predicate? */
          p++;  /* pretend it succeeded and go ahead */
          continue;
        }
        /* else failed: go through */
      }
      fail: { /* pattern failed: try to backtrack */
        do {
          if (backtop-- == 0)
            return 1;  /* no more backtracking */
        } while (back[backtop].s.kind == Sempty || back[backtop].p == NULL);
        p = back[backtop].p;
        continue;
      }
      case ISpan: case IAnySpan:
      case IOpenCapture: case ICloseCapture:
      case IEmptyCapture: case IEmptyCaptureIdx:
      case IFullCapture: {
        p += sizei(p);
        continue;
      }
      case ICloseRunTime: {
        goto fail;  /* be liberal in this case */
      }
      case IFunc: {
        Stream *r = (p+1)->f((p+2)->buff, &dummy_s);
        if (r == NULL) goto fail;
        p += p->i.offset;
        continue;
      }
      case IEnd:  /* cannot happen (should stop before it) */
      default: assert(0); return 0;
    }
  }
  assert(backtop == 0);
  return 0;
}


static void checkrule (lua_State *L, Instruction *op, int from, int to,
                       int postable, int rule) {
  int i;
  int lastopen = 0;  /* more recent OpenCall seen in the code */
  for (i = from; i < to; i += sizei(op + i)) {
    if ((op[i].i.code == IPartialCommit || op[i].i.code == IPartialCloseCommit)
        && op[i].i.offset < 0) {  /* loop? */
      int start = dest(op, i);
      assert((op[start - 1].i.code == IChoice && dest(op, start - 1) == i + 1) ||
             (op[start - 2].i.code == IChoice && dest(op, start - 2) == i + 1));
      if (start <= lastopen) {  /* loop does contain an open call? */
        if (!verify(L, op, op + start, op + i, postable, rule)) /* check body */
          luaL_error(L, "possible infinite loop in %s", val2str(L, rule));
      }
    }
    else if (op[i].i.code == IOpenCall)
      lastopen = i;
  }
  assert(op[i - 1].i.code == IRet);
  verify(L, op, op + from, op + to - 1, postable, rule);
}




/* }====================================================== */




/*
** {======================================================
** Building Patterns
** =======================================================
*/

enum charsetanswer { NOINFO, ISCHARSET, VALIDSTARTS };

typedef struct CharsetTag {
  enum charsetanswer tag;
  Charset cs;
} CharsetTag;


static Instruction *getpatt (lua_State *L, int idx, int *size);


static void check2test (Instruction *p, int n) {
  assert(ischeck(p) && n != 0);
  p->i.offset = n;
}


/*
** invert array slice p[0]-p[e] (both inclusive)
*/
static void invert (Instruction *p, int e) {
  int i;
  for (i = 0; i < e; i++, e--) {
    Instruction temp = p[i];
    p[i] = p[e];
    p[e] = temp;
  }
}


/*
** rotate array slice p[0]-p[e] (both inclusive) 'n' steps
** to the 'left'
*/
static void rotate (Instruction *p, int e, int n) {
  invert(p, n - 1);
  invert(p + n, e - n);
  invert(p, e);
}


#define op_step(p)      ((p)->i.code == IAny ? (p)->i.aux : 1)


static int skipchecks (Instruction *p, int up, int *pn) {
  int i, n = 0;
  for (i = 0; ischeck(p + i); i += sizei(p + i)) {
    int st = op_step(p + i);
    if (n + st > MAXOFF - up) break;
    n += st;
  }
  *pn = n;
  return i;
}


#define ismovablecap(op)        (ismovable(op) && getoff(op) < MAXOFF)

static void optimizecaptures (Instruction *p) {
  int i;
  int limit = 0;
  for (i = 0; p[i].i.code != IEnd; i += sizei(p + i)) {
    if (isjmp(p + i) && dest(p, i) >= limit)
      limit = dest(p, i) + 1;  /* do not optimize jump targets */
    else if (i >= limit && ismovablecap(p + i) && ischeck(p + i + 1)) {
      int end, n, j;  /* found a border capture|check */
      int maxoff = getoff(p + i);
      int start = i;
      /* find first capture in the group */
      while (start > limit && ismovablecap(p + start - 1)) {
        start--;
        if (getoff(p + start) > maxoff) maxoff = getoff(p + start);
      }
      end = skipchecks(p + i + 1, maxoff, &n) + i;  /* find last check */
      if (n == 0) continue;  /* first check is too big to move across */
      assert(n <= MAXOFF && start <= i && i < end);
      for (j = start; j <= i; j++)
        p[j].i.aux += (n << 4);  /* correct offset of captures to be moved */
      rotate(p + start, end - start, i - start + 1);  /* move them up */
      i = end;
      assert(ischeck(p + start) && iscapture(p + i));
    }
  }
}


static int target (Instruction *p, int i) {
  while (p[i].i.code == IJmp)  i += p[i].i.offset;
  return i;
}


static void optimizejumps (Instruction *p) {
  int i;
  for (i = 0; p[i].i.code != IEnd; i += sizei(p + i)) {
    if (isjmp(p + i))
      p[i].i.offset = target(p, dest(p, i)) - i;
  }
}


static void optimizechoice (Instruction *p) {
  assert(p->i.code == IChoice);
  if (ischeck(p + 1)) {
    int lc = sizei(p + 1);
    rotate(p, lc, 1);
    assert(ischeck(p) && (p + lc)->i.code == IChoice);
    (p + lc)->i.aux = op_step(p);
    check2test(p, (p + lc)->i.offset);
    (p + lc)->i.offset -= lc;
  }
}


/*
** A 'headfail' pattern is a pattern that can only fails in its first
** instruction, which must be a check.
*/
static int isheadfail (Instruction *p) {
  Instruction *op = p;
  if (!ischeck(p)) {
    return 0;
  }
  /* check that other operations cannot fail */
  for (p += sizei(p); p->i.code != IEnd; p += sizei(p))
    if (!isnofail(p)) {
      return 0;
    }
  return 1;
}


#define checkpattern(L, idx) ((Instruction *)luaL_checkudata(L, idx, PATTERN_T))


static int jointable (lua_State *L, int p1) {
  int n, n1, i;
  lua_getfenv(L, p1);
  n1 = lua_objlen(L, -1);  /* number of elements in p1's env */
  lua_getfenv(L, -2);
  if (n1 == 0 || lua_equal(L, -2, -1)) {
    lua_pop(L, 2);
    return 0;  /* no need to change anything */
  }
  n = lua_objlen(L, -1);  /* number of elements in p's env */
  if (n == 0) {
    lua_pop(L, 1);  /* removes p env */
    lua_setfenv(L, -2);  /* p now shares p1's env */
    return 0;  /* no need to correct anything */
  }
  lua_createtable(L, n + n1, 0);
  /* stack: p; p1 env; p env; new p env */
  for (i = 1; i <= n; i++) {
    lua_rawgeti(L, -2, i);
    lua_rawseti(L, -2, i);
  }
  for (i = 1; i <= n1; i++) {
    lua_rawgeti(L, -3, i);
    lua_rawseti(L, -2, n + i);
  }
  lua_setfenv(L, -4);  /* new table becomes p env */
  lua_pop(L, 2);  /* remove p1 env and old p env */
  return n;
}


#define copypatt(p1,p2,sz)      memcpy(p1, p2, (sz) * sizeof(Instruction));

#define pattsize(L,idx)         (lua_objlen(L, idx)/sizeof(Instruction) - 1)


static int addpatt (lua_State *L, Instruction *p, int p1idx) {
  Instruction *p1 = (Instruction *)lua_touserdata(L, p1idx);
  int sz = pattsize(L, p1idx);
  int corr = jointable(L, p1idx);
  copypatt(p, p1, sz + 1);
  if (corr != 0) {
    Instruction *px;
    for (px = p; px < p + sz; px += sizei(px)) {
      if (isfenvoff(px)) {
        if(px->i.code == IString)
          (px+1)->i.offset += corr;
        else if(px->i.offset != 0)
          px->i.offset += corr;
      }
    }
  }
  return sz;
}


static void setinstaux (Instruction *i, Opcode op, int offset, int aux) {
  i->i.code = op;
  i->i.offset = offset;
  i->i.aux = aux;
}

#define setinst(i,op,off)       setinstaux(i,op,off,0)

#define setinstcap(i,op,idx,k,n)  setinstaux(i,op,idx,((k) | ((n) << 4)))


static int value2fenv (lua_State *L, int vidx) {
  lua_createtable(L, 1, 0);
  lua_pushvalue(L, vidx);
  lua_rawseti(L, -2, 1);
  lua_setfenv(L, -2);
  return 1;
}


static Instruction *newpatt (lua_State *L, size_t n) {
  Instruction *p;
  if (n >= MAXPATTSIZE - 1)
    luaL_error(L, "pattern too big");
  p = (Instruction *)lua_newuserdata(L, (n + 1) * sizeof(Instruction));
  luaL_getmetatable(L, PATTERN_T);
  lua_setmetatable(L, -2);
  setinst(p + n, IEnd, 0);
  return p;
}


static void fillcharset (Instruction *p, Charset cs) {
  switch (p[0].i.code) {
    case ISet: {
      loopset(i, cs[i] = p[1].buff[i]);
      break;
    }
    case IChar: {
      loopset(i, cs[i] = 0);
      setchar(cs, p[0].i.aux);
      break;
    }
    default: {  /* any char may start unhandled instructions */
      loopset(i, cs[i] = 0xff);
      break;
    }
  }
}


/*
** Function 'tocharset' gets information about which chars may be a
** valid start for a pattern.
*/

static enum charsetanswer tocharset (Instruction *p, CharsetTag *c) {
  if (ischeck(p) && p->i.code != IString && p->i.code != INotAny) {
    fillcharset(p, c->cs);
    if ((p + sizei(p))->i.code == IEnd && op_step(p) == 1)
      c->tag = ISCHARSET;
    else
      c->tag = VALIDSTARTS;
  } else
    c->tag = NOINFO;
  return c->tag;
}


static int exclusiveset (Charset c1, Charset c2) {
  /* non-empty intersection? */
  loopset(i, {if ((c1[i] & c2[i]) != 0) return 0;});
  return 1;  /* no intersection */
}


static int exclusive (CharsetTag *c1, CharsetTag *c2) {
  if (c1->tag == NOINFO || c2->tag == NOINFO)
    return 0;  /* one of them is not filled */
  else return exclusiveset(c1->cs, c2->cs);
}


static Instruction *newcharset (lua_State *L) {
  Instruction *p = newpatt(L, CHARSETINSTSIZE);
  p[0].i.code = ISet;
  p[0].i.offset = 0;
  loopset(i, p[1].buff[i] = 0);
  return p;
}


static int set_l (lua_State *L) {
  size_t l;
  const char *s = luaL_checklstring(L, 1, &l);
  if (l == 1)
    getpatt(L, 1, NULL);  /* a unit set is equivalent to a literal */
  else {
    Instruction *p = newcharset(L);
    while (l--) {
      setchar(p[1].buff, (byte)(*s));
      s++;
    }
  }
  return 1;
}


static int range_l (lua_State *L) {
  int arg;
  int top = lua_gettop(L);
  Instruction *p = newcharset(L);
  for (arg = 1; arg <= top; arg++) {
    int c;
    size_t l;
    const char *r = luaL_checklstring(L, arg, &l);
    luaL_argcheck(L, l == 2, arg, "range must have two characters");
    for (c = (byte)r[0]; c <= (byte)r[1]; c++)
      setchar(p[1].buff, c);
  }
  return 1;
}


static int nter_l (lua_State *L) {
  Instruction *p;
  luaL_argcheck(L, !lua_isnoneornil(L, 1), 1, "non-nil value expected");
  p = newpatt(L, 1);
  setinst(p, IOpenCall, value2fenv(L, 1));
  return 1;
}



static int testpattern (lua_State *L, int idx) {
  if (lua_touserdata(L, idx)) {  /* value is a userdata? */
    if (lua_getmetatable(L, idx)) {  /* does it have a metatable? */
      luaL_getmetatable(L, PATTERN_T);
      if (lua_rawequal(L, -1, -2)) {  /* does it have the correct mt? */
        lua_pop(L, 2);  /* remove both metatables */
        return 1;
      }
    }
  }
  return 0;
}


static Instruction *fix_l (lua_State *L, int t) {
  Instruction *p;
  int i;
  int totalsize = 2;  /* include initial call and jump */
  int n = 0;  /* number of rules */
  int base = lua_gettop(L);
  lua_newtable(L);  /* to store relative positions of each rule */
  lua_pushinteger(L, 1);  /* default initial rule */
  /* collect patterns and compute sizes */
  lua_pushnil(L);
  while (lua_next(L, t) != 0) {
    int l;
    if (lua_tonumber(L, -2) == 1 && lua_isstring(L, -1)) {
      lua_replace(L, base + 2);  /* use this value as initial rule */
      continue;
    }
    if (!testpattern(L, -1))
      luaL_error(L, "invalid field in grammar");
    l = pattsize(L, -1) + 1;  /* space for pattern + ret */
    if (totalsize >= MAXPATTSIZE - l)
      luaL_error(L, "grammar too large");
    luaL_checkstack(L, LUA_MINSTACK, "grammar has too many rules");
    lua_insert(L, -2);  /* put key on top */
    lua_pushvalue(L, -1);  /* duplicate key (for lua_next) */
    lua_pushvalue(L, -1);  /* duplicate key (to index positions table)) */
    lua_pushinteger(L, totalsize);  /* position for this rule */
    lua_settable(L, base + 1);  /* store key=>position in positions table */
    totalsize += l;
    n++;
  }
  luaL_argcheck(L, n > 0, t, "empty grammar");
  p = newpatt(L, totalsize);  /* create new pattern */
  p++;  /* save space for call */
  setinst(p++, IJmp, totalsize - 1);  /* after call, jumps to the end */
  for (i = 1; i <= n; i++) {  /* copy all rules into new pattern */
    p += addpatt(L, p, base + 1 + i*2);
    setinst(p++, IRet, 0);
  }
  p -= totalsize;  /* back to first position */
  totalsize = 2;  /* go through each rule's position */
  for (i = 1; i <= n; i++) {  /* check all rules */
    int l = pattsize(L, base + 1 + i*2) + 1;
    checkrule(L, p, totalsize, totalsize + l, base + 1, base + 2 + i*2);
    totalsize += l;
  }
  lua_pushvalue(L, base + 2);  /* get initial rule */
  lua_gettable(L, base + 1);  /* get its position in postions table */
  i = lua_tonumber(L, -1);  /* convert to number */
  lua_pop(L, 1);
  if (i == 0)  /* is it defined? */
    luaL_error(L, "initial rule not defined in given grammar");
  setinst(p, ICall, i);  /* first instruction calls initial rule */
  /* correct calls */
  for (i = 0; i < totalsize; i += sizei(p + i)) {
    if (p[i].i.code == IOpenCall) {
      int pos = getposition(L, base + 1, p[i].i.offset);
      p[i].i.code = (p[target(p, i + 1)].i.code == IRet) ? IJmp : ICall;
      p[i].i.offset = pos - i;
    }
  }
  optimizejumps(p);
  lua_replace(L, t);  /* put new pattern in old's position */
  lua_settop(L, base);  /* remove rules and positions table */
  return p;
}


static Instruction *any (lua_State *L, int n, int extra, int *offsetp) {
  int offset = offsetp ? *offsetp : 0;
  Instruction *p = newpatt(L, (n - 1)/UCHAR_MAX + extra + 1);
  Instruction *p1 = p + offset;
  for (; n > UCHAR_MAX; n -= UCHAR_MAX)
    setinstaux(p1++, IAny, 0, UCHAR_MAX);
  setinstaux(p1++, IAny, 0, n);
  if (offsetp) *offsetp = p1 - p;
  return p;
}


static Instruction *getpatt (lua_State *L, int idx, int *size) {
  Instruction *p;
  switch (lua_type(L, idx)) {
    case LUA_TSTRING: {
      size_t i, len;
      const char *s = lua_tolstring(L, idx, &len);
      p = newpatt(L, len);
      for (i = 0; i < len; i++)
        setinstaux(p + i, IChar, 0, (byte)s[i]);
      lua_replace(L, idx);
      break;
    }
    case LUA_TNUMBER: {
      int n = lua_tointeger(L, idx);
      if (n == 0)  /* empty pattern? */
        p = newpatt(L, 0);
      else if (n > 0)
        p = any(L, n, 0, NULL);
      else if (-n <= UCHAR_MAX) {
        p = newpatt(L, 2);
        setinstaux(p, IAny, 2, -n);
        setinst(p + 1, IFail, 0);
      }
      else {
        int offset = 2;  /* space for ITestAny & IChoice */
        p = any(L, -n - UCHAR_MAX, 3, &offset);
        setinstaux(p, IAny, offset + 1, UCHAR_MAX);
        setinstaux(p + 1, IChoice, offset, UCHAR_MAX);
        setinst(p + offset, IFailTwice, 0);
      }
      lua_replace(L, idx);
      break;
    }
    case LUA_TBOOLEAN: {
      if (lua_toboolean(L, idx))  /* true? */
        p = newpatt(L, 0);  /* empty pattern (always succeeds) */
      else {
        p = newpatt(L, 1);
        setinst(p, IFail, 0);
      }
      lua_replace(L, idx);
      break;
    }
    case LUA_TTABLE: {
      p = fix_l(L, idx);
      break;
    }
    case LUA_TFUNCTION: {
      p = newpatt(L, 2);
      setinstcap(p, IOpenCapture, value2fenv(L, idx), Cruntime, 0);
      setinstcap(p + 1, ICloseRunTime, 0, Cclose, 0);
      lua_replace(L, idx);
      break;
    }
    default: {
      p = checkpattern(L, idx);
      break;
    }
  }
  if (size) *size = pattsize(L, idx);
  return p;
}


static int getpattl (lua_State *L, int idx) {
  int size;
  getpatt(L, idx, &size);
  return size;
}


static int pattern_l (lua_State *L) {
  lua_settop(L, 1);
  getpatt(L, 1, NULL);
  return 1;
}


#define isany(p)        ((p)->i.code == IAny && ((p) + 1)->i.code == IEnd)
#define isany1(p)        ((p)->i.code == IAny && (p)->i.aux == 1 && ((p) + 1)->i.code == IEnd)
#define isfail(p)       ((p)->i.code == IFail)
#define issucc(p)       ((p)->i.code == IEnd)

static int concat_l (lua_State *L) {
  /* p1; p2; */
  int l1, l2;
  Instruction *p1 = getpatt(L, 1, &l1);
  Instruction *p2 = getpatt(L, 2, &l2);
  if (isfail(p1) || issucc(p2))
    lua_pushvalue(L, 1);  /* fail * x == fail; x * true == x */
  else if (isfail(p2) || issucc(p1))
    lua_pushvalue(L, 2);  /* x * fail == fail; true * x == x */
  else if (isany(p1) && isany(p2))
    any(L, p1->i.aux + p2->i.aux, 0, NULL);
  else {
    Instruction *op = newpatt(L, l1 + l2);
    Instruction *p = op + addpatt(L, op, 1);
    addpatt(L, p, 2);
    optimizecaptures(op);
  }
  return 1;
}

static int isstring (Instruction *p, int psiz) {
  int i;
  for(i = 0; i < psiz; i++, p++) {
    if(p->i.code != IChar) return 0;
  }
  return 1;
}

static void fillstring(luaL_Buffer *b, Instruction *p, int psiz) {
  int i;
  for(i = 0; i < psiz; i++, p++)
    luaL_addchar(b, (char)p->i.aux);
}

static int pattlist_l (lua_State *L) {
  int l;
  Instruction *op;
  Instruction *p;
  Instruction *p1 = getpatt(L, 1, &l);
  if(isstring(p1, l)) {
    int off;
    luaL_Buffer b;
    luaL_buffinit(L, &b);
    fillstring(&b, p1, l);
    luaL_pushresult(&b);
    op = newpatt(L, 2);
    setinst(op++, IString, 0);
    setinst(op++, IString, value2fenv(L, lua_gettop(L) - 1));
  } else {
    op = newpatt(L, 2 + l);
    setinst(op++, IOpen, 0);
    p = op + addpatt(L, op, 1);
    setinst(p, IClose, 0);
  }
  return 1;
}

static int diff_l (lua_State *L) {
  int l1, l2;
  Instruction *p1 = getpatt(L, 1, &l1);
  Instruction *p2 = getpatt(L, 2, &l2);
  CharsetTag st1, st2;
  if (tocharset(p1, &st1) == ISCHARSET && tocharset(p2, &st2) == ISCHARSET) {
    Instruction *p = newcharset(L);
    loopset(i, p[1].buff[i] = st1.cs[i] & ~st2.cs[i]);
  }
  else if (isheadfail(p2)) {
    Instruction *p = newpatt(L, l2 + 1 + l1);
    p += addpatt(L, p, 2);
    check2test(p - l2, l2 + 1);
    setinst(p++, IFail, 0);
    addpatt(L, p, 1);
  }
  else {  /* !e2 . e1 */
    /* !e -> choice L1; e; failtwice; L1: ... */
    Instruction *p = newpatt(L, 1 + l2 + 1 + l1);
    Instruction *pi = p;
    setinst(p++, IChoice, 1 + l2 + 1);
    p += addpatt(L, p, 2);
    setinst(p++, IFailTwice, 0);
    addpatt(L, p, 1);
    optimizechoice(pi);
  }
  return 1;
}


static int unm_l (lua_State *L) {
  Instruction *p = getpatt(L, 1, NULL);
  if (isfail(p)) {  /* -false? */
    newpatt(L, 0);  /* true */
    return 1;
  }
  else if (issucc(p)) {  /* -true? */
    Instruction *p1 = newpatt(L, 1);  /* false */
    setinst(p1, IFail, 0);
    return 1;
  }
  else {  /* -A == '' - A */
    lua_pushliteral(L, "");
    lua_insert(L, 1);
    return diff_l(L);
  }
}


static int pattand_l (lua_State *L) {
  int l1;
  Instruction *p1 = getpatt(L, 1, &l1);
  CharsetTag st1;
  if (isfail(p1) || issucc(p1))
    lua_pushvalue(L, 1);  /* &fail == fail; &true == true */
  else if (tocharset(p1, &st1) == ISCHARSET) {
    Instruction *p = newpatt(L, CHARSETINSTSIZE + 1);
    setinst(p, ISet, CHARSETINSTSIZE + 1);
    loopset(i, p[1].buff[i] = ~st1.cs[i]);
    setinst(p + CHARSETINSTSIZE, IFail, 0);
  }
  else {
    Instruction *p = newpatt(L, 1 + l1 + 2);
    setinst(p++, IChoice, 1 + l1 + 1);
    p += addpatt(L, p, 1);
    setinst(p++, IBackCommit, 2);
    setinst(p, IFail, 0);
  }
  return 1;
}

static int firstpart (Instruction *p, int l) {
  if (istest(p)) {
    int e = p[0].i.offset - 1;
    if ((p[e].i.code == IJmp || p[e].i.code == ICommit) &&
        (e + p[e].i.offset == l ||
         (p[e-1].i.code == INotAny && e+p[e].i.offset == l-1 &&
          p[e+p[e].i.offset].i.code == INotAny)))
      return e + 1;
  }
  else if (p[0].i.code == IChoice) {
    int e = p[0].i.offset - 1;
    if (p[e].i.code == ICommit && e + p[e].i.offset == l)
      return e + 1;
  }
  return 0;
}


static Instruction *auxnew (lua_State *L, Instruction **op, int *size,
                                         int extra) {
  *op = newpatt(L, *size + extra);
  jointable(L, 1);
  *size += extra;
  return *op + *size - extra;
}


static int nofail (Instruction *p, int l) {
  int i;
  for (i = 0; i < l; i += sizei(p + i)) {
    if (!isnofail(p + i)) return 0;
  }
  return 1;
}


static int interfere (Instruction *p1, int l1, CharsetTag *st2) {
  if (nofail(p1, l1) || p1->i.code == IString || p1->i.code == INotAny)  /* p1 cannot fail? */
    return 0;  /* nothing can intefere with it */
  if (st2->tag == NOINFO) return 1;
  assert(p1->i.offset != 0);
  switch (p1->i.code) {
    case IChar: return testchar(st2->cs, p1->i.aux);
    case ISet: return !exclusiveset(st2->cs, (p1 + 1)->buff);
    default: assert(p1->i.code == IAny); return 1;
  }
}

static int islist(Instruction *p, int l) {
  return (p->i.code == IOpen && (p+l-1)->i.code == IClose);
}

static Instruction *basicUnion (lua_State *L, Instruction *p1,
                                int l1, int l2, int *size, CharsetTag *st2) {
  Instruction *op;
  CharsetTag st1;
  tocharset(p1, &st1);
  if (st1.tag == ISCHARSET && st2->tag == ISCHARSET) {
    Instruction *p = auxnew(L, &op, size, CHARSETINSTSIZE);
    setinst(p, ISet, 0);
    loopset(i, p[1].buff[i] = st1.cs[i] | st2->cs[i]);
  }
  else if (exclusive(&st1, st2) || isheadfail(p1)) {
    Instruction *p = auxnew(L, &op, size, l1 + 1 + l2);
    copypatt(p, p1, l1);
    check2test(p, l1 + 1);
    p += l1;
    setinst(p++, IJmp, l2 + 1);
    addpatt(L, p, 2);
  }
  else {
    /* choice L1; e1; commit L2; L1: e2; L2: ... */
    Instruction *p = auxnew(L, &op, size, 1 + l1 + 1 + l2);
    setinst(p++, IChoice, 1 + l1 + 1);
    copypatt(p, p1, l1); p += l1;
    setinst(p++, ICommit, 1 + l2);
    addpatt(L, p, 2);
    optimizechoice(p - (1 + l1 + 1));
  }
  return op;
}


static Instruction *separateparts (lua_State *L, Instruction *p1,
                                   int l1, int l2, int *size, CharsetTag *st2) {
  int sp = firstpart(p1, l1);
  if (sp == 0)  /* first part is entire p1? */
    return basicUnion(L, p1, l1, l2, size, st2);
  else if ((p1 + sp - 1)->i.code == ICommit || !interfere(p1, sp, st2)) {
    Instruction *p;
    int init = *size;
    int end = init + sp;
    *size = end;
    p = separateparts(L, p1 + sp, l1 - sp, l2, size, st2);
    copypatt(p + init, p1, sp);
    (p + end - 1)->i.offset = *size - (end - 1);
    return p;
  }
  else {  /* must change back to non-optimized choice */
    Instruction *p;
    int init = *size;
    int end = init + sp + 1;  /* needs one extra instruction (choice) */
    int sizefirst = sizei(p1);  /* size of p1's first instruction (the test) */
    *size = end;
    p = separateparts(L, p1 + sp, l1 - sp, l2, size, st2);
    copypatt(p + init, p1, sizefirst);  /* copy the test */
    (p + init)->i.offset++;  /* correct jump (because of new instruction) */
    init += sizefirst;
    setinstaux(p + init, IChoice, sp - sizefirst + 1, 1); init++;
    copypatt(p + init, p1 + sizefirst, sp - sizefirst - 1);
    init += sp - sizefirst - 1;
    setinst(p + init, ICommit, *size - (end - 1));
    return p;
  }
}


static int union_l (lua_State *L) {
  int l1, l2;
  int size = 0;
  Instruction *p1 = getpatt(L, 1, &l1);
  Instruction *p2 = getpatt(L, 2, &l2);
  CharsetTag st2;
  if (isfail(p1))  /* check for simple identities */
    lua_pushvalue(L, 2);  /* fail / a == a */
  else if (isfail(p2) || issucc(p1))
    lua_pushvalue(L, 1);  /* a / fail == a; true / a == true */
  else if(islist(p1, l1) && islist(p2, l2)) {
    Instruction *op1, *op2, *op, *p;
    int l;
    op1 = newpatt(L, l1 - 1);
    jointable(L, 1);
    lua_replace(L, 1);
    copypatt(op1, p1 + 1, l1 - 2);
    setinst(op1 + l1 - 2, INotAny, 0);
    op2 = newpatt(L, l2 - 2);
    jointable(L, 2);
    lua_replace(L, 2);
    copypatt(op2, p2 + 1, l2 - 2);
    tocharset(op2, &st2);
    separateparts(L, op1, l1 - 1, l2 - 2, &size, &st2);
    p = getpatt(L, -1, &l);
    op = newpatt(L, l + 2);
    setinst(op++, IOpen, 0);
    addpatt(L, op, -2); op += l;
    setinst(op, IClose, 0);
    } else {
    tocharset(p2, &st2);
    separateparts(L, p1, l1, l2, &size, &st2);
  }
  return 1;
}


static int repeatcharset (lua_State *L, Charset cs, int l1, int n) {
  /* e; ...; e; span; */
  int i;
  Instruction *p = newpatt(L, n*l1 + CHARSETINSTSIZE);
  for (i = 0; i < n; i++) {
    p += addpatt(L, p, 1);
  }
  setinst(p, ISpan, 0);
  loopset(k, p[1].buff[k] = cs[k]);
  return 1;
}

static int repeatany(lua_State *L, int l1, int n) {
  int i;
  Instruction *p = newpatt(L, n*l1 + 1);
  for (i = 0; i < n; i++) {
    p += addpatt(L, p, 1);
  }
  setinst(p, IAnySpan, 0);
  return 1;
}

static Instruction *repeatheadfail (lua_State *L, int l1, int n) {
  /* e; ...; e; L2: e'(L1); jump L2; L1: ... */
  int i;
  Instruction *p = newpatt(L, (n + 1)*l1 + 1);
  Instruction *op = p;
  for (i = 0; i < n; i++) {
    p += addpatt(L, p, 1);
  }
  p += addpatt(L, p, 1);
  check2test(p - l1, l1 + 1);
  setinst(p, IJmp, -l1);
  return op;
}


static Instruction *repeats (lua_State *L, Instruction *p1, int l1, int n) {
  /* e; ...; e; choice L1; L2: e; partialcommit L2; L1: ... */
  int i;
  if(islist(p1, l1)) {
    Instruction *op = newpatt(L, (n + 1)*l1 + 1);
    Instruction *p = op;
    if (!verify(L, p1, p1, p1 + l1, 0, 0))
      luaL_error(L, "loop body may accept empty string");
    for (i = 0; i < n; i++) {
      p += addpatt(L, p, 1);
    }
    setinst(p++, IChoice, 1 + l1);
    p += addpatt(L, p, 1);
    setinst(p - 1, IPartialCloseCommit, -l1 + 2);
    return op;
  } else {
    Instruction *op = newpatt(L, (n + 1)*l1 + 2);
    Instruction *p = op;
    if (!verify(L, p1, p1, p1 + l1, 0, 0))
      luaL_error(L, "loop body may accept empty string");
    for (i = 0; i < n; i++) {
      p += addpatt(L, p, 1);
    }
    setinst(p++, IChoice, 1 + l1 + 1);
    p += addpatt(L, p, 1);
    setinst(p, IPartialCommit, -l1);
    return op;
  }
}


static void optionalheadfail (lua_State *L, int l1, int n) {
  Instruction *op = newpatt(L, n * l1);
  Instruction *p = op;
  int i;
  for (i = 0; i < n; i++) {
    p += addpatt(L, p, 1);
    check2test(p - l1, (n - i)*l1);
  }
}


static void optionals (lua_State *L, int l1, int n) {
  /* choice L1; e; partialcommit L2; L2: ... e; L1: commit L3; L3: ... */
  int i; int l;
  Instruction *p1 = getpatt(L, 1, &l);
  if(islist(p1, l1)) {
    Instruction *op = newpatt(L, n*l1 + 2);
    Instruction *p = op;
    setinst(p++, IChoice, 1 + n*(l1 + 1));
    for (i = 0; i < n; i++) {
      p += addpatt(L, p, 1);
      setinst(p - 1, IPartialCloseCommit, 1);
    }
    setinst(p - 1, IClose, 1); /* correct last */
    setinst(p++, ICommit, 1);
    optimizechoice(op);
  } else {
    Instruction *op = newpatt(L, n*(l1 + 1) + 1);
    Instruction *p = op;
    setinst(p++, IChoice, 1 + n*(l1 + 1));
    for (i = 0; i < n; i++) {
      p += addpatt(L, p, 1);
      setinst(p++, IPartialCommit, 1);
    }
    setinst(p - 1, ICommit, 1);  /* correct last commit */
    optimizechoice(op);
  }
}


static int star_l (lua_State *L) {
  int l1;
  int n = luaL_checkint(L, 2);
  Instruction *p1 = getpatt(L, 1, &l1);
  if (n >= 0) {
    CharsetTag st;
    Instruction *op;
    if (isany1(p1))
      return repeatany(L, l1, n);
    if (tocharset(p1, &st) == ISCHARSET)
      return repeatcharset(L, st.cs, l1, n);
    if (isheadfail(p1))
      op = repeatheadfail(L, l1, n);
    else
      op = repeats(L, p1, l1, n);
    optimizecaptures(op);
    optimizejumps(op);
  }
  else {
    if (isheadfail(p1))
      optionalheadfail(L, l1, -n);
    else
      optionals(L, l1, -n);
  }
  return 1;
}


static int getlabel (lua_State *L, int labelidx) {
  if (labelidx == 0) return 0;
  else return value2fenv(L, labelidx);
}


static int capture_aux (lua_State *L, int kind, int labelidx) {
  int l1, n, list;
  Instruction *p1 = getpatt(L, 1, &l1);
  if(islist(p1, l1) && kind == Cfunction) {
    Instruction *p = newpatt(L, l1 - 1);
    jointable(L, 1);
    copypatt(p, p1 + 1, l1 - 2);
    setinst(p + l1 - 2, INotAny, 0);
    lua_replace(L, 1);
    capture_aux(L, kind, labelidx);
    lua_replace(L, 1);
    return pattlist_l(L);
  }
  int lc = skipchecks(p1, 0, &n);
  if (lc == l1) {  /* got whole pattern? */
    /* may use a IFullCapture instruction at its end */
    Instruction *p = newpatt(L, l1 + 1);
    int label = getlabel(L, labelidx);
    p += addpatt(L, p, 1);
    setinstcap(p, IFullCapture, label, kind, n);
  }
  else {  /* must use open-close pair */
    Instruction *op = newpatt(L, 1 + l1 + 1);
    Instruction *p = op;
    setinstcap(p++, IOpenCapture, getlabel(L, labelidx), kind, 0);
    p += addpatt(L, p, 1);
    setinstcap(p, ICloseCapture, 0, Cclose, 0);
    optimizecaptures(op);
  }
  return 1;
}


static int capture_l (lua_State *L) { return capture_aux(L, Csimple, 0); }
static int tcapture_l (lua_State *L) { return capture_aux(L, Ctable, 0); }
static int capsubst_l (lua_State *L) { return capture_aux(L, Csubst, 0); }

static int rcapture_l (lua_State *L) {
  switch (lua_type(L, 2)) {
    case LUA_TFUNCTION: return capture_aux(L, Cfunction, 2);
    case LUA_TTABLE: return capture_aux(L, Cquery, 2);
    case LUA_TSTRING: return capture_aux(L, Cstring, 2);
    default: return luaL_argerror(L, 2, "invalid replacement value");
  }
}


static int fold_l (lua_State *L) {
  luaL_checktype(L, 2, LUA_TFUNCTION);
  return capture_aux(L, Cfold, 2);
}


static int group_l (lua_State *L) {
  if (lua_isnoneornil(L, 2))
    return capture_aux(L, Cgroup, 0);
  else {
    luaL_checkstring(L, 2);
    return capture_aux(L, Cgroup, 2);
  }
}


static int position_l (lua_State *L) {
  Instruction *p = newpatt(L, 1);
  setinstcap(p, IEmptyCapture, 0, Cposition, 0);
  return 1;
}


static int backref_l (lua_State *L) {
  Instruction *p = newpatt(L, 1);
  int n = getlabel(L, 1);
  setinstcap(p, IEmptyCaptureIdx, n, Cbackref, 0);
  return 1;
}


static int argcap_l (lua_State *L) {
  int n = luaL_checkint(L, 1);
  Instruction *p = newpatt(L, 1);
  luaL_argcheck(L, 0 < n && n <= SHRT_MAX, 1, "invalid argument index");
  setinstcap(p, IEmptyCapture, n, Carg, 0);
  return 1;
}


static int matchtime_l (lua_State *L) {
  int l1 = getpattl(L, 1);
  Instruction *op = newpatt(L, 1 + l1 + 1);
  Instruction *p = op;
  luaL_checktype(L, 2, LUA_TFUNCTION);
  setinstcap(p++, IOpenCapture, value2fenv(L, 2), Cruntime, 0);
  p += addpatt(L, p, 1);
  setinstcap(p, ICloseRunTime, 0, Cclose, 0);
  optimizecaptures(op);
  return 1;
}


static int capconst_l (lua_State *L) {
  int i, j;
  int n = lua_gettop(L);
  Instruction *p = newpatt(L, n > 1 ? n + 2 : n);
  lua_createtable(L, n, 0);  /* new environment for the new pattern */
  if (n > 1) setinstcap(p++, IOpenCapture, 0, Cgroup, 0);
  for (i = j = 1; i <= n; i++) {
    if (lua_isnil(L, i))
      setinstcap(p++, IEmptyCaptureIdx, 0, Cconst, 0);
    else {
      setinstcap(p++, IEmptyCaptureIdx, j, Cconst, 0);
      lua_pushvalue(L, i);
      lua_rawseti(L, -2, j++);
    }
  }
  if (n > 1) setinstcap(p++, ICloseCapture, 0, Cclose, 0);
  lua_setfenv(L, -2);   /* set environment */
  return 1;
}


/* }====================================================== */



/*
** {======================================================
** User-Defined Patterns
** =======================================================
*/

static void newpattfunc (lua_State *L, PattFunc f, const void *ud, size_t l) {
  int n = instsize(l) + 1;
  Instruction *p = newpatt(L, n);
  p[0].i.code = IFunc;
  p[0].i.offset = n;
  p[1].f = f;
  memcpy(p[2].buff, ud, l);
}


#include <ctype.h>

static Stream *span (const void *ud, Stream *s) {
  const char *u = (const char *)ud;
  if(s->kind != Sstring) return NULL;
  s->u.s.s += strspn(s->u.s.s, u);
  return s;
}


static int span_l (lua_State *L) {
  const char *s = luaL_checkstring(L, 1);
  newpattfunc(L, span, s, strlen(s) + 1);
  return 1;
}

/* }====================================================== */



/*
** {======================================================
** Captures
** =======================================================
*/


typedef struct CapState {
  Capture *cap;  /* current capture */
  Capture *ocap;  /* (original) capture list */
  lua_State *L;
  int ptop;  /* index of last argument to 'match' */
  int valuecached;  /* value stored in cache slot */
} CapState;


#define captype(cap)    ((cap)->kind)

#define isclosecap(cap) (captype(cap) == Cclose)

#define closeaddr(c)    ((c)->s.u.s.s + (c)->siz - 1)

#define isfullcap(cap)  ((cap)->siz != 0)

#define getfromenv(cs,v)        lua_rawgeti((cs)->L, penvidx((cs)->ptop), v)
#define pushluaval(cs)          getfromenv(cs, (cs)->cap->idx)

#define pushsubject(cs, c) { if((c)->s.kind == Sstring) lua_pushlstring((cs)->L, (c)->s.u.s.s, (c)->siz - 1); else if((c)->s.kind == Slist) pushitems((cs), (c), (c)->siz-1); else luaL_error((cs)->L, "invalid stream type"); }

#define pushmatch(cs, c1, c2) { if((c1)->s.kind == Sstring) lua_pushlstring((cs)->L, (c1)->s.u.s.s, (c2)->s.u.s.s - (c1)->s.u.s.s); else if((c1)->s.kind == Slist) pushitems((cs), (c1), (c2)->s.u.l.cur - (c1)->s.u.l.cur); else luaL_error((cs)->L, "invalid stream type"); }


#define updatecache(cs,v) { if ((v) != (cs)->valuecached) updatecache_(cs,v); }


static void pushitems (CapState *cs, Capture *c, int n) {
  if(n > 1) {
    int i;
    lua_createtable(cs->L, n, 0);
    lua_rawgeti(cs->L, plistidx(cs->ptop), c->s.u.l.ref);
    for(i = 1; i <= n; i++) {
      lua_rawgeti(cs->L, -1, c->s.u.l.cur + i - 1);
      lua_rawseti(cs->L, -3, i);
    }
    lua_pop(cs->L, 1);
  } else if(n == 1) {
    lua_rawgeti(cs->L, plistidx(cs->ptop), c->s.u.l.ref);
    lua_rawgeti(cs->L, -1, c->s.u.l.cur);
    lua_remove(cs->L, -2);
  } else lua_newtable(cs->L);
}

static void updatecache_ (CapState *cs, int v) {
  getfromenv(cs, v);
  lua_replace(cs->L, subscache(cs));
  cs->valuecached = v;
}


static int pushcapture (CapState *cs);


static Capture *findopen (Capture *cap) {
  int n = 0;
  for (;;) {
    cap--;
    if (isclosecap(cap)) n++;
    else if (!isfullcap(cap))
      if (n-- == 0) return cap;
  }
}


static Capture *nextcap (Capture *cap) {
  if (isfullcap(cap)) return cap + 1;
  else {
    int n = 0;
    for (;;) {
      cap++;
      if (isclosecap(cap)) {
        if (n-- == 0) return cap + 1;
      }
      else if (!isfullcap(cap)) n++;
    }
  }
}


static int pushallvalues (CapState *cs, int addextra) {
  Capture *co = cs->cap;
  int n = 0;
  if (isfullcap(cs->cap++)) {
    pushsubject(cs, co);  /* push whole match */
    return 1;
  }
  while (!isclosecap(cs->cap))
    n += pushcapture(cs);
  if (addextra || n == 0) {  /* need extra? */
    pushmatch(cs, co, cs->cap); /* push whole match */
    n++;
  }
  cs->cap++;  /* skip close entry */
  return n;
}


static Capture *findback (CapState *cs, Capture *cap) {
  lua_State *L = cs->L;
  for (;;) {
    if (cap == cs->ocap) {  /* not found */
      const char *s = lua_tostring(L, -1);
      if (s == NULL) s = lua_pushfstring(L, "(a %s)", luaL_typename(L, -1));
      luaL_error(L, "back reference '%s' not found", s);
    }
    cap--;
    if (isclosecap(cap))
      cap = findopen(cap);
    else if (!isfullcap(cap))
      continue; /* opening an enclosing capture: skip and get previous */
    if (captype(cap) == Cgroup) {
      getfromenv(cs, cap->idx);  /* get group name */
      if (lua_equal(L, -2, -1)) {  /* right group? */
        lua_pop(L, 2);  /* remove reference name and group name */
        return cap;
      }
      else lua_pop(L, 1);  /* remove group name */
    }
  }
}


static int backrefcap (CapState *cs) {
  int n;
  Capture *curr = cs->cap;
  pushluaval(cs);  /* reference name */
  cs->cap = findback(cs, curr);
  n = pushallvalues(cs, 0);
  cs->cap = curr + 1;
  return n;
}


static int tablecap (CapState *cs) {
  lua_State *L = cs->L;
  int n = 0;
  lua_newtable(L);
  if (isfullcap(cs->cap++))
    return 1;  /* table is empty */
  while (!isclosecap(cs->cap)) {
    if (captype(cs->cap) == Cgroup && cs->cap->idx != 0) {  /* named group? */
      int k;
      pushluaval(cs);  /* push group name */
      k = pushallvalues(cs, 0);
      if (k == 0) {  /* no value? */
        lua_pop(L, 1);  /* remove group name */
        continue;  /* and go on */
      }
      else if (k > 1)
        lua_pop(L, k - 1);  /* keep just one value */
      lua_settable(L, -3);
    }
    else {
      int i;
      int k = pushcapture(cs);
      for (i = k; i > 0; i--)
        lua_rawseti(L, -(i + 1), n + i);
      n += k;
    }
  }
  cs->cap++;  /* skip close entry */
  return 1;
}


static int querycap (CapState *cs) {
  int idx = cs->cap->idx;
  int n = pushallvalues(cs, 0);
  if (n > 1)  /* extra captures? */
    lua_pop(cs->L, n - 1);  /* throw them away */
  updatecache(cs, idx);
  lua_gettable(cs->L, subscache(cs));
  if (!lua_isnil(cs->L, -1))
    return 1;
  else {
    lua_pop(cs->L, 1);  /* remove value */
    return 0;
  }
}


static int foldcap (CapState *cs) {
  int n;
  lua_State *L = cs->L;
  int idx = cs->cap->idx;
  if (isfullcap(cs->cap++) || isclosecap(cs->cap) || (n = pushcapture(cs)) == 0)
    return luaL_error(L, "no initial value for fold capture");
  if (n > 1)
    lua_pop(L, n - 1);  /* leave only one result */
  while (!isclosecap(cs->cap)) {
    updatecache(cs, idx);
    lua_pushvalue(L, subscache(cs));  /* get folding function */
    lua_insert(L, -2);  /* put it before accumulator */
    n = pushcapture(cs);  /* get other captures */
    lua_call(L, n + 1, 1);  /* call folding function */
  }
  cs->cap++;  /* skip close entry */
  return 1;
}


static int functioncap (CapState *cs) {
  int n;
  int top = lua_gettop(cs->L);
  pushluaval(cs);
  n = pushallvalues(cs, 0);
  lua_call(cs->L, n, LUA_MULTRET);
  return lua_gettop(cs->L) - top;
}


static int runtimecap (lua_State *L, Capture *close, Capture *ocap,
                       Stream *s, int ptop) {
  CapState cs;
  int n;
  Capture *open = findopen(close);
  assert(captype(open) == Cruntime);
  close->kind = Cclose;
  close->s = *s;
  cs.ocap = ocap; cs.cap = open; cs.L = L;
  cs.valuecached = 0; cs.ptop = ptop;
  luaL_checkstack(L, 4, "too many runtime captures");
  pushluaval(&cs);
  switch(s->kind) {  /* current position */
    case Sstring: {
      lua_pushlstring(L, s->u.s.o, s->u.s.e - s->u.s.o);
      lua_pushinteger(L, s->u.s.s - s->u.s.o + 1);
      break;
    }
    case Slist: {
      lua_rawgeti(L, plistidx(ptop), s->u.l.ref);
      lua_pushinteger(L, s->u.l.cur - 1);
      break;
    }
    default:
      luaL_error(L, "dynamic captures with this stream type not supported");
  }
  n = pushallvalues(&cs, 0);
  lua_call(L, n + 2, LUA_MULTRET);
  return close - open;
}



typedef struct StrAux {
  int isstring;
  union {
    Capture *cp;
    struct {
      const char *s;
      const char *e;
    } s;
  } u;
} StrAux;

#define MAXSTRCAPS      10

static int getstrcaps (CapState *cs, StrAux *cps, int n) {
  int k = n++;
  int skind = cs->cap->s.kind;
  cps[k].isstring = 1;
  switch(skind) {
    case Sstring: cps[k].u.s.s = cs->cap->s.u.s.s; break;
    case Slist: {
      size_t l;
      lua_rawgeti(cs->L, plistidx(cs->ptop), cs->cap->s.u.l.ref);
      lua_rawgeti(cs->L, -1, cs->cap->s.u.l.cur);
      cps[k].u.s.s = lua_tolstring(cs->L, -1, &l);
      cps[k].u.s.e = cps[k].u.s.s + l;
      lua_pop(cs->L, 2);
      break;
    }
    default: luaL_error(cs->L, "no string captures for this stream");
  }
  if (!isfullcap(cs->cap++)) {
    while (!isclosecap(cs->cap)) {
      if (n >= MAXSTRCAPS)  /* too many captures? */
        cs->cap = nextcap(cs->cap);  /* skip it */
      else if (captype(cs->cap) == Csimple)
        n = getstrcaps(cs, cps, n);
      else {
        cps[n].isstring = 0;
        cps[n].u.cp = cs->cap;
        cs->cap = nextcap(cs->cap);
        n++;
      }
    }
    cs->cap++;  /* skip close */
  }
  if(skind == Sstring) cps[k].u.s.e = closeaddr(cs->cap - 1);
  return n;
}


/*
** add next capture (which should be a string) to buffer
*/
static int addonestring (luaL_Buffer *b, CapState *cs, const char *what);

/*
** push next capture
*/
static int pushoneitem (int tab, CapState *cs, const char *what);


static void stringcap (luaL_Buffer *b, CapState *cs) {
  StrAux cps[MAXSTRCAPS];
  int n;
  size_t len, i;
  const char *c;
  updatecache(cs, cs->cap->idx);
  c = lua_tolstring(cs->L, subscache(cs), &len);
  n = getstrcaps(cs, cps, 0) - 1;
  for (i = 0; i < len; i++) {
    if (c[i] != '%' || c[++i] < '0' || c[i] > '9')
      luaL_addchar(b, c[i]);
    else {
      int l = c[i] - '0';
      if (l > n)
        luaL_error(cs->L, "invalid capture index (%d)", l);
      else if (cps[l].isstring)
        luaL_addlstring(b, cps[l].u.s.s, cps[l].u.s.e - cps[l].u.s.s);
      else {
        Capture *curr = cs->cap;
        cs->cap = cps[l].u.cp;
        if (addonestring(b, cs, "capture") == 0)
          luaL_error(cs->L, "no values in capture index %d", l);
        cs->cap = curr;
      }
    }
  }
}

typedef struct SubstAux {
  union {
    luaL_Buffer *b;
    int tab;
  } u;
} SubstAux;

static int pushcapture (CapState *cs);

static void substcap (SubstAux *sa, CapState *cs) {
  switch(cs->cap->s.kind) {
    case Sstring: {
      luaL_Buffer *b = sa->u.b;
      const char *curr = cs->cap->s.u.s.s;
      if (isfullcap(cs->cap))  /* no nested captures? */
        luaL_addlstring(b, curr, cs->cap->siz - 1);  /* keep original text */
      else {
        cs->cap++;
        while (!isclosecap(cs->cap)) {
          const char *next = cs->cap->s.u.s.s;
          luaL_addlstring(b, curr, next - curr);  /* add text up to capture */
          if (addonestring(b, cs, "replacement") == 0)  /* no capture value? */
            curr = next;  /* keep original text in final result */
          else
            curr = closeaddr(cs->cap - 1);  /* continue after match */
        }
        luaL_addlstring(b, curr, cs->cap->s.u.s.s - curr);  /* add last piece of text */
      }
      break;
    }
    case Slist: {
      int ref = cs->cap->s.u.l.ref;
      int tabidx = sa->u.tab;
      int last = lua_objlen(cs->L, tabidx);
      int curr = cs->cap->s.u.l.cur;
      if (isfullcap(cs->cap)) {  /* no nested captures?  keep original */
        int i;
        lua_rawgeti(cs->L, plistidx(cs->ptop), cs->cap->s.u.l.ref);
        for(i = 0; i < cs->cap->siz; i++) {
          lua_rawgeti(cs->L, -1, cs->cap->s.u.l.cur + i);
          lua_rawseti(cs->L, tabidx, ++last);
        }
        lua_pop(cs->L, 1);
      } else {
        cs->cap++;
        while (!isclosecap(cs->cap)) {
          if(cs->cap->s.kind != Slist || cs->cap->s.u.l.ref != ref) {
            luaL_error(cs->L, "can't mix different streams on substitution capture");
          } else {
            int next = cs->cap->s.u.l.cur;
            lua_rawgeti(cs->L, plistidx(cs->ptop), ref);
            for(; curr < next; curr++) { /* add items up to capture */
              lua_rawgeti(cs->L, -1, curr);
              lua_rawseti(cs->L, tabidx, ++last);
            }
            lua_pop(cs->L, 1);
            if (pushoneitem(tabidx, cs, "replacement") == 0) /* no capture value? */
              curr = next;
            else {
              lua_rawseti(cs->L, tabidx, ++last);
              curr = (cs->cap-1)->s.u.l.cur + (cs->cap-1)->siz - 1;
            }
          }
        }
        lua_rawgeti(cs->L, plistidx(cs->ptop), cs->cap->s.u.l.ref);
        for(; curr < cs->cap->s.u.l.cur; curr++) {
          lua_rawgeti(cs->L, -1, curr);
          lua_rawseti(cs->L, tabidx, ++last);
        }
        lua_pop(cs->L, 1);
      }
      break;
    }
  }
  cs->cap++;  /* go to next capture */
}


static int addonestring (luaL_Buffer *b, CapState *cs, const char *what) {
  switch (captype(cs->cap)) {
    case Cstring:
      stringcap(b, cs);  /* add capture directly to buffer */
      return 1;
    case Csubst: {
      SubstAux sa;
      sa.u.b = b;
      substcap(&sa, cs);  /* add capture directly to buffer */
      return 1;
    }
    default: {
      lua_State *L = cs->L;
      int n = pushcapture(cs);
      if (n > 0) {
        if (n > 1) lua_pop(L, n - 1);  /* only one result */
        if (!lua_isstring(L, -1))
          luaL_error(L, "invalid %s value (a %s)", what, luaL_typename(L, -1));
        luaL_addvalue(b);
      }
      return n;
    }
  }
}

static int pushoneitem (int tab, CapState *cs, const char *what) {
  switch (captype(cs->cap)) {
    case Cstring: {
      luaL_Buffer b;
      luaL_buffinit(cs->L, &b);
      stringcap(&b, cs);  /* add capture directly to buffer */
      luaL_pushresult(&b);
      return 1;
    }
    case Csubst: {
      SubstAux sa; sa.u.tab = tab;
      substcap(&sa, cs);  /* add capture directly to buffer */
      return 1;
    }
    default: {
      lua_State *L = cs->L;
      int n = pushcapture(cs);
      if(n > 1) lua_pop(L, n - 1);
      return n;
    }
  }
}


static int pushcapture (CapState *cs) {
  luaL_checkstack(cs->L, 4, "too many captures");
  switch (captype(cs->cap)) {
    case Cposition: {
      lua_pushinteger(cs->L, cs->cap->s.u.s.s - cs->cap->s.u.s.o + 1);
      cs->cap++;
      return 1;
    }
    case Cconst: {
      pushluaval(cs);
      cs->cap++;
      return 1;
    }
    case Carg: {
      int arg = (cs->cap++)->idx;
      if (arg + FIXEDARGS > cs->ptop)
        return luaL_error(cs->L, "reference to absent argument #%d", arg);
      lua_pushvalue(cs->L, arg + FIXEDARGS);
      return 1;
    }
    case Csimple: {
      int k = pushallvalues(cs, 1);
      if (k > 1)
        lua_insert(cs->L, -k);  /* whole match is first result */
      return k;
    }
    case Cruntime: {
      int n = 0;
      while (!isclosecap(cs->cap++)) {
        luaL_checkstack(cs->L, 4, "too many captures");
        lua_rawgeti(cs->L, pdyncapidx(cs->ptop), (cs->cap - 1)->idx);
        luaL_unref(cs->L, pdyncapidx(cs->ptop), (cs->cap - 1)->idx);
        n++;
      }
      return n;
    }
    case Cstring: {
      luaL_Buffer b;
      luaL_buffinit(cs->L, &b);
      stringcap(&b, cs);
      luaL_pushresult(&b);
      return 1;
    }
    case Csubst: {
      switch(cs->cap->s.kind) {
        case Sstring: {
          SubstAux sa;
          luaL_Buffer b;
          luaL_buffinit(cs->L, &b);
          sa.u.b = &b;
          substcap(&sa, cs);
          luaL_pushresult(&b);
          break;
        }
        case Slist: {
          SubstAux sa;
          lua_newtable(cs->L);
          sa.u.tab = lua_gettop(cs->L);
          substcap(&sa, cs);
          break;
        }
      }
      return 1;
    }
    case Cgroup: {
      if (cs->cap->idx == 0)  /* anonymous group? */
        return pushallvalues(cs, 0);  /* add all nested values */
      else {  /* named group: add no values */
        cs->cap = nextcap(cs->cap);  /* skip capture */
        return 0;
      }
    }
    case Cbackref: return backrefcap(cs);
    case Ctable: return tablecap(cs);
    case Cfunction: return functioncap(cs);
    case Cquery: return querycap(cs);
    case Cfold: return foldcap(cs);
    default: assert(0); return 0;
  }
}


static int getcaptures (lua_State *L, Stream *r, int ptop) {
  Capture *capture = (Capture *)lua_touserdata(L, caplistidx(ptop));
  int n = 0;
  if (!isclosecap(capture)) {  /* is there any capture? */
    CapState cs;
    cs.ocap = cs.cap = capture; cs.L = L;
    cs.valuecached = 0; cs.ptop = ptop;
    do {  /* collect their values */
      n += pushcapture(&cs);
    } while (!isclosecap(cs.cap));
  }
  if (n == 0) {  /* no capture values? */
    switch(r->kind) {  /* return only end position */
      case Slist: lua_pushinteger(L, r->u.l.cur); break;
      case Sstring: lua_pushinteger(L, r->u.s.s - r->u.s.o + 1); break;
      default: lua_pushnil(L);
    }
    n = 1;
  }
  return n;
}

/* }====================================================== */


static int version_l (lua_State *L) {
  lua_pushstring(L, VERSION);
  return 1;
}


static int type_l (lua_State *L) {
  if (testpattern(L, 1))
    lua_pushliteral(L, "pattern");
  else
    lua_pushnil(L);
  return 1;
}


static void createcat (lua_State *L, const char *catname, int (catf) (int)) {
  Instruction *p = newcharset(L);
  int i;
  for (i = 0; i < CHAR_MAX; i++)
    if (catf(i)) setchar(p[1].buff, i);
  lua_setfield(L, -2, catname);
}


static int locale_l (lua_State *L) {
  if (lua_isnoneornil(L, 1)) {
    lua_settop(L, 0);
    lua_createtable(L, 0, 12);
  }
  else
    luaL_checktype(L, 1, LUA_TTABLE);
  createcat(L, "alnum", isalnum);
  createcat(L, "alpha", isalpha);
  createcat(L, "cntrl", iscntrl);
  createcat(L, "digit", isdigit);
  createcat(L, "graph", isgraph);
  createcat(L, "lower", islower);
  createcat(L, "print", isprint);
  createcat(L, "punct", ispunct);
  createcat(L, "space", isspace);
  createcat(L, "upper", isupper);
  createcat(L, "xdigit", isxdigit);
  return 1;
}


static int printpat_l (lua_State *L) {
  Instruction *p = getpatt(L, 1, NULL);
  int n, i;
  lua_getfenv(L, 1);
  n = lua_objlen(L, -1);
  printf("listlpeg [");
  for (i = 1; i <= n; i++) {
    printf("%d = ", i);
    lua_rawgeti(L, -1, i);
    if (lua_isstring(L, -1))
      printf("%s  ", lua_tostring(L, -1));
    else
      printf("%s  ", lua_typename(L, lua_type(L, -1)));
    lua_pop(L, 1);
  }
  printf("]\n");
  printpatt(p);
  return 0;
}


static int matchl (lua_State *L) {
  Capture capture[IMAXCAPTURES];
  size_t l;
  Stream s;
  Stream *r;
  Instruction *p = getpatt(L, 1, NULL);
  int ptop = lua_gettop(L);
  lua_pushnil(L);  /* subscache */
  lua_pushlightuserdata(L, capture);  /* caplistidx */
  lua_getfenv(L, 1);  /* penvidx */
  lua_createtable(L, 0, 0); /* lists */
  lua_createtable(L, 0, 0); /* runtime capture values */
  switch(lua_type(L, SUBJIDX)) {
    case LUA_TSTRING: {
      lua_Integer ii; size_t i;
      s.kind = Sstring;
      s.u.s.o = lua_tolstring(L, SUBJIDX, &l);
      ii = luaL_optinteger(L, 3, 1);
      i = (ii > 0) ?
        (((size_t)ii <= l) ? (size_t)ii - 1 : l) :
        (((size_t)-ii <= l) ? l - ((size_t)-ii) : 0);
      s.u.s.s = s.u.s.o + i;
      s.u.s.e = s.u.s.o + l;
      break;
    }
    case LUA_TTABLE: {
      lua_Integer ii; size_t i;
      s.kind = Slist;
      l = lua_objlen(L, SUBJIDX);
      ii = luaL_optinteger(L, 3, 1);
      i = (ii > 0) ?
        (((size_t)ii <= l) ? ii : (int)l) :
        (((size_t)-ii <= l) ? (int)l + ii + 1 : 1);
      lua_pushvalue(L, SUBJIDX);
      s.u.l.ref = luaL_ref(L, plistidx(ptop));
      s.u.l.cur = i;
      break;
    }
    default: luaL_error(L, "subject type not supported");
  }
  r = match(L, &s, p, capture, ptop);
  if (r == NULL) {
    lua_pushnil(L);
    return 1;
  }
  return getcaptures(L, r, ptop);
}


static struct luaL_reg pattreg[] = {
  {"match", matchl},
  {"print", printpat_l},
  {"locale", locale_l},
  {"C", capture_l},
  {"Cf", fold_l},
  {"Cc", capconst_l},
  {"Cg", group_l},
  {"Cp", position_l},
  {"Cb", backref_l},
  {"Carg", argcap_l},
  {"Cmt", matchtime_l},
  {"Cs", capsubst_l},
  {"Ct", tcapture_l},
  {"P", pattern_l},
  {"R", range_l},
  {"S", set_l},
  {"V", nter_l},
  {"L", pattlist_l},
  {"span", span_l},
  {"type", type_l},
  {"version", version_l},
  {NULL, NULL}
};


static struct luaL_reg metapattreg[] = {
  {"__add", union_l},
  {"__pow", star_l},
  {"__sub", diff_l},
  {"__mul", concat_l},
  {"__div", rcapture_l},
  {"__unm", unm_l},
  {"__len", pattand_l},
  {NULL, NULL}
};


extern "C" int luaopen_listlpeg (lua_State *L);
int luaopen_listlpeg (lua_State *L) {
  lua_newtable(L);
  lua_replace(L, LUA_ENVIRONINDEX);  /* empty env for new patterns */
  luaL_newmetatable(L, PATTERN_T);
  luaL_register(L, NULL, metapattreg);
  luaL_register(L, "listlpeg", pattreg);
  lua_pushliteral(L, "__index");
  lua_pushvalue(L, -2);
  lua_settable(L, -4);
  return 1;
}
