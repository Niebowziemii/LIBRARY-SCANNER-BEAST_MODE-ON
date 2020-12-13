#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm> 
#include <set>
#include <tuple>

void print(std::vector<uint64_t> const& a) {
    for (unsigned int i = 0; i < a.size(); i++)
        std::cout << a.at(i) << ' ';
    std::cout << '\n';
}
bool sort_by_sec(const std::tuple<uint64_t, uint64_t>& a,const std::tuple<uint64_t, uint64_t>& b){
    return (std::get<1>(a) > std::get<1>(b));
}
uint64_t get_num_of_diff_books(std::vector<uint64_t> const& first_line) {
    return first_line[0];
}
uint64_t get_num_of_lib(std::vector<uint64_t> const&  first_line) {
    return first_line[1];
}
uint64_t get_num_of_total_days(std::vector<uint64_t> const& first_line) {
    return first_line[2];
}
uint64_t get_num_of_books_in_library(std::vector<std::vector<uint64_t>> const& library) {
    return library[0][0];
}
uint64_t get_num_of_days_to_signup(std::vector<std::vector<uint64_t>> const& library) {
    return library[0][1];
}
uint64_t get_num_of_books_per_day(std::vector<std::vector<uint64_t>> const& library) {
    return library[0][2];
}
std::vector<uint64_t> get_books_from_library(std::vector<std::vector<uint64_t>> const& library) {
    return library[1];
}
int main()
{
    // READING THE FILE AND PREPARING THE STRUCTURE
    std::fstream data_file;
    data_file.open("c_incunabula.txt", std::ios::in);
    std::vector<uint64_t> first_line; //first line of input
    std::vector<uint64_t> book_scores; //second line of input 
    std::vector<uint64_t> library_metadata; 
    std::vector<uint64_t> library_books_id;
    std::vector<std::vector<uint64_t>> library; //[[library_metadata][library_books_id]]
    std::vector<std::vector<std::vector<uint64_t>>> collection_of_libraries; //[[[library_metadata0][library_books_id0]],[[library_metadata1][library_books_id1]]]
    if (data_file.is_open()) {
        std::string temp;
        getline(data_file, temp);
        std::istringstream iss;
        iss.str(temp);
        for (std::string temp; iss >> temp;) {
            first_line.push_back(std::stoi(temp));
        }
        getline(data_file, temp);
        iss.clear();
        iss.str(temp);
        for (std::string temp; iss >> temp;) {
            book_scores.push_back(std::stoi(temp));
        }
        for (unsigned int i = 0; i < get_num_of_lib(first_line); i++){
            library_metadata.clear();
            library_books_id.clear();
            library.clear();

            getline(data_file, temp);
            iss.clear();
            iss.str(temp);
            for (std::string temp; iss >> temp;) {
                library_metadata.push_back(std::stoi(temp));
            }
            getline(data_file, temp);
            iss.clear();
            iss.str(temp);
            for (std::string temp; iss >> temp;) {
                library_books_id.push_back(std::stoi(temp));
            }
            library.push_back(library_metadata);
            library.push_back(library_books_id);
            collection_of_libraries.push_back(library);
        }
    }
    data_file.close();
    uint64_t remaining_days = get_num_of_total_days(first_line);
    std::set<uint64_t> processed_elements;
    long long total_points = 0;

    // CREATING VECTOR WITH ID-SCORE PAIRS OF BOOKS FOR EVERY LIBRARY INITIALLY SORTED
    std::vector<std::tuple<uint64_t, uint64_t>> id_and_scores_of_library;
    std::vector<std::vector<std::tuple<uint64_t, uint64_t>>> collection_of_libraries_with_id_and_scores;
    for (std::vector<std::vector<uint64_t>> i : collection_of_libraries) {
        id_and_scores_of_library.clear();
        for (unsigned j = 0; j < get_num_of_books_in_library(i); j++) {
            std::tuple<uint64_t, uint64_t> id_score_pair(get_books_from_library(i)[j], book_scores[get_books_from_library(i)[j]]);
            id_and_scores_of_library.push_back(id_score_pair);
        }
        std::sort(begin(id_and_scores_of_library), end(id_and_scores_of_library), sort_by_sec);
        collection_of_libraries_with_id_and_scores.push_back(id_and_scores_of_library);
    }
    std::vector<std::vector<std::tuple<uint64_t, uint64_t>>> collection_of_libraries_with_id_and_scores_DEEP_COPY(collection_of_libraries_with_id_and_scores);
    std::vector<std::vector<std::vector<uint64_t>>> collection_of_libraries_DEEP_COPY(collection_of_libraries);
    std::set<uint64_t> processed_elements_DEEP_COPY;
    std::vector<std::tuple<uint64_t, uint64_t, std::vector<uint64_t>>> output_result;
    uint64_t number_of_signed_libraries = 0;
    while (remaining_days > 0 && collection_of_libraries.size()>0) {

        //COUNTING SCORE FOR GIVEN LIBRARY
        std::vector<std::tuple<uint64_t, uint64_t>> scores_of_libraries;
        for (unsigned int i = 0; i < collection_of_libraries_with_id_and_scores.size(); i++) {
            uint64_t total_scanned_books = (remaining_days - get_num_of_days_to_signup(collection_of_libraries[i])) * get_num_of_books_per_day(collection_of_libraries[i]);
            uint64_t sum = 0;
            for (unsigned int j = 0; j < std::min(get_num_of_books_in_library(collection_of_libraries[i]), total_scanned_books); j++) {
                std::set<uint64_t>::iterator it = processed_elements.find(std::get<0>(collection_of_libraries_with_id_and_scores[i][j]));
                if (it == processed_elements.end()) {
                    sum += std::get<1>(collection_of_libraries_with_id_and_scores[i][j]);
                }
            }
            scores_of_libraries.push_back(std::tuple<uint64_t, uint64_t>(i,sum));
        }

        //UPDATE SET OF PROCESSED ELEMENTS
        auto max_lib_Id = std::get<0>(*std::max_element(scores_of_libraries.begin(), scores_of_libraries.end(), [](auto& l, auto& r) {return std::get<1>(l) < std::get<1>(r); }));
        for (unsigned int i = 0; i < collection_of_libraries_with_id_and_scores[max_lib_Id].size(); i++) {
            processed_elements.insert(std::get<0>(collection_of_libraries_with_id_and_scores[max_lib_Id][i]));
        }
        //DECREASE REMAINING DAYS COUNTER
        remaining_days -= get_num_of_days_to_signup(collection_of_libraries[max_lib_Id]);
        total_points += std::get<1>(scores_of_libraries[max_lib_Id]);

        //OBTAIN NECESSARY OUTPUT INFORMATION
        auto it = std::find(collection_of_libraries_with_id_and_scores_DEEP_COPY.begin(), collection_of_libraries_with_id_and_scores_DEEP_COPY.end(), collection_of_libraries_with_id_and_scores[max_lib_Id]);
        uint64_t index;
        if (it != collection_of_libraries_with_id_and_scores_DEEP_COPY.end())
        {
            index = it - collection_of_libraries_with_id_and_scores_DEEP_COPY.begin();
        }
        std::vector<uint64_t> result_books_id;
        uint64_t counter = 0;
        for (auto& i : collection_of_libraries_with_id_and_scores_DEEP_COPY[index]) {
            std::set<uint64_t>::iterator it = processed_elements_DEEP_COPY.find(std::get<0>(i));
            if (it == processed_elements_DEEP_COPY.end()) {
                counter++;
                processed_elements_DEEP_COPY.insert(std::get<0>(i));
                result_books_id.push_back(std::get<0>(i));
            }
        }
        std::tuple<uint64_t, uint64_t, std::vector<uint64_t>> result_record(index, counter, result_books_id);
        output_result.push_back(result_record);

        //REMOVE LIBRARY AFTER THAT
        collection_of_libraries_with_id_and_scores.erase(collection_of_libraries_with_id_and_scores.begin() + max_lib_Id);
        collection_of_libraries.erase(collection_of_libraries.begin() + max_lib_Id);
        scores_of_libraries.erase(scores_of_libraries.begin() + max_lib_Id);
        number_of_signed_libraries++;
    }  
    std::cout << number_of_signed_libraries << '\n';
    for (auto& i : output_result) {
        std::cout << std::get<0>(i) << ' ' << std::get<1>(i) << '\n';
        print(std::get<2>(i));
    }
    std::cout << "total points: " << total_points << '\n';
}
