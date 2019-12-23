#ifndef FILTER_H
#define FILTER_H

#include <QAbstractVideoFilter>

#include <opencv2/core/core.hpp>
#include <opencv2/video/tracking.hpp>
#include <opencv2/objdetect/objdetect.hpp>

#include <QCursor>

const int MAX_CORNERS = 500;
const int MAX_POINTS = 10000;

using namespace std;

class Filter : public QAbstractVideoFilter
{
    Q_OBJECT
public:
    QVideoFilterRunnable *createFilterRunnable() Q_DECL_OVERRIDE;

    Q_INVOKABLE void AddPoint(float x, float y) { point_ = cv::Point2f(x,y); is_added_ = true; }

signals:
    void finished(QObject *e);

private:
    cv::Point2f point_;
    bool is_added_;

    friend class FilterRunnable;
};

class FilterRunnable : public QVideoFilterRunnable {
public:
    FilterRunnable(Filter *filter);

    QVideoFrame run(QVideoFrame *input, const QVideoSurfaceFormat &surfaceFormat, RunFlags flags) Q_DECL_OVERRIDE;

private:
    void Process();
    cv::Mat ProcessPyrLK(cv::Mat &frame_prev, cv::Mat &frame_current);

    Filter *filter_;
    cv::CascadeClassifier *classifier_;

    vector<cv::Point2f> points_[2];
    cv::Point2f point_;

    vector<uchar> status_;
    vector<float> err_;

    cv::Mat frame_prev_, frame_current_;

    bool is_yuv_;
    bool is_tracked_;
};

#endif // FILTER_H
