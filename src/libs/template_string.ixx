//
// Created by Yiran on 2024-12-27.
//

export module template_string;

template<size_t N>
using template_string_impl_char_array = char[N];

export template<size_t N>
struct template_string {
    template_string_impl_char_array<N> str;

    constexpr template_string(const template_string_impl_char_array<N> &s) {
        for (size_t i = 0; i < N; ++i) {
            str[i] = s[i];
        }
    }

    using return_type = const template_string_impl_char_array<N> &;

    constexpr operator return_type() const {
        return str;
    }

    constexpr return_type operator()() const {
        return str;
    }

    constexpr return_type c_str() const {
        return str;
    }

    constexpr return_type operator*() const {
        return str;
    }
};
