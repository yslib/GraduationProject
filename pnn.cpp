#include "pnn.h"
#include "config.h"

PNN::PNN(const std::string & path) :__max_columns(0)
{
    load_pnn(path);
}
void PNN::add_eigen_vector(CLASSIFICATION classification, const std::vector<double>& vec)
{
    if (vec.size() > __max_columns) {
        __max_columns = static_cast<int>(vec.size());
        for (int i = 0; i < __pnn.size(); i++) {
            __pnn[i].__eigen_vector.resize(__max_columns);
        }
    }
    std::vector<double> v = vec;
    v.resize(__max_columns);
    __pnn.emplace_back(classification,v);
    __classification_mapping[classification].second.push_back(int(__pnn.size() - 1));
}
bool PNN::save_pnn(const std::string & path) {
    //Q_UNUSED(path);
    //
    return true;
}
bool PNN::load_pnn(const std::string & path) {
    //Q_UNUSED(path);
    return true;
}
PNN::CLASSIFICATION PNN::classify(const std::vector<double> & vec,
    PNN::PROB_TABLE & prob_table)
{
    ASSERT(vec.size() == __max_columns);
    if (vec.size() > __max_columns) {
        __max_columns = static_cast<int>(vec.size());
        for (int i = 0; i < __pnn.size(); i++) {
            __pnn[i].__eigen_vector.resize(__max_columns);
        }
    }

    for (int i = 0; i < __pnn.size(); i++) {
        int length = static_cast<int>(__pnn[i].__eigen_vector.size());
        for (int j = 0; j < length; j++) {
            __pnn[i].prob += std::sqrt((vec[j] - __pnn[i].__eigen_vector[j])*(vec[j] - __pnn[i].__eigen_vector[j]));
        }
        __pnn[i].prob = expf(-(__pnn[i].prob) / (2 * (0.1*0.1)));
    }
    CLASSIFICATION cls = -1;
    PROB prob = -1.0f;
    double tot = 0.0f;
    for (const std::pair<CLASSIFICATION,PNN::__cell_cls> & c : __classification_mapping) {
        __classification_mapping[c.first].first = 0.0f;
        for (INDEX index : c.second.second) {
            __classification_mapping[c.first].first += __pnn[index].prob;
        }
        if (prob < __classification_mapping[c.first].first) {
            prob = __classification_mapping[c.first].first;
            cls = c.first;
        }
        prob_table[c.first] = __classification_mapping[c.first].first;
        tot+= __classification_mapping[c.first].first;
    }
    for (auto itr = prob_table.begin(); itr != prob_table.end(); ++itr) {
        itr->second /= tot;
    }
    return cls;
}

void PNN::classify(const std::vector<std::vector<double>>& mat,
    std::vector<CLASSIFICATION>& res,
    std::vector<PROB_TABLE>& res_prob_table)
{
    int size = static_cast<int>(mat.size());
    res.resize(size);
    res_prob_table.resize(size);
    for (int i = 0; i < size; i++)
        res[i] = classify(mat[i],res_prob_table[i]);
}
