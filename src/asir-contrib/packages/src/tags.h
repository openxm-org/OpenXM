/* $OpenXM: OpenXM/src/asir-contrib/packages/src/tags.h,v 1.1 2001/08/25 04:39:03 takayama Exp $ */

#define NUMBER 1
#define RPOLYNOMIAL 2
#define RATIONAL 3
#define LIST 4
#define VECTOR 5
#define MATRIX 6
#define STRING 7
#define STRUCT 8
#define DPOLYNOMIAL   9
#define QUOTE 17


#define BASE_FIELD               struct_type("base_field")
#define BASE_IDENTITY            struct_type("base_identity")
#define BASE_RULE                struct_type("base_rule")
#define BASE_TEXT                struct_type("base_text")

#define POLY_FACTORED_POLYNOMIAL struct_type("poly_factored_polynomial")
#define POLY_FACTORED_RATIONAL   struct_type("poly_factored_rational")
#define POLY_RING                struct_type("poly_ring")
#define POLY_POLYNOMIAL          struct_type("poly_polynomial")
#define POLY_IDEAL               struct_type("poly_ideal")
