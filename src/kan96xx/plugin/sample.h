/* $OpenXM$ */
/* op-codes for sample */
#define KPLUGIN_SAMPLE_INIT 0
#define KPLUGIN_SAMPLE_SELECT 1
#define KPLUGIN_SAMPLE_WAIT 2
#define KPLUGIN_SAMPLE_ADD 3

int Kplugin_sample(int opcode,struct object obj);
static void errorSample(char *s);
int Kplugin_push(struct object ob,struct operandStack *operandstack);
struct object Kplugin_pop(struct operandStack *operandstack);
struct object Kplugin_peek(int k,struct operandStack *operandstack);



