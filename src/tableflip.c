#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

#include "include/matestr.h"
#include "include/tableflip.h"

typedef unsigned char bool; /* lol */

#define readln __tf_readln
bool __tf_readln(const char *content, matestr *ln, size_t *offset) {
	size_t old_offset;

	ln->length = 0;
	matestr_normalize(ln);

	if (content[*offset] == '\0') {
		return(0);
	}
	if (content[*offset] == '\n') {
		(*offset)++;
	}
	old_offset = *offset;

	for (; content[*offset] != '\n' && content[*offset] != '\0'; (*offset)++) {}

	if (*offset - old_offset != 0)
		matestr_append(ln, content + old_offset, *offset - old_offset);

	return(1);
}

#define readword __tf_readword
bool __tf_readword(const char* ln, matestr* word, size_t *offset, tftable_t *table) {
	size_t old_offset;

	word->length = 0;
	matestr_normalize(word);

	if (*offset != 0 && ln[*offset] == '"') {
		(*offset)++;
	}

	for (; ln[*offset] != '"' && ln[*offset] != '\0'; (*offset)++) {
		if (ln[*offset] != ' ' && ln[*offset] != '\t') {
			table->start = *offset + 1;
			table->end = *offset + 1;
			table->code = tfe_unexpected_expr;
			return(0);
		}
	}
	if (ln[*offset] == '\0') {
		return(0);
	}
	(*offset)++;
	old_offset = *offset;

	for (; ln[*offset] != '"' && ln[*offset] != '\0'; (*offset)++) {
	}
	if (ln[*offset] == '\0') {
		table->start = old_offset;
		table->end = *offset;
		table->code = tfe_unexpected_end;
		return(0);
	}

	if (*offset == old_offset) {
		return(1);
	}

	matestr_append(word, ln + old_offset, *offset - old_offset);

	return(1);
}

#define newrow __tf_newrow
tfrow_nt __tf_newrow() {
	return calloc(1, sizeof(struct _tableflip_row));
}
#define newcol __tf_newcol
tfcols_nt __tf_newcol() {
	return calloc(1, sizeof(struct _tableflip_colsnode));
}

/*
 * Parses table from content
 */
tftable_t tfparse(const char* content) {
	tftable_t result;
	tfrow_nt first, curr, next_curr;
	tfcols_nt *cols_curr;
	matestr ln, word;
	size_t ln_offset, word_offset;

	result.code = tfe_none;
	result.line = 0;
	first = newrow();
	curr = first;
	ln = matestralloc(128);
	word = matestralloc(64);
	ln_offset = 0;

	while (readln(content, &ln, &ln_offset)) {
		result.line++;
		word_offset = 0;
		next_curr = newrow();
		cols_curr = &next_curr->cols;

		while (readword(ln.cstr, &word, &word_offset, &result)) {
			*cols_curr = newcol();
			(*cols_curr)->str = matestr_new(word.cstr, 0);
			cols_curr = &(*cols_curr)->next;
		}
		if (result.code != tfe_none) break;
		if (next_curr->cols == 0) {
			free(next_curr);
		} else {
			curr->next = next_curr;
			curr = next_curr;
		}
	}

	result.node = first->next;
	free(first);
	free(ln.cstr);
	free(word.cstr);

	return(result);
}

/* --- */

#define colat __tf_colat
tfcols_nt __tf_colat(const tfrow_nt row, u_int32_t nmemb) {
	tfcols_nt col;
	u_int32_t i;

	col = row->cols;
	for (i = 0; i < nmemb; i++) {
		if (col) col = col->next;
		else break;
	}

	return(col);
}

#define totalcols __tf_totalcols
u_int32_t __tf_totalcols(const tfrow_nt row) {
	tfcols_nt col;
	u_int32_t i;
	
	i = 0;
	for (col = row->cols; col != 0; col = col->next) i++;

	return(i);
}

#define maxlen __tf_maxlen
u_int32_t __tf_maxlen(const tfrow_nt row, u_int32_t nmemb) {
	u_int32_t answer;
	tfcols_nt col;
	tfrow_nt _row;

	answer = 0;
	for (_row = row; _row != 0; _row = _row->next) {
		col = colat(_row, nmemb);
		if (!col) break;
		if (col->str.length > answer)
			answer = col->str.length;
	}

	return(answer);
}

/*
 * Print table
 */
void tfprint(const tfrow_nt row) {
	tfrow_nt _row;
	tfcols_nt col;
	u_int32_t i, j, ttl_cols, *col_max_lens, cml_sum;
	bool header;

	ttl_cols = totalcols(row);
	col_max_lens = malloc(sizeof(u_int32_t) * ttl_cols);
	cml_sum = 0;

	for (i = 0; i < ttl_cols; i++) {
		col_max_lens[i] = maxlen(row, i);
		cml_sum += col_max_lens[i];
	}

	header = 1;
	for (_row = row; _row != 0; _row = _row->next) {
		for (i = 0; i < ttl_cols; i++) {
			col = colat(_row, i);
			printf("| ");
			if (col != 0 && header) {
				printf("\033[1m%s\033[0m ", col->str.cstr);
			} else if (col != 0) {
				printf("%s ", col->str.cstr);
			} else {
				printf(" ");
			}
			for (j = 0; j < (col_max_lens[i] - (col != 0 ? col->str.length : 0)); j++) {
				printf(" ");
			}
		}
		printf("|\n");
		if (header) {
			for (i = 0; i < ttl_cols; i++) {
				printf("|");
				for (j = 0; j < col_max_lens[i] + 2; j++) {
					printf("-");
				}
			}
			printf("|\n");
			
			header = 0;
		}
	}
}

/* --- */

/*
 * Outputs a error to stderr
 */
void tferror(const tftable_t table, const char* content) {
	u_int32_t i;
	size_t offset;
	matestr ln;
	char* desc;

	if (table.code == tfe_none) return;
	ln = matestralloc(128);
	offset = 0;
	for (i = 0; i < table.line; i++) {
		readln(content, &ln, &offset);
	}

	switch (table.code) {
		case tfe_unexpected_expr:
			desc = "Unexpected token";
			break;
		case tfe_unexpected_end:
			desc = "Unexpected end of string";
			break;
		default:
			desc = "(unknown error code)";
			break;
	}

	fprintf(stderr, "\033[1;31mError\033[0m: %s #%d at line %d (%d-%d)\n"
			" %3d | %s\n     |\033[0;31m", desc, table.code, table.line, table.start, table.end,
			table.line, ln.cstr);
	for (i = 0; i < table.start; i++) {
		fputc(' ', stderr);
	}
	for (; i <= table.end; i++) {
		fputc('^', stderr);
	}
	fprintf(stderr, " %s\033[0m\n", desc);

	free(ln.cstr);	
}
