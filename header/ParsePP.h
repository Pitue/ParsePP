#ifndef __PARSE_PP_H__
#define __PARSE_PP_H__

#include <vector>
#include <string>
#include <filesystem>
#include <iostream>

#include <cstring>
#include <cstdint>

namespace ParsePP {
	constexpr char DEFAULT_PARAMETER = 0xFU;

	struct Pair {
		char switch_;
		char* val_;
		Pair(char _switch, const char* value) : switch_{ _switch }, val_{ nullptr } {
			size_t l = std::strlen(value);
			val_ = new char[l];
			std::memcpy(val_, value, l);
		}
		~Pair() {
			delete[] val_;
		}
	};
	class Parse {
		std::vector<Pair*> data_;
		std::filesystem::path exe_;
		std::string required_, allowed_;

		Pair* FindPair(char _switch) {
			for (auto& x : data_) {
				if (x->switch_ == _switch) {
					return x;
				}
			}
			return nullptr;
		}
	public:
		Parse() {}
		~Parse() {
			for (auto& x : data_) {
				delete x;
			}
		}

		void LoadRequired(const std::string& req) {
			required_ = req;
		}
		void LoadAllowed(const std::string& al) {
			allowed_ = al;
		}
		void LoadArgs(int argc, const char** argv) {
			exe_ = argv[0];
			if (argc > 1) {
				if (argc % 2 == 0) {
					data_.push_back(new Pair(DEFAULT_PARAMETER, argv[1]));
				}
				for (int i = 2; i < argc; i += 2) {
					if (argv[i][0] != '/' && argv[i][0] != '-') {
						std::cerr << (std::format("Can't find specifier \"{}\"", argv[i][0]));
						exit(1);
					}
					else if (i == argc - 1) {
						std::cerr << ("To few arguments in parameter list!");
						exit(1);
					}
					if (allowed_.length() != 0 && allowed_.find(argv[i][1]) == std::string::npos) {
						std::cerr << (std::format("{} is not an allowed switch!"));
						exit(1);
					}
					data_.push_back(new Pair(argv[i][1], argv[i + 1]));
				}
			}
			int cnt = 0;
			for (auto& x : data_) {
				if (required_.find(x->switch_) != std::string::npos) cnt++;
			}
			if (cnt != required_.length()) {
				std::cerr << "Too few arguments in parameter list! Required: \n";
				for (auto& x : required_) {
					std::cerr << "-" << x << "\n";
				}
				exit(1);
			}
		}

		const char* GetArgumentRaw(char _switch) {
			Pair* t = FindPair(_switch);
			if (!t) throw std::runtime_error(std::format("Couldn't find switch \"{}\"", _switch));
			return t->val_;
		}

		template <typename T>
		bool GetArgument(char _switch, T* ret) {
			throw std::runtime_error("Called \"GetArgument\" with ambiguos type!");
			return false;
		}

		template <>
		bool GetArgument<int>(char _switch, int* ret) {
			Pair* t = FindPair(_switch);
			if (!t) return false;
			*ret = std::stoi(t->val_);
			return true;
		}

		template <>
		bool GetArgument<unsigned long>(char _switch, unsigned long* val) {
			Pair* t = FindPair(_switch);
			if (!t) return false;
			*val = std::stoul(t->val_);
			return true;
		}

		template <>
		bool GetArgument<std::string>(char _switch, std::string* ret) {
			Pair* t = FindPair(_switch);
			if (!t) return false;
			*ret = t->val_;
			return true;
		}

	};
}

#endif