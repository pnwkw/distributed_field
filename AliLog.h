#ifndef ALIMAG_BENCHMARK_ALILOG_H
#define ALIMAG_BENCHMARK_ALILOG_H

// Note: mockup of the original AliLog which contains just enough functions for the rest of imported ALICE code to compile

#include <iostream>
#include <TString.h>

static void AliInfo(const char *t) { std::cout << t << std::endl; }
static void AliInfoF(const char *t, float f) { std::cout << Form(t, f) << std::endl; }
static void AliInfoF(const char *t, const char *s) { std::cout << Form(t, s) << std::endl; }
static void AliInfoF(const char *t, int d, const char *s) { std::cout << Form(t, d, s) << std::endl; }
static void AliWarning(const char *t) { std::cout << t << std::endl; }
static void AliFatal(const char *t) { std::cout << t << std::endl; }
static void AliFatalF(const char *t, ...) { std::cout << t << std::endl; }

static void AliWarningClassF(const char *t, ...) { std::cout << t << std::endl; }
static void AliInfoGeneral(const char *t, ...) { std::cout << t << std::endl; }
static void AliErrorF(const char *t, ...) { std::cout << t << std::endl; }

namespace AliLog {
    enum EType_t {
        kError,
        kFatal
    };
}

static void AliMessageGeneral(const char *t, ...) { std::cout << t << std::endl; }
static void AliFatalGeneral(const char *t, ...) { std::cout << t << std::endl; }

#endif //ALIMAG_BENCHMARK_ALILOG_H
