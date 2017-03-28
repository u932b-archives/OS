#ifndef UTIL_HPP
#define UTIL_HPP

namespace util
{
    enum class  STATE {CREATED, READY, RUNNING, BLOCK, DONE};
    enum class TRANS_STATE {TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK,
        TRANS_TO_DONE};
    void function1();
    void function2();

}

#endif /* UTIL_HPP */
