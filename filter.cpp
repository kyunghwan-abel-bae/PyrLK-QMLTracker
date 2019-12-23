#include "filter.h"
#include "opencvhelper.h"
#include "rgbframehelper.h"

#include <QFile>

QVideoFilterRunnable* Filter::createFilterRunnable() {
    return new FilterRunnable(this);
}

FilterRunnable::FilterRunnable(Filter *filter)
    : filter_(filter),
      classifier_(0),
      is_tracked_(false)
{
    filter_->is_added_ = false;
}

QVideoFrame FilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) {
    Q_UNUSED(surfaceFormat);
    Q_UNUSED(flags);

    // Supports YUV (I420 and NV12) and RGB. The GL path is readback-based and slow.
    if (!input->isValid()
            || (input->handleType() != QAbstractVideoBuffer::NoHandle && input->handleType() != QAbstractVideoBuffer::GLTextureHandle)) {
        qWarning("Invalid input format");
        return QVideoFrame();
    }

    input->map(QAbstractVideoBuffer::ReadOnly);
    if( (input->pixelFormat() == QVideoFrame::Format_YUV420P)
            || (input->pixelFormat() == QVideoFrame::Format_NV12)) {
        is_yuv_ = true;
        frame_current_ = yuvFrameToMat8(*input);
    } else {
        is_yuv_ = false;
        QImage wrapper = imageWrapper(*input);
        if(wrapper.isNull()) {
            if(input->handleType() == QAbstractVideoBuffer::NoHandle)
                input->unmap();
            return *input;
        }
        frame_current_ = imageToMat8(wrapper);
    }
    ensureC3(&frame_current_);
    if(input->handleType() == QAbstractVideoBuffer::NoHandle)
        input->unmap();

    cv::flip(frame_current_, frame_current_, 1);

    cv::Mat frame_prev;

    if(frame_prev_.empty())
        frame_current_.copyTo(frame_prev);
    else
        frame_prev_.copyTo(frame_prev);

    frame_current_.copyTo(frame_prev_);

    return QVideoFrame(mat8ToImage(ProcessPyrLK(frame_prev, frame_current_)));
}

cv::Mat FilterRunnable::ProcessPyrLK(cv::Mat &frame_prev, cv::Mat &frame_current) {
    cv::Size winSize(31, 31);

    cv::Mat frame_output;

    if(frame_current.empty())
        return frame_output;

    if(frame_prev.empty())
        frame_current.copyTo(frame_prev);

    frame_current.copyTo(frame_output);

    cvtColor(frame_prev, frame_prev, cv::COLOR_BGR2GRAY);
    cvtColor(frame_current, frame_current, cv::COLOR_BGR2GRAY);

    if(!is_tracked_) {
        is_tracked_ = true;
        cv::goodFeaturesToTrack(frame_current, points_[1], MAX_CORNERS, 0.01, 10, cv::Mat(), 3, 0, 0.04);
        swap(points_[1], points_[0]);
    }

    if(filter_->is_added_) {
        points_[1].push_back(filter_->point_);
        swap(points_[1], points_[0]);
        filter_->is_added_ = false;
    }

    cv::calcOpticalFlowPyrLK(
                frame_prev,
                frame_current,
                points_[0],
                points_[1],
                status_,
                err_,
                winSize,
                3,
                cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 20, 0.03),
                0,
                0.001
    );

    for(int i=0;i<points_[1].size();i++) {
        cv::circle(frame_output, points_[1][i], 5, cv::Scalar(0,255,0), -1, 8);
        cv::line(frame_output, points_[0][i], points_[1][i], cv::Scalar(255,0,0));
    }

    swap(points_[1], points_[0]);

    return frame_output;
}
