#pragma once

#include "json.h"

#include <vector>
#include <string>
#include <stack>
#include <memory>

namespace json {

    class Builder {
    public:

        class BaseContext;

        class DictItemContext;

        class ArrayItemContext;

        class KeyItemContext;

        KeyItemContext Key(std::string key);

        BaseContext Value(Node::Value value);

        DictItemContext StartDict();

        ArrayItemContext StartArray();

        Builder& EndDict();

        Builder& EndArray();

        Node Build();

    private:
        Node root_;

        std::vector<std::unique_ptr<Node>> nodes_stack_;

        std::stack<std::string> current_keys_;

        bool is_nested_= false;

        Builder& End();

    };

    class Builder::BaseContext {
    public:

        BaseContext(Builder& builder) : builder_(builder) {}

        KeyItemContext Key(std::string key);

        BaseContext Value(Node::Value value);

        DictItemContext StartDict();

        ArrayItemContext StartArray();

        Builder& EndDict();

        Builder& EndArray();

        Node Build();

    private:
        Builder& builder_;
    };

    class Builder::DictItemContext : public BaseContext {
    public:

        DictItemContext(BaseContext base) : BaseContext(base) {}

        DictItemContext Value(Node::Value value) = delete;

        ArrayItemContext StartArray() = delete;

        DictItemContext StartDict() = delete;

        Builder& EndArray() = delete;

        Node Build() = delete;

    };

    class Builder::ArrayItemContext : public BaseContext {
    public:

        ArrayItemContext(BaseContext base) : BaseContext(base) {}

        ArrayItemContext Value(Node::Value value);

        KeyItemContext Key(std::string key) = delete;

        DictItemContext EndDict() = delete;

        Node Build() = delete;
    };

    class Builder::KeyItemContext : public BaseContext {
    public:

        KeyItemContext(BaseContext base) : BaseContext(base) {}

        DictItemContext Value(Node::Value value);

        KeyItemContext Key(std::string key) = delete;

        DictItemContext EndDict() = delete;

        Builder& EndArray() = delete;

        Node Build() = delete;

    };

}