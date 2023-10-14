#include "sink_base.h"
#include <random>

SinkBase::SinkBase() {
    id = arc4random();
}

void SinkBase::putData(uint8_t* data, uint32_t len) {}

void SinkBase::putSample(GstSample* sample) {}
