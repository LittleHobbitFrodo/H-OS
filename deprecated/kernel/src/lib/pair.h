//
//  pair.h
//      part of the H-OS project
//

#ifndef H_OS_LIB_PAIR_H
    #define H_OS_LIB_PAIR_H

template<typename A, typename B> class pair {
  A a;
  B b;

  public:

  inline A& first() const;
  inline B& second() const;

  pair();

  template<bool use_equal_operator = false> pair(A a_, B b_);

  ~pair();
};


#else
    #warning pair.h already included
#endif