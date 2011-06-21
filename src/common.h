
#ifndef WEBCL_COMMON_H_
#define WEBCL_COMMON_H_

#include <v8.h>
#include <node.h>

#include <CL/cl.hpp>

#define WEBCL_COND_RETURN_THROW(error) if (ret == error) return ThrowException(Exception::Error(String::New(#error)));

#endif
