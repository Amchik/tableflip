#include <stdlib.h>
#include <string.h>

#include "./include/matestr.h"

/*
 * Creates new instance of matestr.
 */
matestr matestralloc(size_t n) {
	matestr _new;

	_new.cstr = malloc(n);
	_new.length = 0;
	_new.__allocated = n;
	_new.cstr[0] = '\0';

	return(_new);
}

/*
 * Creates new instance of matestr.
 * Calculates length of cstr, if cstr == 0,
 * passes empty allocated string.
 * If len == 0 it will be ignored
 */
matestr matestr_new(const char* cstr, size_t len) {
	matestr _new;

	if (cstr == 0) {
		_new.cstr = malloc(128);
		_new.length = 0;
		_new.__allocated = 128;
		_new.cstr[0] = '\0';
	} else {
		_new.length = len == 0 ? strlen(cstr) : len;
		_new.__allocated = _new.length + 1;
		_new.cstr = malloc(_new.__allocated);
		memcpy(_new.cstr, cstr, _new.__allocated);
	}

	return(_new);
}
/*
 * Normalizes cstr in matestr
 */
void    matestr_normalize(matestr* self) {
	self->cstr[self->length] = '\0';

	return;
}
/*
 * Optimizes memory in matestr
 */
void    matestr_optimize(matestr* self) {
	if (self->length + 1 == self->__allocated) {
		return;
	}

	self->__allocated = self->length + 1;
	self->cstr = realloc(self->cstr, self->__allocated);

	return;
}
/*
 * Copy string to matestr
 */
void    matestr_append(matestr* self, const char* cstr, size_t len) {
	u_int32_t cstr_len;

	cstr_len = len == 0 ? strlen(cstr) : len;
	if (self->__allocated <= (cstr_len + self->length)) {
		self->__allocated += cstr_len;
		self->cstr = realloc(self->cstr, self->__allocated);
	}

	memcpy(self->cstr + self->length, cstr, cstr_len);
	self->length += cstr_len;
	self->cstr[self->length] = '\0'; /* normalize */

	return;
}
/*
 * Allocs matestr for n bytes
 */
void    matestr_appendm(matestr* self, size_t n) {
	self->__allocated += n;
	self->cstr = realloc(self->cstr, self->__allocated);

	return;
}
void    matestr_replacec(matestr* self, char oldc, char newc) {
		u_int32_t i;
		for (i = 0; i < self->length; i++) {
				if (self->cstr[i] == oldc) {
						self->cstr[i] = newc;
				}
		}
}
