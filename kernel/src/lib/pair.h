//
//  pair.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_PAIR_H
    #define H_OS_LIB_PAIR_H

    template<typename A, typename B> class pair {

        public:

        A first;
        B second;

        pair(A a, B b);

        ~pair();
    };

#else
    #warning pair.h already included
#endif