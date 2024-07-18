//
//  pair.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_PAIR_H
    #ifndef H_OS_LIB_PAIR_CPP
        #define H_OS_LIB_PAIR_CPP

        template<typename A, typename B> pair<A, B>::pair(A a, B b) {
            this->first = A(a);
            this->second = B(b);
        }

        template<typename A, typename B> pair<A, B>::~pair() {
            this->first.~A();
            this->second.~B();
        }




    #else
        #warning pair.cpp already included
    #endif
#else
    #error pair.cpp: pair.h not included
#endif