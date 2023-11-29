#ifndef SINK_CALLBACK_H
#define SINK_CALLBACK_H

#include "sink_base.h"
#include "image_provider/image_provider_abstract.h"
#include <functional>

class SinkCallback : public SinkBase {
public:
    SinkCallback();
    ~SinkCallback() override;
    void start() override;
    void putSample(GstSample* sample) override;
    void setDataCb(std::function<void(uint8_t *, uint32_t)> cb);

private:
    std::function<void(uint8_t *, uint32_t)> m_data_cb;
protected:
    static constexpr auto tag = "SinkCallback: ";
};

#endif
