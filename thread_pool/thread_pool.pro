TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

LIBS += -lpthread
QMAKE_CXXFLAGS += -std=c++11

HEADERS += \
    thread_pool.h \
    threadsafe_queue.h \
    work_stealing_queue.h \
    function_wrapper.h \
    join_threads.h
