//
//	string.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_STRING_H
	#ifndef H_OS_LIB_STRING_C
		#define H_OS_LIB_STRING_C

		void str_take_over(string* this, string* other) {
			if (this->data != null) {
				free(this->data);
			}
			this->data = other->data;
			this->size = other->size;

			other->data = null;
			other->size = 0;
		}

		void str_push(string* this, const char* str) {
			if (this->data == null) {
				this->size = strlen(str) - 1;
				this->data = (char*)alloc(this->size);
				memcpy((void*)str, this->data, this->size);
				return;
			}
			size_t osize = this->size;
			this->size += strlen(str) - 1;
			this->data = (char*)realloca(this->data, this->size, string_realloc_add);
			memcpy((void*)str, (void*)((size_t)this->data + osize), this->size - osize);
		}

		void str_pushs(string* this, const string* other) {
			if (this->data == null) {
				this->size = other->size;
				this->data = (char*)alloc(this->size);
				memcpy(other->data, this->data, this->size);
				return;
			}
			this->data = (char*)realloca(this->data, this->size + other->size, string_realloc_add);
			memcpy(other->data, (void*)((size_t)this->data + this->size), this->size + other->size);
			this->size += other->size;
		}

		void str_pop(string* this, size_t count) {
			if (this->data == null) {
				return;
			}
			if (this->size <= count) {
				free(this->data);
				this->data = null;
				this->size = 0;
			} else {
				this->size -= count;
			}
		}

		void str_set(string* this, const char* str) {
			if (this->data == null) {
				this->size = strlen(str) - 1;
				this->data = (char*)alloc(this->size);
				memcpy((void*)str, this->data, this->size);
				return;
			}
			this->size = strlen(str) - 1;
			this->data = (char*)realloc(this->data, this->size);
			memcpy((void*)str, this->data, this->size);
		}

		void str_sets(string* this, const string* other) {
			if (this->data == null) {
				this->size = other->size;
				this->data = (char*)alloc(this->size);
				memcpy(other->data, this->data, this->size);
				return;
			}
			this->size = other->size;
			this->data = (char*)realloc(this->data, this->size);
			memcpy(other->data, this->data, this->size);
		}

		void str_setss(string* this, const char* str, size_t size) {
			this->size = size;
			if (this->data == null) {
				this->data = (char*)alloc(this->size);
				memcpy((void*)str, this->data, this->size);
				return;
			}
			this->data = (char*)realloc(this->data, this->size);
			memcpy((void*)str, this->data, this->size);
		}

		void str_resize(string* this, size_t size) {
			this->size = size;
			if (this->data != null) {
				this->data = (char*)realloc(this->data, size);
			} else {
				this->data = (char*)alloc(size);
			}
		}

		i8 str_cmp(const string* s1, const char* s2) {
			u32 ln = strlen(s2) - 1;
			if (s1->size < ln) {
				return -1;
			} else if (s1->size > ln) {
				return 1;
			}
			for (u32 i = 0; i < s1->size; i++) {
				if (s1->data[i] != s2[i]) {
					return (i8)(((s1->data[i] > s2[i]) * 2) - 1);
					//  dt[i] > -> 2 - 1 = 1
					//  dt[i] < -> 0 - 1 = -1
				}
			}
			return 0;
		}

		i8 str_cmps(const string* s1, const string* s2) {
			if (s1->size < s2->size) {
				return -1;
			} else if (s1->size > s2->size) {
				return 1;
			}
			for (u32 i = 0; i < s1->size; i++) {
				if (s1->data[i] != s2->data[i]) {
					return (i8)(((s1->data[i] > s2->data[i]) * 2) - 1);
					//  dt[i] > -> 2 - 1 = 1
					//  dt[i] < -> 0 - 1 = -1
				}
			}
			return 0;
		}

		bool str_cmpb(const string* s1, const char* s2) {
			if (s1->size != strlen(s2) - 1) {
				return false;
			}
			for (size_t i = 0; i < s1->size; i++) {
				if (s1->data[i] != s2[i]) {
					return false;
				}
			}
			return true;
		}

		bool str_cmpbs(const string* s1, const string* s2) {
			if (s1->size != s2->size) {
				return false;
			}
			for (size_t i = 0; i < s1->size; i++) {
				if (s1->data[i] != s2->data[i]) {
					return false;
				}
			}
			return true;
		}

	#endif
	//	#warning string.c already included
	//#endif
#else
	#error string.c: string.h not inlcluded
#endif