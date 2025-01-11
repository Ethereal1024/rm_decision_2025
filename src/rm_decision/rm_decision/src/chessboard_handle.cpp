#include "chessboard_handle.hpp"

using namespace RMDecision;

ChessboardHandle::ChessboardHandle(const chessboard_interfaces::msg::Chessboard& msg) : initialed(msg.initialed) {
    faction = static_cast<RMDecision::Faction>(msg.faction);
    robots = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Robot>>>();
    terrains = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Terrain>>>();
    architectures = std::make_shared<std::unordered_map<std::string, std::shared_ptr<Architecture>>>();

    for (const auto& robotMsg : msg.robots) {
        (*robots)[robotMsg.label] = std::make_shared<Robot>(robotMsg);
    }

    for (const auto& terrainMsg : msg.terrains) {
        (*terrains)[terrainMsg.label] = std::make_shared<Terrain>(terrainMsg);
    }

    for(const auto& architectureMsg : msg.architectures) {
        (*architectures)[architectureMsg.label] = std::make_shared<Architecture>(architectureMsg);
    }
}

inline std::shared_ptr<Robot> ChessboardHandle::friend_robot(const uint& id) {
    assert(initialed && "Chessboard uninitialized ERROR");
    std::string label = (faction == RED ? "R" + std::to_string(id) : "B" + std::to_string(id));
    return (*robots)[label];
}

inline std::shared_ptr<Robot> ChessboardHandle::enemy_robot(const uint& id) {
    assert(initialed && "Chessboard uninitialized ERROR");
    std::string label = (faction == RED ? "B" + std::to_string(id) : "R" + std::to_string(id));
    return (*robots)[label];
}

inline std::shared_ptr<Architecture> ChessboardHandle::friend_outpost() {
    assert(initialed && "Chessboard uninitialized ERROR");
    if (faction == RED) {
        return (*architectures)["Red_Outpost"];
    } else {
        return (*architectures)["Blue_Outpost"];
    }
}

inline std::shared_ptr<Architecture> ChessboardHandle::enemy_outpost() {
    assert(initialed && "Chessboard uninitialized ERROR");
    if (faction == BLUE) {
        return (*architectures)["Red_Outpost"];
    } else {
        return (*architectures)["Blue_Outpost"];
    }
}

inline std::shared_ptr<Architecture> ChessboardHandle::friend_base() {
    assert(initialed && "Chessboard uninitialized ERROR");
    if (faction == RED) {
        return (*architectures)["Red_Base"];
    } else {
        return (*architectures)["Blue_Base"];
    }
}

inline std::shared_ptr<Architecture> ChessboardHandle::enemy_base() {
    assert(initialed && "Chessboard uninitialized ERROR");
    if (faction == BLUE) {
        return (*architectures)["Red_Base"];
    } else {
        return (*architectures)["Blue_Base"];
    }
}

void ChessboardHandle::upgrate_from_message(const chessboard_interfaces::msg::Chessboard& msg) {
    assert(initialed && "Chessboard uninitialized ERROR");
    for (const auto& robotMsg : msg.robots) {
        auto& robot = (*robots)[robotMsg.label];
        robot->upgrate_from_message(robotMsg);
    }

    for (const auto& architectureMsg : msg.architectures) {
        auto& architecture = (*architectures)[architectureMsg.label];
        architecture->upgrate_from_message(architectureMsg);
    }
}

chessboard_interfaces::msg::Chessboard ChessboardHandle::to_message() {
    auto msg = chessboard_interfaces::msg::Chessboard();
    msg.faction = faction;
    msg.initialed = initialed;

    for (const auto& elem : *robots) {
        Robot& robot = *elem.second;
        auto robotMsg = robot.to_message();
        msg.robots.push_back(robotMsg);
    }

    for (const auto& elem : *terrains) {
        auto& terrain = *elem.second;
        auto terrainMsg = terrain.to_message();
        msg.terrains.push_back(terrainMsg);
    }

    for (const auto& elem : *architectures) {
        auto& architecture = *elem.second;
        auto architectureMsg = architecture.to_message();
        msg.architectures.push_back(architectureMsg);
    }
    return msg;
}
