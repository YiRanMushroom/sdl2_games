module;

#include <nlohmann/json.hpp>
#include <type_traits>

export module json;
import std_overloads;

export using nlohmann::json;

namespace jsonutil {
    export json parse_no_throw(std::istream &is) {
        try {
            return json::parse(is, nullptr, true, false);
        } catch (json::parse_error &e) {
            return json{};
        }
    }

    export decltype(auto) read_or_default(auto &&provider) requires requires {
        provider();
    } {
        using invoke_result = std::invoke_result_t<decltype(provider)>;
        return [provider = forward_acceptor{std::forward<decltype(provider)>(provider)}
                ](const json &j) -> invoke_result {
            try {
                return j.template get<invoke_result>();
            } catch (json::exception &) {
                return provider.value();
            }
        };
    }

    export decltype(auto) read_or_default(auto &&default_value) {
        return [default_value = forward_acceptor{std::forward<decltype(default_value)>(default_value)}
                ](const json &j) {
            try {
                return j.template get<std::remove_cvref_t<decltype(default_value.value)> >();
            } catch (json::exception &) {
                return default_value.forward();
            }
        };
    }

    export decltype(auto) read_or_default(auto &&key, auto &&... args) requires requires {
        sizeof...(args) > 0;
    } {
        using return_type = decltype([
            last = forward_acceptor{std::get<sizeof...(args) - 1>(std::forward_as_tuple(args...))}
        ] {
            if constexpr (std::is_invocable_v<decltype(last.value)>) {
                return last.value();
            } else {
                return last.value;
            }
        }());

        return [key = forward_acceptor{std::forward<decltype(key)>(key)},
                    tuple = std::forward_as_tuple(std::forward<decltype(args)>(args)...)]
        (const json &j) {
            try {
                if (j.contains(key.value)) {
                    return j.at(key.value)
                           | std::apply
                           ([]<typename... Tp>(Tp &&... unpacked) {
                               return read_or_default(
                                   std::forward<Tp>(unpacked)...);
                           }, tuple);
                }

                if constexpr (std::is_invocable_v<decltype(std::get<sizeof...(args) - 1>(tuple))>) {
                    return std::get<sizeof...(args) - 1>(tuple)();
                } else {
                    return std::get<sizeof...(args) - 1>(tuple);
                }
            } catch (json::exception &) {
                if constexpr (std::is_invocable_v<decltype(std::get<sizeof...(args) - 1>(tuple))>) {
                    return std::get<sizeof...(args) - 1>(tuple)();
                } else {
                    return std::get<sizeof...(args) - 1>(tuple);
                }
            }
        };
    }
}
