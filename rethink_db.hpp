
/*
#include "proto\ql2.pb.h"
#include <string>
#include <stdio.h>
#include <stdint.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <stdlib.h>
#include <stdexcept>
#include <boost/format.hpp>
#include <vector>
#include <boost/unordered_map.hpp>
*/

#include "exception.hpp"
#include "datum.hpp"
#include "connection.hpp"
#include "rql.hpp"

// fix for undefined ssize_t from https://code.google.com/p/cpp-btree/issues/detail?id=6
#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

#ifndef RETHINK_DB_DRIVER
#define RETHINK_DB_DRIVER

#endif