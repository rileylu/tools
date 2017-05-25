TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

LIBS += -lpthread

HEADERS += \
    thread_pool.h \
    threadsafe_queue.h \
    work_stealing_queue.h \
    function_wrapper.h \
    join_threads.h
