//
//	hw/nvm/enums.h
//		part of the CORE kernel belonging to the H-OS project
//

#pragma once

enum nvm_controller_power_scopes {
	nvm_cps_not_reported = 0,
	nvm_cps_controlller_scope = 0b1,
	nvm_cps_domain_scope = 0b10,
	nvm_cps_nvm_subsystem_scope = 0b11
} nvm_controller_power_scopes;

enum nvm_controller_css {
	nvm_ccss_nvm_command_set = 0,
	nvm_ccss_reserved_command_set = 0b101,
	nvm_ccss_reserved2_command_set = 0b001,
	nvm_ccss_all_command_sets_supported = 0b110,
	nvm_ccss_admin_command_set = 0b111
} nvm_controller_css;

enum nvm_arbitration_mechanisms {
	nvm_arbit_round_robin = 0,
	nvm_arbit_weighted_round_robin = 0b001,
	nvm_arbit_reserved = 0b010,
	nvm_arbit_reserved2 = 0b110,
	nvm_arbit_vendor_specific = 0b111
} nvm_arbitration_mechanisms;

enum nvm_shutdown_notifications {
	nvm_shutdown_no_notification = 0,
	nvm_shutdown_notification_normal = 0b01,
	nvm_shutdown_notification_abrupt = 0b10
} nvm_shutdown_notifications;

enum nvm_sutdown_status {
	nvm_shutdown_normal_operation = 0,
	nvm_shutdown_processing = 0b01,
	nvm_shutdown_complete = 0b10
} nvm_shutdown_status;