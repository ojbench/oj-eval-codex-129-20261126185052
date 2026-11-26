#include <cctype>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

class Arguments {
    std::string_view remaining_;

public:
    explicit Arguments(std::string_view line) : remaining_(line) {}

    std::string_view rest() {
        while (!remaining_.empty() &&
               std::isspace(static_cast<unsigned char>(remaining_.front()))) {
            remaining_.remove_prefix(1);
        }
        while (!remaining_.empty() &&
               std::isspace(static_cast<unsigned char>(remaining_.back()))) {
            remaining_.remove_suffix(1);
        }
        return remaining_;
    }

    std::string word() {
        rest();
        std::size_t length = 0;
        while (length < remaining_.size() &&
               !std::isspace(static_cast<unsigned char>(remaining_[length]))) {
            ++length;
        }
        std::string result(remaining_.substr(0, length));
        remaining_.remove_prefix(length);
        return result;
    }
};

bool valid_name(std::string_view name) {
    if (name.empty() || !(std::isalpha(static_cast<unsigned char>(name[0])) ||
                          name[0] == '_')) {
        return false;
    }
    for (unsigned char ch : name) {
        if (!std::isalnum(ch) && ch != '_') return false;
    }
    return true;
}

struct Variable {
    std::size_t depth = 0;
    bool is_string = false;
    int number = 0;
    std::string text;
};

bool parse_value(std::string_view literal, Variable& value) {
    if (value.is_string) {
        if (literal.size() < 2 || literal.front() != '"' || literal.back() != '"') {
            return false;
        }
        value.text = literal.substr(1, literal.size() - 2);
        return true;
    }
    if (literal.empty()) return false;
    bool negative = false;
    if (literal.front() == '-' || literal.front() == '+') {
        negative = literal.front() == '-';
        literal.remove_prefix(1);
    }
    if (literal.empty()) return false;
    std::int64_t number = 0;
    const std::int64_t limit = std::int64_t(std::numeric_limits<int>::max()) + negative;
    for (char ch : literal) {
        if (ch < '0' || ch > '9') return false;
        number = number * 10 + (ch - '0');
        if (number > limit) return false;
    }
    value.number = static_cast<int>(negative ? -number : number);
    return true;
}

class Simulator {
    using Bindings = std::unordered_map<std::string, std::vector<Variable>>;
    Bindings bindings_;
    // Hash-table element pointers survive rehashing; each scope owns its declarations.
    std::vector<std::vector<Bindings::value_type*>> scopes_{1};

    Variable* find(const std::string& name) {
        auto it = bindings_.find(name);
        return it == bindings_.end() ? nullptr : &it->second.back();
    }

public:
    bool execute(std::string_view line) {
        Arguments args(line);
        const std::string command = args.word();
        if (command == "Indent") {
            scopes_.emplace_back();
        } else if (command == "Dedent") {
            if (scopes_.size() == 1) return false;
            for (auto* entry : scopes_.back()) {
                entry->second.pop_back();
                if (entry->second.empty()) bindings_.erase(entry->first);
            }
            scopes_.pop_back();
        } else if (command == "Declare") {
            const std::string type = args.word();
            const std::string name = args.word();
            if ((type != "int" && type != "string") || !valid_name(name)) return false;
            Variable value;
            value.depth = scopes_.size() - 1;
            value.is_string = type == "string";
            const Variable* previous = find(name);
            if (previous && previous->depth == value.depth) return false;
            if (!parse_value(args.rest(), value)) return false;
            auto [it, inserted] = bindings_.try_emplace(name);
            it->second.push_back(std::move(value));
            scopes_.back().push_back(&*it);
        } else if (command == "Add") {
            Variable* result = find(args.word());
            Variable* left = find(args.word());
            Variable* right = find(args.word());
            if (!result || !left || !right || result->is_string != left->is_string ||
                result->is_string != right->is_string) return false;
            if (result->is_string) {
                // Evaluate the sum before assignment, including when operands alias.
                result->text = left->text + right->text;
            } else {
                result->number = left->number + right->number;
            }
        } else if (command == "SelfAdd") {
            Variable* target = find(args.word());
            if (!target) return false;
            Variable value;
            value.is_string = target->is_string;
            if (!parse_value(args.rest(), value)) return false;
            if (target->is_string) target->text += value.text;
            else target->number += value.number;
        } else if (command == "Print") {
            const std::string name = args.word();
            const Variable* value = find(name);
            if (!value) return false;
            std::cout << name << ':';
            if (value->is_string) std::cout << value->text;
            else std::cout << value->number;
            std::cout << '\n';
        } else {
            return false;
        }
        return true;
    }
};

}  // namespace

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    int count;
    if (!(std::cin >> count)) return 0;
    std::string line;
    std::getline(std::cin, line);
    Simulator simulator;
    for (int i = 0; i < count && std::getline(std::cin, line); ++i) {
        if (!simulator.execute(line)) std::cout << "Invalid operation\n";
    }
}
