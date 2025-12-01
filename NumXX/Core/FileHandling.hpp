/* ReadFile.hpp */
#pragma once

#include <fstream>
#include <string>
#include <map>

#include "../Containers/NArray.hpp"

#define NARRAY_FILE_PREFIX "NumXX::NArray"


namespace numxx {

    enum class FileType {
        OTHER,
        TXT,
        CSV,
        NARRAY,
    };

    inline std::map<std::string, FileType> ext_to_type = {
        {"txt", FileType::TXT},
        {"csv", FileType::CSV},
        {"", FileType::TXT}
    };

    // Helper function to get the type of file being opened
    inline FileType get_type(const char* filename) {
        const std::string name(filename);
        const size_t dotPos = name.find_last_of('.');

        std::string extension;
        if (dotPos != std::string::npos && dotPos + 1 < name.size()) {
            extension = name.substr(dotPos + 1);
        } else {
            extension = "";
        }

        std::ifstream file(filename);
        std::string line;
        std::getline(file, line);
        if (line == NARRAY_FILE_PREFIX) {
            return FileType::NARRAY;
        }
        file.close();

        const auto it = ext_to_type.find(extension);
        if (it != ext_to_type.end()) {
            return it->second;
        }
        return FileType::OTHER;
    }


    // Reads the file and creates an NArray; this can create at most an NArray with two dimensions
    template <typename T>
    NArray<T> NArray_from_file(const char* filepath, const char delimiter = ' ', const bool skip = false) {
        std::ifstream file(filepath);
        if (!file) {
            throw error::ArgumentError("Error opening file \"" + std::string(filepath) + "\"");
        }

        std::vector<T> out_vec;
        std::vector<size_t> shape{0, 0};

        std::string line;
        size_t& line_counter = shape[0];

        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string token;
            size_t element_counter = 0;

            while (std::getline(ss, token, delimiter)) {
                if (!token.empty()) {
                    std::stringstream converter(token);
                    T elem;
                    converter >> elem;
                    out_vec.push_back(elem);
                    element_counter++;
                }
            }

            if (line_counter > 0 && element_counter != shape[1]) {
                if (!skip) {
                    throw error::ShapeError(
                        "The input data is inconsistent. Please check line " +
                        std::to_string(line_counter + 1) + " in \"" + std::string(filepath) + "\"."
                    );
                }
                // Skip the line and remove the elements
                std::cerr << "Skipping line " << (line_counter + 1) << "." << std::endl;
                for (int i = 0; i < element_counter; i++) out_vec.pop_back();
                line_counter--;

            } else
                shape[1] = element_counter;

            line_counter++;
        }
        file.close();

        if (line_counter == 1)
            return NArray<T>(std::move(out_vec));

        return NArray<T>(std::move(out_vec), Shape(std::move(shape)));
    }


    template <typename T>
    NArray<T> loadtxt(const char* filepath, const char delimiter = ' ', const bool skip = false) {
        return NArray_from_file<T>(filepath, delimiter, skip);
    }


    template <typename T>
    NArray<T> loadcsv(const char* filepath, const char delimiter = ',', const bool skip = false) {
        return NArray_from_file<T>(filepath, delimiter, skip);
    }


    /* Reads a file that is written in NArray format:
     * "NumXX::NArray" on the first line, followed by the shape then the data */
    template <typename T>
    NArray<T> read_NArray(const char* filepath) {
        // Open file
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file.");
        }

        std::string line;

        // Skip the header line "NumXX::NArray"
        std::getline(file, line);

        // Extract shape
        std::vector<size_t> shape;
        if (std::getline(file, line) && !line.empty()) {
            std::stringstream converter(line);
            size_t dim;
            while (converter >> dim)
                shape.push_back(dim);
        }

        // Create output array and extract data
        auto out = NArray<T>(Shape(shape));

        if (std::getline(file, line) && !line.empty()) {
            std::stringstream converter(line);
            T elem;
            size_t index = 0;

            while (converter >> elem && index < out.get_total_size()) {
                out(index++) = elem;
            }

            if (index != out.get_total_size()) {
                std::cerr << "Warning: Expected " << out.get_total_size()
                          << " elements, but found " << index << ".\n";
            }
        }
        return out;
    }


    /* Universal file reading function:
     * Automatically detects the type of file being read and creates an NArray accordingly */
    template <typename T>
    NArray<T> load_from_file(const char* filepath, const char delimiter = ' ', const bool skip = false) {
        switch (get_type(filepath)) {
            case FileType::NARRAY:
                return read_NArray<T>(filepath);
            case FileType::TXT:
                return loadtxt<T>(filepath, delimiter, skip);
            case FileType::CSV:
                return loadcsv<T>(filepath, ',', skip);
            default:
                try {
                    return NArray_from_file<T>(filepath, delimiter, skip);
                } catch (std::exception&) {
                    std::cerr << "Please check formatting of " << filepath << std::endl;
                    std::terminate();
                }
        }
    }


    // Saves a file in NArray format
    template <typename T>
    void save_as_NArray(const char* filepath, const NArray<T>& arr) {
        std::ofstream file(filepath);

        file << NARRAY_FILE_PREFIX << '\n';
        for (const auto& elem : arr.get_shape().dimensions) file << elem << ' ';
        file << '\n';

        for (const auto& elem: arr) file << elem << ' ';
        file.close();
    }


    // Saves an NArray by spacing elements using a delimiter. Can only save NArrays with up to two dimensions.
    template <typename T>
    void save_to_file(const char* filepath, const NArray<T>& arr, const char delimiter = ' ') {
        const size_t dims = arr.get_shape().get_Ndim();
        if (dims > 2) {
            throw error::ShapeError(
                "Cannot write an array of shape " + util::toString(arr.get_shape()) +
                " to a file, NArray must have at most 2 dimensions."
                "Try using numxx::save_as_NArray() or reshaping the NArray to save."
            );
        }
        std::ofstream file(filepath);

        if (dims == 2) {
            for (size_t i = 0; i < arr.len(); i++) {
                for (size_t k = 0; k < arr.get_shape()[0]; k++) {
                    file << arr[i](k);
                    if (k < arr.get_shape()[0] - 1) file << delimiter;
                }
                file << '\n';
            }
        } else {
            for (size_t i = 0, len = arr.len(); i < len; i++) {
                file << arr(i);
                if (i < len - 1) file << delimiter;
            }
        }
    }

    template <typename T>
    void savetxt(const char* filepath, const NArray<T>& arr, const char delimiter = ' ') {
        save_to_file(filepath, arr, delimiter);
    }

    template <typename T>
    void savecsv(const char* filepath, const NArray<T>& arr, const char delimiter = ',') {
        save_to_file(filepath, arr, delimiter);
    }

} // namespace numxx

#undef NARRAY_FILE_PREFIX
