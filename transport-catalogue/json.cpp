#include "json.h"

using namespace std;

namespace json {

    namespace {

        Node LoadNode(istream& input);

        Node LoadBool(std::istream& input) {
            char c;
            if (!(input >> c)) {
                throw ParsingError("Unexpected end of input");
            }
            if (c == 't') {
                if (!(input >> c) || c != 'r' || !(input >> c) || c != 'u' || !(input >> c) || c != 'e') {
                    throw ParsingError("Expected 'true', but found '" + std::string(1, c) + "'");
                }
                return Node(true);
            } else if (c == 'f') {
                if (!(input >> c) || c != 'a' || !(input >> c) || c != 'l' || !(input >> c) || c != 's' || !(input >> c) || c != 'e') {
                    throw ParsingError("Expected 'false', but found '" + std::string(1, c) + "'");
                }
                return Node(false);
            } else {
                throw ParsingError("Expected boolean value, but found '" + std::string(1, c) + "'");
            }
        }

        Node LoadNull(std::istream& input) {
            char c;
            if (!(input >> c) || c != 'n' || !(input >> c) || c != 'u' || !(input >> c) || c != 'l' || !(input >> c) || c != 'l') {
                throw ParsingError("Expected 'null', but found '" + std::string(1, c) + "'");
            }
            return Node(nullptr);
        }

        Node LoadNumber(std::istream& input) {
            using namespace std::literals;

            std::string parsed_num;

            // Считывает в parsed_num очередной символ из input
            auto read_char = [&parsed_num, &input] {
                parsed_num += static_cast<char>(input.get());
                if (!input) {
                    throw ParsingError("Failed to read number from stream"s);
                }
            };

            // Считывает одну или более цифр в parsed_num из input
            auto read_digits = [&input, read_char] {
                if (!std::isdigit(input.peek())) {
                    throw ParsingError("A digit is expected"s);
                }
                while (std::isdigit(input.peek())) {
                    read_char();
                }
            };

            if (input.peek() == '-') {
                read_char();
            }
            // Парсим целую часть числа
            if (input.peek() == '0') {
                read_char();
                // После 0 в JSON не могут идти другие цифры
            } else {
                read_digits();
            }

            bool is_int = true;
            // Парсим дробную часть числа
            if (input.peek() == '.') {
                read_char();
                read_digits();
                is_int = false;
            }

            // Парсим экспоненциальную часть числа
            if (int ch = input.peek(); ch == 'e' || ch == 'E') {
                read_char();
                if (ch = input.peek(); ch == '+' || ch == '-') {
                    read_char();
                }
                read_digits();
                is_int = false;
            }

            try {
                if (is_int) {
                    // Сначала пробуем преобразовать строку в int
                    try {
                        return std::stoi(parsed_num);
                    } catch (...) {
                        // В случае неудачи, например, при переполнении,
                        // код ниже попробует преобразовать строку в double
                    }
                }
                return std::stod(parsed_num);
            } catch (...) {
                throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
            }
        }

        // Считывает содержимое строкового литерала JSON-документа
        // Функцию следует использовать после считывания открывающего символа ":
        Node LoadString(std::istream& input) {
            using namespace std::literals;

            auto it = std::istreambuf_iterator<char>(input);
            auto end = std::istreambuf_iterator<char>();
            std::string s;
            while (true) {
                if (it == end) {
                    // Поток закончился до того, как встретили закрывающую кавычку?
                    throw ParsingError("String parsing error");
                }
                const char ch = *it;
                if (ch == '"') {
                    // Встретили закрывающую кавычку
                    ++it;
                    break;
                } else if (ch == '\\') {
                    // Встретили начало escape-последовательности
                    ++it;
                    if (it == end) {
                        // Поток завершился сразу после символа обратной косой черты
                        throw ParsingError("String parsing error");
                    }
                    const char escaped_char = *(it);
                    // Обрабатываем одну из последовательностей: \\, \n, \t, \r, \"
                    switch (escaped_char) {
                        case 'n':
                            s.push_back('\n');
                            break;
                        case 't':
                            s.push_back('\t');
                            break;
                        case 'r':
                            s.push_back('\r');
                            break;
                        case '"':
                            s.push_back('"');
                            break;
                        case '\\':
                            s.push_back('\\');
                            break;
                        default:
                            // Встретили неизвестную escape-последовательность
                            throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
                    }
                } else if (ch == '\n' || ch == '\r') {
                    // Строковый литерал внутри- JSON не может прерываться символами \r или \n
                    throw ParsingError("Unexpected end of line"s);
                } else {
                    // Просто считываем очередной символ и помещаем его в результирующую строку
                    s.push_back(ch);
                }
                ++it;
            }

            return Node(std::move(s));
        }

        Node LoadArray(istream& input) {
            Array result;
            char c{};
            input >> c;
            if(c == '\0') {
                throw ParsingError("Invalid JSON array");
            }
            input.putback(c);
            for (; input >> c && c != ']';) {
                if (c != ',') {
                    input.putback(c);
                }
                result.push_back(LoadNode(input));
            }
            return Node(std::move(result));
        }

        Node LoadDict(istream& input) {
            Dict result;
            char c{};
            input >> c;
            if(c == '\0') {
                throw ParsingError("Invalid JSON array");
            }
            input.putback(c);
            for (; input >> c && c != '}';) {
                if (c == ',') {
                    input >> c;
                }
                string key = LoadString(input).AsString();
                input >> c;
                result.insert({move(key), LoadNode(input)});
            }

            return Node(std::move(result));
        }

