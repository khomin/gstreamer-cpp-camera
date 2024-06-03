#ifndef CONTROL_H
#define CONTROL_H

#include <iostream>
#include <thread>
#include <memory>
#include <functional>
#include <QObject>
#include <QTimer>

class Control : public QObject {
    Q_OBJECT
public:
    Q_PROPERTY(QString bitrate READ getBitrate NOTIFY onBitrateChanged)

    Control() {
        m_timer = std::make_shared<QTimer>();
        connect(m_timer.get(), &QTimer::timeout, this, [&] {
            m_bites_diff = m_bite_cnt - m_last_bite_cnt;
            m_last_bite_cnt = m_bite_cnt;
            emit onBitrateChanged();
        });
        m_timer->start(std::chrono::milliseconds(500));
    }

    void addByteCount(uint64_t v) {
        m_bite_cnt += v;
    }

    QString getBitrate() {
        auto sum([&] () -> float {
            auto result = 0.0;
            for (auto value : m_filter) {
                result += value;
            }
            return result;
        });

        auto addToFilter([&](uint64_t v) {
            double res = v;
            m_filter.insert(m_filter.begin(), v);
            if (m_filter.size() >= BITRATE_FILTER_SIZE) {
                auto sumRate = sum();
                res = sumRate / m_filter.size() / BITRATE_MEASURE_SEC;
                m_filter.erase(m_filter.end()-1);
            }
            return res;
        });
        auto v = addToFilter((m_bites_diff));
        return convertBitRate(v * 8);
    }

    static QString convertBitRate(quint64 bitrate) {
        QString res;
        quint64 GbSize = 1024 * 1024 * 1024;
        quint64 MbSize = 1024 * 1024;
        quint64 KbSize = 1024;
        double GbSizePart = 1024 * 1024 * 1024/10;
        double MbSizePart = 1024 * 1024/10;
        double KbSizePart = 1024/10;
        if (bitrate > GbSize) {
            res = "" + QString::number((double)round((double) bitrate / GbSizePart)/10) + "Gb/s";
        } else if (bitrate > MbSize) {
            res = "" + QString::number((double)round((double) bitrate / MbSizePart)/10) + "Mb/s";
        } else if (bitrate > KbSize) {
            res = "" + QString::number((double)round((double) bitrate / KbSizePart)/10) + "Kb/s";
        } else {
            res = "" + QString::number((float) bitrate) + "bit/s";
        }
        return res;
    }

signals:
    void onBitrateChanged();

private:
    uint64_t m_bite_cnt = 0;
    uint64_t m_last_bite_cnt = 0;
    uint64_t m_bites_diff = 0;
    std::vector<double> m_filter;
    std::shared_ptr<QTimer> m_timer;

    static constexpr auto BITRATE_FILTER_SIZE = 6;
    static constexpr auto BITRATE_MEASURE_SEC = 0.5;
};

#endif
