#include "json_builder.h"

namespace json {

    Builder::KeyItemContext Builder::Key(std::string key) {
        current_keys_.push(std::move(key));
        return BaseContext{*this};
    }

    Builder::BaseContext Builder::Value(Node::Value value) {
        Node node = std::visit([](const auto &val) -> Node {
            return Node(val);
        }, value);
        if (nodes_stack_.empty()) {
            root_ = node;
        } else {
            if (nodes_stack_.back()->IsArray()) {
                std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(node);
            } else if (nodes_stack_.back()->IsDict()) {
                std::get<Dict>(nodes_stack_.back()->GetValue()).insert({std::move(current_keys_.top()), std::move(node)});
                current_keys_.pop();
            }
        }
        return BaseContext{*this};
    }

    Builder::DictItemContext Builder::StartDict() {
        Dict dict;
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()){
            is_nested_ = true;
        }
        nodes_stack_.emplace_back(std::make_unique<Node>(dict));
        return BaseContext{*this};
    }

    Builder::ArrayItemContext Builder::StartArray() {
        Array arr;
        if (!nodes_stack_.empty() && nodes_stack_.back()->IsArray()){
            is_nested_ = true;
        }
        nodes_stack_.emplace_back(std::make_unique<Node>(arr));
        return BaseContext{*this};
    }

    Builder& Builder::End() {
        using namespace std::literals;
        auto node = std::move(nodes_stack_.back());
        nodes_stack_.pop_back();
        if (!nodes_stack_.empty()) {
            if (nodes_stack_.back()->IsArray()) {
                std::get<Array>(nodes_stack_.back()->GetValue()).emplace_back(*node);
                is_nested_ = false;
            } else if (nodes_stack_.back()->IsDict()) {
                if (current_keys_.empty()) {
                    throw std::logic_error("Value should be used inside a dictionary with a key!"s);
                }
                std::get<Dict>(nodes_stack_.back()->GetValue()).insert({std::move(current_keys_.top()), *node});
                current_keys_.pop();
            } else {
                throw std::logic_error("Invalid method call!"s);
            }
        } else {
            root_ = *node;
        }
        return *this;
    }

    Builder& Builder::EndDict() {
        using namespace std::literals;
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsDict()) {
            throw std::logic_error("Invalid method call!"s);
        }
        return End();
    }

    Builder& Builder::EndArray() {
        using namespace std::literals;
        if (nodes_stack_.empty() || !nodes_stack_.back()->IsArray()) {
            throw std::logic_error("Invalid method call!"s);
        }
        return End();
    }

    Node Builder::Build() {
        using namespace std::literals;
        if (!nodes_stack_.empty() || std::holds_alternative<std::nullptr_t>(root_.GetValue())) {
            throw std::logic_error("Build called with incomplete data!"s);
        }
        return root_;
    }

    Builder::KeyItemContext Builder::BaseContext::Key(std::string key) {
        return builder_.Key(std::move(key));
    }

    Builder::BaseContext Builder::BaseContext::Value(Node::Value value) {
        return builder_.Value(std::move(value));
    }

    Builder::DictItemContext Builder::BaseContext::StartDict(){
        return builder_.StartDict();
    }

    Builder::ArrayItemContext Builder::BaseContext::StartArray(){
        return builder_.StartArray();
    }

    Builder& Builder::BaseContext::EndDict() {
        return builder_.EndDict();
    }

    Builder& Builder::BaseContext::EndArray() {
        return builder_.EndArray();
    }

    Node Builder::BaseContext::Build() {
        return builder_.Build();
    }

    Builder::ArrayItemContext Builder::ArrayItemContext::Value(Node::Value value) {
        return BaseContext::Value(std::move(value));
    }

    Builder::DictItemContext Builder::KeyItemContext::Value(Node::Value value) {
        return BaseContext::Value(std::move(value));
    }

}