        Node LoadNode(istream& input) {
            char c;
            input >> c;
            if (c == '[') {
                return LoadArray(input);
            } else if (c == '{') {
                return LoadDict(input);
            } else if (c == '"') {
                return LoadString(input);
            } else if (isdigit(c) || c == '-') {
                input.putback(c);
                return LoadNumber(input);
            } else if (c == 't' || c == 'f') {
                input.putback(c);
                return LoadBool(input);
            } else {
                input.putback(c);
                return LoadNull(input);
            }
        }
    }  // namespace

    void Node::PrintValue(std::nullptr_t, const PrintContext& ctx) const {
        ctx.out << "null"sv;
    }

    void Node::PrintValue(const bool& value, const PrintContext& ctx) const {
        ctx.out << (value ? "true"sv : "false"sv);
    }

    void Node::PrintValue(const std::string& value, const PrintContext& ctx) const {
        ctx.out << '"';
        for (char c : value) {
            switch (c) {
                case '\n': ctx.out << "\\n"; break;
                case '\r': ctx.out << "\\r"; break;
                case '\"': ctx.out << "\\\""; break;
                case '\\': ctx.out << "\\\\"; break;
                default: ctx.out << c; break;
            }
        }
        ctx.out << '"';
    }

    void Node::PrintValue(const Array& value, const PrintContext& ctx) const {
        ctx.out << '[';
        if (!value.empty()) {
            const Node& first = value.front();
            ctx.out << '\n';
            ctx.Indented().PrintIndent();
            ctx.Indented().PrintIndent();
            first.PrintNode(first, ctx.Indented());
            for (auto it = std::next(value.begin()); it != value.end(); ++it) {
                ctx.out << ",\n";
                ctx.Indented().PrintIndent();
                ctx.Indented().PrintIndent();
                it->PrintNode(*it, ctx.Indented());
            }
        } else {
            ctx.out << '\n';
        }
        ctx.out << '\n';
        ctx.PrintIndent();
        ctx.PrintIndent();
        ctx.out << ']';
    }

    void Node::PrintValue(const Dict& value, const PrintContext& ctx) const {
        ctx.out << '{';
        if (!value.empty()) {
            const auto& [key, val] = *value.begin();
            ctx.out << '\n';
            ctx.Indented().PrintIndent();
            ctx.Indented().PrintIndent();
            ctx.out << '"' << key << "\": ";
            val.PrintNode(val, ctx.Indented());
            for (auto it = std::next(value.begin()); it != value.end(); ++it) {
                const auto& [key, val] = *it;
                ctx.out << ",\n";
                ctx.Indented().PrintIndent();
                ctx.Indented().PrintIndent();
                ctx.out << '"' << key << "\": ";
                val.PrintNode(val, ctx.Indented());
            }
            ctx.out << '\n';
            ctx.Indented().PrintIndent();
        }
        ctx.out << '}';
    }

    void Node::PrintNode(const Node& node, const PrintContext& ctx) const {
        std::visit(
                [&ctx, this](const auto& value){ Node::PrintValue(value, ctx); },
                node.GetValue());
    }

    const Array& Node::AsArray() const {
        if (Node::IsArray()) {
            return std::get<Array>(value_);
        } else {
            throw std::logic_error("Value is not of type vector<Node>.");
        }
    }

    const Dict& Node::AsMap() const {
        if (Node::IsMap()) {
            return std::get<Dict>(value_);
        } else {
            throw std::logic_error("Value is not of type map<string, Node>.");
        }
    }

    int Node::AsInt() const {
        if (Node::IsInt()) {
            return std::get<int>(value_);
        } else {
            throw std::logic_error("Value is not of type int.");
        }
    }

    const string& Node::AsString() const {
        if (Node::IsString()) {
            return std::get<std::string>(value_);
        } else {
            throw std::logic_error("Value is not of type string.");
        }
    }

    bool Node::AsBool() const {
        if (Node::IsBool()) {
            return std::get<bool>(value_);
        } else {
            throw std::logic_error("Value is not of type bool.");
        }
    }

    double Node::AsDouble() const {
        if (Node::IsDouble()) {
            if(Node::IsPureDouble()){
                return std::get<double>(value_);
            } else {
                return static_cast<double>(std::get<int>(value_));
            }
        } else {
            throw std::logic_error("Value is not of type double.");
        }
    }

    bool Node::IsInt() const {
        return std::holds_alternative<int>(value_);
    }

    bool Node::IsDouble() const {
        return std::holds_alternative<int>(value_) || std::holds_alternative<double>(value_);
    }

    bool Node::IsPureDouble() const {
        return std::holds_alternative<double>(value_);
    }

    bool Node::IsBool() const {
        return std::holds_alternative<bool>(value_);
    }

    bool Node::IsString() const {
        return std::holds_alternative<std::string>(value_);
    }

    bool Node::IsNull() const {
        return std::holds_alternative<std::nullptr_t>(value_);
    }

    bool Node::IsArray() const {
        return std::holds_alternative<Array>(value_);
    }

    bool Node::IsMap() const {
        return std::holds_alternative<Dict>(value_);
    }

    const Node::Value& Node::GetValue() const {
        return value_;
    }

    Document::Document(Node root)
            : root_(std::move(root)) {
    }

    const Node& Document::GetRoot() const {
        return root_;
    }

    Document Load(istream& input) {
        return Document{LoadNode(input)};
    }

    void Print(const Document& doc, std::ostream& output) {
        PrintContext ctx(output, 2);
        const auto& node = doc.GetRoot();
        node.PrintNode(node, ctx);
    }

}  // namespace json