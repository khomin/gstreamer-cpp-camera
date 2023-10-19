#include "sink_base.h"
#include <random>

SinkBase::SinkBase() {
    m_id = random();
}

SinkBase::~SinkBase() {}

void SinkBase::putData(uint8_t* data, uint32_t len) {}

void SinkBase::putSample(GstSample* sample) {}


uint64_t SinkBase::getId() {
    return m_id;
}

bool SinkBase::getError() {
    return m_error;
}
