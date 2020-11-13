/* SPDX-License-Identifier: GPLv3-or-later */

#ifndef ACE3X_QT_SINK_HPP_
#define ACE3X_QT_SINK_HPP_

#include <spdlog/sinks/base_sink.h>

#include <QPlainTextEdit>

template <typename Mutex>
class qt_sink : public spdlog::sinks::base_sink<Mutex> {
public:
    qt_sink(QPlainTextEdit* log)
        : text_edit_(log)
    {
    }

protected:
    void sink_it_(const spdlog::details::log_msg& msg) override
    {
        spdlog::memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        text_edit_->moveCursor(QTextCursor::End);
        text_edit_->insertPlainText(QString::fromStdString(fmt::to_string(formatted)));
        text_edit_->moveCursor(QTextCursor::End);
    }

    void flush_() override
    {
    }

private:
    QPlainTextEdit* text_edit_;
};

#include <spdlog/details/null_mutex.h>

#include <mutex>

using qt_sink_mt = qt_sink<std::mutex>;
using qt_sink_st = qt_sink<spdlog::details::null_mutex>;

#endif    // ACE3X_QT_SINK_HPP_
