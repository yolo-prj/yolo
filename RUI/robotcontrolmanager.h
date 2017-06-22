#ifndef ROBOTCONTROLMANAGER_H
#define ROBOTCONTROLMANAGER_H

#include <QObject>
#include <memory>
#include <string>
#include <yolo/YFramework/YNetworkManager.h>
#include <yolo/YFramework/YNetworkMessageListener.h>
#include <yolo/YFramework/YConnectionLostListener.h>
#include <yolo/YFramework/YHeartbeatManager.h>
#include <yolo/YFramework/YMessageFormatParser.h>
#include <QtDebug>
#include "robotcontroller.h"
#include "roboteventlistener.h"
#include "templatesingletone.h"
#include "robotmode.h"

#include <map>
#include <tuple>


namespace yolo {

class RobotControlManager : public QObject, public TemplateSingleton<RobotControlManager>
{
    Q_OBJECT
public:
    explicit RobotControlManager(QObject *parent = nullptr);
    ~RobotControlManager();

    class NetworkEventListener : public YNetworkMessageListener
    {
    public:
        virtual void onReceiveMessage(byte* data, uint length);
        virtual void onTcpClientConnected(int handle, string addr, ushort port);
        virtual void onTcpClientDisconnected(int handle, string addr, ushort port);

        void SetParent(RobotControlManager* robot_controller_manager)
        {
            robot_controller_manager_ = robot_controller_manager;
        }

    private:
        RobotControlManager* robot_controller_manager_;
    };

    class NetworkConnectionListener : public YNetworkMessageListener, public YConnectionLostListener
    {
    public:
        virtual void connectionLost(string addr, ushort port);
        void SetParent(RobotControlManager* robot_controller_manager)
        {
            robot_controller_manager_ = robot_controller_manager;
        }

        virtual void onReceiveMessage(byte* data, uint length) {}

    private:
        RobotControlManager* robot_controller_manager_;
    };

    class RobotEventInfoListener : public YNetworkMessageListener
    {
    public:
        virtual void onReceiveMessage(byte* data, uint length);

        void SetParent(RobotControlManager* robot_controller_manager)
        {
            robot_controller_manager_ = robot_controller_manager;
        }

    private:
        RobotControlManager* robot_controller_manager_;
    };

    class RobotDebugInfoListener : public YNetworkMessageListener
    {
    public:
        virtual void onReceiveMessage(byte* data, uint length);

        void SetParent(RobotControlManager* robot_controller_manager)
        {
            robot_controller_manager_ = robot_controller_manager;
        }

    private:
        RobotControlManager* robot_controller_manager_;
    };

signals:

public slots:

private:
    std::shared_ptr<YNetworkManager> manager_;

    std::shared_ptr<NetworkEventListener> listener_;
    std::shared_ptr<NetworkConnectionListener> connection_listener_;
    std::shared_ptr<RobotEventInfoListener> event_info_listener_;
    std::shared_ptr<RobotDebugInfoListener> debug_info_listener_;

    std::shared_ptr<RobotEventListener> event_listener_;

    std::map<int, RobotController> robot_controllers_;
    int selected_robot_;

    std::shared_ptr<YMessageFormatParser> parser_;

    int FindRobotHandler(const std::string addr);

    void AddController(int handler, const std::string addr, unsigned int port);
    void RemoveController(int handler);
    void ProcessRobotDisconncetion(int handler);

public :
    void Initialize(const std::string config, const std::string msgformat);
    bool Start();
    void Stop();

    int SendFirstConfig(const int handler);
    int SendCommand(const int handler, const int opcode, std::tuple<std::string, std::string, int>&& command);
    int SelectRobot(const int handler);

    std::tuple<int, std::string, unsigned int, RobotMode> GetRobotInfo(int handler);

    void RegisterListener(const std::shared_ptr<RobotEventListener>& listener);
    void UnregisterListener();
};

}


#endif // ROBOTCONTROLMANAGER_H
