#pragma once

#include <chrono>
#include <future>
#include <string>
#include <unordered_map>
#include <vector>

#include "geometry_msgs/msg/point_stamped.hpp"
#include "geometry_msgs/msg/pose_stamped.hpp"
#include "nav2_msgs/action/navigate_to_pose.hpp"
#include "rclcpp_action/client_goal_handle.hpp"
#include "rm_decision_interfaces/msg/prism.hpp"
#include "rm_decision_defs.hpp"

namespace RMDecision {
class Self {
public:
    uint id;
    uint hp;
    uint ammo;
    uint shooter_heat;
    PoseStamped pose;
};

class Track {
public:
    bool tracking;
    uint id;
    uint hp;
    PoseStamped pose;
};

class Game {
public:
    bool game_start;
    bool buy_ammo_ordered;
    uint coins;
    uint projectile_allowance;
};

class Call {
public:
    PlaneCoordinate target;
    bool is_called;
};

class Prism {
public:
    Prism(){}
    Prism(const rm_decision_interfaces::msg::Prism& msg);

    std::unordered_map<std::string, double> parameters;
    Self self;
    Track track;
    Game game;
    Call call;

    void upgrade_from_message(const rm_decision_interfaces::msg::Prism& msg);

    rm_decision_interfaces::msg::Prism to_message();
};
}  // namespace RMDecision
