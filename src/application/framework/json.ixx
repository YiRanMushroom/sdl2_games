module;

#include <nlohmann/json.hpp>

export module json;

import ywl.prelude;

export using nlohmann::json;

export template<typename T, typename F> requires std::is_invocable_v<F, T>
constexpr decltype(auto) operator|(T &&t, F &&f) {
    return std::forward<F>(f)(std::forward<T>(t));
}

namespace jsonutil {
    export json parse_no_throw(std::istream &is) {
        try {
            return json::parse(is, nullptr, true, false);
        } catch (json::parse_error &e) {
            return json{};
        }
    }

    export template<typename T> requires std::is_convertible_v<T, std::string>
    auto get_or_default(T &&default_value) {
        return [default_value = std::forward<T>(default_value)](const json &j) -> std::string {
            try {
                return j.get<std::string>();
            } catch (json::exception &) {
                return std::move(default_value);
            }
        };
    }

    export template<typename T>
    auto get_or_default(T &&default_value) {
        return [default_value = std::forward<T>(default_value)](const json &j) {
            try {
                return j.get<std::decay_t<decltype(default_value)> >();
            } catch (json::exception &) {
                return std::move(default_value);
            }
        };
    }

    export template<typename First, typename... Rest> requires (sizeof...(Rest) > 0)
    decltype(auto) get_or_default(First&& first, Rest&&... rest)  {
        return [first = std::forward_as_tuple(std::forward<First>(first)),
                rest = std::forward_as_tuple(std::forward<Rest>(rest)...)](const json &j) {
            try {
                return j.at(std::get<0>(first)) | std::apply
                ([]<typename... Tp>(Tp &&... unpacked) {
                    return get_or_default(std::forward<Tp>(unpacked)...);
                }, rest);
            } catch (json::exception &) {
                return std::get<sizeof...(Rest) - 1>(rest);
            }
        };
    }
}
