#include "navigator/navigator.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_components/component_manager.hpp"

int main(int argc, char **argv) {
    rclcpp::init(argc, argv);
    auto exec = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();
    auto node = std::make_shared<Navigator>(rclcpp::NodeOptions());
    exec->add_node(node);
    exec->spin();
    rclcpp::shutdown();
    return 0;
}
