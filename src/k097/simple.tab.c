/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1


/* Substitute the variable and function names.  */
#define yyparse         KCparse
#define yylex           KClex
#define yyerror         KCerror
#define yydebug         KCdebug
#define yynerrs         KCnerrs
#define yylval          KClval
#define yychar          KCchar

/* First part of user prologue.  */
#line 4 "simple.y"

#include <stdio.h>
#include "d.h"

#line 83 "simple.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "simple.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_ID = 3,                         /* ID  */
  YYSYMBOL_QUOTE = 4,                      /* QUOTE  */
  YYSYMBOL_SINGLEQUOTE = 5,                /* SINGLEQUOTE  */
  YYSYMBOL_NUMBER = 6,                     /* NUMBER  */
  YYSYMBOL_CLASS = 7,                      /* CLASS  */
  YYSYMBOL_SUPER = 8,                      /* SUPER  */
  YYSYMBOL_OPERATOR = 9,                   /* OPERATOR  */
  YYSYMBOL_FINAL = 10,                     /* FINAL  */
  YYSYMBOL_EXTENDS = 11,                   /* EXTENDS  */
  YYSYMBOL_INCETANCEVARIABLE = 12,         /* INCETANCEVARIABLE  */
  YYSYMBOL_THIS = 13,                      /* THIS  */
  YYSYMBOL_NEW = 14,                       /* NEW  */
  YYSYMBOL_SIZEOFTHISCLASS = 15,           /* SIZEOFTHISCLASS  */
  YYSYMBOL_STARTOFTHISCLASS = 16,          /* STARTOFTHISCLASS  */
  YYSYMBOL_MODULE = 17,                    /* MODULE  */
  YYSYMBOL_PRINT = 18,                     /* PRINT  */
  YYSYMBOL_LOCAL = 19,                     /* LOCAL  */
  YYSYMBOL_DEF = 20,                       /* DEF  */
  YYSYMBOL_SM1 = 21,                       /* SM1  */
  YYSYMBOL_LOAD = 22,                      /* LOAD  */
  YYSYMBOL_TEST = 23,                      /* TEST  */
  YYSYMBOL_SPECIAL = 24,                   /* SPECIAL  */
  YYSYMBOL_AUTO = 25,                      /* AUTO  */
  YYSYMBOL_BREAK = 26,                     /* BREAK  */
  YYSYMBOL_CASE = 27,                      /* CASE  */
  YYSYMBOL_CHAR = 28,                      /* CHAR  */
  YYSYMBOL_CONST = 29,                     /* CONST  */
  YYSYMBOL_CONTINUE = 30,                  /* CONTINUE  */
  YYSYMBOL_DEFAULT = 31,                   /* DEFAULT  */
  YYSYMBOL_DO = 32,                        /* DO  */
  YYSYMBOL_DOUBLE = 33,                    /* DOUBLE  */
  YYSYMBOL_ELSE = 34,                      /* ELSE  */
  YYSYMBOL_ENUM = 35,                      /* ENUM  */
  YYSYMBOL_EXTERN = 36,                    /* EXTERN  */
  YYSYMBOL_FLOAT = 37,                     /* FLOAT  */
  YYSYMBOL_FOR = 38,                       /* FOR  */
  YYSYMBOL_GOTO = 39,                      /* GOTO  */
  YYSYMBOL_IF = 40,                        /* IF  */
  YYSYMBOL_INT = 41,                       /* INT  */
  YYSYMBOL_LONG = 42,                      /* LONG  */
  YYSYMBOL_REGISTER = 43,                  /* REGISTER  */
  YYSYMBOL_RETURN = 44,                    /* RETURN  */
  YYSYMBOL_SHORT = 45,                     /* SHORT  */
  YYSYMBOL_SIGNED = 46,                    /* SIGNED  */
  YYSYMBOL_SIZEOF = 47,                    /* SIZEOF  */
  YYSYMBOL_STATIC = 48,                    /* STATIC  */
  YYSYMBOL_STRUCT = 49,                    /* STRUCT  */
  YYSYMBOL_SWITCH = 50,                    /* SWITCH  */
  YYSYMBOL_TYPEDEF = 51,                   /* TYPEDEF  */
  YYSYMBOL_UNION = 52,                     /* UNION  */
  YYSYMBOL_UNSIGNED = 53,                  /* UNSIGNED  */
  YYSYMBOL_VOLATILE = 54,                  /* VOLATILE  */
  YYSYMBOL_VOID = 55,                      /* VOID  */
  YYSYMBOL_WHILE = 56,                     /* WHILE  */
  YYSYMBOL_PSFOR = 57,                     /* PSFOR  */
  YYSYMBOL_PROMPT = 58,                    /* PROMPT  */
  YYSYMBOL_59_ = 59,                       /* '='  */
  YYSYMBOL_PUT = 60,                       /* PUT  */
  YYSYMBOL_MULTPUT = 61,                   /* MULTPUT  */
  YYSYMBOL_NEGATEPUT = 62,                 /* NEGATEPUT  */
  YYSYMBOL_RESIDUEPUT = 63,                /* RESIDUEPUT  */
  YYSYMBOL_OR = 64,                        /* OR  */
  YYSYMBOL_AND = 65,                       /* AND  */
  YYSYMBOL_66_ = 66,                       /* '|'  */
  YYSYMBOL_67_ = 67,                       /* '&'  */
  YYSYMBOL_EQUAL = 68,                     /* EQUAL  */
  YYSYMBOL_NOTEQUAL = 69,                  /* NOTEQUAL  */
  YYSYMBOL_70_ = 70,                       /* '<'  */
  YYSYMBOL_71_ = 71,                       /* '>'  */
  YYSYMBOL_LESSEQUAL = 72,                 /* LESSEQUAL  */
  YYSYMBOL_GREATEREQUAL = 73,              /* GREATEREQUAL  */
  YYSYMBOL_LEFTSHIFT = 74,                 /* LEFTSHIFT  */
  YYSYMBOL_RIGHTSHIFT = 75,                /* RIGHTSHIFT  */
  YYSYMBOL_76_ = 76,                       /* '+'  */
  YYSYMBOL_77_ = 77,                       /* '-'  */
  YYSYMBOL_78_ = 78,                       /* '*'  */
  YYSYMBOL_79_ = 79,                       /* '/'  */
  YYSYMBOL_80_ = 80,                       /* '%'  */
  YYSYMBOL_UNARYMINUS = 81,                /* UNARYMINUS  */
  YYSYMBOL_82_ = 82,                       /* '!'  */
  YYSYMBOL_INCREMENT = 83,                 /* INCREMENT  */
  YYSYMBOL_DECREMENT = 84,                 /* DECREMENT  */
  YYSYMBOL_85_ = 85,                       /* '~'  */
  YYSYMBOL_86_ = 86,                       /* '^'  */
  YYSYMBOL_87_ = 87,                       /* '.'  */
  YYSYMBOL_MEMBER = 88,                    /* MEMBER  */
  YYSYMBOL_89_ = 89,                       /* ';'  */
  YYSYMBOL_90_ = 90,                       /* ':'  */
  YYSYMBOL_91_ = 91,                       /* ','  */
  YYSYMBOL_92_ = 92,                       /* '['  */
  YYSYMBOL_93_ = 93,                       /* ']'  */
  YYSYMBOL_94_ = 94,                       /* '{'  */
  YYSYMBOL_95_ = 95,                       /* '}'  */
  YYSYMBOL_96_ = 96,                       /* '('  */
  YYSYMBOL_97_ = 97,                       /* ')'  */
  YYSYMBOL_YYACCEPT = 98,                  /* $accept  */
  YYSYMBOL_program = 99,                   /* program  */
  YYSYMBOL_globalstatements = 100,         /* globalstatements  */
  YYSYMBOL_sentence = 101,                 /* sentence  */
  YYSYMBOL_statements = 102,               /* statements  */
  YYSYMBOL_localsentence = 103,            /* localsentence  */
  YYSYMBOL_argList = 104,                  /* argList  */
  YYSYMBOL_return = 105,                   /* return  */
  YYSYMBOL_break = 106,                    /* break  */
  YYSYMBOL_list_prefix = 107,              /* list_prefix  */
  YYSYMBOL_list = 108,                     /* list  */
  YYSYMBOL_curryBrace = 109,               /* curryBrace  */
  YYSYMBOL_if_prefix = 110,                /* if_prefix  */
  YYSYMBOL_if_body = 111,                  /* if_body  */
  YYSYMBOL_if = 112,                       /* if  */
  YYSYMBOL_for_prefix0 = 113,              /* for_prefix0  */
  YYSYMBOL_for_exit = 114,                 /* for_exit  */
  YYSYMBOL_for_inc = 115,                  /* for_inc  */
  YYSYMBOL_for_prefix = 116,               /* for_prefix  */
  YYSYMBOL_for = 117,                      /* for  */
  YYSYMBOL_PSfor_prefix = 118,             /* PSfor_prefix  */
  YYSYMBOL_PSfor = 119,                    /* PSfor  */
  YYSYMBOL_while_prefix0 = 120,            /* while_prefix0  */
  YYSYMBOL_while_prefix = 121,             /* while_prefix  */
  YYSYMBOL_while = 122,                    /* while  */
  YYSYMBOL_print = 123,                    /* print  */
  YYSYMBOL_sm1 = 124,                      /* sm1  */
  YYSYMBOL_load = 125,                     /* load  */
  YYSYMBOL_sm1ArgList = 126,               /* sm1ArgList  */
  YYSYMBOL_primitive = 127,                /* primitive  */
  YYSYMBOL_functionCall_prefix = 128,      /* functionCall_prefix  */
  YYSYMBOL_functionCall = 129,             /* functionCall  */
  YYSYMBOL_idList = 130,                   /* idList  */
  YYSYMBOL_functionHead = 131,             /* functionHead  */
  YYSYMBOL_voidfunctionHead = 132,         /* voidfunctionHead  */
  YYSYMBOL_functionArg = 133,              /* functionArg  */
  YYSYMBOL_functionBody = 134,             /* functionBody  */
  YYSYMBOL_functionDefinition = 135,       /* functionDefinition  */
  YYSYMBOL_declaration_prefix = 136,       /* declaration_prefix  */
  YYSYMBOL_declaration = 137,              /* declaration  */
  YYSYMBOL_extern_idList = 138,            /* extern_idList  */
  YYSYMBOL_extern_declaration_prefix = 139, /* extern_declaration_prefix  */
  YYSYMBOL_extern_declaration = 140,       /* extern_declaration  */
  YYSYMBOL_arrayValue = 141,               /* arrayValue  */
  YYSYMBOL_array = 142,                    /* array  */
  YYSYMBOL_arrayIndex = 143,               /* arrayIndex  */
  YYSYMBOL_array_prefix = 144,             /* array_prefix  */
  YYSYMBOL_id_set = 145,                   /* id_set  */
  YYSYMBOL_set = 146,                      /* set  */
  YYSYMBOL_exp = 147,                      /* exp  */
  YYSYMBOL_test = 148,                     /* test  */
  YYSYMBOL_special = 149,                  /* special  */
  YYSYMBOL_member_functionCall = 150,      /* member_functionCall  */
  YYSYMBOL_member_functionCall_prefix = 151, /* member_functionCall_prefix  */
  YYSYMBOL_class_definition = 152,         /* class_definition  */
  YYSYMBOL_class_definition_prefix = 153,  /* class_definition_prefix  */
  YYSYMBOL_incetance_variables = 154,      /* incetance_variables  */
  YYSYMBOL_incetance_variables_list = 155, /* incetance_variables_list  */
  YYSYMBOL_operatorfunctionHead = 156,     /* operatorfunctionHead  */
  YYSYMBOL_super = 157,                    /* super  */
  YYSYMBOL_new = 158                       /* new  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int16 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  93
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1464

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  98
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  61
/* YYNRULES -- Number of rules.  */
#define YYNRULES  163
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  303

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   329


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    82,     2,     2,     2,    80,    67,     2,
      96,    97,    78,    76,    91,    77,    87,    79,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    90,    89,
      70,    59,    71,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    92,     2,    93,    86,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    94,    66,    95,    85,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    60,    61,    62,    63,    64,    65,
      68,    69,    72,    73,    74,    75,    81,    83,    84,    88
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    42,    42,    47,    48,    53,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    68,    69,    74,    75,
      76,    77,    78,    79,    80,    81,    85,    86,    91,    92,
      95,   100,   103,   104,   108,   109,   113,   116,   117,   120,
     122,   123,   128,   129,   132,   133,   136,   140,   142,   146,
     151,   165,   170,   174,   175,   179,   184,   188,   192,   193,
     194,   195,   196,   198,   199,   200,   201,   202,   206,   214,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   232,   241,   243,   248,   249,   253,   262,
     271,   274,   281,   283,   288,   292,   296,   303,   307,   312,
     314,   318,   322,   324,   328,   331,   334,   335,   338,   350,
     353,   362,   363,   367,   368,   369,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   381,   382,   383,
     384,   385,   386,   387,   388,   389,   390,   391,   395,   399,
     400,   401,   402,   403,   408,   409,   410,   411,   418,   426,
     437,   446,   448,   454,   460,   469,   486,   490,   496,   500,
     506,   516,   520,   529
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "ID", "QUOTE",
  "SINGLEQUOTE", "NUMBER", "CLASS", "SUPER", "OPERATOR", "FINAL",
  "EXTENDS", "INCETANCEVARIABLE", "THIS", "NEW", "SIZEOFTHISCLASS",
  "STARTOFTHISCLASS", "MODULE", "PRINT", "LOCAL", "DEF", "SM1", "LOAD",
  "TEST", "SPECIAL", "AUTO", "BREAK", "CASE", "CHAR", "CONST", "CONTINUE",
  "DEFAULT", "DO", "DOUBLE", "ELSE", "ENUM", "EXTERN", "FLOAT", "FOR",
  "GOTO", "IF", "INT", "LONG", "REGISTER", "RETURN", "SHORT", "SIGNED",
  "SIZEOF", "STATIC", "STRUCT", "SWITCH", "TYPEDEF", "UNION", "UNSIGNED",
  "VOLATILE", "VOID", "WHILE", "PSFOR", "PROMPT", "'='", "PUT", "MULTPUT",
  "NEGATEPUT", "RESIDUEPUT", "OR", "AND", "'|'", "'&'", "EQUAL",
  "NOTEQUAL", "'<'", "'>'", "LESSEQUAL", "GREATEREQUAL", "LEFTSHIFT",
  "RIGHTSHIFT", "'+'", "'-'", "'*'", "'/'", "'%'", "UNARYMINUS", "'!'",
  "INCREMENT", "DECREMENT", "'~'", "'^'", "'.'", "MEMBER", "';'", "':'",
  "','", "'['", "']'", "'{'", "'}'", "'('", "')'", "$accept", "program",
  "globalstatements", "sentence", "statements", "localsentence", "argList",
  "return", "break", "list_prefix", "list", "curryBrace", "if_prefix",
  "if_body", "if", "for_prefix0", "for_exit", "for_inc", "for_prefix",
  "for", "PSfor_prefix", "PSfor", "while_prefix0", "while_prefix", "while",
  "print", "sm1", "load", "sm1ArgList", "primitive", "functionCall_prefix",
  "functionCall", "idList", "functionHead", "voidfunctionHead",
  "functionArg", "functionBody", "functionDefinition",
  "declaration_prefix", "declaration", "extern_idList",
  "extern_declaration_prefix", "extern_declaration", "arrayValue", "array",
  "arrayIndex", "array_prefix", "id_set", "set", "exp", "test", "special",
  "member_functionCall", "member_functionCall_prefix", "class_definition",
  "class_definition_prefix", "incetance_variables",
  "incetance_variables_list", "operatorfunctionHead", "super", "new", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-181)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-110)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     268,   -43,   -45,  -181,  -181,   -37,    60,  -181,    20,   -29,
    -181,  -181,   -15,    10,    -8,    13,    -4,    -3,    -1,     7,
      14,    15,    -6,   840,   840,   840,  -181,  -181,   840,    94,
    -181,   172,   697,  -181,   512,  -181,   746,    23,  -181,    23,
    -181,   218,    23,  -181,  -181,  -181,  -181,  -181,   314,  -181,
      16,    16,  -181,  -181,    37,   840,   -39,  -181,  1131,  -181,
    -181,  -181,    79,    16,    17,  -181,  -181,  -181,  -181,    95,
      89,   840,   362,   840,  -181,   115,   116,    18,  -181,  -181,
     117,    83,   840,    30,   791,   840,  -181,   120,  -181,    28,
     -34,   -34,   832,  -181,  -181,  -181,    27,  1067,  -181,   840,
    -181,   558,  -181,    35,    38,  -181,    92,  -181,  -181,  -181,
    -181,  1162,  -181,   399,  1192,  -181,  -181,  -181,   877,  -181,
    -181,    32,     5,   -10,   -10,   840,    62,  1099,   840,  -181,
    -181,   840,   840,   840,   840,   840,   840,   840,   840,   840,
     840,   840,   840,   840,   840,   840,   840,   840,   840,   840,
     840,   840,   840,   127,   840,  -181,  -181,     8,   268,   -10,
     156,  -181,   162,  1312,  -181,   915,   953,  -181,  -181,  -181,
    -181,   -48,    73,    70,    71,   991,    72,  -181,  1222,  1029,
     111,  -181,  -181,   840,  1312,  -181,    76,   652,  -181,  -181,
     512,  -181,  -181,    77,  1312,  -181,  -181,  -181,  -181,    88,
     -17,  -181,  -181,  -181,   178,   148,   186,   652,  -181,  1312,
    -181,   840,  1312,  1312,  1312,  1312,  1312,   551,  1335,  1357,
     392,  1376,  1376,   -18,   -18,   -18,   -18,   467,   467,    28,
      28,   -34,   -34,   -34,   -34,   101,  -181,   427,  -181,  -181,
    -181,   -25,    96,  -181,   101,  -181,   104,  -181,  -181,    87,
    -181,  -181,  -181,  -181,  -181,  -181,  -181,  -181,   840,  -181,
    -181,  -181,  -181,  -181,  -181,  -181,   196,   106,   -14,   652,
    -181,   -13,   107,  -181,  -181,  -181,   108,  -181,   198,  -181,
    -181,  -181,  -181,  1252,  -181,  -181,  -181,   109,  -181,   200,
    -181,  -181,  -181,   203,  -181,  -181,   138,   840,  1282,   206,
     130,   118,  -181
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,    68,    69,    70,    71,     0,   161,    72,     0,
     147,   148,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    31,     0,     0,
       2,     0,     0,    75,     0,     8,     0,     0,     5,     0,
       6,     0,     0,     7,    76,    77,    78,   113,     0,    74,
       0,     0,     9,    82,   104,     0,     0,    81,     0,    79,
      80,    13,     0,     0,     0,   114,    14,   108,    83,     0,
       0,     0,     0,     0,    88,     0,     0,    63,    60,    61,
       0,     0,     0,     0,     0,     0,    52,     0,    15,   117,
     115,   116,     0,     1,     3,    32,     0,    27,    30,    29,
      25,     0,    37,     0,     0,    38,    39,    21,    18,    19,
      20,     0,    45,     0,     0,    49,    51,    54,     0,    55,
      85,     0,     0,   103,   103,     0,     0,   107,     0,   137,
     138,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    10,    11,     0,     0,   103,
       0,   143,     0,   112,   163,     0,     0,   160,    89,    67,
      65,     0,     0,     0,     0,     0,     0,    43,     0,     0,
       0,    73,    33,     0,    28,    34,     0,    17,    23,    24,
       0,    22,    48,     0,    46,    44,    53,    84,    87,     0,
       0,    97,   101,    94,     0,   103,     0,     0,    96,   111,
     105,     0,   110,   136,   146,   145,   144,   134,   133,   125,
     124,   126,   135,   127,   128,   129,   131,   130,   132,   118,
     119,   120,   121,   122,   123,   141,   140,     0,   139,   159,
     157,     0,     0,    95,     0,   142,     0,   162,    56,     0,
      57,    62,    59,    58,   149,   150,    42,    36,     0,    26,
      35,    16,    40,    41,    47,    91,     0,     0,     0,     0,
     100,     0,     0,   106,   153,   152,     0,   156,     0,   154,
     155,    66,    64,     0,    86,    90,    98,     0,   102,     0,
      93,   151,   158,     0,    92,    99,     0,     0,     0,     0,
       0,     0,    50
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -181,  -181,    -2,  -181,  -180,   -33,   -30,  -181,  -181,  -181,
     131,   -27,  -181,  -181,     2,  -181,  -181,  -181,  -181,     3,
    -181,     4,  -181,  -181,     6,  -181,  -181,  -181,  -181,  -181,
    -181,  -181,    12,  -181,  -181,   -21,  -119,  -181,  -181,  -181,
    -181,  -181,    22,  -181,  -181,     9,  -181,  -181,  -181,     0,
    -181,  -181,    57,  -181,  -181,  -181,  -181,  -181,  -181,  -181,
    -181
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
       0,    29,    30,    31,   186,   187,    96,   103,   104,    32,
      33,   105,    34,   106,   107,    36,   113,   193,    37,   108,
      39,   109,    41,    42,   110,    44,    45,    46,   171,    47,
      48,    49,   200,    50,    51,   123,   203,    52,   204,   205,
     271,   206,   207,    53,    54,   126,    55,    56,    57,   111,
      59,    60,   236,   237,    61,    62,   158,   241,    63,    64,
      65
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      58,   102,    35,    38,    40,   208,    43,   261,   198,   201,
     115,   239,   116,    74,  -109,   119,    78,    79,   121,    75,
     128,   169,   170,    89,    90,    91,   202,   272,    92,    94,
     124,    58,    97,    35,    38,    40,   114,    43,  -109,  -109,
     243,   118,   159,   249,   129,   130,    66,    67,    97,   250,
      69,    68,   152,   153,   154,   127,   145,   146,   147,   148,
     149,   150,   151,    70,   277,    76,   278,    72,   152,   153,
     154,   163,   165,   166,   266,   286,   288,   266,   289,    71,
     267,    73,   175,    88,   178,   179,   173,   174,    77,   287,
     281,   282,    82,    83,    93,    84,   125,   240,   157,   184,
     162,   161,   199,    85,   160,    80,   149,   150,   151,    81,
      86,    87,   122,   194,   152,   153,   154,   101,   167,   168,
     182,   172,    27,   180,   188,   209,   190,   189,   212,   197,
     235,   213,   214,   215,   216,   217,   218,   219,   220,   221,
     222,   223,   224,   225,   226,   227,   228,   229,   230,   231,
     232,   233,   234,   259,   238,   210,   242,   262,    58,   244,
      35,    38,    40,   263,    43,   246,   251,   252,   253,   255,
     258,   260,    -4,     1,   264,     2,     3,     4,     5,     6,
       7,   198,   265,    97,   202,     8,     9,    10,    11,   270,
      12,   279,    13,    14,    15,    16,    17,   274,   280,   284,
     285,   292,   290,   295,   294,   291,   296,   276,   297,   300,
      18,   127,    19,   301,   176,   302,   268,   245,     0,     0,
     273,     2,     3,     4,     5,     0,     7,   269,    20,    21,
      22,     8,     9,    10,    11,     0,    12,    97,     0,    14,
      15,    16,    17,     0,     0,     0,     0,     0,     0,    23,
       0,     0,     0,     0,    24,     0,     0,    25,   283,     0,
       0,    26,     0,     0,    27,     0,     0,    -4,    28,     1,
       0,     2,     3,     4,     5,     6,     7,     0,     0,     0,
       0,     8,     9,    10,    11,     0,    12,     0,    13,    14,
      15,    16,    17,     0,     0,    23,     0,   298,     0,     0,
      24,     0,     0,    25,     0,     0,    18,     0,    19,     0,
      27,     0,     0,     0,    28,   117,     0,     2,     3,     4,
       5,     0,     7,     0,    20,    21,    22,     8,     9,    10,
      11,     0,    12,     0,     0,    14,    15,    16,    17,     0,
       0,     0,     0,     0,     0,    23,     0,     0,     0,     0,
      24,     0,     0,    25,     0,     0,     0,    26,     0,     0,
      27,     0,     0,     0,    28,     2,     3,     4,     5,     0,
       7,     0,     0,     0,     0,     8,     9,    10,    11,     0,
      12,     0,     0,    14,    15,    16,    17,     0,     0,     0,
       0,    23,     0,     0,     0,     0,    24,     0,     0,    25,
       0,     0,     2,     3,     4,     5,    27,     7,     0,     0,
      28,   120,     8,     9,    10,    11,     0,    12,     0,     0,
      14,    15,    16,    17,     0,     0,     0,     0,     0,     0,
       2,     3,     4,     5,     0,     7,     0,     0,     0,    23,
       8,     9,    10,    11,    24,    12,     0,    25,    14,    15,
      16,    17,     0,     0,    27,     0,     0,     0,    28,   164,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,    23,     0,   152,   153,
     154,    24,     0,     0,    25,     0,     0,     0,     0,     0,
       0,    27,     0,     0,     0,    28,   192,     0,     0,     0,
       0,     0,     0,     0,    23,     0,     0,     0,     0,    24,
       0,     0,    25,     0,     0,     2,     3,     4,     5,    27,
       7,     0,     0,    28,   275,     8,     9,    10,    11,     0,
      12,     0,     0,    14,    15,    16,    17,     0,    98,     0,
       0,     0,     0,   147,   148,   149,   150,   151,     0,     0,
      18,     0,    19,   152,   153,   154,    99,     0,     0,     0,
       0,     2,     3,     4,     5,     0,     7,     0,    20,    21,
       0,     8,     9,    10,    11,     0,    12,     0,     0,    14,
      15,    16,    17,     0,    98,     0,     0,     0,     0,    23,
       0,     0,     0,     0,    24,     0,    18,    25,    19,     0,
       0,   100,    99,     0,    27,     0,   101,     0,    28,     0,
       0,     0,     0,     0,    20,    21,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,     0,     0,     0,    23,     0,   152,   153,   154,
      24,     0,     0,    25,     0,     0,     0,   100,     0,     0,
      27,     0,     0,   185,    28,     2,     3,     4,     5,     0,
       7,     0,     0,     0,     0,     8,     9,    10,    11,     0,
      12,     0,     0,    14,    15,    16,    17,     0,    98,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      18,     0,    19,     0,     0,     0,    99,     0,     0,     0,
       2,     3,     4,     5,     0,     7,     0,     0,    20,    21,
       8,     9,    10,    11,     0,    12,     0,     0,    14,    15,
      16,    17,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,     0,     0,    24,     0,     0,    25,     0,     0,
       0,   100,     0,     0,    27,     0,     0,     0,    28,     2,
       3,     4,     5,     0,     7,     0,     0,     0,     0,     8,
       9,    10,    11,     0,    12,     0,     0,    14,    15,    16,
      17,     0,     0,     0,    23,     0,     0,     0,     0,    24,
       0,     0,    25,     0,     0,     0,     0,     0,     0,    27,
      95,     0,     0,    28,     2,     3,     4,     5,     0,     7,
       0,     0,     0,     0,     8,     9,    10,    11,     0,    12,
       0,     0,    14,    15,    16,    17,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,     0,    24,     0,
       0,    25,     0,     0,     0,   112,     0,     0,    27,     0,
       0,     0,    28,     2,     3,     4,     5,     0,     7,     0,
       0,     0,     0,     8,     9,    10,    11,     0,    12,     0,
       0,    14,    15,    16,    17,     0,     0,     0,    23,     0,
       0,     0,     0,    24,     0,     0,    25,     0,     0,     0,
     177,     0,     0,    27,     0,     0,     0,    28,     0,     0,
       0,     0,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,    23,   152,   153,
     154,     0,    24,     0,     0,    25,     0,     0,     0,   181,
       0,     0,    27,     0,     0,     0,    28,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,     0,     0,
       0,     0,     0,   152,   153,   154,     0,     0,     0,     0,
       0,     0,     0,     0,   196,   131,   132,   133,   134,   135,
     136,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,     0,     0,     0,     0,
       0,   152,   153,   154,     0,     0,     0,     0,     0,     0,
       0,     0,   247,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
     148,   149,   150,   151,     0,     0,     0,     0,     0,   152,
     153,   154,     0,     0,     0,     0,     0,     0,     0,     0,
     248,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,     0,     0,     0,     0,     0,   152,   153,   154,
       0,     0,     0,     0,     0,     0,     0,     0,   254,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
       0,     0,     0,     0,     0,   152,   153,   154,     0,     0,
       0,     0,     0,     0,     0,     0,   257,   131,   132,   133,
     134,   135,   136,   137,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,     0,     0,
       0,     0,     0,   152,   153,   154,     0,     0,   183,   131,
     132,   133,   134,   135,   136,   137,   138,   139,   140,   141,
     142,   143,   144,   145,   146,   147,   148,   149,   150,   151,
       0,     0,     0,     0,     0,   152,   153,   154,     0,     0,
     211,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,     0,     0,     0,     0,     0,   152,   153,   154,
     155,   156,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,     0,   152,   153,
     154,   191,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,     0,   152,   153,
     154,   195,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,     0,   152,   153,
     154,   256,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,     0,   152,   153,
     154,   293,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,     0,   152,   153,
     154,   299,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
     149,   150,   151,     0,     0,     0,     0,     0,   152,   153,
     154,   137,   138,   139,   140,   141,   142,   143,   144,   145,
     146,   147,   148,   149,   150,   151,     0,     0,     0,     0,
       0,   152,   153,   154,   138,   139,   140,   141,   142,   143,
     144,   145,   146,   147,   148,   149,   150,   151,     0,     0,
       0,     0,     0,   152,   153,   154,   141,   142,   143,   144,
     145,   146,   147,   148,   149,   150,   151,     0,     0,     0,
       0,     0,   152,   153,   154
};

