// Copyright 2017 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef JOINT_TRAJECTORY_COMMAND_BROADCASTER__JOINT_TRAJECTORY_COMMAND_BROADCASTER_HPP_
#define JOINT_TRAJECTORY_COMMAND_BROADCASTER__JOINT_TRAJECTORY_COMMAND_BROADCASTER_HPP_

#include <joint_trajectory_command_broadcaster/visibility_control.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "control_msgs/msg/dynamic_joint_state.hpp"
#include "controller_interface/controller_interface.hpp"
#include \
  <om_joint_trajectory_command_broadcaster/joint_trajectory_command_broadcaster_parameters.hpp>
#include "realtime_tools/realtime_publisher.hpp"
#include "sensor_msgs/msg/joint_state.hpp"
#include "urdf/model.h"
#include "trajectory_msgs/msg/joint_trajectory.hpp"
#include "std_msgs/msg/bool.hpp"

namespace joint_trajectory_command_broadcaster
{
/**
 * \brief Joint Trajectory Command Broadcaster for leader-follower control in a ros2_control system.
 *
 * JointStateBroadcaster publishes state interfaces from ros2_control as ROS messages.
 * There is a possibility to publish all available states (typical use), or only specific ones.
 * The latter is, for example, used when hardware provides multiple measurement sources for some
 * of its states, e.g., position.
 * It is possible to define a mapping of measurements
 * from different sources stored in custom interfaces to standard dynamic names in JointState
 * message.
 * If "joints" or "interfaces" parameter is empty, all available states are published.
 *
 * \param use_local_topics Flag to publish topics in local namespace.
 * \param joints Names of the joints to publish.
 * \param interfaces Names of interfaces to publish.
 * \param map_interface_to_joint_state.{HW_IF_POSITION|HW_IF_VELOCITY|HW_IF_EFFORT} mapping
 * between custom interface names and standard names in sensor_msgs::msg::JointState message.
 *
 * Publishes to:
 * - \b joint_states (sensor_msgs::msg::JointState): Joint states related to movement
 * (position, velocity, effort).
 * - \b dynamic_joint_states (control_msgs::msg::DynamicJointState): Joint states regardless of
 * its interface type.
 */
class JointTrajectoryCommandBroadcaster : public controller_interface::ControllerInterface
{
public:
  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  JointTrajectoryCommandBroadcaster();

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::InterfaceConfiguration command_interface_configuration() const override;

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::InterfaceConfiguration state_interface_configuration() const override;

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::return_type update(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::CallbackReturn on_init() override;

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::CallbackReturn on_configure(
    const rclcpp_lifecycle::State & previous_state) override;

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  JOINT_TRAJECTORY_COMMAND_BROADCASTER_PUBLIC
  controller_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

protected:
  bool init_joint_data();
  void init_joint_trajectory_msg();
  bool use_all_available_interfaces() const;
  void collision_callback(const std_msgs::msg::Bool::SharedPtr msg);

protected:
  // Optional parameters
  std::shared_ptr<ParamListener> param_listener_;
  Params params_;
  std::unordered_map<std::string, std::string> map_interface_to_joint_state_;

  std::vector<std::string> joint_names_;
  std::vector<double> joint_offsets_;
  std::shared_ptr<rclcpp::Publisher<trajectory_msgs::msg::JointTrajectory>>
  joint_trajectory_publisher_;
  std::shared_ptr<realtime_tools::RealtimePublisher<trajectory_msgs::msg::JointTrajectory>>
  realtime_joint_trajectory_publisher_;

  std::unordered_map<std::string, std::unordered_map<std::string, double>> name_if_value_mapping_;

  urdf::Model model_;
  bool is_model_loaded_ = false;

  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr collision_flag_sub_;
  std::atomic_bool collision_detected_{false};
};

}  // namespace joint_trajectory_command_broadcaster

#endif  // JOINT_TRAJECTORY_COMMAND_BROADCASTER__JOINT_TRAJECTORY_COMMAND_BROADCASTER_HPP_
