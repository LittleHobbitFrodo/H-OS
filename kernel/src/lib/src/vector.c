//
//	vector.c
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

#ifdef H_OS_LIB_VECTOR_H
	#ifndef H_OS_LIB_VECTOR_C
		#define H_OS_LIB_VECTOR_C

		void vec_resize(vector* this, size_t len)  {
			//	destruction of each popped element must be done manually
			if (this->data == null) {
				this->len = len;
				this->data = alloc(this->len * this->bsize);
				return;
			}
			if (this->len == len) {
				return;
			}
			this->len = len;
			if (this->len < len) {
				this->data = realloca(this->data, this->len * this->bsize, vector_realloc_add);
			} else {
				this->data = realloc(this->data, this->len * this->bsize);
			}
		}

		void* vec_push(vector* this, size_t count) {
			if (this->data == null) {
				this->len = count;
				this->data = alloc(this->len * this->bsize);
				return this->data;
			}
			size_t olen = this->len;
			this->len += count;
			this->data = realloca(this->data, this->len * this->bsize, vector_realloc_add);
			return (void*)((size_t)this->data + (this->bsize * olen));
		}

		void vec_pop(vector* this, size_t count) {
			if (this->data != null) {
				if (this->len <= count) {
					free(this->data);
					this->data = null;
					this->len = 0;
				} else {
					this->len -= count;
					this->data = realloc(this->data, this->len * this->bsize);
				}
			}
		}

	#endif
	//	#warning vector.c already included
	//#endif
#else
	#error vector.c: vector.h not included
#endif