static const yytype_int16 yycheck[] =
{
       0,    34,     0,     0,     0,   124,     0,   187,     3,    19,
      37,     3,    39,     3,    59,    42,     3,     4,    48,     9,
      59,     3,     4,    23,    24,    25,    36,   207,    28,    31,
      51,    31,    32,    31,    31,    31,    36,    31,    83,    84,
     159,    41,    63,    91,    83,    84,    89,    92,    48,    97,
      87,    96,    86,    87,    88,    55,    74,    75,    76,    77,
      78,    79,    80,     3,    89,    55,    91,    96,    86,    87,
      88,    71,    72,    73,    91,    89,    89,    91,    91,    59,
      97,    96,    82,    89,    84,    85,     3,     4,    96,   269,
       3,     4,    96,    96,     0,    96,    59,    89,    19,    99,
      11,     6,    97,    96,    87,    92,    78,    79,    80,    96,
      96,    96,    96,   113,    86,    87,    88,    94,     3,     3,
      93,     4,    92,     3,    89,   125,    34,    89,   128,    97,
       3,   131,   132,   133,   134,   135,   136,   137,   138,   139,
     140,   141,   142,   143,   144,   145,   146,   147,   148,   149,
     150,   151,   152,   183,   154,    93,   158,   190,   158,     3,
     158,   158,   158,   190,   158,     3,    93,    97,    97,    97,
      59,    95,     0,     1,    97,     3,     4,     5,     6,     7,
       8,     3,    94,   183,    36,    13,    14,    15,    16,     3,
      18,    95,    20,    21,    22,    23,    24,    96,    94,     3,
      94,     3,    95,     3,    95,    97,     3,   237,    70,     3,
      38,   211,    40,    83,    83,    97,   204,   160,    -1,    -1,
     211,     3,     4,     5,     6,    -1,     8,   205,    56,    57,
      58,    13,    14,    15,    16,    -1,    18,   237,    -1,    21,
      22,    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,   258,    -1,
      -1,    89,    -1,    -1,    92,    -1,    -1,    95,    96,     1,
      -1,     3,     4,     5,     6,     7,     8,    -1,    -1,    -1,
      -1,    13,    14,    15,    16,    -1,    18,    -1,    20,    21,
      22,    23,    24,    -1,    -1,    77,    -1,   297,    -1,    -1,
      82,    -1,    -1,    85,    -1,    -1,    38,    -1,    40,    -1,
      92,    -1,    -1,    -1,    96,    97,    -1,     3,     4,     5,
       6,    -1,     8,    -1,    56,    57,    58,    13,    14,    15,
      16,    -1,    18,    -1,    -1,    21,    22,    23,    24,    -1,
      -1,    -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,
      82,    -1,    -1,    85,    -1,    -1,    -1,    89,    -1,    -1,
      92,    -1,    -1,    -1,    96,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      18,    -1,    -1,    21,    22,    23,    24,    -1,    -1,    -1,
      -1,    77,    -1,    -1,    -1,    -1,    82,    -1,    -1,    85,
      -1,    -1,     3,     4,     5,     6,    92,     8,    -1,    -1,
      96,    97,    13,    14,    15,    16,    -1,    18,    -1,    -1,
      21,    22,    23,    24,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    -1,    77,
      13,    14,    15,    16,    82,    18,    -1,    85,    21,    22,
      23,    24,    -1,    -1,    92,    -1,    -1,    -1,    96,    97,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    77,    -1,    86,    87,
      88,    82,    -1,    -1,    85,    -1,    -1,    -1,    -1,    -1,
      -1,    92,    -1,    -1,    -1,    96,    97,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    85,    -1,    -1,     3,     4,     5,     6,    92,
       8,    -1,    -1,    96,    97,    13,    14,    15,    16,    -1,
      18,    -1,    -1,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,    -1,    76,    77,    78,    79,    80,    -1,    -1,
      38,    -1,    40,    86,    87,    88,    44,    -1,    -1,    -1,
      -1,     3,     4,     5,     6,    -1,     8,    -1,    56,    57,
      -1,    13,    14,    15,    16,    -1,    18,    -1,    -1,    21,
      22,    23,    24,    -1,    26,    -1,    -1,    -1,    -1,    77,
      -1,    -1,    -1,    -1,    82,    -1,    38,    85,    40,    -1,
      -1,    89,    44,    -1,    92,    -1,    94,    -1,    96,    -1,
      -1,    -1,    -1,    -1,    56,    57,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    -1,    77,    -1,    86,    87,    88,
      82,    -1,    -1,    85,    -1,    -1,    -1,    89,    -1,    -1,
      92,    -1,    -1,    95,    96,     3,     4,     5,     6,    -1,
       8,    -1,    -1,    -1,    -1,    13,    14,    15,    16,    -1,
      18,    -1,    -1,    21,    22,    23,    24,    -1,    26,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      38,    -1,    40,    -1,    -1,    -1,    44,    -1,    -1,    -1,
       3,     4,     5,     6,    -1,     8,    -1,    -1,    56,    57,
      13,    14,    15,    16,    -1,    18,    -1,    -1,    21,    22,
      23,    24,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    77,
      -1,    -1,    -1,    -1,    82,    -1,    -1,    85,    -1,    -1,
      -1,    89,    -1,    -1,    92,    -1,    -1,    -1,    96,     3,
       4,     5,     6,    -1,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    -1,    18,    -1,    -1,    21,    22,    23,
      24,    -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    82,
      -1,    -1,    85,    -1,    -1,    -1,    -1,    -1,    -1,    92,
      93,    -1,    -1,    96,     3,     4,     5,     6,    -1,     8,
      -1,    -1,    -1,    -1,    13,    14,    15,    16,    -1,    18,
      -1,    -1,    21,    22,    23,    24,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    77,    -1,    -1,    -1,    -1,    82,    -1,
      -1,    85,    -1,    -1,    -1,    89,    -1,    -1,    92,    -1,
      -1,    -1,    96,     3,     4,     5,     6,    -1,     8,    -1,
      -1,    -1,    -1,    13,    14,    15,    16,    -1,    18,    -1,
      -1,    21,    22,    23,    24,    -1,    -1,    -1,    77,    -1,
      -1,    -1,    -1,    82,    -1,    -1,    85,    -1,    -1,    -1,
      89,    -1,    -1,    92,    -1,    -1,    -1,    96,    -1,    -1,
      -1,    -1,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    77,    86,    87,
      88,    -1,    82,    -1,    -1,    85,    -1,    -1,    -1,    97,
      -1,    -1,    92,    -1,    -1,    -1,    96,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    60,    61,    62,    63,    64,    65,    66,
      67,    68,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,
      87,    88,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      97,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    97,    60,    61,    62,
      63,    64,    65,    66,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    -1,    -1,    91,    60,
      61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      -1,    -1,    -1,    -1,    -1,    86,    87,    88,    -1,    -1,
      91,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,    88,
      89,    90,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    89,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    -1,    -1,    -1,    -1,    -1,    86,    87,
      88,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    -1,    -1,    -1,    -1,
      -1,    86,    87,    88,    67,    68,    69,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    -1,    -1,
      -1,    -1,    -1,    86,    87,    88,    70,    71,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    -1,    -1,    -1,
      -1,    -1,    86,    87,    88
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     1,     3,     4,     5,     6,     7,     8,    13,    14,
      15,    16,    18,    20,    21,    22,    23,    24,    38,    40,
      56,    57,    58,    77,    82,    85,    89,    92,    96,    99,
     100,   101,   107,   108,   110,   112,   113,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   127,   128,   129,
     131,   132,   135,   141,   142,   144,   145,   146,   147,   148,
     149,   152,   153,   156,   157,   158,    89,    92,    96,    87,
       3,    59,    96,    96,     3,     9,    55,    96,     3,     4,
      92,    96,    96,    96,    96,    96,    96,    96,    89,   147,
     147,   147,   147,     0,   100,    93,   104,   147,    26,    44,
      89,    94,   103,   105,   106,   109,   111,   112,   117,   119,
     122,   147,    89,   114,   147,   109,   109,    97,   147,   109,
      97,   104,    96,   133,   133,    59,   143,   147,    59,    83,
      84,    60,    61,    62,    63,    64,    65,    66,    67,    68,
      69,    70,    71,    72,    73,    74,    75,    76,    77,    78,
      79,    80,    86,    87,    88,    89,    90,    19,   154,   133,
      87,     6,    11,   147,    97,   147,   147,     3,     3,     3,
       4,   126,     4,     3,     4,   147,   108,    89,   147,   147,
       3,    97,    93,    91,   147,    95,   102,   103,    89,    89,
      34,    89,    97,   115,   147,    89,    97,    97,     3,    97,
     130,    19,    36,   134,   136,   137,   139,   140,   134,   147,
      93,    91,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,     3,   150,   151,   147,     3,
      89,   155,   100,   134,     3,   150,     3,    97,    97,    91,
      97,    93,    97,    97,    97,    97,    89,    97,    59,   104,
      95,   102,   103,   109,    97,    94,    91,    97,   130,   140,
       3,   138,   102,   143,    96,    97,   104,    89,    91,    95,
      94,     3,     4,   147,     3,    94,    89,   102,    89,    91,
      95,    97,     3,    89,    95,     3,     3,    70,   147,    89,
       3,    83,    97
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_uint8 yyr1[] =
{
       0,    98,    99,   100,   100,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,   102,   102,   103,   103,
     103,   103,   103,   103,   103,   103,   104,   104,   105,   105,
     106,   107,   108,   108,   109,   109,   110,   111,   111,   112,
     112,   112,   113,   113,   114,   114,   115,   116,   116,   117,
     118,   119,   120,   121,   121,   122,   123,   124,   125,   125,
     125,   125,   125,   126,   126,   126,   126,   126,   127,   127,
     127,   127,   127,   127,   127,   127,   127,   127,   127,   127,
     127,   127,   127,   128,   129,   129,   130,   130,   131,   132,
     133,   133,   134,   134,   135,   135,   135,   136,   137,   138,
     138,   139,   140,   140,   141,   142,   143,   143,   144,   145,
     146,   146,   146,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   148,
     149,   150,   150,   151,   152,   153,   154,   154,   155,   155,
     156,   157,   158,   158
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     1,     1,     1,
       2,     2,     1,     1,     2,     2,     2,     1,     1,     1,
       1,     1,     2,     2,     2,     1,     3,     1,     2,     1,
       1,     1,     2,     3,     2,     3,     4,     1,     1,     2,
       4,     4,     4,     3,     2,     1,     1,     4,     3,     2,
      13,     2,     2,     3,     2,     2,     4,     4,     4,     4,
       2,     2,     4,     0,     3,     1,     3,     1,     1,     1,
       1,     1,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     2,     3,     2,     3,     1,     2,     3,
       4,     3,     4,     3,     3,     3,     3,     1,     3,     3,
       1,     1,     3,     0,     1,     3,     3,     1,     2,     1,
       3,     3,     3,     1,     1,     2,     2,     2,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     1,     1,     4,
       4,     3,     2,     2,     4,     5,     3,     2,     3,     1,
       3,     1,     4,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 9: /* sentence: functionDefinition  */
#line 57 "simple.y"
                         {sendKan(0); }
#line 1688 "simple.tab.c"
    break;

  case 10: /* sentence: exp ';'  */
#line 58 "simple.y"
                         {sendKan(0); }
#line 1694 "simple.tab.c"
    break;

  case 11: /* sentence: exp ':'  */
#line 59 "simple.y"
                         {pkkan(" [ ] {showln} sendmsg2 \n"); sendKan(0); }
#line 1700 "simple.tab.c"
    break;

  case 12: /* sentence: ';'  */
#line 60 "simple.y"
                         {sendKan(0); }
#line 1706 "simple.tab.c"
    break;

  case 13: /* sentence: class_definition  */
#line 61 "simple.y"
                         {sendKan(0); }
#line 1712 "simple.tab.c"
    break;

  case 15: /* sentence: PROMPT ';'  */
#line 63 "simple.y"
                         {sendKan(10); }
#line 1718 "simple.tab.c"
    break;

  case 28: /* return: RETURN exp  */
#line 91 "simple.y"
                 { pkkan(" /FunctionValue set  {/ExitPoint goto} exec %%return\n");}
#line 1724 "simple.tab.c"
    break;

  case 29: /* return: RETURN  */
#line 92 "simple.y"
            {pkkan("  {/ExitPoint goto} exec %%return void\n");}
#line 1730 "simple.tab.c"
    break;

  case 30: /* break: BREAK  */
#line 95 "simple.y"
               { pkkan(" exit "); }
#line 1736 "simple.tab.c"
    break;

  case 31: /* list_prefix: '['  */
#line 100 "simple.y"
         { pkkan("[ "); }
#line 1742 "simple.tab.c"
    break;

  case 32: /* list: list_prefix ']'  */
#line 103 "simple.y"
                               { pkkan("  ] "); }
#line 1748 "simple.tab.c"
    break;

  case 33: /* list: list_prefix argList ']'  */
#line 104 "simple.y"
                               { pkkan("  ] "); }
#line 1754 "simple.tab.c"
    break;

  case 36: /* if_prefix: IF '(' exp ')'  */
#line 113 "simple.y"
                   { pkkan(" %% if-condition\n  { %%ifbody\n"); }
#line 1760 "simple.tab.c"
    break;

  case 37: /* if_body: localsentence  */
#line 116 "simple.y"
                     { pkkan("  }%%end if if body\n  { %%if- else part\n"); }
#line 1766 "simple.tab.c"
    break;

  case 38: /* if_body: curryBrace  */
#line 117 "simple.y"
                   { pkkan("  }%%end if if body\n  { %%if- else part\n"); }
#line 1772 "simple.tab.c"
    break;

  case 39: /* if: if_prefix if_body  */
#line 120 "simple.y"
                                        { pkkan("  } ifelse\n"); }
#line 1778 "simple.tab.c"
    break;

  case 40: /* if: if_prefix if_body ELSE localsentence  */
#line 122 "simple.y"
                                             { pkkan("  } ifelse\n"); }
#line 1784 "simple.tab.c"
    break;

  case 41: /* if: if_prefix if_body ELSE curryBrace  */
#line 123 "simple.y"
                                        { pkkan("  } ifelse\n"); }
#line 1790 "simple.tab.c"
    break;

  case 42: /* for_prefix0: FOR '(' exp ';'  */
#line 128 "simple.y"
                     { pkkan("%%for init.\n%%for\n{ "); }
#line 1796 "simple.tab.c"
    break;

  case 43: /* for_prefix0: FOR '(' ';'  */
#line 129 "simple.y"
                     { pkkan("%%nothing for init.\n%%for\n{ "); }
#line 1802 "simple.tab.c"
    break;

  case 44: /* for_exit: exp ';'  */
#line 132 "simple.y"
               { pkkan(" {  } {exit} ifelse\n[ {%%increment\n"); }
#line 1808 "simple.tab.c"
    break;

  case 45: /* for_exit: ';'  */
#line 133 "simple.y"
               { pkkan("%%no exit rule.\n[ {%%increment\n"); }
#line 1814 "simple.tab.c"
    break;

  case 46: /* for_inc: exp  */
#line 136 "simple.y"
               { pkkan("} %%end of increment{A}\n"); }
#line 1820 "simple.tab.c"
    break;

  case 47: /* for_prefix: for_prefix0 for_exit for_inc ')'  */
#line 141 "simple.y"
               {  pkkan("{%%start of B part{B}\n"); }
#line 1826 "simple.tab.c"
    break;

  case 48: /* for_prefix: for_prefix0 for_exit ')'  */
#line 143 "simple.y"
               {  pkkan("  } %% dummy A\n{%%start of B part{B}\n"); }
#line 1832 "simple.tab.c"
    break;

  case 49: /* for: for_prefix curryBrace  */
#line 147 "simple.y"
               { pkkan("} %% end of B part. {B}\n"); 
                 pkkan(" 2 1 roll] {exec} map pop\n} loop %%end of for\n"); }
#line 1839 "simple.tab.c"
    break;

  case 50: /* PSfor_prefix: PSFOR '(' ID '=' exp ';' ID '<' exp ';' ID INCREMENT ')'  */
#line 152 "simple.y"
     { 
       pkkan(" 2 -1 roll \n");
       pkkan("%%PSfor initvalue.\n (integer) data_conversion \n");
       pkkan(" 2 -1 roll \n");
       ips(yyvsp[-10]); 
       ips(yyvsp[-6]); ips(yyvsp[-2]); 
       /* They must be equal id, but it is not checked. */
       pkkan(" (1).. sub  (integer) data_conversion  1  2 -1 roll \n");
       pkkan("{ %% for body\n (universalNumber) data_conversion ");
       pkkan("/"); printObjectSymbol(yyvsp[-10]); pkkan("  set \n");
     }
#line 1855 "simple.tab.c"
    break;

  case 51: /* PSfor: PSfor_prefix curryBrace  */
#line 166 "simple.y"
     { pkkan("  } for \n"); }
#line 1861 "simple.tab.c"
    break;

  case 52: /* while_prefix0: WHILE '('  */
#line 170 "simple.y"
               { pkkan("\n%%while\n{ "); }
#line 1867 "simple.tab.c"
    break;

  case 53: /* while_prefix: while_prefix0 exp ')'  */
#line 174 "simple.y"
                            { pkkan(" { } {exit} ifelse\n "); }
#line 1873 "simple.tab.c"
    break;

  case 54: /* while_prefix: while_prefix0 ')'  */
#line 175 "simple.y"
                            { pkkan("%%no exit condition.\n "); }
#line 1879 "simple.tab.c"
    break;

  case 55: /* while: while_prefix curryBrace  */
#line 179 "simple.y"
                             {   pkkan("} loop\n"); }
#line 1885 "simple.tab.c"
    break;

  case 56: /* print: PRINT '(' exp ')'  */
#line 184 "simple.y"
                      { pkkan(" print\n");}
#line 1891 "simple.tab.c"
    break;

  case 58: /* load: LOAD '(' QUOTE ')'  */
#line 192 "simple.y"
                         {   loadFile(yyvsp[-1]);  }
#line 1897 "simple.tab.c"
    break;

  case 59: /* load: LOAD '(' ID ')'  */
#line 193 "simple.y"
                         {   loadFile(yyvsp[-1]);  }
#line 1903 "simple.tab.c"
    break;

  case 60: /* load: LOAD ID  */
#line 194 "simple.y"
                         {   loadFile(yyvsp[0]);  }
#line 1909 "simple.tab.c"
    break;

  case 61: /* load: LOAD QUOTE  */
#line 195 "simple.y"
                         {   loadFile(yyvsp[0]);  }
#line 1915 "simple.tab.c"
    break;

  case 62: /* load: LOAD '[' QUOTE ']'  */
#line 196 "simple.y"
                         {   loadFileWithCpp(yyvsp[-1]); }
#line 1921 "simple.tab.c"
    break;

  case 64: /* sm1ArgList: sm1ArgList ',' QUOTE  */
#line 199 "simple.y"
                          { pkkan(" "); printObjectSymbol(yyvsp[0]); pkkan(" "); }
#line 1927 "simple.tab.c"
    break;

  case 65: /* sm1ArgList: QUOTE  */
#line 200 "simple.y"
                          { pkkan(" "); printObjectSymbol(yyvsp[0]); pkkan(" "); }
#line 1933 "simple.tab.c"
    break;

  case 66: /* sm1ArgList: sm1ArgList ',' ID  */
#line 201 "simple.y"
                          { pkkan(" "); printObjectSymbol(yyvsp[0]); pkkan(" "); }
#line 1939 "simple.tab.c"
    break;

  case 67: /* sm1ArgList: ID  */
#line 202 "simple.y"
                          { pkkan(" "); printObjectSymbol(yyvsp[0]); pkkan(" "); }
#line 1945 "simple.tab.c"
    break;

  case 68: /* primitive: ID  */
#line 207 "simple.y"
   { int tmp0;
     if ((tmp0 = K00getIncetanceVariable(objectSymbolToString(yyvsp[0]))) != -1) {
       pkkan(" this "); pkkanInteger(tmp0); pkkan(" get ");
     } else {
       printObjectSymbol(yyvsp[0]); pkkan(" ");
     }
   }
#line 1957 "simple.tab.c"
    break;

  case 69: /* primitive: QUOTE  */
#line 214 "simple.y"
              { pkkan("("); printObjectSymbol(yyvsp[0]); pkkan(") "); }
#line 1963 "simple.tab.c"
    break;

  case 71: /* primitive: NUMBER  */
#line 216 "simple.y"
              { pkkan("("); printObjectSymbol(yyvsp[0]); pkkan(").. ");}
#line 1969 "simple.tab.c"
    break;

  case 72: /* primitive: THIS  */
#line 217 "simple.y"
              { pkkan(" this "); }
#line 1975 "simple.tab.c"
    break;

  case 83: /* functionCall_prefix: ID '('  */
#line 232 "simple.y"
           { pkkan("this [ %% function args \n"); yyval = yyvsp[-1]; }
#line 1981 "simple.tab.c"
    break;

  case 84: /* functionCall: functionCall_prefix argList ')'  */
#line 242 "simple.y"
       {pkkan("] {");printObjectSymbol(yyvsp[-2]);pkkan("} sendmsg2 \n");}
#line 1987 "simple.tab.c"
    break;

  case 85: /* functionCall: functionCall_prefix ')'  */
#line 244 "simple.y"
       {pkkan("] {");printObjectSymbol(yyvsp[-1]);pkkan("} sendmsg2 \n");}
#line 1993 "simple.tab.c"
    break;

  case 86: /* idList: idList ',' ID  */
#line 248 "simple.y"
                    { pkkan("/");printObjectSymbol(yyvsp[0]); pkkan(" "); ips(yyvsp[0]);}
#line 1999 "simple.tab.c"
    break;

  case 87: /* idList: ID  */
#line 249 "simple.y"
                    { pkkan("/");printObjectSymbol(yyvsp[0]); pkkan(" "); ips(yyvsp[0]);}
#line 2005 "simple.tab.c"
    break;

  case 88: /* functionHead: DEF ID  */
#line 254 "simple.y"
        { pkkan("/"); printObjectSymbol(yyvsp[0]); pkkan(" {\n"); ips(yyvsp[0]);
          pkdebug("In function : ", objectSymbolToString(yyvsp[0]),
		  " of class ",K00getCurrentContextName());
	  pkkan(" /Arglist set /Argthis set /FunctionValue [ ] def\n [/this ");
	  yyval = yyvsp[0];}
#line 2015 "simple.tab.c"
    break;

  case 89: /* voidfunctionHead: DEF VOID ID  */
#line 263 "simple.y"
        { pkkan("/"); printObjectSymbol(yyvsp[0]); pkkan(" {\n"); ips(yyvsp[0]);
          pkdebug("In function : ", objectSymbolToString(yyvsp[0]),
		  " of class ",K00getCurrentContextName());
	  pkkan(" /Arglist set /Argthis set /FunctionValue [ ] def\n [/this ");
	  yyval = yyvsp[-1];}
#line 2025 "simple.tab.c"
    break;

  case 90: /* functionArg: '(' idList ')' '{'  */
#line 272 "simple.y"
     {pkkan(" ] /ArgNames set ArgNames pushVariables [ %%function body\n");
      pkkan(" [Argthis] Arglist join ArgNames mapset\n");}
#line 2032 "simple.tab.c"
    break;

  case 91: /* functionArg: '(' ')' '{'  */
#line 275 "simple.y"
     {pkkan(" ] /ArgNames set ArgNames pushVariables [ %%function body\n");
      pkkan(" [Argthis]  ArgNames mapset\n"); }
#line 2039 "simple.tab.c"
    break;

  case 92: /* functionBody: declaration extern_declaration statements '}'  */
#line 282 "simple.y"
          { pkkan("/ExitPoint ]pop popVariables %%pop the local variables\n"); }
#line 2045 "simple.tab.c"
    break;

  case 94: /* functionDefinition: functionHead functionArg functionBody  */
#line 289 "simple.y"
        {pkkan("/ExitPoint ]pop popVariables %%pop argValues\n");
	 pkdebug2();
         pkkan("FunctionValue } def\n%%end of function\n\n"); }
#line 2053 "simple.tab.c"
    break;

  case 95: /* functionDefinition: operatorfunctionHead functionArg functionBody  */
#line 293 "simple.y"
        {pkkan("/ExitPoint ]pop popVariables %%pop argValues\n");
	 pkdebug2();
         pkkan("FunctionValue } def\n%%end of function\n\n"); }
#line 2061 "simple.tab.c"
    break;

  case 96: /* functionDefinition: voidfunctionHead functionArg functionBody  */
#line 297 "simple.y"
        {pkkan("/ExitPoint ]pop popVariables %%pop argValues\n");
	 pkdebug2();
         pkkan("} def\n%%end of function\n\n"); }
#line 2069 "simple.tab.c"
    break;

  case 97: /* declaration_prefix: LOCAL  */
#line 303 "simple.y"
          { pkkan("[ %%start of local variables\n"); }
#line 2075 "simple.tab.c"
    break;

  case 98: /* declaration: declaration_prefix idList ';'  */
#line 308 "simple.y"
       { pkkan("] pushVariables [ %%local variables\n"); }
#line 2081 "simple.tab.c"
    break;

  case 99: /* extern_idList: extern_idList ',' ID  */
#line 313 "simple.y"
           {     ;}
#line 2087 "simple.tab.c"
    break;

  case 100: /* extern_idList: ID  */
#line 314 "simple.y"
           {     ;}
#line 2093 "simple.tab.c"
    break;

  case 101: /* extern_declaration_prefix: EXTERN  */
#line 318 "simple.y"
           {   ; }
#line 2099 "simple.tab.c"
    break;

  case 102: /* extern_declaration: extern_declaration_prefix extern_idList ';'  */
#line 323 "simple.y"
       {  ; }
#line 2105 "simple.tab.c"
    break;

  case 104: /* arrayValue: array  */
#line 328 "simple.y"
            { pkkan(" Get\n"); }
#line 2111 "simple.tab.c"
    break;

  case 105: /* array: array_prefix arrayIndex ']'  */
#line 331 "simple.y"
                               {pkkan(" ] "); }
#line 2117 "simple.tab.c"
    break;

  case 108: /* array_prefix: ID '['  */
#line 339 "simple.y"
     { int tmp0;
       if ((tmp0 = K00getIncetanceVariable(objectSymbolToString(yyvsp[-1]))) != -1) {
         pkkan(" this "); pkkanInteger(tmp0); pkkan(" get [");
       } else {
         printObjectSymbol(yyvsp[-1]); pkkan(" [");
       }
     }
#line 2129 "simple.tab.c"
    break;

  case 109: /* id_set: ID  */
#line 350 "simple.y"
                   { yyval = yyvsp[0];}
#line 2135 "simple.tab.c"
    break;

  case 110: /* set: id_set '=' exp  */
#line 354 "simple.y"
    { int tmp0;
      if ((tmp0 = K00getIncetanceVariable(objectSymbolToString(yyvsp[-2]))) != -1) {
	pkkan(" this "); pkkanInteger(tmp0);
	pkkan(" 3 -1 roll   put\n");
      }else {
	pkkan("/"); printObjectSymbol(yyvsp[-2]); pkkan(" "); ips(yyvsp[-2]); pkkan(" set\n");
      }
    }
#line 2148 "simple.tab.c"
    break;

  case 111: /* set: array '=' exp  */
#line 362 "simple.y"
                   { pkkan(" Put\n"); }
#line 2154 "simple.tab.c"
    break;

  case 112: /* set: THIS '=' exp  */
#line 363 "simple.y"
                   { pkkan(" /this set \n"); }
#line 2160 "simple.tab.c"
    break;

  case 115: /* exp: '!' exp  */
#line 369 "simple.y"
                 { pkkan(" not\n"); }
#line 2166 "simple.tab.c"
    break;

  case 117: /* exp: '-' exp  */
#line 371 "simple.y"
                 { pkkan(" (0)..  2 1 roll {sub} sendmsg \n"); }
#line 2172 "simple.tab.c"
    break;

  case 118: /* exp: exp '+' exp  */
#line 372 "simple.y"
                 { pkkan(" {add} sendmsg2 \n"); }
#line 2178 "simple.tab.c"
    break;

  case 119: /* exp: exp '-' exp  */
#line 373 "simple.y"
                 { pkkan(" {sub} sendmsg2 \n"); }
#line 2184 "simple.tab.c"
    break;

  case 120: /* exp: exp '*' exp  */
#line 374 "simple.y"
                 { pkkan(" {mul} sendmsg2 \n"); }
#line 2190 "simple.tab.c"
    break;

  case 121: /* exp: exp '/' exp  */
#line 375 "simple.y"
                 { pkkan(" {div} sendmsg2 \n"); }
#line 2196 "simple.tab.c"
    break;

  case 123: /* exp: exp '^' exp  */
#line 377 "simple.y"
                 { pkkan(" power\n"); }
#line 2202 "simple.tab.c"
    break;

  case 126: /* exp: exp EQUAL exp  */
#line 380 "simple.y"
                  { pkkan(" eq\n"); }
#line 2208 "simple.tab.c"
    break;

  case 127: /* exp: exp '<' exp  */
#line 381 "simple.y"
                { pkkan(" lt\n"); }
#line 2214 "simple.tab.c"
    break;

  case 128: /* exp: exp '>' exp  */
#line 382 "simple.y"
                { pkkan(" gt\n"); }
#line 2220 "simple.tab.c"
    break;

  case 129: /* exp: exp LESSEQUAL exp  */
#line 383 "simple.y"
                      { pkkan(" lessThanOrEqual\n"); }
#line 2226 "simple.tab.c"
    break;

  case 131: /* exp: exp GREATEREQUAL exp  */
#line 385 "simple.y"
                         { pkkan(" greaterThanOrEqual\n"); }
#line 2232 "simple.tab.c"
    break;

  case 133: /* exp: exp AND exp  */
#line 387 "simple.y"
                { pkkan(" and\n"); }
#line 2238 "simple.tab.c"
    break;

  case 134: /* exp: exp OR exp  */
#line 388 "simple.y"
                { pkkan(" or\n"); }
#line 2244 "simple.tab.c"
    break;

  case 135: /* exp: exp NOTEQUAL exp  */
#line 389 "simple.y"
                      { pkkan(" eq not\n"); }
#line 2250 "simple.tab.c"
    break;

  case 137: /* exp: id_set INCREMENT  */
#line 391 "simple.y"
                     { pkkan("/");
		       printObjectSymbol(yyvsp[-1]); ips(yyvsp[-1]);
		       pkkan(" "); printObjectSymbol(yyvsp[-1]);
		       pkkan(" (1).. {add} sendmsg2 "); pkkan("def\n"); }
#line 2259 "simple.tab.c"
    break;

  case 138: /* exp: id_set DECREMENT  */
#line 395 "simple.y"
                     { pkkan("/");
		       printObjectSymbol(yyvsp[-1]); ips(yyvsp[-1]);
		       pkkan(" "); printObjectSymbol(yyvsp[-1]);
		       pkkan(" (1).. {sub} sendmsg2 "); pkkan("def\n"); }
#line 2268 "simple.tab.c"
    break;

  case 140: /* exp: exp '.' member_functionCall  */
#line 400 "simple.y"
                                {  pkkan(" sendmsg2 \n"); }
#line 2274 "simple.tab.c"
    break;

  case 141: /* exp: exp '.' ID  */
#line 401 "simple.y"
                 { /* Not implemented yet. */ }
#line 2280 "simple.tab.c"
    break;

  case 142: /* exp: super '.' member_functionCall  */
#line 402 "simple.y"
                                  {  pkkan(" supmsg2 \n"); }
#line 2286 "simple.tab.c"
    break;

  case 143: /* exp: NUMBER '.' NUMBER  */
#line 403 "simple.y"
                      {  pkkan("("); printObjectSymbol(yyvsp[-2]);
			 pkkan(")..  (double) dc ");
			 pkkan("("); printObjectSymbol(yyvsp[0]); pkkan(")..  ");
                         pkkan("("); printTens(yyvsp[0]); pkkan(").. ");
			 pkkan(" div (double) dc  add\n"); }
#line 2296 "simple.tab.c"
    break;

  case 147: /* exp: SIZEOFTHISCLASS  */
#line 412 "simple.y"
    {
      int tmp0;
      tmp0 = K00getIncetanceVariable("K00sizeof");
      pkkan(" ("); pkkanInteger(tmp0); pkkan(").. ");
    }
#line 2306 "simple.tab.c"
    break;

  case 148: /* exp: STARTOFTHISCLASS  */
#line 419 "simple.y"
    {
      int tmp0;
      tmp0 = K00getIncetanceVariable("K00start");
      pkkan(" ("); pkkanInteger(tmp0); pkkan(").. ");
    }
#line 2316 "simple.tab.c"
    break;

  case 149: /* test: TEST '(' exp ')'  */
#line 427 "simple.y"
          { /* It is not used now. */
	    sendKan(1);
	    Sm1obj = KSpop();
	    if (Sm1obj.tag != Sdollar) {
	      fprintf(stderr," Argument of test must be a string.\n");
	    }
	    testNewFunction((struct Object *)&Sm1obj); 
	  }
#line 2329 "simple.tab.c"
    break;

  case 150: /* special: SPECIAL '(' list ')'  */
#line 438 "simple.y"
      {
          fprintf(stderr," special is used to extend the function. \n");
      }
#line 2337 "simple.tab.c"
    break;

  case 151: /* member_functionCall: member_functionCall_prefix argList ')'  */
#line 447 "simple.y"
       {pkkan("] {");printObjectSymbol(yyvsp[-2]);pkkan("}  ");}
#line 2343 "simple.tab.c"
    break;

  case 152: /* member_functionCall: member_functionCall_prefix ')'  */
#line 449 "simple.y"
       {pkkan("] {");printObjectSymbol(yyvsp[-1]);pkkan("}  ");}
#line 2349 "simple.tab.c"
    break;

  case 153: /* member_functionCall_prefix: ID '('  */
#line 454 "simple.y"
            { pkkan(" [ %% function args \n"); yyval = yyvsp[-1]; }
#line 2355 "simple.tab.c"
    break;

  case 154: /* class_definition: class_definition_prefix incetance_variables globalstatements '}'  */
#line 461 "simple.y"
      { pkkan(" PrimitiveContextp setcontext ");
	/* debug */ K00foo1();
        K00toPrimitiveClass();
      }
#line 2364 "simple.tab.c"
    break;

  case 155: /* class_definition_prefix: CLASS ID EXTENDS ID '{'  */
#line 470 "simple.y"
      { ips(yyvsp[-3]);
	pkkan("[ $") ; printObjectSymbol(yyvsp[-3]); pkkan("$ ");
	printObjectSymbol(yyvsp[-1]); pkkan(" 0 get  newcontext ] /");
	printObjectSymbol(yyvsp[-3]);	pkkan(" set \n");
	printObjectSymbol(yyvsp[-3]); pkkan(" 0 get setcontext \n");

	if (K00declareClass(objectSymbolToString(yyvsp[-3]),
			    objectSymbolToString(yyvsp[-1])) == -1) {
	  /* error */
	  KCerror("Super class has not been defined or Invalid class name.");
	}else{
	  K00putIncetanceVariable(IRESET," ");
	}
      }
#line 2383 "simple.tab.c"
    break;

  case 156: /* incetance_variables: LOCAL incetance_variables_list ';'  */
#line 487 "simple.y"
      {
	K00putIncetanceVariable(IEXIT," ");
      }
#line 2391 "simple.tab.c"
    break;

  case 157: /* incetance_variables: LOCAL ';'  */
#line 491 "simple.y"
      {
	K00putIncetanceVariable(IEXIT," ");
      }
#line 2399 "simple.tab.c"
    break;

  case 158: /* incetance_variables_list: incetance_variables_list ',' ID  */
#line 497 "simple.y"
    {
      K00putIncetanceVariable(IPUT,objectSymbolToString(yyvsp[0]));
    }
#line 2407 "simple.tab.c"
    break;

  case 159: /* incetance_variables_list: ID  */
#line 501 "simple.y"
    {
      K00putIncetanceVariable(IPUT,objectSymbolToString(yyvsp[0]));
    }
#line 2415 "simple.tab.c"
    break;

  case 160: /* operatorfunctionHead: DEF OPERATOR ID  */
#line 507 "simple.y"
        { pkkan("/"); printObjectSymbol(yyvsp[0]); pkkan(" {\n"); ips(yyvsp[0]);
          pkdebug("In function : ", objectSymbolToString(yyvsp[0]),", of class  ",
		  K00getCurrentContextName());
	  pkkan(" /Arglist set /Arglist [Arglist] def ");
          pkkan(" /Argthis set /FunctionValue [ ] def\n [/this ");
	  yyval = yyvsp[0];}
#line 2426 "simple.tab.c"
    break;

  case 161: /* super: SUPER  */
#line 516 "simple.y"
            { pkkan(" this "); }
#line 2432 "simple.tab.c"
    break;

  case 162: /* new: NEW '(' exp ')'  */
#line 521 "simple.y"
       { int tmp;
	 tmp = K00getIncetanceVariable("K00sizeof")
	   +K00getIncetanceVariable("K00start");
	 pkkanInteger(tmp);
	 pkkan(" ");
	 pkkan(K00getCurrentContextName());
	 pkkan(" cclass \n");
       }
#line 2445 "simple.tab.c"
    break;

  case 163: /* new: NEW '(' ')'  */
#line 530 "simple.y"
       { int tmp;
         pkkan(" PrimitiveObject ");
	 tmp = K00getIncetanceVariable("K00sizeof")
	   +K00getIncetanceVariable("K00start");
	 pkkanInteger(tmp);
	 pkkan(" ");
	 pkkan(K00getCurrentContextName());
	 pkkan(" cclass \n");
       }
#line 2459 "simple.tab.c"
    break;


#line 2463 "simple.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

