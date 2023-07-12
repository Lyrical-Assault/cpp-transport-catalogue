#pragma once

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>
#include <utility>
#include <sstream>

using namespace::std::literals::string_view_literals;

namespace json {

    // Контекст вывода, хранит ссылку на поток вывода и текущий отсуп
    struct PrintContext {
        PrintContext(std::ostream& out)
        : out(out) {
        }

        PrintContext(std::ostream& out, int indent_step, int indent = 0)
        : out(out)
        , indent_step(indent_step)
        , indent(indent) {
        }

        void PrintIndent() const {
            for (int i = 0; i < indent; ++i) {
                out.put(' ');
            }
        }

        // Возвращает новый контекст вывода с увеличенным смещением
        PrintContext Indented() const {
            return {out, indent_step, indent_step + indent};
        }

        std::ostream& out;
        int indent_step = 4;
        int indent = 0;
    };

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;

    // Эта ошибка должна выбрасываться при ошибках парсинга JSON
    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node final : private std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>{
    public:
        using variant::variant;
        using Value = std::variant<std::nullptr_t, Array, Dict, bool, int, double, std::string>;

        template <typename T>
        Node(T&& value)
                : value_(std::forward<T>(value)){
        }
        int AsInt() const;

        bool AsBool() const;

        double AsDouble() const;

        const std::string& AsString() const;

        const Dict& AsMap() const;

        const Array& AsArray() const;

        bool IsInt() const;

        bool IsDouble() const;

        bool IsPureDouble() const;

        bool IsBool() const;

        bool IsString() const;

        bool IsNull() const;

        bool IsArray() const;

        bool IsMap() const;

        const Value& GetValue() const;

        bool operator==(const Node& other) const {
            return this->value_ == other.value_;
        }

        bool operator!=(const Node& other) const {
            return !(*this == other);
        }

        // Шаблон, подходящий для вывода double и int
        template <typename Value>
        void PrintValue(const Value& value, const PrintContext& ctx) const {
            ctx.out << value;
        }

        void PrintValue(std::nullptr_t, const PrintContext& ctx) const;

        void PrintValue(const bool& value, const PrintContext& ctx) const;

        void PrintValue(const std::string& value, const PrintContext& ctx) const;

        void PrintValue(const Array& value, const PrintContext& ctx) const;

        void PrintValue(const Dict& value, const PrintContext& ctx) const;

        void PrintNode(const Node& node, const PrintContext& ctx) const;

    private:
        Value value_;
    };

    class Document {
    public:
        explicit Document(Node root);

        const Node& GetRoot() const;

        bool operator==(const Document& other) const {
            return this->root_ == other.root_;
        }
        bool operator!=(const Document& other) const {
            return !(*this == other);
        }

    private:
        Node root_;
    };

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json