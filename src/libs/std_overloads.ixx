module;

#include <type_traits>
#include <concepts>
#include <format>

export module std_overloads;

import template_string;
import std_essentials;

// hash
namespace std {
    export template<typename T> requires requires(T t) { { t.hash_code() } -> std::same_as<size_t>; }
    struct hash<T> {
        size_t operator()(const T &t) const {
            return t.hash_code();
        }
    };
}

export template<template_string s>
constexpr auto operator ""_fmt() {
    return []<typename... Tp>(Tp &&... args) {
        return std::format(*s, std::forward<Tp>(args)...);
    };
}

export template<typename Tp>
concept receive_format_context = requires(Tp t, std::format_context &ctx) { t.stdfmt(ctx); };

export template<typename Tp> requires receive_format_context<Tp>
struct std::formatter<Tp> {
    static constexpr auto parse(std::format_parse_context &ctx) {
        if constexpr (requires { { Tp::stdfmt_parse(ctx) } -> std::same_as<std::format_parse_context::iterator>; }) {
            return Tp::stdfmt_parse(ctx);
        } else
            return ctx.begin();
    }

    template<typename FormatContext>
    static auto format(const Tp &t, FormatContext &ctx) {
        return t.stdfmt(ctx);
    }
};

namespace yan {
    namespace __impl {
        template<typename... Tp>
        struct overload_Tp : Tp... {
            using Tp::operator()...;
        };
    }

    export template<typename... Tp>
    constexpr auto overload(Tp &&...) {
        return __impl::overload_Tp<Tp...>{};
    }

    export template<typename... Tp>
    using overload_t = __impl::overload_Tp<Tp...>;
}

export decltype(auto) operator|(auto &&lhs, auto &&rhs) requires requires { rhs(lhs); } {
    return rhs(lhs);
}

export template<typename Tp>
struct forward_acceptor {
    Tp value;

    explicit forward_acceptor(Tp &&value) : value(std::forward<Tp>(value)) {
        static_assert(std::is_same_v<decltype(value), decltype(this->value)>);
    }

    decltype(auto) forward(this auto &&self) {
        return std::forward<decltype(self.value)>(self.value);
    }
};

export template<typename Tp>
forward_acceptor(Tp &&) -> forward_acceptor<Tp &&>;
