//
//  init.cpp
//      part of the H-OS project
//

#ifdef H_OS_LIB_K_MANAGEMENT_H
	#ifndef H_OS_LIB_K_MANAGEMENT_CPP
		#define H_OS_LIB_K_MANAGEMENT_CPP

		void init() {
			//  1)  initialize font and flush screen
			//  2)  initialize memory
			//  3)  parse commandline arguments
			//  4)  initialize GDT



			//  1)  initialize font ...
			font::init();
			screen::flush();


			out::printl("starting system initialization");

			//mem::map::display();

			//	cpu info
			//hw::cpu::init();
			hw::cpu::vids::get();
			output << "detected cpu: " << hw::cpu::vendors[hw::cpu::vendor].name << out::endl;
			output << "vid: " << (const char*)hw::cpu::vid << out::endl;

			//  initialize memory
			size_t code;
			if ((code = mem::init() != manage::codes::mem::ok)) {
				panic(code);
			}

			//  parse cmdline arguments
			if (manage::parse_cmd() == manage::codes::parse::cmdargs_null) {
				output << out::change(out::col::yellow) << "WARNING" << out::change() << ": no command line arguments given -> setting defaults" << out::endl;
			}

			//  initialize interrupts

			//  initialize hardware

			output << out::change(out::col::green) << "kernel initialized" << out::change() << out::endl;

			output << out::change(out::col::green) << "stating H-OS kernel version " << KERNEL_VERSION << out::change() << out::endl;
		}

		void panic(size_t errorcode) {
			//  once here, never going out

			//  report stuff and hang the system -> no going back

			output << out::change(out::col::critical) << "PANIC: CRITICAL ERROR" << out::change(out::col::white) << ":\t";
			switch (errorcode) {
				case manage::codes::mem::memmap_not_found: {
					out::printl("memory map was not found");
					break;
				}
				case manage::codes::mem::cannot_allocate_memory_for_kernel_heap: {
					out::printl("cannot allocate memory for kernel heap");
					break;
				}
				case manage::codes::mem::unsupported_paging_mode: {
					out::printl("unsupported paging mode");
					break;
				}
				case manage::codes::mem::cannot_locate_kernel_entry: {
					out::printl("somehow kernel memory map entry has not returned (was not found)");
					break;
				}


				default: {
					out::printl("unknown error");
					break;
				}
			}

			output << " ->\t" << out::change(out::col::critical);

			switch (manage::state) {
				case manage::states::init::memory: {
					output << "FAILED TO INITIALIZE MEMORY" << out::change(out::col::white) << " ->\t" << out::change(out::col::critical) << "INITIALIZATION TERMINATED" << out::endl;
					break;
				}
				default: {
					output << "UNKNOWN FAILURE" << out::change() << " ->\t" << out::change(out::col::critical) << "INITIALIZATION TERMINATED" << out::endl;
					break;
				}
			}

			output << "Shutting down..." << out::change() << out::endl;
		}


		u32 manage::parse_cmd() {

			if (manage::args.response == nullptr) {
				return manage::codes::parse::cmdargs_null;
			}

			const char* cmd = manage::args.response->kernel_file->cmdline;
			u32 len = string::strlen(cmd);
			u32 wstart = 0;
			u32 wlen = 0;

			vector<string> args;

			for (u32 i = 0; i < len; i++) {
				switch (cmd[i]) {
					case ' ': case '\t': case '\n': {
						if (wlen > 0) {
							string arg = string(wlen, true);
							for (u32 ii = 0; ii < wlen; ii++) {
								arg[ii] = cmd[wstart+ii];
							}
							args.push().take_over(arg);
						}
						wlen = 0;
						wstart = 0;
						break;
					}
					case '\0': {
						//  important
						break;
					}
					default: {
						if (wlen == 0) {
							wstart = i;
						}
						wlen++;
						break;
					}
				}
			}
			if (wlen != 0) {
				string arg = string(wlen, true);
				for (u32 ii = 0; ii < wlen; ii++) {
					arg[ii] = cmd[wstart+ii];
				}
				args.push().take_over(arg);
				wlen = 0;
				wstart = 0;
			}

			auto error = [&args](const char* msg, u32 i, bool for_arg = false) {
				output << out::change(out::col::red) << "ERROR | cmd" << out::change() << ":\t" << msg;
				if (for_arg) {
					output << " for option \'" << args[i] << '\'';
				}
				output << ", skipping" << out::endl;
			};

			auto unrecognized_value = [&args](string& value, string* option = nullptr) {
				output << out::change(out::col::red) << "ERROR | cmd: " << out::change() << ":\t";
				output << "value \'" << value << "\' not recognized";
				if (option != nullptr) {
					output << " for option \'" << *option << '\'';
				}
				output << ", skipping" << out::endl;
			};

			for (u32 i = 0; i < args.size(); i++) {
				if (args[i] == "-vocality") {
					if (++i < args.size()) {
						if (args[i][0] == '-') {
							error("expected value", i, true);
							continue;
						}
						if (args[i] == "stfu") {
							manage::vocal::level = manage::vocal::levels::stfu;
						} else if (args[i] == "quiet") {
							manage::vocal::level = manage::vocal::levels::quiet_please;
						} else if (args[i] == "normal") {
							manage::vocal::level = manage::vocal::levels::normal;
						} else if (args[i] == "vocal") {
							manage::vocal::level = manage::vocal::levels::vocal;
						} else if (args[i] == "everything") {
							manage::vocal::level = manage::vocal::levels::report_everything;
						} else {
							unrecognized_value(args[i], &args[i-1]);
						}
					} else {
						error("missing value", i, true);
					}
				}

				else {
					output << out::change(out::col::red) << "ERROR | cmd: " << out::change() << ":\toption \'" << args[i] << "\' not recognized, skipping" << out::endl;
				}
			}


			return manage::codes::parse::ok;
		}


	#else
		#warning k_management.cpp already included
	#endif

#else
	#error k_management.cpp: k_management.h not included
#endif