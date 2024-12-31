module;

#include <nlohmann/json.hpp>
#include <type_traits>
#include <utility>

export module json;
export import std_overloads;

export using nlohmann::json;

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

    // template<typename T>
    // using return_type_of = std::remove_cvref_t<decltype([] {
    //     using clean_type = std::remove_pointer_t<std::remove_all_extents_t<std::remove_cvref_t<T> > >;
    //     if constexpr (std::is_same_v<decltype(char{}), decltype(clean_type{})>) {
    //         return std::string{};
    //     } else {
    //         // static_assert(!std::is_same_v<decltype(char{}), decltype(clean_type{})>, "Unsupported type");
    //         return clean_type{};
    //     }
    // }())>;
    //
    // export decltype(auto) read_or_default(auto &&default_value) {
    //     using return_type = return_type_of<decltype(default_value)>;
    //     return [default_value = forward_acceptor{std::forward<decltype(default_value)>(default_value)}
    //             ](const json &j) -> return_type {
    //         try {
    //             return j.get<return_type>();
    //         } catch (json::exception &) {
    //             return default_value.forward();
    //         }
    //     };
    // }
    //
    // export decltype(auto) read_or_default(auto &&key, auto &&... args) requires requires {
    //     sizeof...(args) > 0;
    // } {
    //     using return_type = return_type_of<decltype(std::get<sizeof...(args) - 1>(std::forward_as_tuple(args...)))>;
    //     return [key = forward_acceptor{std::forward<decltype(key)>(key)},
    //                 tuple = std::forward_as_tuple(std::forward<decltype(args)>(args)...)]
    //     (const json &j) -> return_type{
    //         try {
    //             if (j.contains(key.value)) {
    //                 return j.at(key.value)
    //                        | std::apply
    //                        ([]<typename... Tp>(Tp &&... unpacked) -> return_type {
    //                            return read_or_default(
    //                                std::forward<Tp>(unpacked)...);
    //                        }, tuple);
    //             }
    //
    //             if constexpr (std::is_invocable_v<decltype(std::get<sizeof...(args) - 1>(tuple))>) {
    //                 return std::get<sizeof...(args) - 1>(tuple)();
    //             } else {
    //                 return std::get<sizeof...(args) - 1>(tuple);
    //             }
    //         } catch (json::exception &) {
    //             if constexpr (std::is_invocable_v<decltype(std::get<sizeof...(args) - 1>(tuple))>) {
    //                 return std::get<sizeof...(args) - 1>(tuple)();
    //             } else {
    //                 return std::get<sizeof...(args) - 1>(tuple);
    //             }
    //         }
    //     };
    // }
}
