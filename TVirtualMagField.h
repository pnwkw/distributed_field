#ifndef PAPER_ALIMAG_BENCHMARK_TVIRTUALMAGFIELD_H
#define PAPER_ALIMAG_BENCHMARK_TVIRTUALMAGFIELD_H

#include <TNamed.h>

class TVirtualMagField : public TNamed
{
public:
    TVirtualMagField()                 : TNamed() {}
    TVirtualMagField(const char *name) : TNamed(name,"") {}
    virtual ~TVirtualMagField() {}

    virtual void Field(const Double_t *x, Double_t *B) = 0;

    ClassDef(TVirtualMagField, 1)              // Abstract base field class
};

#endif //PAPER_ALIMAG_BENCHMARK_TVIRTUALMAGFIELD_H
