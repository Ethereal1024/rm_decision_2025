#include "sensing_unit.hpp"

using namespace RMDecision;

void SensingUnit::init_chessboard(const Faction& faction) {
    chessboard_.faction = faction;
    init_map_declare<Robot, Robot>(
        RMDecision::DefaultInfo::robots, *chessboard_.robots);
    init_map_declare<std::vector<double>, Terrain>(
        RMDecision::DefaultInfo::terrains, *chessboard_.terrains, Terrain::array_to_terrain);
    init_map_declare<std::vector<double>, Architecture>(
        RMDecision::DefaultInfo::architecture, *chessboard_.architectures, Architecture::array_to_architecture);

    name_objects<Robot>(*chessboard_.robots);
    name_objects<Terrain>(*chessboard_.terrains);
    name_objects<Architecture>(*chessboard_.architectures);
    chessboard_.initialed = true;
}

SensingUnit::SensingUnit(const rclcpp::NodeOptions& options, const Faction& faction) : Node("observe_unit", options), chessboard_(faction) {
    init_chessboard(faction);
    all_robot_hp_sub_ = this->create_subscription<rm_decision_interfaces::msg::AllRobotHP>(
        "all_robot_hp", 10, std::bind(&SensingUnit::all_robot_hp_callback, this, std::placeholders::_1));

    friend_location_sub_ = this->create_subscription<rm_decision_interfaces::msg::FriendLocation>(
        "friend_location", 10, std::bind(&SensingUnit::friend_location_callback, this, std::placeholders::_1));

    tracking_pose_sub_ = this->create_subscription<geometry_msgs::msg::PointStamped>(
        "/tracker/enemypose", 10, std::bind(&SensingUnit::tracking_pose_callback, this, std::placeholders::_1));

    from_serial_sub_ = this->create_subscription<rm_decision_interfaces::msg::FromSerial>(
        "fromjudge", 10, std::bind(&SensingUnit::from_serial_callback, this, std::placeholders::_1));

    receive_serial_sub_ = this->create_subscription<rm_decision_interfaces::msg::ReceiveSerial>(
        "receive_serial", 10, std::bind(&SensingUnit::receive_serial_callback, this, std::placeholders::_1));

    robot_status_sub_ = this->create_subscription<rm_decision_interfaces::msg::RobotStatus>(
        "robot_status", 10, std::bind(&SensingUnit::robot_status_callback, this, std::placeholders::_1));

    target_sub_ = this->create_subscription<auto_aim_interfaces::msg::Target>(
        "/tracker/target", rclcpp::SensorDataQoS(), std::bind(&SensingUnit::target_callback, this, std::placeholders::_1));
}

void SensingUnit::all_robot_hp_callback(const rm_decision_interfaces::msg::AllRobotHP& msg) {
    assert(msg.color == chessboard_.faction && "Faction Maching ERROR");

    auto& robots = *chessboard_.robots;
    robots["R1"]->hp = msg.red_1_robot_hp;
    robots["R2"]->hp = msg.red_2_robot_hp;
    robots["R3"]->hp = msg.red_3_robot_hp;
    robots["R4"]->hp = msg.red_4_robot_hp;
    robots["R5"]->hp = msg.red_5_robot_hp;
    robots["R7"]->hp = msg.red_7_robot_hp;

    robots["B1"]->hp = msg.blue_1_robot_hp;
    robots["B2"]->hp = msg.blue_2_robot_hp;
    robots["B3"]->hp = msg.blue_3_robot_hp;
    robots["B4"]->hp = msg.blue_4_robot_hp;
    robots["B5"]->hp = msg.blue_5_robot_hp;
    robots["B7"]->hp = msg.blue_7_robot_hp;

    auto& architectures = *chessboard_.architectures;
    architectures["Red_Outpost"]->hp = msg.red_outpost_hp;
    architectures["Red_Base"]->hp = msg.red_base_hp;
    architectures["Blue_Outpost"]->hp = msg.blue_outpost_hp;
    architectures["Blue_Base"]->hp = msg.blue_base_hp;

    prism_.self.hp = chessboard_.friend_robot(prism_.self.id)->hp;
    prism_.track.hp = chessboard_.enemy_robot(prism_.track.id)->hp;
}

void SensingUnit::friend_location_callback(const rm_decision_interfaces::msg::FriendLocation& msg) {
    auto lambda = [&](uint id, double x, double y) {
        chessboard_.friend_robot(id)->pose = PlaneCoordinate(x, y).to_pose_stamped(this->now());
    };

    lambda(1, msg.hero_x, msg.hero_y);
    lambda(2, msg.engineer_x, msg.engineer_y);
    lambda(3, msg.standard_3_x, msg.standard_3_y);
    lambda(4, msg.standard_4_x, msg.standard_4_y);
    lambda(5, msg.standard_5_x, msg.standard_5_y);
}

void SensingUnit::from_serial_callback(const rm_decision_interfaces::msg::FromSerial& msg) {
    assert(msg.color == chessboard_.faction && "Faction Maching ERROR");

    uint self_hp, self_base, self_outpost, enemy_outpost_hp;

    if (msg.color == 1) {
        self_hp = msg.red_7;
        self_base = msg.red_base_hp;
        self_outpost = msg.red_outpost_hp;
        enemy_outpost_hp = msg.blue_outpost_hp;
    } else {
        self_hp = msg.blue_7;
        self_base = msg.blue_base_hp;
        self_outpost = msg.blue_outpost_hp;
        enemy_outpost_hp = msg.red_outpost_hp;
    }

    prism_.self.hp = self_hp;
    prism_.game.game_start = msg.gamestart;
    prism_.game.coins = msg.remaining_gold_coin;
    prism_.game.projectile_allowance = msg.projectile_allowance_17mm;
    prism_.call.target = PlaneCoordinate(msg.target_pos_x, msg.target_pos_y);
    prism_.call.is_called = true;

    chessboard_.friend_robot(prism_.self.id)->hp = self_hp;
    chessboard_.friend_base()->hp = self_base;
    chessboard_.friend_outpost()->hp = self_outpost;
    chessboard_.enemy_outpost()->hp = enemy_outpost_hp;
}

void SensingUnit::tracking_pose_callback(const geometry_msgs::msg::PointStamped::SharedPtr& msg) {
    prism_.track.pose.header.stamp = this->now();
    prism_.track.pose.header.frame_id = "map";
    prism_.track.pose.pose.position.x = msg->point.x;
    prism_.track.pose.pose.position.y = msg->point.y;
    prism_.track.pose.pose.position.z = msg->point.z;
}

void SensingUnit::target_callback(const auto_aim_interfaces::msg::Target& msg) {
    prism_.track.id = msg.armors_num;
    prism_.track.tracking = msg.tracking;
}
