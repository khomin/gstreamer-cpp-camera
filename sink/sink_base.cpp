#include "sink_base.h"

SinkBase::SinkBase() {}

SinkBase::~SinkBase() {}

void SinkBase::putData(uint8_t* data, uint32_t len) {}

void SinkBase::putSample(GstSample* sample) {}
