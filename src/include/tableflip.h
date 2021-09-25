/* vim: ft=c
 */
#if !defined(__RlwrVF_tableflip_h)
#define __RlwrVF_tableflip_h

#include "matestr.h"

typedef struct _tableflip_colsnode* tfcols_nt;
struct _tableflip_colsnode {
	matestr str;

	tfcols_nt next;
};

typedef struct _tableflip_row* tfrow_nt;
struct _tableflip_row {
	tfcols_nt cols;

	tfrow_nt next;
};

enum tferror {
	tfe_none,
	tfe_unexpected_end,
	tfe_unexpected_expr
};

typedef struct {
	tfrow_nt node;

	u_int32_t line;
	u_int32_t start;
	u_int32_t end;
	enum tferror code;
} tftable_t;

/*
 * Parses table from content
 */
tftable_t tfparse(const char* content);

/*
 * Print table
 */
void tfprint(const tfrow_nt row);

/*
 * Outputs a error to stderr
 */
void tferror(const tftable_t table, const char* content);

#endif
