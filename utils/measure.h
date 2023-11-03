#ifndef MEASURE_H
#define MEASURE_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <iostream>
#include <QDateTime>

struct Measure {
    Measure() {}
    Measure(Measure const&) = delete;
    void operator=(Measure const&) = delete;

    static Measure* instance() {
        static Measure instance;
        return &instance;
    }

    ~Measure() {}

    _GMainLoop* Loop;

    void onEncodePutSample() {
        static bool m = false;
        if(!m) {
            m = true;
            t1 = QDateTime::currentMSecsSinceEpoch();
            std::cout << "BTEST: encode put sample capture: " << t1 << std::endl;
        }
    }

    void onImageSampleReady() {
        static bool m = false;
        if(!m) {
            m = true;
            t2 = QDateTime::currentMSecsSinceEpoch();
            std::cout << "BTEST: image sample capture: " << (t2-t1)/1000 << std::endl;
        }
    }

    void onEncodeSampleReady() {
        static bool m = false;
        if(!m) {
            m = true;
            t3 = QDateTime::currentMSecsSinceEpoch();
            std::cout << "BTEST: encode sample ready capture: " << (t3-t2)/1000 << std::endl;
        }
    }

    void onDecodePutSample() {
        static bool m = false;
        if(!m) {
            m = true;
            t4 = QDateTime::currentMSecsSinceEpoch();
            std::cout << "BTEST: decode put sample capture: " << (t4) << std::endl;
        }
    }

    void onDecodeSampleReady() {
        static bool m = false;
        if(!m) {
            m = true;
            t5 = QDateTime::currentMSecsSinceEpoch();
            std::cout << "BTEST: decode sample ready capture: " << ((t5-t4)) << std::endl;
        }
    }

    uint64_t t1 = 0;
    uint64_t t2 = 0;
    uint64_t t3 = 0;
    uint64_t t4 = 0;
    uint64_t t5 = 0;
};

#endif // MEASURE_H
