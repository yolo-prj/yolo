#include "robotcontrolmanager.h"
#include "imagereceiver.h"
#include <memory>
#include <QtDebug>
#include <tuple>

using namespace std;
using namespace yolo;

namespace yolo {
RobotControlManager::RobotControlManager(QObject *parent) :
     QObject(parent)
{

}

RobotControlManager::~RobotControlManager()
{
    Stop();
}

void RobotControlManager::Initialize(const std::string config, const std::string msgformat)
{
    parser_ = std::make_shared<YMessageFormatParser>(msgformat);
    parser_->parse();

    manager_ = make_shared<YNetworkManager>(config);
    listener_ = make_shared<NetworkEventListener>();
    listener_->SetParent(this);
    manager_->addNetworkMessageListener(1, listener_.get());

    hbmanager_ = make_shared<YHeartbeatManager>(manager_.get());
    manager_->setHeartbeatManager(hbmanager_.get());
    //hbmanager_->setConnectionLostListener(connection_listener_.get());

    connection_listener_ = make_shared<NetworkConnectionListener>();
    connection_listener_->SetParent(this);
    //manager_->addNetworkMessageListener(100, connection_listener_.get());
    hbmanager_->setConnectionLostListener(connection_listener_.get());

    event_info_listener_ = make_shared<RobotEventInfoListener>();
    event_info_listener_->SetParent(this);
    manager_->addNetworkMessageListener(1001, event_info_listener_.get());

    debug_info_listener_ = make_shared<RobotDebugInfoListener>();
    debug_info_listener_->SetParent(this);
    manager_->addNetworkMessageListener(1300, debug_info_listener_.get());

    ImageReceiver::GetInstance().SetNetworkManager(manager_);
}

bool RobotControlManager::Start()
{
    qDebug() << "Start Network Manager";

    if(!manager_->start())
    {
        qWarning( "network error!!" );
        return false;
    }

    hbmanager_->startHeartbeat();


    return true;
}

void RobotControlManager::Stop()
{
    hbmanager_->stopHeartbeat();
    manager_->stop();
    robot_controllers_.clear();
    qDebug() << "Stop Network Manager";
}

void RobotControlManager::AddController(int handler, const std::string addr, unsigned int port)
{
    qDebug() << "AddController() : " << handler;

    if ( robot_controllers_.find(handler) == robot_controllers_.end() ) {
        RobotController controller(handler, addr, port);
        robot_controllers_[handler] = controller;
    } else {
        qDebug() <<  "Duplicated Controller : " << handler;
    }
}

void RobotControlManager::RemoveController(int handler)
{
    qDebug() << "RemoveController() : " << handler;

    robot_controllers_.erase(handler);
}

void RobotControlManager::ChangeMode(int handler, RobotMode mode)
{
    qDebug() << "ChangeMode() : " << handler;

    if ( robot_controllers_.find(handler) == robot_controllers_.end() ) {
        qDebug() <<  "Duplicated Controller : " << handler;
    } else {
        RobotController& controller = robot_controllers_[handler];
        controller.ChangeMode(mode);
    }
}

int RobotControlManager::SendFirstConfig(const int handler)
{
    YMessage msg;

    msg.set("id", handler);
    msg.set("udp_port", 9004);
    msg.set("height", 240);
    msg.set("width", 320);
    msg.set("sampling_rate", 20);
    msg.set("tilt_angle", 50);
    msg.set("pan_angle", 50);
    msg.setOpcode(200);

    byte* data;
    uint len;
    data = msg.serialize(len);

    cout << "handler : " << handler << ", opcode " << 200 << ", length : " << len << endl;

    manager_->sendTcpPacket(handler, 200, data, len);

}

int RobotControlManager::SendDebugCommand(const int handler, const std::string debug_command, int state)
{
    YMessage msg;

    msg.set("id", handler);
    msg.set("debug", debug_command);
    msg.set("state", state);
    msg.setOpcode(300);

    byte* data;
    uint len;
    data = msg.serialize(len);

    cout << "handler : " << handler << ", opcode " << 200 << ", length : " << len << endl;

    manager_->sendTcpPacket(handler, 300, data, len);
}

int RobotControlManager::SendCommand(const int handler, const int opcode, std::tuple<std::string, std::string, int>&& command)
{
    qDebug() << "SendCommand() : " << handler;

    if ( robot_controllers_.find(handler) == robot_controllers_.end() ) {
        qDebug() <<  "no controller : " << handler;
    } else {
        YMessage msg;

        std::string id;
        std::string cmd;
        int state;
        std::tie(id, cmd, state) = command;

        msg.set("id", id);
        msg.set("command", cmd);
        msg.set("state", state);
        msg.setOpcode(opcode);

        byte* data;
        uint len;
        data = msg.serialize(len);

        cout << "handler : " << handler << ", opcode " << opcode << ", length : " << len << endl;

        manager_->sendTcpPacket(handler, opcode, data, len);
    }

    return 0;
}

int RobotControlManager::SelectRobot(const int handler)
{
    qDebug() << "SelectRobot() : " << handler;
    selected_robot_ = handler;

    //TBD : Need to implent robot selection sequence
}

std::tuple<int, std::string, unsigned int, RobotMode>
RobotControlManager::GetRobotInfo(int handler)
{
    if ( robot_controllers_.find(handler) == robot_controllers_.end() ) {
      qDebug() <<  "no controller : " << handler;
    } else {
        RobotController& controller = robot_controllers_[handler];

        return std::make_tuple(controller.handle(),
                                         controller.addr(),
                                         controller.port(),
                                         controller.mode());
    }
}

int RobotControlManager::FindRobotHandler(const std::string addr)
{
    for(auto item : robot_controllers_)
    {
        auto controller = static_cast<RobotController>(item.second);
        if(controller.addr().compare(addr) == 0) {
            return item.first;
        }
    }

    return -1;
}

void RobotControlManager::ProcessRobotDisconncetion(int handler)
{
    qDebug() << "ProcessRobotDisconncetion()";
    RemoveController(handler);
}

void RobotControlManager::RegisterListener(const std::shared_ptr<RobotEventListener>& listener)
{
    event_listener_ = listener;
}

void RobotControlManager::UnregisterListener()
{
    event_listener_.reset();
}

void RobotControlManager::NetworkEventListener::onReceiveMessage(byte* data, uint length)
{
    qDebug() << "message received : do noting, it's only for tcp connection";
}

void RobotControlManager::NetworkEventListener::onTcpClientConnected(int handle, string addr, ushort port)
{
    qDebug() << "tcp client connected : " << handle << ", "<< addr.c_str() << ", " << port;

    robot_controller_manager_->AddController(handle, addr, port);
    robot_controller_manager_->event_listener_->OnRobotConnected(handle);

    robot_controller_manager_->SendFirstConfig(handle);
}

void RobotControlManager::NetworkEventListener::onTcpClientDisconnected(int handle, string addr, ushort port)
{
    qDebug() << "tcp client disconnected : " << handle << ", " << addr.c_str() << ", " << port;

    robot_controller_manager_->RemoveController(handle);
    robot_controller_manager_->event_listener_->OnRobotDisconnected(handle);
}

void RobotControlManager::NetworkConnectionListener::connectionLost(string addr, ushort port)
{
    qDebug() << "connection Lost!!! : " << addr.c_str() << ", port : " << port << endl;

    int handler = robot_controller_manager_->FindRobotHandler(addr);
    if( handler == -1)
    {
        qDebug() << "No matched controllers";
        return;
    }

    //robot_controller_manager_->ProcessRobotDisconncetion(handler);
    robot_controller_manager_->event_listener_->OnRobotInvalidDisconnected(handler);
}

void RobotControlManager::NetworkConnectionListener::reconnected(string addr, ushort port)
{
    qDebug() << "reconnected!!! : " << addr.c_str() << ", port : " << port << endl;

    int handler = robot_controller_manager_->FindRobotHandler(addr);
    if( handler == -1)
    {
        qDebug() << "No matched controllers";
        return;
    }

    //robot_controller_manager_->ProcessRobotDisconncetion(handler);
    //robot_controller_manager_->event_listener_->OnRobotInvalidDisconnected(handler);
}

void RobotControlManager::RobotEventInfoListener::onReceiveMessage(byte* data, uint length)
{
    qDebug() << "RobotEventInfoListener::onReceiveMessage()";

    YMessage msg;
    msg.deserialize(data, length);

    std::string str = msg.getString("id");
    int id = std::stoi(str);
    std::string event = msg.getString("event");
    int state = msg.getInt("state");

    if(robot_controller_manager_->robot_controllers_.find(id) == robot_controller_manager_->robot_controllers_.end()) {
        qDebug() << "Invalid Id : " << id;
    } else {

        qDebug() << "event : " << event.c_str();

        if(event.compare("mode_changed") == 0) {
            RobotMode mode = static_cast<RobotMode>(state);
            robot_controller_manager_->ChangeMode(id, mode);
            robot_controller_manager_->event_listener_->OnRobotModeChanged(id, mode);
        } else if(event.compare("error") == 0) {
            robot_controller_manager_->event_listener_->OnRobotErrorEventReceived(id, state);
        }
        else {

        }
    }
}

void RobotControlManager::RobotDebugInfoListener::onReceiveMessage(byte* data, uint length)
{
    qDebug() << "obotDebugInfoListener::onReceiveMessage";
    YMessage msg;
    msg.deserialize(data, length);

    int id = msg.getInt("id");
    std::string debug_info = msg.getString("debug_info");
    int state = msg.getInt("state");

    robot_controller_manager_->event_listener_->OnRobotDebugInfoReceived(id, debug_info, state);
}


}

