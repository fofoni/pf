/*
 * Universidade Federal do Rio de Janeiro
 * Escola Politécnica
 * Projeto Final de Graduação
 * Ambiente de Teste para Filtros Adaptativos
 * Pedro Angelo Medeiros Fonini
 * Orientador: Markus Lima
 */

/**
 *
 * \file AdaptiveFilter.h
 *
 * Holds the interface to the `AdaptiveFilter` class.
 *
 * \author Pedro Angelo Medeiros Fonini
 */

#ifndef ADAPTIVEFILTER_H
#define ADAPTIVEFILTER_H

#include <string>
#include <sstream>
#include <stdexcept>

template <typename SAMPLE_T>
class AdaptiveFilter
{

public:

    // adaptive filter {init, close, run, restart}
    typedef void *(*afi_t)(void);
    typedef int (*afc_t)(void *);
    typedef SAMPLE_T (*afr_t)(void *, SAMPLE_T, SAMPLE_T);
    typedef void *(*aft_t)(void *);

    AdaptiveFilter(std::string dso_path);
    ~AdaptiveFilter();

    // TODO: fazer outro ctor, sem nenhum argumento, que inicializa um
    // adapf "dummy", no op.

    void test();

    SAMPLE_T get_sample(SAMPLE_T x, SAMPLE_T y) {
        return (*run)(data, x, y);
    }

private:

    template <typename SYM_T>
    SYM_T get_sym(std::string sym_name);

    // TODO: fazer função raise(string) para embutir o path e o dlerror
    // no AdapfException (com opção p não mostrar dlerror)

    std::string path;

    void *lib;

    afi_t init;
    afc_t close;
    afr_t run;
    aft_t restart;

    void *data;

};


class AdapfException: public std::runtime_error {

public:

    AdapfException(const std::string& desc,
                   const std::string& dso_path,
                   const std::string& dlerr)
      : runtime_error(desc), description(desc), dlerror(dlerr), path(dso_path),
        what_msg(
            std::string(runtime_error::what()) + ". DSO: " + path
            + " . DL error: " + dlerror
        )
    {
    }

    virtual const char* what() const throw() {
        return what_msg.c_str();
    }

private:
    std::string description;
    std::string dlerror;
    std::string path;
    std::string what_msg;
    static std::ostringstream statwstream;

};


template <typename SAMPLE_T>
template <typename SYM_T>
SYM_T AdaptiveFilter<SAMPLE_T>::get_sym(std::string sym_name) {
    std::string full_sym_name = std::string("adapf_") + sym_name;
    SYM_T sym = reinterpret_cast<SYM_T>(dlsym(lib, full_sym_name.c_str()));
    if (!sym)
        throw AdapfException(
                std::string("Could not open ") + full_sym_name
                + " symbol from shared object file",
                path, dlerror());
    return sym;
}

#endif // ADAPTIVEFILTER_H
