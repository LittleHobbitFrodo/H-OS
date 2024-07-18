//
//  vector.cpp
//      part of the H-OS project
//

#include "../memory/heap.h"
#ifdef H_OS_LIB_VECTOR_H
	#ifndef H_OS_LIB_VECTOR_CPP
		#define H_OS_LIB_VECTOR_CPP

		template<typename T> vector<T>::vector() {
			this->dt = nullptr;
			this->len = 0;
			//this->algn = 0;
		}

		template<typename T> vector<T>::vector(T& first) {
			this->len = 1;
			this->dt = (T*)mem::heap::alloc(sizeof(T) * (vector::alloc_add + (1 * vector::alloc_add == 0)));
			if constexpr (type::is_same<T, string>::value) {
				//  string-class only optimizations
				if (first.can_take_over()) {
					this->dt[0].take_over(first);
				} else {
					this->dt[0].string(first);
				}
			} else {
				this->dt[0].T(first);
			}
		}

		template<typename T> inline void vector<T>::clear() {
			if (this->dt != nullptr) {
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i].~T();
				}
				mem::heap::free(this->dt);
				this->dt = nullptr;
				this->len = 0;
			}
		}


		template<typename T> inline T& vector<T>::at(u32 i) {
			return this->dt[i];
		}

		template<typename T> inline T& vector<T>::operator[](u32 i) {
			return this->dt[i];
		}

		template<typename T> inline void vector<T>::push(T& instance) {
			if constexpr (type::is_same<T, string>::value) {
				//  string-class only optimization
				if (this->dt == nullptr) {
					this->len = 1;
					this->dt = (T*)mem::heap::alloc(sizeof(string) * (vector::alloc_add + (1 * (vector::alloc_add == 0))));
					this->dt[0] = string();
					if (instance.can_take_over()) {
						this->dt[0].take_over(instance);
					} else {
						this->dt[0] = instance;
					}
				} else {
					this->dt = (T*)mem::heap::realloc(this->dt, ++this->len * sizeof(string), sizeof(string) * (vector::realloc_add + (1 * (vector::realloc_add == 0))));
					this->dt[this->len - 1] = string();
					if (instance.can_take_over()) {
						this->dt[this->len - 1].take_over(instance);
					} else {
						this->dt[this->len - 1] = instance;
					}
				}
			} else {
				if (this->dt != nullptr) {
					this->dt = (T *) mem::heap::realloc(this->dt, sizeof(T) * (++this->len), sizeof(T) * (vector::realloc_add + (1 * (vector::realloc_add == 0))));
					this->dt[this->len - 1] = T(instance);
				} else {
					this->len = 1;
					this->dt = (T *) mem::heap::alloc(sizeof(T) * vector::alloc_add);
					this->dt[0] = T(instance);
				}
			}
		}

		template<typename T> inline void vector<T>::pushn(T instance) {
			if constexpr (type::is_same<T, string>::value) {
				//  string-class only optimization
				if (this->dt == nullptr) {
					this->len = 1;
					this->dt = (T *) mem::heap::alloc(sizeof(string) * (vector::alloc_add + (1 * (vector::alloc_add == 0))));
					this->dt[0] = string();
					if (instance.can_take_over()) {
						this->dt[0].take_over(instance);
					} else {
						this->dt[0] = instance;
					}
				} else {
					this->dt = (T *) mem::heap::realloc(this->dt, ++this->len * sizeof(string), sizeof(string) * (vector::realloc_add + (1 * (vector::realloc_add == 0))));
					this->dt[this->len - 1] = string();
					if (instance.can_take_over()) {
						this->dt[this->len - 1].take_over(instance);
					} else {
						this->dt[this->len - 1] = instance;
					}
				}
			} else {
				if (this->dt != nullptr) {
					this->dt = (T *) mem::heap::realloc(this->dt, sizeof(T) * (++this->len), sizeof(T) * (vector::realloc_add + (1 * (vector::realloc_add == 0))));
					this->dt[this->len - 1] = T(instance);
				} else {
					this->len = 1;
					this->dt = (T *) mem::heap::alloc(sizeof(T) * vector::alloc_add);
					this->dt[0] = T(instance);
				}

			}
		}

		template<typename T> void vector<T>::pop(u32 count) {
			if (this->dt != nullptr) {
				if (count >= this->len) {
					for (u32 i = 0; i < this->len; i++) {
						this->dt[i].~T();
					}
					mem::heap::free(this->dt);
					this->dt = nullptr;
					this->len = 0;
				} else {
					for (u32 i = this->len - count; i < this->len; i++) {
						this->dt[i].~T();
					}
					this->len -= count;

					this->dt = (T*)mem::heap::realloc(this->dt, sizeof(T) * this->len, sizeof(T) * (vector::realloc_add > 1));
				}
			}
		}

		template<typename T> inline void vector<T>::operator--() {
			if (this->dt != nullptr) {
				if (this->len > 1) {
					this->dt[--this->len].~T();
					this->dt = (T *) mem::heap::realloc(this->dt, this->len * sizeof(T));
				} else {
					mem::heap::free(this->dt);
					this->dt = nullptr;
					this->len = 0;
				}
			}
		}

		template<typename T> inline u32 vector<T>::size() const {
			return this->len;
		}

		template<typename T> inline const T* vector<T>::data() const {
			return this->dt;
		}


		template<typename T> inline void vector<T>::operator+=(T &instance) {
			this->push(instance);
		}

		template<typename T> inline void vector<T>::shrink_to(u32 nsize) {
			if (this->len > nsize) {
				//  this condition also means that dt is not null
				for (u32 i = nsize-1; i < this->len; i++) {
					this->dt[i].~T();
				}
				this->len = nsize;
				this->realloc();
			}
		}

		template<typename T> inline void vector<T>::shrink_until(T& value) {
			if (this->dt != nullptr) {
				u32 nsize = 0;
				for (i64 i = this->len - 1; i >= 0; i--) {
					if (this->dt[i] != value) {
						nsize = i;
						break;
					}
					this->dt[i].~T();
				}
				if (nsize == 0) {
					this->clear();
				} else {
					this->len = nsize+1;
					this->realloc();
				}
			}
		}

		template<typename T> inline void vector<T>::shrink_untiln(T value) {
			if (this->dt != nullptr) {
				u32 nsize = 0;
				for (i64 i = this->len - 1; i >= 0; i--) {
					if (this->dt[i] != value) {
						nsize = i;
						break;
					}
					this->dt[i].~T();
				}
				if (nsize == 0) {
					this->clear();
				} else {
					this->len = nsize+1;
					this->realloc();
				}
			}
		}


		template<typename T> inline void vector<T>::resize(u32 nsize, bool realloc) {
			if (this->dt == nullptr) {
				this->len = nsize;
				this->dt = (T*)mem::heap::alloc(sizeof(T) * nsize);
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i] = T();
				}
			} else if (this->len > nsize) {
				this->shrink_to(nsize);
				if (realloc) {
					this->realloc();
				}
			} else {
				this->dt = (T*)mem::heap::realloc(this->dt, sizeof(T) * nsize);
				for (u32 i = this->len; i < nsize; i++) {
					this->dt[i] = T();
				}
				this->len = nsize;
			}
		}

		template<typename T> inline void vector<T>::realloc() {
			if (this->dt != nullptr) {
				if (mem::heap::block_size(this->dt) > (this->len * sizeof(T)) + sizeof(struct mem::heap::segment)) {
					mem::heap::divide_block((struct mem::heap::segment*)(this->dt - sizeof(struct mem::heap::segment)), this->len * sizeof(T));
				}
				/*if (sizeof(T) * this->len < mem::heap::block_size(this->dt) - ((vector::realloc_add / 2) * sizeof(T))) {
					u8* dat = (u8*)mem::heap::alloc(sizeof(T) * this->len);
					u64 alen = sizeof(T) * this->len;
					for (u64 i = 0; i < alen; i++) {
						dat[i] = ((u8*)this->dt)[i];
					}
					mem::heap::free(this->dt);
					this->dt = (T*)dat;
				}*/
			}
		}



		template<typename T> vector<T>::~vector() {
			if (this->dt != nullptr) {
				for (u32 i = 0; i < this->len; i++) {
					this->dt[i].~T();
				}
				mem::heap::free(this->dt);
			}
		}


		template<typename T> aligned_vector<T>::aligned_vector(u8 align) {
			this->dt = aligned_ptr<T>(align);
			this->len = 0;
		}
		template<typename T> aligned_vector<T>::aligned_vector(T &first, u8 align) {
			this->dt = aligned_ptr<T>(sizeof(T), align);
			this->len = 1;
		}
		template<typename T> aligned_vector<T>::~aligned_vector() {
			this->dt.~aligned_ptr();
		}

		template<typename T> void aligned_vector<T>::set_align(u8 align) {
			this->dt.set_align(align);
		}
		template<typename T> u8 aligned_vector<T>::align() const {
			return this->dt.align();
		}

		template<typename T> T& aligned_vector<T>::at(u32 i) {
			return this->dt.ptr()[i];
		}
		template<typename T> T& aligned_vector<T>::operator[](u32 i) {
			return this->dt.ptr()[i];
		}

		template<typename T> void aligned_vector<T>::push(T &instance) {
			if (this->dt.ptr() == nullptr) {
				this->len = 1;
				this->dt.alloc(sizeof(T) * aligned_vector::alloc_add);
				this->dt.ptr()[0] = T(instance);
			} else {
				this->dt.realloc(++this->len * sizeof(T), aligned_vector::realloc_add * sizeof(T));
				this->dt.ptr()[this->len - 1] = T(instance);
			}
		}
		template<typename T> void aligned_vector<T>::pushn(T instance) {
			if (this->dt.ptr() == nullptr) {
				this->len = 1;
				this->dt.alloc(sizeof(T) * aligned_vector::alloc_add);
				this->dt.ptr()[0] = T(instance);
			} else {
				this->dt.realloc(++this->len * sizeof(T), aligned_vector::realloc_add * sizeof(T));
				this->dt.ptr()[this->len - 1] = T(instance);
			}
		}

		template<typename T> void aligned_vector<T>::pop(u32 count) {
			if (this->dt.ptr() != nullptr) {
				if (this->len >= count) {
					for (u32 i = 0; i < this->len; i++) {
						this->dt.ptr()[i].~T();
					}
					this->dt.free();
					this->len = 0;
				} else {
					for (u32 i = this->len - count; i < this->len; ++i) {
						this->dt.ptr()[i].~T();
					}
					this->len -= count;
				}
			}
		}

		template<typename T> void aligned_vector<T>::operator--() {
			if (this->len > 1) {
				--this->len;
				this->dt.ptr()[this->len].~t();
			} else if (this->len == 1) {
				this->dt.ptr()[0].~T();
				this->dt.free();
				this->len = 0;
			}
		}

		template<typename T> void aligned_vector<T>::clear() {
			if (this->dt.ptr() != nullptr) {
				for (u32 i = 0; i < this->len; i++) {
					this->dt.ptr()[i].~T();
				}
				this->dt.free();
				this->len = 0;
			}
		}

		template<typename T> u32 aligned_vector<T>::size() const {
			return this->len;
		}

		template<typename T> const T* aligned_vector<T>::data() const {
			return this->dt.ptr();
		}

		template<typename T> void aligned_vector<T>::operator+=(T &instance) {
			this->push(instance);
		}

		template<typename T> void aligned_vector<T>::resize(u32 nsize) {
			if (this->dt.ptr() == nullptr) {
				this->len = nsize;
				this->dt.alloc(this->len * sizeof(T));
				for (u32 i = 0; i < this->len; i++) {
					this->dt.ptr()[i] = T();
				}
			} else if (this->len > nsize) {
				this->shrink_to(nsize);
			} else {
				this->dt.realloc(nsize * sizeof(T));
				for (u32 i = this->len; i < nsize; i++) {
					this->dt.ptr()[i] = T();
				}
				this->len = nsize;
			}
		}

		template<typename T> void aligned_vector<T>::realloc() {
			if (this->dt.ptr() != nullptr) {
				if (mem::heap::block_size(this->dt.ptr()) > (this->len * sizeof(T)) + sizeof(struct mem::heap::segment)) {
					mem::heap::divide_block((struct mem::heap::segment*)(this->dt.ptr() - sizeof(struct mem::heap::segment)), this->len * sizeof(T));
				}
			}
		}

		template<typename T> void aligned_vector<T>::shrink_to(u32 nsize) {
			if (nsize < this->len) {
				for (u32 i = nsize; i < this->len; i++) {
					this->dt.ptr()[i].~T();
				}
				this->len = nsize;
				this->realloc();
			}
		}

		template<typename T> void aligned_vector<T>::shrink_until(T &value) {
			u32 nsize = 0;
			for (i64 i = this->len - 1; i >= 0; i--) {
				if (this->dt.ptr()[i] != value) {
					nsize = i;
					break;
				}
				this->dt.ptr()[i].~T();
			}
			if (nsize == 0) {
				this->clear();
			} else {
				this->len = nsize+1;
				this->realloc();
			}
		}

		template<typename T> void aligned_vector<T>::shrink_untiln(T value) {
			u32 nsize = 0;
			for (i64 i = this->len - 1; i >= 0; i--) {
				if (this->dt.ptr()[i] != value) {
					nsize = i;
					break;
				}
				this->dt.ptr()[i].~T();
			}
			if (nsize == 0) {
				this->clear();
			} else {
				this->len = nsize+1;
				this->realloc();
			}
		}

	#else
		#warning vector.cpp already included
	#endif
#else
	#error vector.cpp: vector.h not included
#endif