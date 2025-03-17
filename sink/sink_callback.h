#ifndef SINK_CALLBACK_H
#define SINK_CALLBACK_H

#include "sink_base.h"
#include <functional>

class SinkCallback : public SinkBase {
public:
    SinkCallback();
    ~SinkCallback() override;

    void start() override;
    void pause() override;

    void putSample(GstSample* sample) override;
    void onData(std::function<void(uint8_t *, uint32_t)> cb);

private:
    std::function<void(uint8_t *, uint32_t)> m_on_data;
protected:
    static constexpr auto TAG = "SinkCallback: ";
};

#endif
