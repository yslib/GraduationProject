#ifndef PNN_H
#define PNN_H

#include <unordered_map>

#ifndef _MSC_VER

#include <vector>
#include <string>
#endif

//Probabilistic Neural Networks
class PNN
{
public:
    typedef int INDEX;
    typedef int CLASSIFICATION;
    typedef double PROB;
    typedef std::unordered_map<CLASSIFICATION, PROB> PROB_TABLE;
    //typedef std::vector<double> wtf;

    PNN(const std::string & path = std::string());
    void add_eigen_vector(CLASSIFICATION classification, const std::vector<double> & vec);
    bool save_pnn(const std::string & path);
    bool load_pnn(const std::string & path);
    CLASSIFICATION classify(const std::vector<double> & vec,
        PROB_TABLE & prob_table);
    void classify(const std::vector<std::vector<double> > & mat,
        std::vector<CLASSIFICATION> & res,
        std::vector<PROB_TABLE> & res_prob_table
    );

private:
    struct __cell
    {
        __cell() {}
        __cell(CLASSIFICATION classification, const std::vector<double> & vec):__classification(classification),
        __eigen_vector(vec){}
        CLASSIFICATION __classification;
        std::vector<double> __eigen_vector;
        double prob;
    };

    static const int MAGIC_NUMBER = 0xAB930427;
    typedef std::pair<PROB, std::vector<INDEX> > __cell_cls;

    std::vector<__cell> __pnn;
    int __max_columns;
    std::unordered_map<CLASSIFICATION,__cell_cls> __classification_mapping;
};
#endif // PNN_H
