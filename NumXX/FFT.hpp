/* FFT.hpp */
#pragma once

#include <cmath>

#include "Containers/NArray.hpp"

#define M_2PI 6.283185307179586


namespace numxx::fft {

    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<complex<T>> dft(const NArray<T>& x_n) {
        const size_t N_samples = x_n.get_total_size();
        const size_t N_2 = std::ceil(N_samples / 2.0);
        NArray<complex<T>> dft_out(Shape{N_samples});

        for (size_t k = 0; k < N_2 + 1; k++) {
            complex<T> sum = 0;
            for (size_t n = 0; n < N_samples; n++) {
                sum += numxx::polar<T>(
                    x_n.get_data()[n],
                    (-M_2PI*static_cast<T>(k)*static_cast<T>(n)/N_samples)
                );
            }
            dft_out(k) = sum;
            if (k > 0 && k < N_2)
                dft_out(N_samples - k) = conj(sum);
        }
        return dft_out;
    }


    template <typename T, typename = std::enable_if_t<std::is_arithmetic_v<T>>>
    NArray<complex<T>> idft(const NArray<complex<T>>& X_k) {
        const size_t N_samples = X_k.get_total_size();
        NArray<complex<T>> x_n(Shape{N_samples});

        for (size_t n = 0; n < N_samples; n++) {
            complex<T> sum = 0;
            for (size_t k = 0; k < N_samples; k++) {
                T angle = (M_2PI * static_cast<T>(k) * static_cast<T>(n) / N_samples);
                sum += X_k.get_data()[k] * complex<T>(std::cos(angle), std::sin(angle));
            }
            x_n(n) = sum / static_cast<T>(N_samples);
        }
        return x_n;
    }


    inline NArray<double> fftfreq(const size_t& n_samples, const float& diff = 1.0f) {
        const size_t N_2 = (n_samples + 1) / 2;
        NArray<double> out(Shape{n_samples});

        for (size_t i = 0; i < N_2 + 1; i++) {
            const auto f = diff * static_cast<double>(i);
            out(i) = f;
            if (i > 0 && i < N_2)
                out(n_samples - i) = -f;
        }
        return out;
    }


} // namespace numxx::fft

#undef M_2PI