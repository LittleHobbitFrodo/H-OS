//
//  pair.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_PAIR_H
    #ifndef H_OS_LIB_PAIR_CPP
        #define H_OS_LIB_PAIR_CPP

    template<typename A, typename B> pair<A, B>::pair() {
        this->a = A();
        this->b = B();
    }
    template<typename A, typename B> template<bool use_equal_operator> pair<A, B>::pair(A a_, B b_) {
        if constexpr (use_equal_operator) {
            this->a = A();
            this->a = a_;
            this->b = B();
            this->b = b_;
        } else {
            this->a = A(a);
            this->b = B(b);
        }
    }
    template<typename A, typename B> pair<A, B>::~pair() {
        this->a.~A();
        this->b.~B();
    }
    template<typename A, typename B> A& pair<A, B>::first() const {
        return this->a;
    }
    template<typename A, typename B> B& pair<A, B>::second() const {
        return this->b;
    }

    #else
        #warning pair.cpp already included
    #endif
#else
    #error pair.cpp: pair.h not included
#endif