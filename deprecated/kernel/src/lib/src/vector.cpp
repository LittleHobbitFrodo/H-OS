//
//  vector.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_VECTOR_H
	#ifndef H_OS_LIB_VECTOR_CPP
		#define H_OS_LIB_VECTOR_CPP


		template<typename T> vector<T>::vector() {
			this->d = nullptr;
			this->l = 0;
		}

		template<typename T> vector<T>::vector(size_t size) {
			if (size != 0) {
				this->l = size;
				this->d = (T*)mem::heap::alloc(sizeof(T) * this->l);
				for (size_t i = 0; i < size; i++) {
					this->d[i] = T();
				}
			} else {
				this->d = nullptr;
				this->l = 0;
			}
		}

		template<typename T> vector<T>::vector(vector<T> &vec) {
			this->clear();
			this->d = vec.d;
			this->l = vec.l;

			vec.d = nullptr;
			vec.l = 0;
		}

		template<typename T> vector<T>::~vector() {
			this->clear();
		}

		template<typename T> template<bool realloc> void vector<T>::resize(size_t size) {
			if (this->d == nullptr) {
				this->l = size;
				this->d = (T*)mem::heap::alloc(sizeof(T) * size);
				for (size_t i = 0; i < size; i++) {
					this->d[i] = T();
				}
			} else {
				if (this->l == size) {
					return;
				}
				if (this->l < size) {
					this->d = (T*)mem::heap::realloc(this->d, sizeof(T) * size);
					for (size_t i = this->l; i < size; ++i) {
						this->d[i] = T();
					}
				} else {
					for (size_t i = this->l - 1; i >= size; --i) {
						this->d[i].~T();
					}
					this->l = size;
					if constexpr  (realloc) {
						this->realloc();
					}
				}
			}
		}

		template<typename T>T& vector<T>::push() {
		    if (this->d == nullptr) {
		    	this->d = (T*)mem::heap::alloc(sizeof(T));
		    	this->l = 1;
		    	this->d[0] = T();
		    	return this->d[0];
		    } else {
		    	++this->l;
		    	this->d = (T*)mem::heap::realloc(this->d, sizeof(T) * this->l);
		    	this->d[this->l - 1] = T();
		    	return this->d[this->l - 1];
		    }
		}

		template<typename T> void vector<T>::pop(size_t size) {
		    if (size == 0) {
			    return;
		    } else if (size >= this->l) {
			    this->clear();
		    } else {
			    for (size_t i = this->l - size; i < this->l; --i) {
					this->d[i].~T();
			    }
		    	this->l -= size;
		    }
		}

		template<typename T> void vector<T>::operator+=(T &add) {
		    auto a = this->push();
			a = add;
		}

		template<typename T> void vector<T>::operator--() {
		    this->pop();
		}

		template<typename T> void vector<T>::clear() {
		    if (this->d != nullptr) {
			    mem::heap::free(this->d);
		    	this->d = nullptr;
		    	this->l = 0;
		    }
		}

		template<typename T> size_t vector<T>::size() const {
		    return this->l;
		}

		template<typename T> T& vector<T>::at(size_t i) const {
		    return this->d[i];
		}

		template<typename T> T& vector<T>::operator[](size_t i) const {
			return this->d[i];
		}

		template<typename T>T* vector<T>::data() const {
		    return this->d;
		}

		template<typename T> bool vector<T>::empty() const {
		    return this->d == nullptr;
		}

		template<typename T> void vector<T>::realloc() {
			if (this->d != nullptr) {
				if (mem::heap::block_size(this->d) > (this->l * sizeof(T)) + sizeof(struct mem::heap::segment)) {
					mem::heap::divide_block((struct mem::heap::segment*)((size_t)this->d - sizeof(struct mem::heap::segment)), this->l * sizeof(T));
				}
			}
		}






		template<typename T> aligned_vector<T>::aligned_vector() {
		    this->d = aligned_ptr<T>();
			this->l = 0;
		}

		template<typename T> aligned_vector<T>::aligned_vector(size_t align) {
		    this->d = aligned_ptr<T>(align);
			this->l = 0;
		}

		template<typename T> aligned_vector<T>::aligned_vector(size_t size, size_t align) {
		    this->d = aligned_ptr<T>(sizeof(T) * size, align);
			this->l = size;
		}

		template<typename T> aligned_vector<T>::aligned_vector(aligned_vector<T> &vec) {
		    //	aligned_ptr::take_over frees the memory if there is any data allocated
			this->d.take_over(vec.d);
			this->l = vec.l;
			vec.l = 0;
		}

		template<typename T> aligned_vector<T>::~aligned_vector() {
			this->d.free();
			this->l = 0;
		}

		template<typename T> template<bool realloc> void aligned_vector<T>::resize(size_t size) {
			if (this->d == nullptr) {
				this->d.alloc(sizeof(T) * size);
				this->l = size;
				for (size_t i = 0; i < this->l; ++i) {
					this->d.ptr()[i] = T();
				}
			} else {
				if (this->l == size) {
					return;
				}
				if (this->l < size) {
					this->d.realloc(sizeof(T) * size);
					for (size_t i = this->l; i < size; ++i) {
						this->d.ptr()[i] = T();
					}
				} else {
					for (size_t i = this->l - 1; i >= size; --i) {
						this->d.ptr()[i].~T();
					}
					this->l = size;
					if constexpr  (realloc) {
						this->realloc();
					}
				}
			}
		}



		template<typename T> T& aligned_vector<T>::at(size_t i) const {
		    return this->d.ptr()[i];
		}

		template<typename T> T& aligned_vector<T>::operator[](size_t i) const {
		    return this->d.ptr()[i];
		}

		template<typename T> T& aligned_vector<T>::push() {
			if (this->d.ptr() == nullptr) {
				this->d.alloc(sizeof(T));
				this->l = 1;
				this->d.ptr()[0] = T();
				return this->d.ptr()[0];
			} else {
				this->d.realloc(sizeof(T) * ++this->l);
				this->d.ptr()[this->l - 1] = T();
				return this->d.ptr()[this->l - 1];
			}
		}


		template<typename T> void aligned_vector<T>::pop(size_t size) {
			if (size == 0) {
				return;
			} else if (size >= this->l) {
				this->clear();
			} else {
				for (size_t i = this->l - size; i < this->l; --i) {
					this->d.ptr()[i].~T();
				}
				this->l -= size;
			}
		}

		template<typename T> void aligned_vector<T>::operator+=(T& add) {
		    auto a = this->push();
			a = add;
		}

		template<typename T> void aligned_vector<T>::operator--() {
		    this->pop();
		}

		template<typename T> void aligned_vector<T>::clear() {
		    this->d.free();
			this->l = 0;
		}

		template<typename T> size_t aligned_vector<T>::size() const {
		    return this->l;
		}

		template<typename T> T* aligned_vector<T>::data() const {
		    return this->d.ptr();
		}

		template<typename T> bool aligned_vector<T>::empty() const {
		    return this->d.ptr() == nullptr;
		}

		template<typename T> size_t aligned_vector<T>::offset() const {
		    return this->d.offset();
		}

		template<typename T> size_t aligned_vector<T>::align() const {
		    return this->d.align();
		}

		template<typename T> void aligned_vector<T>::realloc() {
			if (this->d.ptr() != nullptr) {
				if (mem::heap::block_size(this->d.ptr()) > (this->l * sizeof(T)) + sizeof(struct mem::heap::segment)) {
					mem::heap::divide_block((struct mem::heap::segment*)((size_t)this->d.ptr() - sizeof(struct mem::heap::segment)), this->l * sizeof(T));
				}
			}
		}

		template<typename T, size_t size> constexpr size_t static_index(const T* arr, T value) {
			for (size_t i = 0; i < size; i++) {
				if (arr[i] == value) {
					return i;
				}
			}
		}

	#else
		#warning vector.cpp already included
	#endif
#else
	#error vector.cpp: vector.h not included
#endif