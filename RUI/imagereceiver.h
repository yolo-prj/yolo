#ifndef IMAGERECEIVER_H
#define IMAGERECEIVER_H

#include <thread>
#include <memory>

#include "templatesingletone.h"
#include "imagereceivelistener.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YNetworkMessageListener.h>

#include <QDebug>

const int kBUFSIZE = (1024*64);

namespace  yolo {
class ImageReceiver : public TemplateSingleton<ImageReceiver>
{
public:
    ImageReceiver();
    ~ImageReceiver();


private:
    std::shared_ptr<YNetworkManager> manager_;
    std::shared_ptr<ImageReceiveListener> image_receive_listener_;

    int time_period_;

    class ImageMessageListener : public YNetworkMessageListener
    {
    public:
        virtual void onReceiveMessage(byte* data, uint length);

        void SetParent(ImageReceiver* parent) {
            parent_ = parent;
        }

    private:
        ImageReceiver* parent_;
    };

    std::shared_ptr<ImageMessageListener> image_message_listener_;

public:

    int StartReceiving();
    int StopReceiving();

    void RegisterListener(const std::shared_ptr<ImageReceiveListener>& interface);
    void UnregisterListener();

    void SetTimePeriod(int ms);

    void TransferImage(byte* data, uint length);
    bool ReceiveImageAsJpeg(byte* data, const uint length, cv::Mat *image);

    void SetNetworkManager(const std::shared_ptr<YNetworkManager>& manager);
};

}

#endif // IMAGERECEIVER_H
